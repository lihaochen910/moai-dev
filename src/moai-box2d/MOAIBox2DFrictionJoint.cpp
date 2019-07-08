// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <Box2D/Box2D.h>
#include <moai-box2d/MOAIBox2DArbiter.h>
#include <moai-box2d/MOAIBox2DBody.h>
#include <moai-box2d/MOAIBox2DFrictionJoint.h>
#include <moai-box2d/MOAIBox2DWorld.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getMaxForce
	@text	See Box2D documentation.
	
	@in		MOAIBox2DFrictionJoint self
	@out	number maxForce					in kg * units / s^2, converted from N [kg * m / s^2].
*/
mrb_value MOAIBox2DFrictionJoint::_getMaxForce ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DFrictionJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2FrictionJoint* joint = ( b2FrictionJoint* )self->mJoint;

	return state.ToRValue ( joint->GetMaxForce () / unitsToMeters );
}

//----------------------------------------------------------------//
/**	@lua	getMaxTorque
	@text	See Box2D documentation.
	
	@in		MOAIBox2DFrictionJoint self
	@out	number maxTorque		in (kg * units / s^2) * units, converted from N-m.
*/
mrb_value MOAIBox2DFrictionJoint::_getMaxTorque ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DFrictionJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2FrictionJoint* joint = ( b2FrictionJoint* )self->mJoint;
	float unitsToMeters = self->GetUnitsToMeters();
	/* Convert to/from N-m (kg m / s^2) * m from/to (kg unit / s^2) * unit */

	return state.ToRValue ( joint->GetMaxTorque () / ( unitsToMeters * unitsToMeters ) );
}

//----------------------------------------------------------------//
/**	@lua	setMaxForce
	@text	See Box2D documentation.
	
	@in		MOAIBox2DFrictionJoint self
	@opt	number maxForce		in kg * units / s^2, converted to N [kg * m / s^2]. Default value is 0.
	@out	nil
*/
mrb_value MOAIBox2DFrictionJoint::_setMaxForce ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DFrictionJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float maxForce = state.GetParamValue < float >( 1, 0.0f ) * unitsToMeters;

	b2FrictionJoint* joint = ( b2FrictionJoint* )self->mJoint;
	joint->SetMaxForce ( maxForce );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setMaxTorque
	@text	See Box2D documentation.
	
	@in		MOAIBox2DFrictionJoint self
	@opt	number maxTorque		in (kg * units / s^2) * units, converted to N-m. Default value is 0.
	@out	nil
*/
mrb_value MOAIBox2DFrictionJoint::_setMaxTorque ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DFrictionJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float unitsToMeters = self->GetUnitsToMeters();
	/* Convert to/from N-m (kg m / s^2) * m from/to (kg unit / s^2) * unit */
	float maxTorque = state.GetParamValue < float >( 1, 0.0f ) * unitsToMeters * unitsToMeters;

	b2FrictionJoint* joint = ( b2FrictionJoint* )self->mJoint;
	joint->SetMaxTorque ( maxTorque );
	
	return mrb_nil_value ();
}

//================================================================//
// MOAIBox2DFrictionJoint
//================================================================//

//----------------------------------------------------------------//
MOAIBox2DFrictionJoint::MOAIBox2DFrictionJoint () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIBox2DJoint )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIBox2DFrictionJoint::~MOAIBox2DFrictionJoint () {
}

//----------------------------------------------------------------//
void MOAIBox2DFrictionJoint::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIBox2DFrictionJoint::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getMaxForce",			_getMaxForce, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getMaxTorque",			_getMaxTorque, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setMaxForce",			_setMaxForce, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setMaxTorque",			_setMaxTorque, MRB_ARGS_ARG ( 0, 1 ) );
	
}
