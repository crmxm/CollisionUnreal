// Fill out your copyright notice in the Description page of Project Settings.

#include "Collision.h"
#include "CollisionManagerActor.h"

#include "CollisionManager.h"
#include "ContactManager.h"

#include <thread>

// Sets default values
ACollisionManagerActor::ACollisionManagerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionManager::Create();
	ContactManager::Create();
}

// Called when the game starts or when spawned
void ACollisionManagerActor::BeginPlay()
{
	Super::BeginPlay();

	EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (InputComponent)
	{
		InputComponent->BindAction("DebugInput0", EInputEvent::IE_Pressed, this, &ACollisionManagerActor::ToggleWorking);
		InputComponent->BindAction("DebugInput1", EInputEvent::IE_Pressed, this, &ACollisionManagerActor::StartDebugStopMode);

		InputComponent->BindAction("Group1", EInputEvent::IE_Pressed, this, &ACollisionManagerActor::ToggleGroup<0>);
		InputComponent->BindAction("Group2", EInputEvent::IE_Pressed, this, &ACollisionManagerActor::ToggleGroup<1>);
		InputComponent->BindAction("Group3", EInputEvent::IE_Pressed, this, &ACollisionManagerActor::ToggleGroup<2>);
		InputComponent->BindAction("Group4", EInputEvent::IE_Pressed, this, &ACollisionManagerActor::ToggleGroup<3>);
		InputComponent->BindAction("Group5", EInputEvent::IE_Pressed, this, &ACollisionManagerActor::ToggleGroup<4>);
		InputComponent->BindAction("Group6", EInputEvent::IE_Pressed, this, &ACollisionManagerActor::ToggleGroup<5>);
	}

	CollisionManager::instance->SetWorking(false);
}

void ACollisionManagerActor::EndPlay(const EEndPlayReason::Type)
{
}

// Called every frame
void ACollisionManagerActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	CollisionManager::instance->Update(DeltaTime);
}

ACollisionManagerActor::~ACollisionManagerActor()
{
	ContactManager::Destroy();
	CollisionManager::Destroy();
}

void ACollisionManagerActor::DebugRotate()
{
	const float MATH_PI = 3.1415926f;
	CollisionManager::instance->obj[0]->Rotate({ MATH_PI / 4, 0, 0 });
}

void ACollisionManagerActor::DebugSpeedUp()
{
	CollisionManager::instance->SetWorking(true);
}

void ACollisionManagerActor::ToggleWorking()
{
	CollisionManager::instance->SetWorking(!CollisionManager::instance->GetWorking());
}

void ACollisionManagerActor::StartDebugStopMode()
{
	for (unsigned int i = 0; i < CollisionManager::instance->size; i++)
		if (CollisionManager::instance->obj[i]->gID != 31)
			CollisionManager::instance->obj[i]->isDebugStopped = true;

	CollisionManager::instance->SetWorking(true);
}

template<unsigned int N>
void ACollisionManagerActor::ToggleGroup()
{
	for (unsigned int i = 0; i < CollisionManager::instance->size; i++)
		if (CollisionManager::instance->obj[i]->gID == N)
			CollisionManager::instance->obj[i]->isDebugStopped = false;
}
