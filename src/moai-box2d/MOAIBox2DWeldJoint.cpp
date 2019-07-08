// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <Box2D/Box2D.h>
#include <moai-box2d/MOAIBox2DArbiter.h>
#include <moai-box2d/MOAIBox2DBody.h>
#include <moai-box2d/MOAIBox2DWeldJoint.h>
#include <moai-box2d/MOAIBox2DWorld.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getDampingRatio
	@text	See Box2D documentation.

	@in		MOAIBox2DDistanceJoint self
	@out	number 	dampingRatio
 */
mrb_value MOAIBox2DWeldJoint::_getDampingRatio ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWeldJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2WeldJoint* joint = ( b2WeldJoint* )self->mJoint;

	return state.ToRValue ( joint->GetDampingRatio () );
}

//----------------------------------------------------------------//
/**	@lua    getFrequency
	@text   See Box2D documentation.

	@in		MOAIBox2DDistanceJoint 		self
	@out	number 	frequency			In Hz.
 */
mrb_value MOAIBox2DWeldJoint::_getFrequency ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWeldJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2WeldJoint* joint = ( b2WeldJoint* )self->mJoint;

	return state.ToRValue ( joint->GetFrequency () );
}

//----------------------------------------------------------------//
/**	@lua    setDampingRatio
	@text   See Box2D documentation.
	
	@in		MOAIBox2DDistanceJoint 		self
	@opt	number dampingRatio 		Default value is 0.
	@out	nil
 */
mrb_value MOAIBox2DWeldJoint::_setDampingRatio ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWeldJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float dampingRatio = state.GetParamValue < float >( 1, 0.0f );

	b2WeldJoint* joint = ( b2WeldJoint* )self->mJoint;
	joint->SetDampingRatio ( dampingRatio );

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setFrequency
	@text	See Box2D documentation.
	   
	@in		MOAIBox2DDistanceJoint 	self
	@opt	number frequency		In Hz. Default value is 0.
	@out	nil
 */
mrb_value MOAIBox2DWeldJoint::_setFrequency ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWeldJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float frequency = state.GetParamValue < float >( 1, 0.0f );

	b2WeldJoint* joint = ( b2WeldJoint* )self->mJoint;
	joint->SetFrequency ( frequency );

	return mrb_nil_value ();
}

//================================================================//
// MOAIBox2DWeldJoint
//================================================================//

//----------------------------------------------------------------//
MOAIBox2DWeldJoint::MOAIBox2DWeldJoint () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIBox2DJoint )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIBox2DWeldJoint::~MOAIBox2DWeldJoint () {
}

//----------------------------------------------------------------//
void MOAIBox2DWeldJoint::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIBox2DWeldJoint::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getDampingRatio",		_getDampingRatio, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getFrequency",			_getFrequency, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setDampingRatio",		_setDampingRatio, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setFrequency",			_setFrequency, MRB_ARGS_ARG ( 0, 1 ) );

}
