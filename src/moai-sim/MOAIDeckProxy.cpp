// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAICollisionShape.h>
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIDeckProxy.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGfxResource.h>
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAIImage.h>
#include <moai-sim/MOAIShader.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAISurfaceSampler2D.h>
#include <moai-sim/MOAITexture.h>
#include <moai-sim/MOAITextureBase.h>
#include <moai-sim/MOAITransform.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDeckProxy::_setDeck ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDeckProxy, "U" )
	
	self->mDeck.Set ( *self, state.GetRubyObject < MOAIDeck >( 1, true ));
	return context;
}

//================================================================//
// MOAIDeckProxy
//================================================================//

//----------------------------------------------------------------//
MOAIDeckProxy::MOAIDeckProxy () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIDeck )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIDeckProxy::~MOAIDeckProxy () {

	this->mDeck.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAIDeckProxy::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIDeck::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIDeckProxy::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIDeck::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "setDeck", _setDeck, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAIDeckProxy::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {

	MOAIDeck::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIDeckProxy::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {

	MOAIDeck::SerializeOut ( state, serializer );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
ZLBounds MOAIDeckProxy::MOAIDeck_ComputeMaxBounds () {

	return this->mDeck ? this->mDeck->MOAIDeck_ComputeMaxBounds () : ZLBounds::ZL_BOUNDS_EMPTY;
}

//----------------------------------------------------------------//
void MOAIDeckProxy::MOAIDeck_Draw ( u32 idx ) {

	if ( this->mDeck ) {
		this->mDeck->MOAIDeck_Draw ( this->MOAIDeckProxy_Remap ( idx ));
	}
}

//----------------------------------------------------------------//
ZLBounds MOAIDeckProxy::MOAIDeck_GetBounds ( u32 idx ) {

	return this->mDeck ? this->mDeck->MOAIDeck_GetBounds ( this->MOAIDeckProxy_Remap ( idx )) : ZLBounds::ZL_BOUNDS_EMPTY;
}

//----------------------------------------------------------------//
MOAICollisionShape* MOAIDeckProxy::MOAIDeck_GetCollisionShape ( u32 idx ) {

	return this->mDeck ? this->mDeck->MOAIDeck_GetCollisionShape ( this->MOAIDeckProxy_Remap ( idx )) : 0;
}

//----------------------------------------------------------------//
bool MOAIDeckProxy::MOAIDeck_Overlap ( u32 idx, const ZLVec2D& vec, u32 granularity, ZLBounds* result ) {

	return this->mDeck ? this->mDeck->MOAIDeck_Overlap ( this->MOAIDeckProxy_Remap ( idx ), vec, granularity, result ) : false;
}

//----------------------------------------------------------------//
bool MOAIDeckProxy::MOAIDeck_Overlap ( u32 idx, const ZLVec3D& vec, u32 granularity, ZLBounds* result ) {

	return this->mDeck ? this->mDeck->MOAIDeck_Overlap ( this->MOAIDeckProxy_Remap ( idx ), vec, granularity, result ) : false;
}

//----------------------------------------------------------------//
u32 MOAIDeckProxy::MOAIDeckProxy_Remap ( u32 idx ) {

	return idx;
}
