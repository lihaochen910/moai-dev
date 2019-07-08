// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <moai-sim/MOAIMaterialHolder.h>
#include <moai-sim/MOAIShader.h>
#include <moai-sim/MOAITexture.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialHolder::_getBlendMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialHolder, "U" )
	return self->mMaterial.GetBlendMode ().Push ( state );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialHolder::_getCullMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialHolder, "U" )
	return state.ToRValue ( self->mMaterial.GetCullMode () );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialHolder::_getDepthMask ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialHolder, "U" )
	return state.ToRValue ( self->mMaterial.GetDepthMask () );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialHolder::_getDepthTest ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialHolder, "U" )
	return state.ToRValue ( self->mMaterial.GetDepthTest () );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialHolder::_getLight ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialHolder, "UN" )
	
	u32 name = state.GetParamValue < u32 >( 1,  MOAI_UNKNOWN_MATERIAL_GLOBAL );
	return state.ToRValue < MOAIRubyObject* >( self->mMaterial.GetLight ( name ) );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialHolder::_getShader ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialHolder, "U" )
	return state.ToRValue < MOAIRubyObject* >( self->mMaterial.GetShader () );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialHolder::_getTexture ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialHolder, "U" )
	
	u32 name = state.GetParamValue < u32 >( 1,  MOAI_UNKNOWN_MATERIAL_GLOBAL );
	return state.ToRValue < MOAIRubyObject* >( self->mMaterial.GetTexture ( name ) );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialHolder::_setBlendMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialHolder, "U" )

	if ( state.IsNil ( state.GetParamValue ( 1 ) )) {
		self->mMaterial.SetBlendMode ();
	}
	else {
		MOAIBlendMode blendMode;
		blendMode.Init ( state, 1 );
		self->mMaterial.SetBlendMode ( blendMode );
	}
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialHolder::_setCullMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialHolder, "U" )
	
	if ( state.IsNil ( state.GetParamValue ( 1 ) )) {
		self->mMaterial.SetCullMode ();
	}
	else {
		self->mMaterial.SetCullMode ( state.GetParamValue < int >( 1, 0 ));
	}
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialHolder::_setDepthMask ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialHolder, "U" )
	
	if ( state.IsNil ( state.GetParamValue ( 1 ) )) {
		self->mMaterial.SetDepthMask ();
	}
	else {
		self->mMaterial.SetDepthMask ( state.GetParamValue < bool >( 1, false ));
	}
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialHolder::_setDepthTest ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialHolder, "U" )
	
	if ( state.IsNil ( state.GetParamValue ( 1 ) )) {
		self->mMaterial.SetDepthTest ();
	}
	else {
		self->mMaterial.SetDepthTest ( state.GetParamValue < int >( 1, 0 ));
	}
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialHolder::_setLight ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialHolder, "U" )
	
	u32 name = state.GetParamValue < u32 >( 1, MOAI_UNKNOWN_MATERIAL_GLOBAL );
	MOAILight* light = state.GetRubyObject < MOAILight >( 2, true );
	self->mMaterial.SetLight ( name, light );
	
	return state.ToRValue < MOAIRubyObject* >( light );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialHolder::_setShader ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialHolder, "U" )
	
	MOAIShader* shader = MOAIShader::AffirmShader ( state, 1 );
	self->mMaterial.SetShader ( shader );
	
	return state.ToRValue < MOAIRubyObject* >( shader );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialHolder::_setTexture ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialHolder, "U" )
	
	u32 idx = 1;
	u32 name = MOAI_UNKNOWN_MATERIAL_GLOBAL;
	
	if ( state.ParamIsType ( 1, MRB_TT_FIXNUM )) {
		name = state.GetParamValue < u32 >( idx++, name );
	}
	
	MOAITextureBase* texture = MOAITexture::AffirmTexture ( state, idx );
	if ( name != MOAI_UNKNOWN_MATERIAL_GLOBAL ) {
		self->mMaterial.SetTexture ( name, texture );
	}
	else {
		self->mMaterial.SetTexture ( texture );
	}
	return state.ToRValue < MOAIRubyObject* >( texture );
}

//================================================================//
// MOAIMaterialHolder
//================================================================//

//----------------------------------------------------------------//
MOAIMaterialHolder::MOAIMaterialHolder () {

	RTTI_BEGIN
		RTTI_EXTEND ( MOAIMaterialHolder )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIMaterialHolder::~MOAIMaterialHolder () {
}

//----------------------------------------------------------------//
void MOAIMaterialHolder::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "getBlendMode", _getBlendMode, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getCullMode", _getCullMode, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getDepthMask", _getDepthMask, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getDepthTest", _getDepthTest, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getLight", _getLight, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "getShader", _getShader, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getTexture", _getTexture, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setBlendMode", _setBlendMode, MRB_ARGS_ARG ( 0, 3 ) );
	state.DefineInstanceMethod ( klass, "setCullMode", _setCullMode, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setDepthMask", _setDepthMask, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setDepthTest", _setDepthTest, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setLight", _setLight, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setShader", _setShader, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setTexture", _setTexture, MRB_ARGS_ANY () );

}
