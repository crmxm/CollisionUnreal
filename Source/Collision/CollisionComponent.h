// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"

#include "Vector.h"

#include "CollisionComponent.generated.h"

class USphereCollisionComponent;
class UBoxCollisionComponent;

//UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
UCLASS( )
class COLLISION_API UCollisionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCollisionComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type) override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

protected:
	FVector centerL;
	FVector impulse;
	FVector angularI;

public:
	UPROPERTY(EditInstanceOnly)
	bool isStatic;
	float time;
	UPROPERTY(EditInstanceOnly)
	float mass;
	float massInv;
	FMatrix inertia, inertiaInv;

	FVector center;
	UPROPERTY(EditInstanceOnly)
	FVector velocity;
	UPROPERTY(EditInstanceOnly)
	FVector angularV;

	UPROPERTY(EditInstanceOnly)
	float fCoeff;
	UPROPERTY(EditInstanceOnly)
	float rCoeff;

	USceneComponent * root;

protected:
	virtual void Translate(const FVector & v) 
	{ 
		center += v; 
		FQuat T = root->ComponentToWorld.GetRotation();
		FQuat B = root->RelativeRotation.Quaternion();
		FQuat AInv = B * T.Inverse();
		FVector temp = AInv.RotateVector(v);

		root->RelativeLocation += temp;
		root->UpdateComponentToWorld(); 
	};

	virtual void Rotate(const FVector & v) 
	{
		float angle = v.Size();

		FVector axis = v;
		axis.Normalize();

		FQuat R(axis, angle);
		FQuat T = root->ComponentToWorld.GetRotation();
		FQuat B = root->RelativeRotation.Quaternion();
		FQuat AInv = B * T.Inverse();
		FQuat Bp = AInv * R * T;
		root->RelativeRotation = Bp.Rotator();

		root->UpdateComponentToWorld();
	};


public:
	inline void SetImpulse(const FVector & v) { impulse += v; };
	inline void SetAngularI(const FVector & v) { angularI += v; };
	inline void Translate(float t) { Translate(velocity * t); };
	inline void Rotate(float t) { Rotate(angularV * t); };
	inline void UpdateVelocity() { velocity += impulse * massInv; impulse.Set(0, 0, 0); };
	inline void UpdateAngularV() { angularV += inertiaInv.TransformVector(angularI); angularI.Set(0, 0, 0); };

	virtual bool SphereCollisionDetect(const USphereCollisionComponent *) const { return false; };
	virtual bool BoxCollisionDetect(const UBoxCollisionComponent *) const { return false; };
	virtual bool CollisionDetect(const UCollisionComponent * pPO) const { return false; };
	virtual void ApplyForce(float t) {};

	friend class ACollisionManagerActor;
};
