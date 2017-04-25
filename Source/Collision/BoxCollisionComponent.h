// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CollisionComponent.h"
#include "BoxCollisionComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup = (Custom), meta = (BlueprintSpawnableComponent) )
class COLLISION_API UBoxCollisionComponent : public UCollisionComponent
{
	GENERATED_BODY()
public:
	UBoxCollisionComponent() {};

	virtual void BeginPlay() override;
private:
	FVector vertice[8];
public:
	UPROPERTY(EditInstanceOnly)
	FVector radiusVector;
private:
	void SetVertice()
	{
		vertice[0] = centerL - radiusVector;
		vertice[1] = centerL + FVector(-radiusVector.X, -radiusVector.Y,  radiusVector.Z);
		vertice[2] = centerL + FVector(-radiusVector.X,  radiusVector.Y, -radiusVector.Z);
		vertice[3] = centerL + FVector(-radiusVector.X,  radiusVector.Y,  radiusVector.Z);
		vertice[4] = centerL + FVector( radiusVector.X, -radiusVector.Y, -radiusVector.Z);
		vertice[5] = centerL + FVector( radiusVector.X, -radiusVector.Y,  radiusVector.Z);
		vertice[6] = centerL + FVector( radiusVector.X,  radiusVector.Y, -radiusVector.Z);
		vertice[7] = centerL + radiusVector;

		for (auto & vertex : vertice)
			vertex = root->ComponentToWorld.TransformPosition(vertex);
	};
public:
	virtual void Translate(const FVector & v) override 
	{ 
		UCollisionComponent::Translate(v); 
		for (auto & vertex : vertice)
			vertex += v;
	};

	virtual void Rotate(const FVector & v) override
	{
		UCollisionComponent::Rotate(v);
		SetVertice();
	};

	virtual bool SphereCollisionDetect(const USphereCollisionComponent * pSCC) const override;
	virtual bool BoxCollisionDetect(const UBoxCollisionComponent * pBCC) const override;

	virtual bool CollisionDetect(const UCollisionComponent * pCC) const override { return pCC->BoxCollisionDetect(this); };

	friend static void CollisionContactBoxs(const UBoxCollisionComponent *, const UBoxCollisionComponent *);
};

FVector ClosestPointOnBox(const FVector & v, const UBoxCollisionComponent * pBCC);
