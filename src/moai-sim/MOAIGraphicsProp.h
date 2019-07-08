// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIGRAPHICSPROP_H
#define	MOAIGRAPHICSPROP_H

#include <moai-sim/MOAIIndexedPropBase.h>
#include <moai-sim/MOAIGraphicsPropBase.h>

class MOAICellCoord;
class MOAIDeck;
class MOAIGrid;
class MOAILayoutFrame;
class MOAIMaterialBatch;
class MOAIOverlapPrim2D;
class MOAIPartition;
class MOAIPartitionCell;
class MOAIPartitionLevel;
class MOAIPartitionResultBuffer;
class MOAIScissorRect;
class MOAIShader;
class MOAISurfaceSampler2D;
class MOAITextureBase;

//================================================================//
// MOAIGraphicsProp
//================================================================//
// TODO: doxygen
class MOAIGraphicsProp :
	public MOAIIndexedPropBase,
	public MOAIGraphicsPropBase {
protected:

	//----------------------------------------------------------------//
	void					MOAIDrawable_Draw						( int subPrimID );
	bool					MOAINode_ApplyAttrOp					( u32 attrID, MOAIAttribute& attr, u32 op );
	void					MOAINode_Update							();
	ZLBounds				MOAIPartitionHull_GetModelBounds		(); // get the prop bounds in model space
	bool					MOAIPartitionHull_Inside				( ZLVec3D vec, float pad );

public:

	DECL_RUBY_FACTORY ( MOAIGraphicsProp, MOAINode )

	//----------------------------------------------------------------//
							MOAIGraphicsProp			();
	virtual					~MOAIGraphicsProp			();
	void					RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void					RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void					SerializeIn					( MOAIRubyState& state, MOAIDeserializer& serializer );
	void					SerializeOut				( MOAIRubyState& state, MOAISerializer& serializer );
};

#endif
