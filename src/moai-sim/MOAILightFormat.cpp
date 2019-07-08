// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAILightFormat.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAILightFormat::_reserveTextures ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAILightFormat, "U" )
	
	self->mTextures = state.GetParamValue < u32 >( 1, 0 );
	self->mDirty = true;
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAILightFormat::_reserveUniform ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAILightFormat, "U" )
	
	self->mUniforms.Init ( state.GetParamValue < u32 >( 1, 0 ));
	self->mDirty = true;
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAILightFormat::_setUniform ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAILightFormat, "U" )
	
	u32 idx = state.GetParamValue < u32 >( 1, 1 ) - 1;
	
	if ( idx < self->mUniforms.Size ()) {
	
		MOAILightFormatUniform& uniform = self->mUniforms [ idx ];
		uniform.mType	= state.GetParamValue < u32 >( 2, 0 );
		uniform.mWidth	= state.GetParamValue < u32 >( 3, 1 );
		self->mDirty = true;
	}
	return context;
}

//================================================================//
// MOAILightFormat
//================================================================//

//----------------------------------------------------------------//
void MOAILightFormat::Bless () {

	if ( this->mDirty ) {
	
		size_t base = 0;
	
		size_t nUniforms = this->mUniforms.Size ();
		for ( size_t i = 0; i < nUniforms; ++i ) {
			
			MOAILightFormatUniform& uniform = this->mUniforms [ i ];
			uniform.mBase = base;
			base += uniform.GetSize ();
		}
		this->mBufferSize = base;
		this->mDirty = false;
	}
}

//----------------------------------------------------------------//
MOAILightFormatUniform* MOAILightFormat::GetUniform ( u32 uniformID ) {

	return uniformID < this->mUniforms.Size () ? &this->mUniforms [ uniformID ] : 0;
}

//----------------------------------------------------------------//
MOAILightFormat::MOAILightFormat () :
	mTextures ( 0 ),
	mDirty ( false ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAILightFormat::~MOAILightFormat () {
}

//----------------------------------------------------------------//
void MOAILightFormat::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAILightFormat::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "reserveTextures",	_reserveTextures, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "reserveUniform",	_reserveUniform, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setUniform",		_setUniform, MRB_ARGS_REQ ( 3 ) );

}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
