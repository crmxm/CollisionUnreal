// Fill out your copyright notice in the Description page of Project Settings.

#include "Collision.h"
#include "ContactManager.h"

#include <cassert>
#include "CollisionComponent.h"

ContactManager * ContactManager::instance = nullptr;

unsigned int ContactManager::createCount = 0;

const float EPSILON_TIME = 1e-4f;

void ContactManager::AddContact(UCollisionComponent * pCO1, UCollisionComponent * pCO2, float d, const FVector & normal, const FVector & contactPoint)
{
	FVector rA = contactPoint - pCO1->center;
	FVector rB = contactPoint - pCO2->center;

	FVector kA = FVector::CrossProduct(rA, normal);
	FVector kB = FVector::CrossProduct(rB, normal);

	float speed = FVector::DotProduct((pCO1->velocity - pCO2->velocity), normal) +
		FVector::DotProduct(pCO1->angularV, kA) - FVector::DotProduct(pCO2->angularV, kB);

	if (fabs(speed) <= EPSILON)
		return;

	float t = -d / speed;

	AddContact({ t, speed, pCO1, pCO2, normal, kA, kB, contactPoint });
}

void ContactManager::ResolveTop()
{
	assert(!contacts.empty());

	const Contact & top = contacts.top();

	contacts.pop();

	float e = std::min(top.pCO1->rCoeff, top.pCO2->rCoeff);

	FVector kAL = top.pCO1->root->ComponentToWorld.InverseTransformVector(top.kA);
	FVector kBL = top.pCO2->root->ComponentToWorld.InverseTransformVector(top.kB);
	FVector uAL = top.pCO1->inertiaInv.TransformVector(kAL);
	FVector uBL = top.pCO2->inertiaInv.TransformVector(kBL);

	float numer = -(1 + e) * top.nSpeed;
	float denom = top.pCO1->massInv + top.pCO2->massInv + FVector::DotProduct(kAL, uAL) + FVector::DotProduct(kBL, uBL);

	float f = numer / denom;

	FVector impulse = f * top.normal;

	top.pCO1->SetImpulse(impulse);
	top.pCO2->SetImpulse(-impulse);
	top.pCO1->SetAngularI(f * kAL);
	top.pCO2->SetAngularI(-f * kBL);

	return;
}

void ContactManager::Resolve()
{
	while (!contacts.empty())
	{
		const Contact & top = contacts.top();

		float t = top.time;
		float & t1 = top.pCO1->time;
		float & t2 = top.pCO2->time;

		if ((t1 == 0) && (t2 == 0))
		{
			t1 = t2 = t;
			ResolveTop();
		}
		else if ((t1 != 0) && (t2 != 0))
			contacts.pop();
		else if ((t1 >= t + EPSILON_TIME) || (t2 >= t + EPSILON_TIME))
			contacts.pop();
		else if (t1 == 0)
		{
			t1 = t2;
			ResolveTop();
		}
		else
		{
			t2 = t1;
			ResolveTop();
		}
	}
}
