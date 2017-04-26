// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <cassert>

#include "CollisionComponent.h"

const unsigned int MAX_COLLISION_OBJ = 256;
const unsigned int MAX_FORCE = 4;


/**
 * 
 */
class COLLISION_API CollisionManager final
{
public:
	static CollisionManager * instance;
	static unsigned int createCount;
	static void Create() { createCount++; if (!instance) instance = new CollisionManager; };
	static void Destroy() { if (!instance) return; createCount--; if (createCount > 0) return; delete instance; instance = nullptr; };
private:
	UCollisionComponent * obj[MAX_COLLISION_OBJ];
	UForceFieldTriggerComponent * force[MAX_FORCE];

	unsigned int size;
	unsigned int nonStaticSize;
	unsigned int forceSize;

	bool isWorking;

private:
	CollisionManager() : size(0), nonStaticSize(0), isWorking(false), forceSize(0) {};
public:
	~CollisionManager() {};

	void Update(float time);
	void SetWorking(bool b) { isWorking = b; };
	void AddObj(UCollisionComponent * pCO) 
	{
		obj[size++] = (pCO->isStatic) ? pCO : obj[nonStaticSize];
		if (!pCO->isStatic)
			obj[nonStaticSize++] = pCO;
	};
	void AddForce(UForceFieldTriggerComponent * pFFTC)
	{
		force[forceSize++] = pFFTC;
	};

	void RemoveObj(UCollisionComponent * pCO)
	{
		unsigned int index;

		if (pCO->isStatic)
		{
			for (index = nonStaticSize; index < size; index++)
				if (obj[index] == pCO)
					break;

			assert(index < size);
			obj[index] = obj[--size];
		}
		else
		{
			for (index = 0; index < nonStaticSize; index++)
				if (obj[index] == pCO)
					break;

			assert(index < nonStaticSize);
			obj[index] = obj[--nonStaticSize];
			obj[nonStaticSize] = obj[--size];
		}
	};
	void RemoveForce(UForceFieldTriggerComponent * pFFTC)
	{
		unsigned int index;
		for (index = 0; index < forceSize; index++)
			if (force[index] == pFFTC)
				break;

		assert(index < forceSize);

		force[index] = force[--forceSize];
	};

private:
	void PreCollisionUpdate(float time)
	{
		for (unsigned int i = 0; i < nonStaticSize; i++)
		{
			obj[i]->ApplyForce(time, (const UForceFieldTriggerComponent **) &force[0], forceSize);
			obj[i]->Rotate(time);
			obj[i]->Translate(time);
			obj[i]->time = 0;
		}
	};

	void PostCollisionDetect() {
		for (unsigned int i = 0; i < nonStaticSize; i++)
		{
			obj[i]->Translate(-obj[i]->time);
			obj[i]->Rotate(-obj[i]->time);
			obj[i]->UpdateVelocity();
			obj[i]->UpdateAngularV();
		}
	};
	friend class ACollisionManagerActor;
};
