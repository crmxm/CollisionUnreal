// Fill out your copyright notice in the Description page of Project Settings.

#include "Collision.h"
#include "BoxCollisionComponent.h"
#include "SphereCollisionComponent.h"
#include "ContactManager.h"
#include "CollisionManager.h"
#include "ForceFieldTriggerComponent.h"

void UBoxCollisionComponent::BeginPlay()
{
	UCollisionComponent::BeginPlay();

	massInv = (mass == 0) ? 0 : 1.0f / mass;
	float temp = radiusVector.SizeSquared();
	inertia.M[0][0] = mass * (temp - Sqr(radiusVector.X)) / 3;
	inertia.M[1][1] = mass * (temp - Sqr(radiusVector.Y)) / 3;
	inertia.M[2][2] = mass * (temp - Sqr(radiusVector.Z)) / 3;
	inertia.M[3][3] = 1;
	if (mass != 0)
		inertiaInv = inertia.Inverse();
	else
		inertiaInv = inertia;

	//center = root->ComponentToWorld.TransformPosition(centerL);
	//SetVertice();
}

bool UBoxCollisionComponent::SphereCollisionDetect(const USphereCollisionComponent * pSCC) const
{
	FVector point = ClosestPointOnBox(pSCC->center, this);
	FVector dist = point - pSCC->center;

	if (dist.SizeSquared() > Sqr(pSCC->radius))
		return false;

	isCollided = pSCC->isCollided = true;

	if (childSize)
	{
		bool b = false;
		for (unsigned int i = 0; i < childSize; i++)
			b |= childs[i]->SphereCollisionDetect(pSCC);

		return b;
	}

	if (pSCC->childSize)
	{
		bool b = false;
		for (unsigned int i = 0; i < pSCC->childSize; i++)
			b |= pSCC->childs[i]->BoxCollisionDetect(this);

		return b;
	}

	float f = dist.Size();
	dist.Normalize();

	ContactManager::instance->AddContact((UCollisionComponent *) this, (UCollisionComponent *)pSCC,
		pSCC->radius - f, dist, point);
	return true;
}

bool CollisionTestBoxs(const UBoxCollisionComponent * pBCC1, const UBoxCollisionComponent * pBCC2)
{
	FTransform trans1 = pBCC1->GetComponentToWorld();
	FTransform trans2 = pBCC2->GetComponentToWorld();

	FVector rx = trans2.InverseTransformVector(trans1.GetUnitAxis(EAxis::X));
	FVector ry = trans2.InverseTransformVector(trans1.GetUnitAxis(EAxis::Y));
	FVector rz = trans2.InverseTransformVector(trans1.GetUnitAxis(EAxis::Z));
	FVector arx = rx.GetAbs();
	FVector ary = ry.GetAbs();
	FVector arz = rz.GetAbs();

	//TODO: simplify
	//FVector dist = pBCC2->center - pBCC1->center;
	FVector d = trans1.InverseTransformPosition(pBCC2->center);

	FVector radius1 = pBCC1->radiusVector;
	FVector radius2 = pBCC2->radiusVector;

	if (fabs(d.X) > radius1.X + FVector::DotProduct(arx, radius2)) return false;
	if (fabs(d.Y) > radius1.Y + FVector::DotProduct(ary, radius2)) return false;
	if (fabs(d.Z) > radius1.Z + FVector::DotProduct(arz, radius2)) return false;

	if (fabs(d.X*rx.X + d.Y*ry.X + d.Z*rz.X)>radius1.X*arx.X + radius1.Y*ary.X + radius1.Z*arz.X + radius2.X) return false;
	if (fabs(d.X*rx.Y + d.Y*ry.Y + d.Z*rz.Y)>radius1.X*arx.Y + radius1.Y*ary.Y + radius1.Z*arz.Y + radius2.Y) return false;
	if (fabs(d.X*rx.Z + d.Y*ry.Z + d.Z*rz.Z)>radius1.X*arx.Z + radius1.Y*ary.Z + radius1.Z*arz.Z + radius2.Z) return false;

	if (fabs(d.Z*ry.X - d.Y*rz.X)>radius1.Y*arz.X + radius1.Z*ary.X + radius2.Y*arx.Z + radius2.Z*arx.Y) return false;
	if (fabs(d.Z*ry.Y - d.Y*rz.Y)>radius1.Y*arz.Y + radius1.Z*ary.Y + radius2.X*arx.Z + radius2.Z*arx.X) return false;
	if (fabs(d.Z*ry.Z - d.Y*rz.Z)>radius1.Y*arz.Z + radius1.Z*ary.Z + radius2.X*arx.Y + radius2.Y*arx.X) return false;

	if (fabs(d.X*rz.X - d.Z*rx.X)>radius1.X*arz.X + radius1.Z*arx.X + radius2.Y*ary.Z + radius2.Z*ary.Y) return false;
	if (fabs(d.X*rz.Y - d.Z*rx.Y)>radius1.X*arz.Y + radius1.Z*arx.Y + radius2.X*ary.Z + radius2.Z*ary.X) return false;
	if (fabs(d.X*rz.Z - d.Z*rx.Z)>radius1.X*arz.Z + radius1.Z*arx.Z + radius2.X*ary.Y + radius2.Y*ary.X) return false;

	if (fabs(d.Y*rx.X - d.X*ry.X)>radius1.X*ary.X + radius1.Y*arx.X + radius2.Y*arz.Z + radius2.Z*arz.Y) return false;
	if (fabs(d.Y*rx.Y - d.X*ry.Y)>radius1.X*ary.Y + radius1.Y*arx.Y + radius2.X*arz.Z + radius2.Z*arz.X) return false;
	if (fabs(d.Y*rx.Z - d.X*ry.Z)>radius1.X*ary.Z + radius1.Y*arx.Z + radius2.X*arz.Y + radius2.Y*arz.X) return false;

	return true;
}

static inline int MinIndex(FVector & v)
{
	return (v.X <= v.Y) ? ((v.X <= v.Z) ? 0 : 2) : ((v.Y <= v.Z) ? 1 : 2);
}

struct LineSegment
{
	FVector start;
	FVector dir;
	float length;
};

static float ClosestPointOnSegment(const LineSegment & l1, const LineSegment & l2, FVector & pointOn1, FVector & pointOn2) 
{
	float s, t;
	FVector r = l1.start - l2.start;
	float b = FVector::DotProduct(l1.dir, l2.dir);
	float c = FVector::DotProduct(l1.dir, r);
	float f = FVector::DotProduct(l2.dir, r);
	float denom = 1 - Sqr(b);
	if (denom != 0)
		s = clamp((b * f - c) / denom, 0, l1.length);
	else
		s = 0;
	
	t = b * s + f;

	if (t < 0)
	{
		t = 0;
		s = clamp(-c, 0, l1.length);
	}
	else if (t > l2.length)
	{
		t = l2.length;
		s = clamp(b - c, 0, l1.length);
	}

	pointOn1 = l1.start + s * l1.dir;
	pointOn2 = l2.start + t * l2.dir;

	return (pointOn2 - pointOn1).SizeSquared();
};

static inline bool TestPointInBox(const FVector & v, const UBoxCollisionComponent * pBCC)
{
	FVector R = pBCC->ComponentToWorld.InverseTransformPosition(v);
	FVector D = pBCC->radiusVector - R.GetAbs();

	return D.GetMin() > 0;
}

static void CollisionContactBoxs(const UBoxCollisionComponent * pBCC1, const UBoxCollisionComponent * pBCC2)
{
	float distVertexSurface = pBCC1->radiusVector.Size();
	bool isVertexSurface = false;
	FVector vVertexSurface;
	FVector nVectexSurface;
	bool isNormalPointTo2 = false;

	FTransform trans1 = pBCC1->GetComponentToWorld();
	FTransform trans2 = pBCC2->GetComponentToWorld();

	float temp;

	for (auto & vertex : pBCC1->vertice)
	{
		FVector R = trans2.InverseTransformPosition(vertex);
		FVector D = pBCC2->radiusVector - R.GetAbs();

		int index = MinIndex(D);
		temp = D[index];

		if (temp > 0 && temp < distVertexSurface)
		{
			isVertexSurface = true;
			distVertexSurface = temp;
			isNormalPointTo2 = true;
			vVertexSurface = vertex;
			nVectexSurface = trans2.GetUnitAxis(EAxis::Type(index + 1)) * ((R[index] >= 0) ? 1.0f : -1.0f);
		}
	}

	for (auto & vertex : pBCC2->vertice)
	{
		FVector R = trans1.InverseTransformPosition(vertex);
		FVector D = pBCC1->radiusVector - R.GetAbs();

		int index = MinIndex(D);
		temp = D[index];

		if (temp > 0 && temp < distVertexSurface)
		{
			isVertexSurface = true;
			distVertexSurface = temp;
			isNormalPointTo2 = false;
			vVertexSurface = vertex;
			nVectexSurface = trans1.GetUnitAxis(EAxis::Type(index + 1)) * ((R[index] >= 0) ? 1.0f : -1.0f);
		}
	}

	float distEdgesSqr = 4 * pBCC1->radiusVector.SizeSquared();
	FVector v1, v2;

	FVector pointOn1, pointOn2;

	unsigned int EdgeBegLUT[12] = { 0, 1, 2, 3, 0, 1, 4, 5, 0, 2, 4, 6 };
	for (unsigned int i = 0; i < 12; i++)
	{
		unsigned int mode1 = 2 - i / 4;
		LineSegment line1({ pBCC1->vertice[EdgeBegLUT[i]], trans1.GetUnitAxis(EAxis::Type(mode1)), 2 * pBCC1->radiusVector[mode1] });
		for (unsigned int j = 0; j < 12; j++)
		{
			unsigned int mode2 = 2 - j / 4;
			LineSegment line2({ pBCC2->vertice[EdgeBegLUT[j]], trans2.GetUnitAxis(EAxis::Type(mode2)), 2 * pBCC2->radiusVector[mode2] });

			temp = ClosestPointOnSegment(line1, line2, pointOn1, pointOn2);
			if (TestPointInBox(pointOn2, pBCC1) && TestPointInBox(pointOn1, pBCC2))
			{
				if (temp < distEdgesSqr)
				{
					distEdgesSqr = temp;
					v1 = pointOn1;
					v2 = pointOn2;
				}
			}
		}
	}

	FVector nEdges = v2 - v1;
	float distEdges = nEdges.Size();
	nEdges.Normalize();

	if (isVertexSurface && distVertexSurface > distEdges)
		ContactManager::instance->AddContact((UCollisionComponent *)((isNormalPointTo2) ? pBCC1 : pBCC2),
		(UCollisionComponent *)((isNormalPointTo2) ? pBCC2 : pBCC1), distVertexSurface, nVectexSurface, vVertexSurface);
	else
		ContactManager::instance->AddContact((UCollisionComponent *)pBCC1, (UCollisionComponent *)pBCC2,
			distEdges, nEdges, (v1 + v2) / 2);
	return;
}

bool UBoxCollisionComponent::BoxCollisionDetect(const UBoxCollisionComponent * pBCC) const
{
	if (!CollisionTestBoxs(this, pBCC))
		return false;
	
	isCollided = pBCC->isCollided = true;

	if (childSize)
	{
		bool b = false;
		for (unsigned int i = 0; i < childSize; i++)
			b |= childs[i]->BoxCollisionDetect(pBCC);

		return b;
	}

	if (pBCC->childSize)
	{
		bool b = false;
		for (unsigned int i = 0; i < pBCC->childSize; i++)
			b |= pBCC->childs[i]->BoxCollisionDetect(this);

		return b;
	}

	CollisionContactBoxs(this, pBCC);

	return true;
}

void UBoxCollisionComponent::DrawCollider() const
{
	const unsigned int EdgeVertexLUT[12][2] = { 
		{0, 1}, {2, 3}, {4, 5}, {6, 7}, 
		{0, 2}, {1, 3}, {4, 6}, {5, 7}, 
		{0, 4}, {1, 5}, {2, 6}, {3, 7} 
	};
	
	for (auto pair : EdgeVertexLUT)
		DrawDebugLine(GetWorld(), vertice[pair[0]], vertice[pair[1]], (isCollided) ? FColor::Red :FColor::Green);
}

void UBoxCollisionComponent::ApplyForce(float t, const UForceFieldTriggerComponent ** force, unsigned int size)
{
	FVector a(0, 0, 0);
	FVector w(0, 0, 0);

	for (unsigned int i = 0; i < size; i++)
	{
		auto f = force[i];
		if (f->BoxCollisionDetect(this))
		{
			a += f->GetAcceleration(velocity);
			w += f->GetAngularA(angularV, radiusVector.SizeSquared());
		}
	}

	velocity += a * t;
	angularV += inertiaInv.GetScaleVector() * w * mass * t;

	UCollisionComponent::ApplyForce(t, force, size);
}

FVector ClosestPointOnBox(const FVector & v, const UBoxCollisionComponent * pBCC)
{
	FVector vL = pBCC->ComponentToWorld.InverseTransformPosition(v);
	FVector radius = pBCC->radiusVector;

	vL.X = clamp(vL.X, -radius.X, radius.X);
	vL.Y = clamp(vL.Y, -radius.Y, radius.Y);
	vL.Z = clamp(vL.Z, -radius.Z, radius.Z);

	return pBCC->ComponentToWorld.TransformPosition(vL);
}
