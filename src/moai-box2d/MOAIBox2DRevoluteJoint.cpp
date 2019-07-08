// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <Box2D/Box2D.h>
#include <moai-box2d/MOAIBox2DArbiter.h>
#include <moai-box2d/MOAIBox2DBody.h>
#include <moai-box2d/MOAIBox2DRevoluteJoint.h>
#include <moai-box2d/MOAIBox2DWorld.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getJointAngle
	@text	See Box2D documentation.
	
	@in		MOAIBox2DRevoluteJoint self
	@out	number angle					in degrees, converted from radians
*/
mrb_value MOAIBox2DRevoluteJoint::_getJointAngle ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DRevoluteJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2RevoluteJoint* joint = ( b2RevoluteJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetJointAngle () * ( float )R2D );
}

//----------------------------------------------------------------//
/**	@lua	getJointSpeed
	@text	See Box2D documentation.
	
	@in		MOAIBox2DRevoluteJoint self
	@out	number jointSpeed	in degrees/s, converted from radians/s
*/
mrb_value MOAIBox2DRevoluteJoint::_getJointSpeed ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DRevoluteJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2RevoluteJoint* joint = ( b2RevoluteJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetJointSpeed () * ( float )R2D );
}

//----------------------------------------------------------------//
/**	@lua	getLowerLimit
	@text	See Box2D documentation.
	
	@in		MOAIBox2DRevoluteJoint self
	@out	number lowerLimit	in degrees, converted from radians
*/
mrb_value MOAIBox2DRevoluteJoint::_getLowerLimit ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DRevoluteJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2RevoluteJoint* joint = ( b2RevoluteJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetLowerLimit () * ( float )R2D );
}

//----------------------------------------------------------------//
/**	@lua	getMotorSpeed
	@text	See Box2D documentation.
	
	@in		MOAIBox2DRevoluteJoint self
	@out	number motorSpeed	in degrees/s, converted from radians/s
*/
mrb_value MOAIBox2DRevoluteJoint::_getMotorSpeed ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DRevoluteJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2RevoluteJoint* joint = ( b2RevoluteJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetMotorSpeed () * ( float )R2D );
}

//----------------------------------------------------------------//
/**	@lua	getMotorTorque
	@text	See Box2D documentation.
	
	@in		MOAIBox2DRevoluteJoint self
	@out	number motorTorque		in (kg * units / s^2) * units, converted from N-m..
*/
mrb_value MOAIBox2DRevoluteJoint::_getMotorTorque ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DRevoluteJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float unitsToMeters = self->GetUnitsToMeters ();
	b2RevoluteJoint* joint = ( b2RevoluteJoint* )self->mJoint;
	float step = ( float )( 1.0 / MOAISim::Get ().GetStep ());
	/* Convert from N-m (kg m / s^2) * m => (kg unit / s^2) * unit */

	return state.ToRValue ( joint->GetMotorTorque (step) / (unitsToMeters * unitsToMeters) );
}

//----------------------------------------------------------------//
/**	@lua	getUpperLimit
	@text	See Box2D documentation.
	
	@in		MOAIBox2DRevoluteJoint self
	@out	number upperLimit	in degrees, converted from radians
*/
mrb_value MOAIBox2DRevoluteJoint::_getUpperLimit ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DRevoluteJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2RevoluteJoint* joint = ( b2RevoluteJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetUpperLimit () * ( float )R2D );
}

//----------------------------------------------------------------//
/**	@lua	isLimitEnabled
	@text	See Box2D documentation.
	
	@in		MOAIBox2DRevoluteJoint self
	@out	boolean limitEnabled
*/
mrb_value MOAIBox2DRevoluteJoint::_isLimitEnabled ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DRevoluteJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2RevoluteJoint* joint = ( b2RevoluteJoint* )self->mJoint;
	
	return state.ToRValue ( joint->IsLimitEnabled () );
}

//----------------------------------------------------------------//
/**	@lua	isMotorEnabled
	@text	See Box2D documentation.
	
	@in		MOAIBox2DRevoluteJoint self
	@out	boolean motorEnabled
*/
mrb_value MOAIBox2DRevoluteJoint::_isMotorEnabled ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DRevoluteJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2RevoluteJoint* joint = ( b2RevoluteJoint* )self->mJoint;
	
	return state.ToRValue ( joint->IsMotorEnabled () );
}

//----------------------------------------------------------------//
/**	@lua	setLimit
	@text	See Box2D documentation.
	
	@in		MOAIBox2DRevoluteJoint self
	@opt	number lower			in degrees, converted to radians. Default value is 0.
	@opt	number upper			in degrees, converted to radians. Default value is 0.
	@out	nil
*/
mrb_value MOAIBox2DRevoluteJoint::_setLimit ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DRevoluteJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	float lower	= state.GetParamValue < float >( 1, 0.0f );
	float upper	= state.GetParamValue < float >( 2, 0.0f );
	
	b2RevoluteJoint* joint = ( b2RevoluteJoint* )self->mJoint;
	joint->SetLimits ( lower * ( float )D2R, upper * ( float )D2R );
	joint->EnableLimit ( true );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setLimitEnabled
	@text	See Box2D documentation.
	
	@in		MOAIBox2DRevoluteJoint self
	@opt	boolean enabled			Default value is 'true'
	@out	nil
*/
mrb_value MOAIBox2DRevoluteJoint::_setLimitEnabled ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DRevoluteJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	bool enabled = state.GetParamValue < bool >( 1, true );
	
	b2RevoluteJoint* joint = ( b2RevoluteJoint* )self->mJoint;
	joint->EnableLimit ( enabled );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setMaxMotorTorque
	@text	See Box2D documentation.
	
	@in		MOAIBox2DRevoluteJoint self
	@opt	number maxMotorTorque		in (kg * units / s^2) * units, converted to N-m. Default value is 0.
	@out	nil
*/
mrb_value MOAIBox2DRevoluteJoint::_setMaxMotorTorque ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DRevoluteJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float unitsToMeters = self->GetUnitsToMeters ();
	/* Convert to N-m (kg m / s^2) * m from (kg unit / s^2) * unit */
	float maxMotorTorque = state.GetParamValue < float >( 1, 0.0f ) * unitsToMeters * unitsToMeters;

	b2RevoluteJoint* joint = ( b2RevoluteJoint* )self->mJoint;
	joint->SetMaxMotorTorque ( maxMotorTorque );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setMotor
	@text	See Box2D documentation.
			If speed is determined to be zero, the motor is disabled, unless forceEnable is set.

	@in		MOAIBox2DRevoluteJoint self
	@opt	number speed			in degrees/s, converted to radians/s. Default value is 0.
	@opt	number maxMotorTorque		in (kg * units / s^2) * units, converted to N-m. Default value is 0.
	@opt	boolean forceEnable		Default value is false.
	@out	nil
*/
mrb_value MOAIBox2DRevoluteJoint::_setMotor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DRevoluteJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	float unitsToMeters = self->GetUnitsToMeters ();
	float speed	= state.GetParamValue < float >( 1, 0.0f );
	float max	= state.GetParamValue < float >( 2, 0.0f );
	bool forceEnable = state.GetParamValue < bool >( 3, false );

	b2RevoluteJoint* joint = ( b2RevoluteJoint* )self->mJoint;
	joint->SetMotorSpeed ( speed * ( float )D2R );
	/* Convert from N-m (kg m / s^2) * m => (kg unit / s^2) * unit */
	joint->SetMaxMotorTorque ( max * unitsToMeters * unitsToMeters );
	joint->EnableMotor ( forceEnable ? true : ( speed != 0.0f ) );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setMotorSpeed
	@text	See Box2D documentation.

	@in		MOAIBox2DRevoluteJoint self
	@opt	number motorSpeed		in degrees/s, converted to radians/s. Default value is 0.
	@out	nil

 */
mrb_value MOAIBox2DRevoluteJoint::_setMotorSpeed ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DRevoluteJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float speed = state.GetParamValue < float >( 1, 0.0f ) * ( float )D2R;

	b2RevoluteJoint* joint = ( b2RevoluteJoint* )self->mJoint;
	joint->SetMotorSpeed ( speed );

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setMotorEnabled
	@text	See Box2D documentation.
	
	@in		MOAIBox2DRevoluteJoint self
	@opt	boolean enabled			Default value is 'true'
	@out	nil
*/
mrb_value MOAIBox2DRevoluteJoint::_setMotorEnabled ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DRevoluteJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	bool enabled = state.GetParamValue < bool >( 1, true );
	
	b2RevoluteJoint* joint = ( b2RevoluteJoint* )self->mJoint;
	joint->EnableMotor ( enabled );
	
	return mrb_nil_value ();
}

//================================================================//
// MOAIBox2DRevoluteJoint
//================================================================//

//----------------------------------------------------------------//
MOAIBox2DRevoluteJoint::MOAIBox2DRevoluteJoint () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIBox2DJoint )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIBox2DRevoluteJoint::~MOAIBox2DRevoluteJoint () {
}

//----------------------------------------------------------------//
void MOAIBox2DRevoluteJoint::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIBox2DRevoluteJoint::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getJointAngle",				_getJointAngle, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getJointSpeed",				_getJointSpeed, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getLowerLimit",				_getLowerLimit, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getMotorSpeed",				_getMotorSpeed, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getMotorTorque",				_getMotorTorque, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getUpperLimit",				_getUpperLimit, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isLimitEnabled",				_isLimitEnabled, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isMotorEnabled",				_isMotorEnabled, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setLimit",					_setLimit, MRB_ARGS_ARG ( 0, 2 ) );
	state.DefineInstanceMethod ( klass, "setLimitEnabled",			_setLimitEnabled, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setMaxMotorTorque",			_setMaxMotorTorque, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setMotor",					_setMotor, MRB_ARGS_ARG ( 0, 3 ) );
	state.DefineInstanceMethod ( klass, "setMotorSpeed",				_setMotorSpeed, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setMotorEnabled",			_setMotorEnabled, MRB_ARGS_ARG ( 0, 1 ) );

}
