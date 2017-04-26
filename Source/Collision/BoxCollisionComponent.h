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
		vertice[0] = -radiusVector;
		vertice[1] = FVector(-radiusVector.X, -radiusVector.Y,  radiusVector.Z);
		vertice[2] = FVector(-radiusVector.X,  radiusVector.Y, -radiusVector.Z);
		vertice[3] = FVector(-radiusVector.X,  radiusVector.Y,  radiusVector.Z);
		vertice[4] = FVector( radiusVector.X, -radiusVector.Y, -radiusVector.Z);
		vertice[5] = FVector( radiusVector.X, -radiusVector.Y,  radiusVector.Z);
		vertice[6] = FVector( radiusVector.X,  radiusVector.Y, -radiusVector.Z);
		vertice[7] = radiusVector;

		for (auto & vertex : vertice)
			vertex = ComponentToWorld.TransformPosition(vertex);
	};
public:
	virtual bool SphereCollisionDetect(const USphereCollisionComponent * pSCC) const override;
	virtual bool BoxCollisionDetect(const UBoxCollisionComponent * pBCC) const override;

	virtual bool CollisionDetect(const UCollisionComponent * pCC) const override { return pCC->BoxCollisionDetect(this); };

	virtual void UpdateTransform() override
	{
		UCollisionComponent::UpdateTransform();
		SetVertice();
	}

	virtual void DrawCollider() const override;

	virtual void ApplyForce(float t, const UForceFieldTriggerComponent ** force, unsigned int size) override;

	friend static void CollisionContactBoxs(const UBoxCollisionComponent *, const UBoxCollisionComponent *);
};

bool CollisionTestBoxs(const UBoxCollisionComponent * pBCC1, const UBoxCollisionComponent * pBCC2);
FVector ClosestPointOnBox(const FVector & v, const UBoxCollisionComponent * pBCC);
