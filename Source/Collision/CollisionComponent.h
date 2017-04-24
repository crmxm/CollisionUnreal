// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"

#include "Vector.h"

#include "CollisionComponent.generated.h"
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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

private:

	//MoI, MoIInv;
	FVector centerL;
	FVector impulse;
	FVector angularI;

public:
	bool isStatic;
	float time;
	float mass, massInv;

	FVector center;
	FVector velocity;
	FVector angularV;

	float fCoeff, rCoeff;

private:
	virtual void Translate(const FVector & v) { this->GetAttachmentRoot()->RelativeLocation += this->GetAttachmentRoot()->ComponentToWorld.InverseTransformVector(v); center += v; };
	virtual void Rotate(const FVector & v) 
	{
		float angle = v.Size();

		FVector axis = v;
		axis.Normalize();

		auto a = GetAttachmentRoot();

		FQuat R(axis, angle);
		FQuat T = GetAttachmentRoot()->ComponentToWorld.GetRotation();
		FQuat B = GetAttachmentRoot()->RelativeRotation.Quaternion();
		FQuat A = T * B.Inverse();
		FQuat AInv = B * T.Inverse();
		FQuat Bp = AInv * R * T;
		GetAttachmentRoot()->RelativeRotation = Bp.Rotator();

		a->UpdateComponentToWorld();

	};//TODO
public:
	inline void SetImpulse(const FVector & v) { impulse += v; };
	inline void SetAngularI(const FVector & v) { angularI += v; };
	inline void Translate(float time) { Translate(velocity * time); };
	inline void Rotate(float time) { Rotate(angularV * time); };
	inline void UpdateVelocity() { velocity += impulse * massInv; impulse.Set(0, 0, 0); };

	virtual bool CollisionDetect(const UCollisionComponent * pPO) { return false; };
	virtual void ApplyForce(float time) {};

	friend class ACollisionManagerActor;
};
