// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"

#include "Vector.h"

#include "CollisionComponent.generated.h"

const unsigned int MAX_CHILD_NUM = 16;
const float vThresholdSqr = 1e-4f;
const float wThresholdSqr = 1e-8f;

class USphereCollisionComponent;
class UBoxCollisionComponent;
class UForceFieldTriggerComponent;

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

	virtual void OnUpdateTransform(EUpdateTransformFlags flags, ETeleportType teleport) override;

protected:
	FVector impulse;
	FVector angularIL;

public:
	UPROPERTY(EditInstanceOnly)
	bool isStatic;
	UPROPERTY(EditInstanceOnly)
	bool isTrigger;

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
	float rCoeff;

	unsigned int childSize = 0;
	UPROPERTY(EditInstanceOnly)
	bool rooted = true;

	UCollisionComponent * childs[MAX_CHILD_NUM];

	UCollisionComponent * rootCC = this;
	USceneComponent * root = nullptr;

protected:
	virtual void Translate(const FVector & v) 
	{ 
		if (!rooted)
			return;

		FQuat T = root->ComponentToWorld.GetRotation();
		FQuat B = root->RelativeRotation.Quaternion();
		FQuat AInv = B * T.Inverse();
		FVector temp = AInv.RotateVector(v);

		root->RelativeLocation += temp;
		root->UpdateComponentToWorld(); 
	};

	virtual void Rotate(const FVector & v) 
	{
		if (!rooted)
			return;

		float angle = v.Size();

		FVector axis = ComponentToWorld.InverseTransformVector(v);
		axis.Normalize();

		FQuat R(axis, angle);
		//FQuat T = root->ComponentToWorld.GetRotation();
		FQuat B = root->RelativeRotation.Quaternion();
		//FQuat AInv = B * T.Inverse();
		//FQuat Bp = AInv * R * T;
		FQuat C = RelativeRotation.Quaternion();
		FQuat Bp = B * C * R * C.Inverse();
		FVector vC = RelativeLocation;
		root->RelativeRotation = Bp.Rotator();
		root->RelativeLocation += B.RotateVector(vC) - Bp.RotateVector(vC);

		root->UpdateComponentToWorld();
	};

	void SetChilds(UCollisionComponent * rootCC);

public:
	inline void SetImpulse(const FVector & v) { impulse += v; };
	inline void SetAngularI(const FVector & v) { angularIL += v; };
	inline void Translate(float t) { Translate(velocity * t); };
	inline void Rotate(float t) { Rotate(angularV * t); };
	inline void UpdateVelocity() { velocity += impulse * massInv; impulse.Set(0, 0, 0); };
	inline void UpdateAngularV() { angularV += ComponentToWorld.TransformVector(inertiaInv.TransformVector(angularIL)); angularIL.Set(0, 0, 0); };

	virtual bool SphereCollisionDetect(const USphereCollisionComponent *) const { return false; };
	virtual bool BoxCollisionDetect(const UBoxCollisionComponent *) const { return false; };
	virtual bool CollisionDetect(const UCollisionComponent * pPO) const { return false; };
	virtual void ApplyForce(float t, const UForceFieldTriggerComponent ** force, unsigned int size) 
	{ 
		if (velocity.SizeSquared() < vThresholdSqr)
			velocity.Set(0, 0, 0);
		if (angularV.SizeSquared() < wThresholdSqr)
			angularV.Set(0, 0, 0);
	};
	virtual void UpdateTransform() { center = ComponentToWorld.GetLocation(); };
	virtual void DrawCollider() const {};

	friend class ACollisionManagerActor;
};
