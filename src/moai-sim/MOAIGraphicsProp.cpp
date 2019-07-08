// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIDebugLines.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGraphicsProp.h>
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAILayoutFrame.h>
#include <moai-sim/MOAIMaterialMgr.h>
#include <moai-sim/MOAIPartition.h>
#include <moai-sim/MOAIPartitionResultBuffer.h>
#include <moai-sim/MOAIRenderMgr.h>
#include <moai-sim/MOAIScissorRect.h>
#include <moai-sim/MOAIShader.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAISurfaceSampler2D.h>
#include <moai-sim/MOAITexture.h>
#include <moai-sim/MOAITextureBase.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//

//================================================================//
// MOAIGraphicsProp
//================================================================//

//----------------------------------------------------------------//
MOAIGraphicsProp::MOAIGraphicsProp () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIIndexedPropBase )
		RTTI_EXTEND ( MOAIGraphicsPropBase )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIGraphicsProp::~MOAIGraphicsProp () {
}

//----------------------------------------------------------------//
void MOAIGraphicsProp::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIIndexedPropBase::RegisterRubyClass ( state, klass );
	MOAIGraphicsPropBase::RegisterRubyClass ( state, klass );

}

//----------------------------------------------------------------//
void MOAIGraphicsProp::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIIndexedPropBase::RegisterRubyFuncs ( state, klass );
	MOAIGraphicsPropBase::RegisterRubyFuncs ( state, klass );

}

//----------------------------------------------------------------//
void MOAIGraphicsProp::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	
	MOAIIndexedPropBase::SerializeIn ( state, serializer );
	MOAIGraphicsPropBase::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIGraphicsProp::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	
	MOAIIndexedPropBase::SerializeOut ( state, serializer );
	MOAIGraphicsPropBase::SerializeOut ( state, serializer );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAIGraphicsProp::MOAIDrawable_Draw ( int subPrimID ) {
	UNUSED ( subPrimID );

	if ( !this->IsVisible ()) return;
	if ( !this->mDeck ) return;
	if ( this->IsClear ()) return;

	this->PushGfxState ();
	this->LoadVertexTransform ();
	this->LoadUVTransform ();
	
	this->mDeck->Draw ( this->mIndex - 1 );
	
	this->PopGfxState ();
}

//----------------------------------------------------------------//
bool MOAIGraphicsProp::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {
	
	if ( MOAIIndexedPropBase::MOAINode_ApplyAttrOp ( attrID, attr, op )) return true;
	if ( MOAIGraphicsPropBase::MOAINode_ApplyAttrOp ( attrID, attr, op )) return true;
	return false;
}

//----------------------------------------------------------------//
void MOAIGraphicsProp::MOAINode_Update () {
	
	MOAIGraphicsPropBase::MOAINode_Update ();
}

//----------------------------------------------------------------//
ZLBounds MOAIGraphicsProp::MOAIPartitionHull_GetModelBounds () {
	
	return this->mDeck ? this->mDeck->GetBounds ( this->mIndex - 1 ) : ZLBounds::EMPTY;
}

//----------------------------------------------------------------//
bool MOAIGraphicsProp::MOAIPartitionHull_Inside ( ZLVec3D vec, float pad ) {

	ZLAffine3D worldToLocal = this->GetWorldToLocalMtx ();
	worldToLocal.Transform ( vec );

	bool passTrivial = this->InsideModelBounds ( vec, pad );
	
	// TODO: handle grids
	// TODO: handle padding
	if ( passTrivial && this->mDeck && ( this->mHitGranularity > HIT_TEST_COARSE )) {
	
		//return this->mDeck->Inside ( this->mIndex, this->mMaterialBatch, this->mHitGranularity, vec, pad );
		return this->mDeck->Overlap ( this->mIndex - 1, vec.Vec2D(), this->mHitGranularity, 0 );
	}
	return passTrivial;
}
