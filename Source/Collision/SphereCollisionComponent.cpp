// Fill out your copyright notice in the Description page of Project Settings.

#include "Collision.h"
#include "SphereCollisionComponent.h"
#include "BoxCollisionComponent.h"
#include "ForceFieldTriggerComponent.h"

#include "ContactManager.h"

void USphereCollisionComponent::BeginPlay()
{
	UCollisionComponent::BeginPlay();

	massInv = (mass == 0) ? 0 : 1.0f / mass;
	inertia.M[0][0] = inertia.M[1][1] = inertia.M[2][2] = 2 * mass * radius * radius / 5;
	inertia.M[3][3] = 1;
	if (mass != 0)
		inertiaInv = inertia.Inverse();
	else
		inertiaInv = inertia;

	//center = root->ComponentToWorld.TransformPosition(centerL);
}

bool USphereCollisionComponent::SphereCollisionDetect(const USphereCollisionComponent * pSCC) const
{
	FVector dist = pSCC->center - center;

	if (dist.SizeSquared() > Sqr(radius + pSCC->radius))
		return false;

	if (childSize)
	{
		bool b = false;
		for (unsigned int i = 0; i < childSize; i++)
			b |= childs[i]->SphereCollisionDetect(pSCC);

		return b;
	}

	if (pSCC->childSize)
	{
		bool b = false;
		for (unsigned int i = 0; i < pSCC->childSize; i++)
			b |= pSCC->childs[i]->SphereCollisionDetect(this);

		return b;
	}

	float f = dist.Size();
	dist.Normalize();

	ContactManager::instance->AddContact((UCollisionComponent *) this, (UCollisionComponent *) pSCC, 
		radius + pSCC->radius - f, -dist, dist * (radius - pSCC->radius + f) / 2 + center);

	return true;
}

bool USphereCollisionComponent::BoxCollisionDetect(const UBoxCollisionComponent * pBCC) const
{
	FVector point = ClosestPointOnBox(center, pBCC);
	FVector dist = point - center;

	if (dist.SizeSquared() > Sqr(radius))
		return false;

	if (childSize)
	{
		bool b = false;
		for (unsigned int i = 0; i < childSize; i++)
			b |= childs[i]->BoxCollisionDetect(pBCC);

		return b;
	}

	if (pBCC->childSize)
	{
		bool b = false;
		for (unsigned int i = 0; i < pBCC->childSize; i++)
			b |= pBCC->childs[i]->SphereCollisionDetect(this);

		return b;
	}

	float f = dist.Size();
	dist.Normalize();

	ContactManager::instance->AddContact((UCollisionComponent *) pBCC, (UCollisionComponent *)this,
		radius - f, dist, point);
	return true;
}

void USphereCollisionComponent::DrawCollider() const
{
	DrawDebugSphere(GetWorld(), center, radius, 16, FColor::Green);
}

void USphereCollisionComponent::ApplyForce(float t, const UForceFieldTriggerComponent ** force, unsigned int size)
{
	FVector a(0, 0, 0);
	FVector w(0, 0, 0);

	for (unsigned int i = 0; i < size; i++)
	{
		auto f = force[i];
		if (f->SphereCollisionDetect(this))
		{
			a += f->GetAcceleration(velocity);
			w += f->GetAngularA(angularV, Sqr(radius));
		}
	}

	velocity += a * t;
	angularV += inertiaInv.GetScaleVector() * w * mass * t;

	UCollisionComponent::ApplyForce(t, force, size);
}
