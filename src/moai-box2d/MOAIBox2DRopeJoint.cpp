// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <Box2D/Box2D.h>
#include <moai-box2d/MOAIBox2DArbiter.h>
#include <moai-box2d/MOAIBox2DBody.h>
#include <moai-box2d/MOAIBox2DRopeJoint.h>
#include <moai-box2d/MOAIBox2DWorld.h>

MOAIBox2DRopeJoint::MOAIBox2DRopeJoint () {
	
	RTTI_BEGIN
	RTTI_EXTEND ( MOAIBox2DJoint )
	RTTI_END
}

//----------------------------------------------------------------//
/**	@lua	getLimitState
 @text	See Box2D documentation.
 
 @in		MOAIBox2DRopeJoint self
 @out	number limitState	one of the "LimitState" codes
 */
mrb_value MOAIBox2DRopeJoint::_getLimitState ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DRopeJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2RopeJoint* joint = ( b2RopeJoint* )self->mJoint;
	
	return state.ToRValue ( ( u32 )joint->GetLimitState () );
}

//----------------------------------------------------------------//
/**	@lua	getMaxLength
	@text	See Box2D documentation.
	
	@in		MOAIBox2DRopeJoint self
	@out	number maxLength			in units, converted from meters.
 */
mrb_value MOAIBox2DRopeJoint::_getMaxLength ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DRopeJoint, "U" )
	
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2RopeJoint* joint = ( b2RopeJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetMaxLength() / unitsToMeters );
}

//----------------------------------------------------------------//
/**	@lua	setMaxLength
	@text	See Box2D documentation.
	
	@in		MOAIBox2DRopeJoint self
	@in		number maxLength			in units, converted to meters. Default is 0.
	@out	nil
 */
mrb_value MOAIBox2DRopeJoint::_setMaxLength ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DRopeJoint, "U" )
	
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	float length = state.GetParamValue < float >( 1, 0.0f ) * unitsToMeters;
	b2RopeJoint* joint = ( b2RopeJoint* )self->mJoint;
	joint->SetMaxLength ( length );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
MOAIBox2DRopeJoint::~MOAIBox2DRopeJoint () {
	
}

//----------------------------------------------------------------//
void MOAIBox2DRopeJoint::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIBox2DRopeJoint::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getLimitState",			_getLimitState, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getMaxLength",			_getMaxLength, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setMaxLength",			_setMaxLength, MRB_ARGS_REQ ( 1 ) );

}
