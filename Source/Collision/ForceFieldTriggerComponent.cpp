// Fill out your copyright notice in the Description page of Project Settings.

#include "Collision.h"
#include "ForceFieldTriggerComponent.h"
#include "SphereCollisionComponent.h"
#include "BoxCollisionComponent.h"
#include "CollisionManager.h"

void UForceFieldTriggerComponent::BeginPlay()
{
	USceneComponent::BeginPlay();

	isTrigger = true;
	isStatic = true;

	CollisionManager::instance->AddForce(this);

	UpdateTransform();
}

void UForceFieldTriggerComponent::EndPlay(const EEndPlayReason::Type reason)
{
	if (reason == EEndPlayReason::Destroyed)
		CollisionManager::instance->RemoveForce(this);
}

bool UForceFieldTriggerComponent::SphereCollisionDetect(const USphereCollisionComponent * pSCC) const
{
	FVector point = ClosestPointOnBox(pSCC->center, this);
	FVector dist = point - pSCC->center;

	if (dist.SizeSquared() > Sqr(pSCC->radius))
		return false;
	
	return true;
}

bool UForceFieldTriggerComponent::BoxCollisionDetect(const UBoxCollisionComponent * pBCC) const
{
	if (!CollisionTestBoxs(this, pBCC))
		return false;

	return true;
}