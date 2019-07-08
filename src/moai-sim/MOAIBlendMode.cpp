// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIBlendMode.h>
//#include <moai-sim/MOAIGfxMgr.h>

//================================================================//
// MOAIBlendMode
//================================================================//

//----------------------------------------------------------------//
void MOAIBlendMode::Init ( MOAIRubyState& state, int idx ) {

	u32 equation	= state.GetParamValue < u32 >( idx++, ZGL_BLEND_MODE_ADD );
	u32 srcFactor	= state.GetParamValue < u32 >( idx++, ZGL_BLEND_FACTOR_ONE );
	u32 dstFactor	= state.GetParamValue < u32 >( idx, ZGL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA );
	
	this->SetBlend ( equation, srcFactor, dstFactor );
}

//----------------------------------------------------------------//
mrb_value MOAIBlendMode::Push ( MOAIRubyState& state ) const {

	mrb_value ary = mrb_ary_new ( state );

	mrb_ary_push ( state, ary, state.ToRValue ( this->mEquation ) );
	mrb_ary_push ( state, ary, state.ToRValue ( this->mSourceFactor ) );
	mrb_ary_push ( state, ary, state.ToRValue ( this->mDestFactor ) );

	return ary;
}

//----------------------------------------------------------------//
void MOAIBlendMode::SetBlend ( int equation, int srcFactor, int dstFactor ) {

	// ZGL_BLEND_MODE_ADD
	// ZGL_BLEND_MODE_SUBTRACT
	// ZGL_BLEND_MODE_REVERSE_SUBTRACT
	// (ZGL_BLEND_MODE_MIN and GL_BLEND_MODE_MAX unsupported on iOS)
	this->mEquation = equation;

	// ZGL_BLEND_FACTOR_ZERO
	// ZGL_BLEND_FACTOR_ONE
	// ZGL_BLEND_FACTOR_DST_COLOR
	// ZGL_BLEND_FACTOR_ONE_MINUS_DST_COLOR
	// ZGL_BLEND_FACTOR_SRC_ALPHA
	// ZGL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
	// ZGL_BLEND_FACTOR_DST_ALPHA
	// ZGL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA
	// ZGL_BLEND_FACTOR_SRC_ALPHA_SATURATE
	this->mSourceFactor = srcFactor;
	
	// ZGL_BLEND_FACTOR_ZERO
	// ZGL_BLEND_FACTOR_ONE
	// ZGL_BLEND_FACTOR_SRC_COLOR
	// ZGL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR
	// ZGL_BLEND_FACTOR_SRC_ALPHA
	// ZGL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
	// ZGL_BLEND_FACTOR_DST_ALPHA
	// ZGL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA
	this->mDestFactor = dstFactor;
}

//----------------------------------------------------------------//
MOAIBlendMode::MOAIBlendMode () :
	mEquation ( ZGL_BLEND_MODE_ADD ),
	mSourceFactor ( ZGL_BLEND_FACTOR_ONE ),
	mDestFactor ( ZGL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA ) {
}

//----------------------------------------------------------------//
MOAIBlendMode::MOAIBlendMode ( int equation, int srcFactor, int dstFactor ) :
	mEquation ( equation ),
	mSourceFactor ( srcFactor ),
	mDestFactor ( dstFactor ) {
}

//----------------------------------------------------------------//
MOAIBlendMode::~MOAIBlendMode () {
}
