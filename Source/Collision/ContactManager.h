// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <queue>
#include "Vector.h"

const float EPSILON = 0.001f;

/**
 * 
 */
class UCollisionComponent;

struct Contact
{
	float time;
	float nSpeed;

	UCollisionComponent * pCO1;
	UCollisionComponent * pCO2;

	FVector normal, kA, kB, contactPoint;

};

class COLLISION_API ContactManager
{
public:
	static ContactManager * instance;
	static unsigned int createCount;
	static void Create() { createCount++; if (!instance) instance = new ContactManager; };
	static void Destroy() { if (!instance) return; createCount--; if (createCount > 0) return; delete instance; instance = nullptr; };

private:
	struct Comp
	{
		bool operator() (const Contact & left, const Contact & right) { return left.time > right.time; };
	};

	std::priority_queue<Contact, std::vector<Contact>, Comp> contacts;

private:
	ContactManager() {};
public:
	~ContactManager() {};

	void AddContact(UCollisionComponent * pCO1, UCollisionComponent * pCO2, float d, const FVector & normal, const FVector & contactPoint);

	void AddContact(Contact && val) { contacts.push(val); };

	void ResolveTop();
	void Resolve();
};
