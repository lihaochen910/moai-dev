// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIDebugLines.h>
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIFrameBufferTexture.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAITableViewLayer.h>
#include <moai-sim/MOAIMaterialMgr.h>
#include <moai-sim/MOAIPartitionHolder.h>
#include <moai-sim/MOAIPartitionResultBuffer.h>
#include <moai-sim/MOAIPartitionResultMgr.h>
#include <moai-sim/MOAIRenderMgr.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAITextureBase.h>
#include <moai-sim/MOAITransform.h>
#include <moai-sim/MOAIVertexFormatMgr.h>

//================================================================//
// local
//==========================================================s======//

//----------------------------------------------------------------//

//================================================================//
// MOAITableViewLayer
//================================================================//

//----------------------------------------------------------------//
MOAITableViewLayer::MOAITableViewLayer () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAITableLayer )
		RTTI_EXTEND ( MOAIViewLayer )
	RTTI_END
}

//----------------------------------------------------------------//
MOAITableViewLayer::~MOAITableViewLayer () {
}

//----------------------------------------------------------------//
void MOAITableViewLayer::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAITableLayer::RegisterRubyClass ( state, klass );
	MOAIViewLayer::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAITableViewLayer::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAITableLayer::RegisterRubyFuncs ( state, klass );
	MOAIViewLayer::RegisterRubyFuncs ( state, klass );
}

//----------------------------------------------------------------//
void MOAITableViewLayer::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {

	MOAITableLayer::SerializeIn ( state, serializer );
	MOAIViewLayer::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAITableViewLayer::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {

	MOAITableLayer::SerializeOut ( state, serializer );
	MOAIViewLayer::SerializeOut ( state, serializer );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAITableViewLayer::MOAIDrawable_Draw ( int subPrimID ) {

	this->MOAIViewLayer::MOAIDrawable_Draw ( subPrimID );
}

//----------------------------------------------------------------//
void MOAITableViewLayer::MOAIViewLayer_Draw () {
	
	MOAIDrawable::Draw ( MOAIRubyRuntime::Get ().State (), this->mRenderTable );
	
	if ( MOAIDebugLinesMgr::Get ().IsVisible () && this->mShowDebugLines ) {
		MOAIDrawable::Draw ( MOAIRubyRuntime::Get ().State (), this->mRenderTable, true );
	}
}
