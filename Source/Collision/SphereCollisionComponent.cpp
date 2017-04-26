// Fill out your copyright notice in the Description page of Project Settings.

#include "Collision.h"
#include "SphereCollisionComponent.h"
#include "BoxCollisionComponent.h"

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
