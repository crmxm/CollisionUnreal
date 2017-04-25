// Fill out your copyright notice in the Description page of Project Settings.

#include "Collision.h"
#include "SphereCollisionComponent.h"

#include "ContactManager.h"

static inline float Sqr(float a) { return a * a; };

void USphereCollisionComponent::BeginPlay()
{
	UCollisionComponent::BeginPlay();

	massInv = (mass == 0) ? 0 : 1.0f / mass;
	inertia.M[0][0] = inertia.M[1][1] = inertia.M[2][2] = 2 * mass * radius * radius / 5;
	inertia.M[3][3] = 1;
	inertiaInv = inertia.Inverse();

	center = root->ComponentToWorld.TransformPosition(centerL);
}

bool USphereCollisionComponent::SphereCollisionDetect(const USphereCollisionComponent * pSCC) const
{
	FVector dist = pSCC->center - center;

	if (dist.SizeSquared() > Sqr(radius + pSCC->radius))
		return false;

	float f = dist.Size();
	dist.Normalize();

	ContactManager::instance->AddContact((UCollisionComponent *) this, (UCollisionComponent *) pSCC, 
		radius + pSCC->radius - f, -dist, dist * (radius - pSCC->radius + f) / 2 + center);

	return true;
}

bool USphereCollisionComponent::BoxCollisionDetect(const UBoxCollisionComponent * pBCC) const
{
	return false;
}
