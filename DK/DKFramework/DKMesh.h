//
//  File: DKMesh.h
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2004-2015 Hongtae Kim. All rights reserved.
//

#pragma once
#include "../DKInclude.h"
#include "../DKFoundation.h"
#include "DKModel.h"
#include "DKIndexBuffer.h"
#include "DKMaterial.h"
#include "DKPrimitiveIndex.h"
#include "DKSceneState.h"

////////////////////////////////////////////////////////////////////////////////
// DKMesh
// Graphical polygon mesh class, an abstract class.
// one mesh object can have one material, and one or more Buffers.
// mesh object can have various material properties also.
// sbuclass should define how buffers are used in rendering process.
//
// Note:
//    Subclass should implement it's own Clone() function.
////////////////////////////////////////////////////////////////////////////////

namespace DKFramework
{
	class DKGL_API DKMesh : public DKModel
	{
	public:
		virtual ~DKMesh(void);

		enum DrawFaceMode
		{
			DrawFaceNone = 0,	// draw nothing!
			DrawFaceCCW,		// front-face
			DrawFaceCW,			// back-face
			DrawFaceBoth,		// all-faces
		};
		using TextureArray = DKMaterial::TextureArray;
		using TextureSampler = DKMaterial::Sampler;
		using TextureSamplerMap = DKFoundation::DKMap<DKFoundation::DKString, TextureSampler>;
		using PropertyArray = DKMaterial::PropertyArray;
		using PropertyMap = DKFoundation::DKMap<DKFoundation::DKString, PropertyArray>;

		// material
		DKMaterial* Material(void)								{return material;}
		const DKMaterial* Material(void) const					{return material;}
		// test whether material are adoptable
		virtual bool CanAdoptMaterial(const DKMaterial* m) const {return false;}
		virtual void SetMaterial(DKMaterial* m); // assigning material.

		// Append texture to sampler for name.
		void AppendSampler(const DKFoundation::DKString& name, DKTexture* texture);
		// Set multiple textures and one sampler for name.
		void SetSampler(const DKFoundation::DKString& name, const TextureArray& textures, DKTextureSampler* sampler);
		// Set one texture and one sampler for name. (Remove sampler if texture is NULL)
		void SetSampler(const DKFoundation::DKString& name, DKTexture* texture, DKTextureSampler* sampler);
		// get sampler data
		TextureSampler* Sampler(const DKFoundation::DKString& name);
		const TextureSampler* Sampler(const DKFoundation::DKString& name) const;
		size_t SamplerCount(void) const;
		// remove sampler
		void RemoveSampler(const DKFoundation::DKString& name);
		void RemoveAllSamplers(void);

		// Material Properties
		void SetMaterialProperty(const DKFoundation::DKString& name, const PropertyArray& value);
		size_t MaterialPropertyCount(const DKFoundation::DKString& name) const;
		const PropertyArray* MaterialProperty(const DKFoundation::DKString& name) const;
		PropertyArray* MaterialProperty(const DKFoundation::DKString& name);
		void RemoveMaterialProperty(const DKFoundation::DKString& name);
		void RemoveAllMaterialProperties(void);

		// get property maps to access directly.
		const PropertyMap& MaterialPropertyMap(void) const		{return materialProperties;}
		const TextureSamplerMap& SamplerMap(void) const			{ return samplers; }
		PropertyMap& MaterialPropertyMap(void)					{return materialProperties;}
		TextureSamplerMap& SamplerMap(void)						{ return samplers; }

		// set default polygon primitive type.
		// If mesh has IndexBuffer, IndexBuffer's primitive type is used instead.
		void SetDefaultPrimitiveType(DKPrimitive::Type t)		{ primitiveType = t; }
		DKPrimitive::Type DefaultPrimitiveType(void) const		{ return primitiveType; }
		virtual DKPrimitive::Type PrimitiveType(void) const		{ return primitiveType; }

		void SetDrawFace(DrawFaceMode face)		{drawFace = face;}
		DrawFaceMode DrawFace(void) const		{return drawFace;}

		void SetScale(float s)					{ scale = DKVector3(s, s, s); }
		void SetScale(const DKVector3& s)		{ scale = s; }
		const DKVector3& Scale(void) const		{ return scale; }

		bool IsHidden(void) const				{ return hidden; }
		void SetHidden(bool h)					{ hidden = h; }

		// Bounding information for collision (optional)
		void SetAabb(const DKAabb& aabb)				{ this->aabb = aabb; }
		void SetBoundingSphere(const DKSphere& sphere)	{ this->boundingSphere = sphere; }
		const DKAabb& Aabb(void) const					{ return this->aabb; }
		const DKSphere& BoundingSphere(void) const		{ return this->boundingSphere; }

		// Bounding information applied by local-scale
		DKAabb ScaledAabb(void) const;
		DKSphere ScaledBoundingSphere(void) const;
		const DKMatrix4& ScaledWorldTransformMatrix(void) const;

		struct RenderInfo
		{
			DKPrimitive::Type primitive;
			DKIndexBuffer::Type indexType;
			int numVerts;
			int numIndices;
			int numInstances;
		};
		bool Bind(DKSceneState&, RenderInfo* info, const DKBlendState* blending = NULL) const;

		virtual DKFoundation::DKObject<DKSerializer> Serializer(void) override;

		unsigned int drawingGroupFlags;	// filtering bits mask

	protected:
		virtual bool BindTransform(DKSceneState&) const;
		virtual bool BindPrimitiveIndex(DKPrimitive::Type*, int*, DKIndexBuffer::Type*) const = 0;
		virtual int BindStream(const DKVertexStream&) const = 0;

		void OnUpdateSceneState(const DKNSTransform&) override;

		DKMesh(void);
		DKMesh* Copy(UUIDObjectMap&, const DKMesh*);

		DrawFaceMode				drawFace;
		DKPrimitive::Type			primitiveType;

		bool		hidden;
		DKVector3	scale;
		DKAabb		aabb;
		DKSphere	boundingSphere;
		DKMatrix4	scaledTransformMatrix;

		// materialProperties is object own values, used when rendering.
		// this values will overrides material's. (DKShaderConstant::UniformUserDefine only)
		PropertyMap	materialProperties;

		// samplers will overrides material's
		TextureSamplerMap	samplers;

		DKFoundation::DKObject<DKMaterial>	material;
	};
}
