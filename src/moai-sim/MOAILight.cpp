// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAILight.h>
#include <moai-sim/MOAILightFormat.h>
#include <moai-sim/MOAIShader.h>
#include <moai-sim/MOAITexture.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAILight::_getFormat ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAILight, "U" )
	return state.ToRValue < MOAIRubyObject* >( self->mFormat );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAILight::_setFormat ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAILight, "U" )
	self->mFormat.Set ( *self, state.GetRubyObject < MOAILightFormat >( 1, true ));
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAILight::_setTexture ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAILight, "U" )
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAILight::_setTransform ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAILight, "U" )
	return context;
}

//----------------------------------------------------------------//
mrb_value MOAILight::_setUniform ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAILight, "U" )

	u32 uniformID	= state.GetParamValue < u32 >( 1, 1 ) - 1;
	self->SetUniform ( M, 2, self->mBuffer, uniformID, 0 );
	return context;
}

//================================================================//
// MOAILight
//================================================================//

//----------------------------------------------------------------//
void MOAILight::ApplyUniforms ( void* buffer, size_t bufferSize ) {

	size_t srcSize = this->mBuffer.Size ();
	if ( srcSize < bufferSize ) {
		memcpy ( buffer, this->mBuffer, srcSize );
	}
}

//----------------------------------------------------------------//
void MOAILight::BindTextures ( u32 textureOffset ) {

	MOAIGfxMgr& gfx = MOAIGfxMgr::Get ();

	size_t nTextures = this->mTextures.Size ();
	for ( u32 i = 0; i < nTextures; ++i ) {
		gfx.mGfxState.SetTexture ( this->mTextures [ i ], textureOffset + i );
	}
}

//----------------------------------------------------------------//
MOAILight::MOAILight () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAINode )
	RTTI_END
}

//----------------------------------------------------------------//
MOAILight::~MOAILight () {

	this->mFormat.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAILight::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAINode::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAILight::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAINode::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getFormat", _getFormat, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setFormat", _setFormat, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setTexture", _setTexture, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setTransform", _setTransform, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setUniform", _setUniform, MRB_ARGS_ANY () );

}

//----------------------------------------------------------------//
void MOAILight::SetFormat ( MOAILightFormat* format ) {

	this->mFormat.Set ( *this, format );
	if ( format ) {
		format->Bless ();
		this->mBuffer.Init ( format->mBufferSize );
	}
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAILight::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	return this->mFormat ? this->MOAIShaderUniformSchema::ApplyAttrOp ( this->mBuffer, attrID, attr, op ) : false;
}

//----------------------------------------------------------------//
MOAIShaderUniformHandle MOAILight::MOAIShaderUniformSchema_GetUniformHandle ( void* buffer, u32 uniformID ) const {

	MOAIShaderUniformHandle uniform;
	uniform.mBuffer = 0;

	if ( this->mFormat ) {
	
		const MOAILightFormatUniform& lightUniform = this->mFormat->mUniforms [ uniformID ];

		uniform.mType		= lightUniform.mType;
		uniform.mWidth		= lightUniform.mWidth;
		uniform.mBuffer		= ( void* )(( size_t )this->mBuffer.Data () + lightUniform.mBase );
	}
	return uniform;
}

