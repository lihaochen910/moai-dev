// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAIMaterialMgr.h>
#include <moai-sim/MOAIQuadBrush.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAIStretchDeck.h>
#include <moai-sim/MOAITextureBase.h>
#include <moai-sim/MOAITransformBase.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
mrb_value MOAIStretchDeck::_setStretchFactor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStretchDeck, "U" )

	self->mStretchFactor = state.GetParamValue < float >( 1, 0.0f );

	return context;
}

//================================================================//
// MOAIStretchDeck
//================================================================//

//----------------------------------------------------------------//
ZLVec3D MOAIStretchDeck::BindStretchVertexTransform () const {

	ZLVec3D stretch ( 1.0f, 1.0f, 1.0f );

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	if ( this->mStretchFactor > 0.0f ) {
	
		// TODO: propery implement stretch factor
	
		ZLMatrix4x4 worldTransform = gfxState.GetMtx ( MOAIGfxState::MODEL_TO_WORLD_MTX );
		stretch = worldTransform.GetStretch ();
		
		ZLMatrix4x4 noStretchMtx;
		noStretchMtx.Scale ( 1.0f / stretch.mX, 1.0f / stretch.mY, 1.0f / stretch.mZ );
		noStretchMtx.Append ( worldTransform );
		noStretchMtx.Append ( gfxState.GetMtx ( MOAIGfxState::WORLD_TO_CLIP_MTX ));
		
		gfxState.SetVertexTransform ( noStretchMtx );
	}
	else {
	
		gfxState.SetVertexTransform ( MOAIGfxState::MODEL_TO_CLIP_MTX );
	}
	
	return stretch;
}

//----------------------------------------------------------------//
MOAIStretchDeck::MOAIStretchDeck () :
	mStretchFactor ( 1.0f ) {

	RTTI_BEGIN
		RTTI_EXTEND ( MOAIDeck )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIStretchDeck::~MOAIStretchDeck () {
}

//----------------------------------------------------------------//
void MOAIStretchDeck::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIDeck::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIStretchDeck::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIDeck::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "setStretchFactor", _setStretchFactor, MRB_ARGS_ARG ( 0, 1 ) );

}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
