// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIMatrix.h>
#include <moai-sim/MOAISim.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMatrix::_getMatrix ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMatrix, "U" )
	
	return state.ToRValue ( ZLMatrix4x4 ( *(ZLAffine3D* )self ) );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMatrix::_invert ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMatrix, "U" )
	
	self->Inverse ();
	self->ScheduleUpdate ();
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMatrix::_setMatrix ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMatrix, "U" )
	
	size_t size = state.GetParamsCount ();
	
	switch ( size ) {
			
		case 9:
			*( ZLAffine3D* )self = ZLAffine3D ( state.GetParamValue < ZLMatrix3x3 >( 1, ZLMatrix3x3::IDENT ));
			break;
			
		case 12:
			*( ZLAffine3D* )self = state.GetParamValue < ZLAffine3D >( 1, ZLAffine3D::IDENT );
			break;
			
		case 16:
			*( ZLAffine3D* )self = ZLAffine3D ( state.GetParamValue < ZLMatrix4x4 >( 1, ZLMatrix4x4::IDENT ));
			break;
	}
	
	self->ScheduleUpdate ();
	
	return context;
}

//================================================================//
// MOAIMatrix
//================================================================//

//----------------------------------------------------------------//
MOAIMatrix::MOAIMatrix () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAITransformBase )
	RTTI_END
	
	this->Ident ();
}

//----------------------------------------------------------------//
MOAIMatrix::~MOAIMatrix () {
}

//----------------------------------------------------------------//
void MOAIMatrix::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	MOAITransformBase::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "ATTR_MATRIX",	MOAIMatrixAttr::Pack ( ATTR_MATRIX ) );
}

//----------------------------------------------------------------//
void MOAIMatrix::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAITransformBase::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getMatrix", _getMatrix, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "invert", _invert, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setMatrix", _setMatrix, MRB_ARGS_ANY () );
	
}

//----------------------------------------------------------------//
void MOAIMatrix::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAIMatrix::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAIMatrix::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	// TODO: these values may need to be cached for performance reasons
	if ( MOAIMatrix::MOAIMatrixAttr::Check ( attrID )) {

		switch ( UNPACK_ATTR ( attrID )) {
			
			case ATTR_MATRIX:
			
				*( ZLAffine3D* )this = attr.ApplyNoAdd < ZLAffine3D >( *( ZLAffine3D* )this, op, MOAIAttribute::ATTR_READ_WRITE );
				return true;
		}
	}
	return MOAITransformBase::MOAINode_ApplyAttrOp ( attrID, attr, op );
}

//----------------------------------------------------------------//
void MOAIMatrix::MOAITransformBase_BuildLocalToWorldMtx ( ZLAffine3D& localToWorldMtx ) {

	localToWorldMtx = *this;
}
