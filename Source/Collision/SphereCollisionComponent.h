// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CollisionComponent.h"
#include "SphereCollisionComponent.generated.h"

/**
 * 
 */
class USphereCollisionComponent;
class UBoxCollisionComponent;

UCLASS( ClassGroup = (Custom), meta = (BlueprintSpawnableComponent) )
class COLLISION_API USphereCollisionComponent : public UCollisionComponent
{
	GENERATED_BODY()
public:
	USphereCollisionComponent() {};

	virtual void BeginPlay() override;

private:
	UPROPERTY(EditInstanceOnly)
	float radius;

protected:

	virtual void Translate(const FVector & v) override { UCollisionComponent::Translate(v); };
	virtual void Rotate(const FVector & v) override { UCollisionComponent::Rotate(v); };
public:
	virtual bool SphereCollisionDetect(const USphereCollisionComponent * pSCO) const;
	virtual bool BoxCollisionDetect(const UBoxCollisionComponent * pBCO) const;

	virtual bool CollisionDetect(const UCollisionComponent * pCO) const { return pCO->SphereCollisionDetect(this); };
};
