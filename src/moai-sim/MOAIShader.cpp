// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIColor.h>
#include <moai-sim/MOAIEaseDriver.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIShader.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAITransformBase.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIShader::_getAttributeID ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIShader, "UN" )

	u32 uniformID		= state.GetParamValue < u32 >( 1, 1 ) - 1;
	u32 index			= state.GetParamValue < u32 >( 2, 1 ) - 1;
	
	if ( self->mProgram ) {
		return state.ToRValue ( self->mProgram->GetAttributeID ( uniformID, index ) );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIShader::_setProgram ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIShader, "U" )
	
	self->SetProgram ( state.GetRubyObject < MOAIShaderProgram >( 1, true ));

	return context;
}

//----------------------------------------------------------------//
mrb_value MOAIShader::_setUniform ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIShader, "U" )

	u32 uniformID	= state.GetParamValue < u32 >( 1, 1 ) - 1;

	if ( self->mProgram ) {
		self->mProgram->SetUniform ( M, 2, self->mPendingUniformBuffer, uniformID, 0 );
	}
	return context;
}

//----------------------------------------------------------------//
mrb_value MOAIShader::_setUniformArrayItem ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIShader, "U" )

	u32 uniformID	= state.GetParamValue < u32 >( 1, 1 ) - 1;
	u32 index		= state.GetParamValue < u32 >( 2, 1 ) - 1;

	if ( self->mProgram ) {
		self->mProgram->SetUniform ( M, 3, self->mPendingUniformBuffer, uniformID, index );
	}
	return context;
}

//================================================================//
// MOAIShader
//================================================================//

//----------------------------------------------------------------//
MOAIShader* MOAIShader::AffirmShader ( MOAIRubyState& state, int idx ) {

	MOAIShader* shader = 0;

	if ( state.ParamIsType ( idx, MRB_TT_FIXNUM )) {
		shader = MOAIShaderMgr::Get ().GetShader ( state.GetParamValue < u32 >( idx, MOAIShaderMgr::UNKNOWN_SHADER ));
	}
	else {
		shader = state.GetRubyObject < MOAIShader >( idx, true );
	}
	return shader;
}

//----------------------------------------------------------------//
void MOAIShader::ApplyUniforms () {

	if ( this->mProgram ) {
		this->mProgram->ApplyUniforms ( this->mPendingUniformBuffer );
	}
}

//----------------------------------------------------------------//
void MOAIShader::BindUniforms () {
	
	if ( this->mProgram ) {
		this->mProgram->BindUniforms ();
	}
}

//----------------------------------------------------------------//
bool MOAIShader::HasDirtyUniforms () {

	return !this->mProgram->mUniformBuffer.IsIdentical ( this->mPendingUniformBuffer );
}

//----------------------------------------------------------------//
MOAIShader::MOAIShader () {

	RTTI_BEGIN
		RTTI_EXTEND ( MOAINode )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIShader::~MOAIShader () {

	this->SetProgram ( 0 );
}

//----------------------------------------------------------------//
void MOAIShader::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAINode::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIShader::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAINode::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getAttributeID", _getAttributeID, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setProgram", _setProgram, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setUniform", _setUniform, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setUniformArrayItem", _setUniformArrayItem, MRB_ARGS_REQ ( 3 ) );

}

//----------------------------------------------------------------//
void MOAIShader::ScheduleTextures () {

	if ( this->mProgram ) {
		this->mProgram->ScheduleTextures ();
	}
}

//----------------------------------------------------------------//
void MOAIShader::SetProgram ( MOAIShaderProgram* program ) {
	
	this->mProgram.Set ( *this, program );
	
	if ( program ) {
		program->InitUniformBuffer ( this->mPendingUniformBuffer );
	}
}

//----------------------------------------------------------------//
void MOAIShader::UpdateUniforms () {

	if ( this->mProgram ) {
		this->mProgram->UpdateUniforms ( this->mPendingUniformBuffer );
	}
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAIShader::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	if ( this->mProgram ) {
		return this->mProgram->ApplyAttrOp ( this->mPendingUniformBuffer, attrID, attr, op );
	}
	return false;
}
