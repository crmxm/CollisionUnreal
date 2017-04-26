// Fill out your copyright notice in the Description page of Project Settings.

#include "Collision.h"
#include "CollisionComponent.h"

#include "CollisionManager.h"

// Sets default values for this component's properties
UCollisionComponent::UCollisionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCollisionComponent::BeginPlay()
{
	Super::BeginPlay();

	isStatic |= isTrigger;

	CollisionManager::instance->AddObj(this);

	root = GetAttachmentRoot();

	if (isStatic)
	{
		mass = 0;
		velocity.Set(0, 0, 0);
		angularV.Set(0, 0, 0);
	}
	// ...
	
}

void UCollisionComponent::EndPlay(const EEndPlayReason::Type reason)
{
	if (reason == EEndPlayReason::Destroyed)
		CollisionManager::instance->RemoveObj(this);
}


// Called every frame
void UCollisionComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

