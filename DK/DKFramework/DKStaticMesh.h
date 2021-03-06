﻿//
//  File: DKStaticMesh.h
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2004-2016 Hongtae Kim. All rights reserved.
//

#pragma once
#include "../DKInclude.h"
#include "../DKFoundation.h"
#include "DKMesh.h"
#include "DKVertexBuffer.h"
#include "DKIndexBuffer.h"

////////////////////////////////////////////////////////////////////////////////
// DKStaticMesh
// a mesh object which have static-buffer data
// this class can have multiple vertex buffers.
// and buffers can be separated or interleaved.
//
////////////////////////////////////////////////////////////////////////////////

namespace DKFramework
{
	class DKGL_API DKStaticMesh : public DKMesh
	{
	public:
		struct StreamInfo
		{
			const DKVertexBuffer::Decl* decl;
			const DKVertexBuffer* buffer;
			size_t offset;				// stream offset of vertex buffer.
		};

		DKStaticMesh(void);
		~DKStaticMesh(void);

		bool AddVertexBuffer(DKVertexBuffer* buffer);
		size_t NumberOfVertexBuffers(void) const;
		DKVertexBuffer* VertexBufferAtIndex(unsigned int index);
		const DKVertexBuffer* VertexBufferAtIndex(unsigned int index) const;
		void RemoveVertexBuffer(const DKVertexBuffer* buffer);
		void RemoveAllVertexBuffers(void);

		void SetIndexBuffer(DKIndexBuffer* buffer);
		DKIndexBuffer* IndexBuffer(void);
		const DKIndexBuffer* IndexBuffer(void) const;

		// stream
		virtual StreamInfo FindVertexStream(DKVertexStream::Stream stream) const;
		virtual StreamInfo FindVertexStream(const DKFoundation::DKString& name) const;
		virtual StreamInfo FindVertexStream(DKVertexStream::Stream stream, const DKFoundation::DKString& name) const;

		bool MakeInterleaved(DKVertexBuffer::MemoryLocation location = DKVertexBuffer::MemoryLocationStatic, DKVertexBuffer::BufferUsage usage = DKVertexBuffer::BufferUsageDraw);
		bool MakeSeparated(DKVertexBuffer::MemoryLocation location = DKVertexBuffer::MemoryLocationStatic, DKVertexBuffer::BufferUsage usage = DKVertexBuffer::BufferUsageDraw);
		bool UpdateStream(DKVertexStream::Stream stream, const DKFoundation::DKString& name, DKVertexStream::Type type, bool normalize, size_t vertexSize, size_t vertexCount, void* data, DKVertexBuffer::MemoryLocation location = DKVertexBuffer::MemoryLocationStatic, DKVertexBuffer::BufferUsage usage = DKVertexBuffer::BufferUsageDraw);
		bool RemoveStream(DKVertexStream::Stream stream, const DKFoundation::DKString& name);

		bool CanAdoptMaterial(const DKMaterial* m) const override;
		DKPrimitive::Type PrimitiveType(void) const override;

		DKFoundation::DKObject<DKSerializer> Serializer(void) override;

	protected:
		int BindStream(const DKVertexStream&) const override;
		bool BindPrimitiveIndex(DKPrimitive::Type*, int*, DKIndexBuffer::Type*) const override;

		DKFoundation::DKObject<DKModel> Clone(UUIDObjectMap&) const override;
		DKStaticMesh* Copy(UUIDObjectMap&, const DKStaticMesh*);

		DKFoundation::DKArray<DKFoundation::DKObject<DKVertexBuffer>>	vertexBuffers;
		DKFoundation::DKObject<DKIndexBuffer>							indexBuffer;
	};
}
