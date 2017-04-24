// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CollisionManagerActor.generated.h"

UCLASS()
class COLLISION_API ACollisionManagerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollisionManagerActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type) override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual ~ACollisionManagerActor() override;

public:
	void DebugRotate();
};
