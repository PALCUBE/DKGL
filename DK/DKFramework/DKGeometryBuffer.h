//
//  File: DKGeometryBuffer.h
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2004-2015 Hongtae Kim. All rights reserved.
//

#pragma once
#include "../DKInclude.h"
#include "../DKFoundation.h"
#include "DKResource.h"

////////////////////////////////////////////////////////////////////////////////
// DKGeomeryBuffer
// a buffer interface for OpenGL / OpenGL ES
// Use DKVertexBuffer for Vertex-Buffer. (see DKVertexBuffer.h)
// Use DKIndexBuffer for Index-Buffer. (see DKIndexBuffer.h)
////////////////////////////////////////////////////////////////////////////////

namespace DKFramework
{
	class DKGL_API DKGeometryBuffer : public DKResource
	{
	public:
		enum BufferType
		{
			BufferTypeVertexArray,
			BufferTypeElementArray,
		};
		enum MemoryLocation
		{
			MemoryLocationStatic,		// write once, used many times
			MemoryLocationDynamic,		// write repeatedly, used many times
			MemoryLocationStream,		// write once and used at most a few times
		};
		enum BufferUsage
		{
			BufferUsageDraw,			// draw (App -> GL)
			BufferUsageRead,			// read from GL (GL -> App)
			BufferUsageCopy,			// read, draw both (GL -> GL)
		};
		enum AccessMode // acess mode for locked buffer.
		{
			AccessModeReadOnly,			// read only
			AccessModeWriteOnly,		// write only
			AccessModeReadWrite,		// read, write
		};

		virtual ~DKGeometryBuffer(void);
		bool Bind(void) const;

		// Lock buffer and get temporary pointer. (UpdateContent, UpdateSubContent is faster)
		void* Lock(AccessMode lock);
		// Unlock buffer.
		void Unlock(void);
		bool IsLocked(AccessMode* lock) const;

		// Get data address where GL-buffer has been mapped.
		void* AcquireLockedBuffer(void) const;

		// Copy object contents into buffer object and return.
		DKFoundation::DKObject<DKFoundation::DKBuffer> CopyContent(void) const;

		// update sub content
		bool UpdateSubContent(const void* data, size_t offset, size_t size);

		size_t			Size(void) const			{return resourceSize;}
		BufferType		Type(void) const			{return resourceType;}
		MemoryLocation	Location(void) const		{return resourceLocation;}
		BufferUsage		Usage(void) const			{return resourceUsage;}

		// check the buffer is valid
		bool	IsValid(void) const;

		// create serializer object for serialization.
		DKFoundation::DKObject<DKSerializer> Serializer(void);

		// DKResource::Validate() override
		bool Validate(void)					{return IsValid();}

	protected:
		DKGeometryBuffer(void);

		bool UpdateContent(BufferType t, MemoryLocation m, BufferUsage u, const DKFoundation::DKData* data);
		bool UpdateContent(BufferType t, MemoryLocation m, BufferUsage u, const void* data, size_t size);

		virtual void StructuredLayout(DKFoundation::DKArray<DKVariant::StructElem>& layout, size_t& elementSize) const = 0;

	private:
		BufferType			resourceType;
		MemoryLocation		resourceLocation;
		BufferUsage			resourceUsage;
		unsigned int		resourceId;
		size_t				resourceSize;
	};
}
