// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <Box2D/Box2D.h>
#include <moai-box2d/MOAIBox2DArbiter.h>
#include <moai-box2d/MOAIBox2DBody.h>
#include <moai-box2d/MOAIBox2DMouseJoint.h>
#include <moai-box2d/MOAIBox2DWorld.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getDampingRatio
	@text	See Box2D documentation.
	
	@in		MOAIBox2DMouseJoint self
	@out	number dampingRatio
*/
mrb_value MOAIBox2DMouseJoint::_getDampingRatio ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMouseJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2MouseJoint* joint = ( b2MouseJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetDampingRatio () );
}

//----------------------------------------------------------------//
/**	@lua	getFrequency
	@text	See Box2D documentation.
	
	@in		MOAIBox2DMouseJoint self
	@out	number frequency			in Hz
*/
mrb_value MOAIBox2DMouseJoint::_getFrequency ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMouseJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2MouseJoint* joint = ( b2MouseJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetFrequency () );
}

//----------------------------------------------------------------//
/**	@lua	getMaxForce
	@text	See Box2D documentation.
	
	@in		MOAIBox2DMouseJoint self
	@out	number maxForce				in kg * units / s^2 converted from N [kg * m / s^2]
*/
mrb_value MOAIBox2DMouseJoint::_getMaxForce ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMouseJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2MouseJoint* joint = ( b2MouseJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetMaxForce () / unitsToMeters );
}

//----------------------------------------------------------------//
/**	@lua	getTarget
	@text	See Box2D documentation.
	
	@in		MOAIBox2DMouseJoint self
	@out	number x					in units, world coordinates, converted from meters
	@out	number y					in units, world coordinates, converted from meters
*/
mrb_value MOAIBox2DMouseJoint::_getTarget ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMouseJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2MouseJoint* joint = ( b2MouseJoint* )self->mJoint;
	
	b2Vec2 target = joint->GetTarget ();
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( target.x / unitsToMeters );
	ret [ 1 ] = state.ToRValue ( target.y / unitsToMeters );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	setDampingRatio
	@text	See Box2D documentation.
	
	@in		MOAIBox2DMouseJoint self
	@opt	number dampingRatio		Default value is 0.
	@out	nil
*/
mrb_value MOAIBox2DMouseJoint::_setDampingRatio ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMouseJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float dampingRatio = state.GetParamValue < float >( 1, 0.0f );

	b2MouseJoint* joint = ( b2MouseJoint* )self->mJoint;
	joint->SetDampingRatio ( dampingRatio );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setFrequency
	@text	See Box2D documentation.
	
	@in		MOAIBox2DMouseJoint self
	@opt	number frequency		in Hz. Default value is 0.
	@out	nil
*/
mrb_value MOAIBox2DMouseJoint::_setFrequency ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMouseJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float frequency = state.GetParamValue < float >( 1, 0.0f );

	b2MouseJoint* joint = ( b2MouseJoint* )self->mJoint;
	joint->SetFrequency ( frequency );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setMaxForce
	@text	See Box2D documentation.
	
	@in		MOAIBox2DMouseJoint self
	@opt	number maxForce		in kg * units / s^2 converted to N [kg * m / s^2]. Default value is 0.
	@out	nil
*/
mrb_value MOAIBox2DMouseJoint::_setMaxForce ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMouseJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float maxForce = state.GetParamValue < float >( 1, 0.0f ) * unitsToMeters;

	b2MouseJoint* joint = ( b2MouseJoint* )self->mJoint;
	joint->SetMaxForce ( maxForce );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setTarget
	@text	See Box2D documentation.
	
	@in		MOAIBox2DMouseJoint self
	@opt	number x		in units, world coordinates, converted to meters. Default value is 0.
	@opt	number y		in units, world coordinates, converted to meters. Default value is 0.
	@out	nil
*/
mrb_value MOAIBox2DMouseJoint::_setTarget ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMouseJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2Vec2 target;
	
	target.x = state.GetParamValue < float >( 1, 0.0f ) * unitsToMeters;
	target.y = state.GetParamValue < float >( 2, 0.0f ) * unitsToMeters;

	b2MouseJoint* joint = ( b2MouseJoint* )self->mJoint;
	joint->SetTarget ( target );
	
	return mrb_nil_value ();
}

//================================================================//
// MOAIBox2DMouseJoint
//================================================================//

//----------------------------------------------------------------//
MOAIBox2DMouseJoint::MOAIBox2DMouseJoint () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIBox2DJoint )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIBox2DMouseJoint::~MOAIBox2DMouseJoint () {
}

//----------------------------------------------------------------//
void MOAIBox2DMouseJoint::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIBox2DMouseJoint::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getDampingRatio",		_getDampingRatio, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getFrequency",			_getFrequency, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getMaxForce",			_getMaxForce, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getTarget",				_getTarget, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setDampingRatio",		_setDampingRatio, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setFrequency",			_setFrequency, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setMaxForce",			_setMaxForce, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setTarget",				_setTarget, MRB_ARGS_ARG ( 0, 2 ) );

}
