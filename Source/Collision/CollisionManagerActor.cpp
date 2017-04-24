// Fill out your copyright notice in the Description page of Project Settings.

#include "Collision.h"
#include "CollisionManagerActor.h"

#include "CollisionManager.h"
#include "ContactManager.h"

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
		InputComponent->BindAction("DebugInput0", EInputEvent::IE_Pressed, this, &ACollisionManagerActor::DebugRotate);
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
