// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BoxCollisionComponent.h"
#include "ForceFieldTriggerComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup = (Custom), meta = (BlueprintSpawnableComponent) )
class COLLISION_API UForceFieldTriggerComponent : public UBoxCollisionComponent
{
	GENERATED_BODY()
public:
	UForceFieldTriggerComponent() {};

	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type) override;

private:
	UPROPERTY(EditInstanceOnly)
	float airDragCoeff;
	UPROPERTY(EditInstanceOnly)
	FVector acceleration;

public:
	virtual bool SphereCollisionDetect(const USphereCollisionComponent *) const override;
	virtual bool BoxCollisionDetect(const UBoxCollisionComponent * pBCC) const override;
	virtual bool CollisionDetect(const UCollisionComponent * pCC) const override { return false; };

	FVector GetAcceleration(const FVector & v) const { return acceleration + airDragCoeff * v; };
	FVector GetAngularA(const FVector & w, float radiusSqr) const { return w * radiusSqr * airDragCoeff * 2 / 3; };
};
