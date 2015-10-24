﻿//
//  File: DKStaticTriangleMeshShape.cpp
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2012-2015 Hongtae Kim. All rights reserved.
//

#include "Private/BulletUtils.h"
#include "DKStaticTriangleMeshShape.h"

using namespace DKFoundation;
using namespace DKFramework;
using namespace DKFramework::Private;

struct DKStaticTriangleMeshShape::IndexedTriangleData : public btStridingMeshInterface
{
	int numTriangles;
	void* vertices;
	size_t numVertices;
	void* indices;
	size_t numIndices;
	PHY_ScalarType indexType;

	~IndexedTriangleData(void)
	{
		if (vertices)
			free(vertices);
		if (indices)
			free(indices);
	}

	template <typename IndexType>
	IndexedTriangleData(const DKVector3* vertices,
						size_t numVertices,
						const IndexType* indices,
						size_t numIndices,
						const DKAabb& aabb)
		: vertices(NULL)
		, numVertices(0)
		, indices(NULL)
		, numIndices(0)
		, indexType(PHY_INTEGER)
		, aabbMin(BulletVector3(aabb.positionMin))
		, aabbMax(BulletVector3(aabb.positionMax))
	{
		static_assert(sizeof(IndexType) == 2 || sizeof(IndexType) == 4, "index size must be 2 or 4");

		this->numTriangles = (int)(numIndices / 3);
		if (numIndices % 3)
			numIndices -= numIndices % 3;

		if (this->numTriangles == 0 || vertices == NULL || numVertices == 0 || indices == 0)
			return;


		this->numVertices = numVertices;
		this->numIndices = numIndices;

		this->vertices = malloc(numVertices * sizeof(DKVector3));
		memcpy(this->vertices, vertices, numVertices * sizeof(DKVector3));

		if (sizeof(IndexType) == 4 && numVertices <= 0xffff)
		{
			this->indexType = PHY_SHORT;

			this->indices = malloc(numIndices * sizeof(unsigned short));
			for (size_t i = 0; i < numIndices; ++i)
				reinterpret_cast<unsigned short*>(this->indices)[i] = static_cast<unsigned short>(indices[i]);
		}
		else
		{
			this->indexType = (sizeof(IndexType) == 4) ? PHY_INTEGER : PHY_SHORT;

			this->indices = malloc(numIndices * sizeof(IndexType));
			memcpy(this->indices, indices, numIndices * sizeof(IndexType));
		}

		if (this->aabbMax.x() < this->aabbMin.x() || this->aabbMax.y() < this->aabbMin.y() || this->aabbMax.z() < this->aabbMin.z())
			this->calculateAabbBruteForce(this->aabbMin, this->aabbMax);
	}

	// override from btStridingMeshInterface
	void getLockedVertexIndexBase(unsigned char **vertexbase, int& numverts, PHY_ScalarType& type, int& stride, unsigned char **indexbase, int & indexstride, int& numfaces, PHY_ScalarType& indicestype, int subpart) override
	{
		return getLockedReadOnlyVertexIndexBase((const unsigned char**)vertexbase, numverts, type, stride, (const unsigned char**)indexbase, indexstride, numfaces, indicestype, subpart);
	}
	void getLockedReadOnlyVertexIndexBase(const unsigned char **vertexbase, int& numverts, PHY_ScalarType& type, int& stride, const unsigned char **indexbase, int & indexstride, int& numfaces, PHY_ScalarType& indicestype, int subpart) const override
	{
		DKASSERT_DEBUG(subpart == 0);
		(*vertexbase) = (const unsigned char*)this->vertices;
		numverts = (int)this->numVertices;
		type = PHY_FLOAT;
		stride = sizeof(DKVector3);
		numfaces = this->numTriangles;
		(*indexbase) = (unsigned char*)this->indices;
		if (this->indexType == PHY_INTEGER)
			indexstride = 3 * sizeof(unsigned int);
		else
			indexstride = 3 * sizeof(unsigned short);
		indicestype = this->indexType;
	}
	void unLockVertexBase(int subpart) override {}
	void unLockReadOnlyVertexBase(int subpart) const override {}
	int	getNumSubParts() const override { return 1; }
	void preallocateVertices(int numverts) override {}
	void preallocateIndices(int numindices) override {}
	bool hasPremadeAabb() const override { return true; }
	void setPremadeAabb(const btVector3& aabbMin, const btVector3& aabbMax) const override
	{
		this->aabbMin = aabbMin;
		this->aabbMax = aabbMax;
	}
	void getPremadeAabb(btVector3* aabbMin, btVector3* aabbMax) const override
	{
		*aabbMin = this->aabbMin;
		*aabbMax = this->aabbMax;
	}

	mutable btVector3 aabbMin;
	mutable btVector3 aabbMax;
};


DKStaticTriangleMeshShape::DKStaticTriangleMeshShape(
	const DKVector3* verts, size_t numVertices,
	const unsigned int* indices, size_t numIndices,
	const DKAabb& precalculatedAabb)
	: DKStaticTriangleMeshShape(new IndexedTriangleData(verts, numVertices, indices, numIndices, precalculatedAabb))
{
}

DKStaticTriangleMeshShape::DKStaticTriangleMeshShape(
	const DKVector3* verts, size_t numVertices,
	const unsigned short* indices, size_t numIndices,
	const DKAabb& precalculatedAabb)
	: DKStaticTriangleMeshShape(new IndexedTriangleData(verts, numVertices, indices, numIndices, precalculatedAabb))
{
}

DKStaticTriangleMeshShape::DKStaticTriangleMeshShape(IndexedTriangleData* data)
	: DKConcaveShape(ShapeType::StaticTriangleMesh, new btBvhTriangleMeshShape(data, true, true))
	, meshData(data)
{
}

DKStaticTriangleMeshShape::~DKStaticTriangleMeshShape(void)
{
	delete meshData;
}

size_t DKStaticTriangleMeshShape::NumberOfVertices(void) const
{
	return this->meshData->numVertices;
}

size_t DKStaticTriangleMeshShape::NumberOfIndices(void) const
{
	return this->meshData->numIndices;
}

size_t DKStaticTriangleMeshShape::IndexSize(void) const
{
	return (this->meshData->indexType == PHY_INTEGER) ? 4 : 2;
}

size_t DKStaticTriangleMeshShape::NumberOfTriangles(void) const
{
	return this->meshData->numTriangles;
}

const DKVector3& DKStaticTriangleMeshShape::VertexAtIndex(int index) const
{
	return reinterpret_cast<DKVector3*>(this->meshData->vertices)[index];
}

DKTriangle DKStaticTriangleMeshShape::TriangleAtIndex(int index) const
{
	size_t numTriangles = this->meshData->numTriangles;
	DKASSERT_DEBUG(index >= 0 && index < numTriangles);

	DKTriangle triangle;
	if (this->meshData->indexType == PHY_INTEGER)
	{
		unsigned int* idx = &reinterpret_cast<unsigned int*>(this->meshData->indices)[index * 3];
		triangle.position1 = reinterpret_cast<DKVector3*>(this->meshData->vertices)[idx[0]];
		triangle.position2 = reinterpret_cast<DKVector3*>(this->meshData->vertices)[idx[0]];
		triangle.position3 = reinterpret_cast<DKVector3*>(this->meshData->vertices)[idx[0]];
	}
	else
	{
		unsigned short* idx = &reinterpret_cast<unsigned short*>(this->meshData->indices)[index * 3];
		triangle.position1 = reinterpret_cast<DKVector3*>(this->meshData->vertices)[idx[0]];
		triangle.position2 = reinterpret_cast<DKVector3*>(this->meshData->vertices)[idx[0]];
		triangle.position3 = reinterpret_cast<DKVector3*>(this->meshData->vertices)[idx[0]];
	}
	return triangle;
}

DKAabb DKStaticTriangleMeshShape::Aabb(void) const
{
	return DKAabb(BulletVector3(this->meshData->aabbMin), BulletVector3(this->meshData->aabbMax));
}

const DKVector3* DKStaticTriangleMeshShape::VertexData(void) const
{
	return (DKVector3*)this->meshData->vertices;
}

const void* DKStaticTriangleMeshShape::IndexData(void) const
{
	return this->meshData->indices;
}
