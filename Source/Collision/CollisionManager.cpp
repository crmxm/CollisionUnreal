// Fill out your copyright notice in the Description page of Project Settings.

#include "Collision.h"
#include "CollisionManager.h"

#include "ContactManager.h"

const float MAX_FRAME_TIME = 1.0f / 30.0f;

CollisionManager * CollisionManager::instance = nullptr;
unsigned int CollisionManager::createCount = 0;

void CollisionManager::Update(float time)
{
	if (!isWorking)
		return;

	time = (time > MAX_FRAME_TIME) ? MAX_FRAME_TIME : time;

	PreCollisionUpdate(time);
	for (unsigned int i = 0; i < nonStaticSize; i++)
		for (unsigned int j = i + 1; j < size; j++)
		{
			bool b = obj[i]->CollisionDetect(obj[j]);
			obj[i]->isCollided |= b;
			obj[j]->isCollided |= b;
		}

	ContactManager::instance->Resolve();
	PostCollisionDetect();
}
