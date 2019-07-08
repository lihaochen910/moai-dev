// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <Box2D/Box2D.h>
#include <moai-box2d/MOAIBox2DArbiter.h>
#include <moai-box2d/MOAIBox2DBody.h>
#include <moai-box2d/MOAIBox2DWheelJoint.h>
#include <moai-box2d/MOAIBox2DWorld.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//

//================================================================//
// MOAIBox2DWheelJoint
//================================================================//

//----------------------------------------------------------------//
MOAIBox2DWheelJoint::MOAIBox2DWheelJoint () {
	
	RTTI_BEGIN
	RTTI_EXTEND ( MOAIBox2DJoint )
	RTTI_END
}

//----------------------------------------------------------------//
/**	@lua	getJointTranslation
	@text	See Box2D documentation.
	
	@in		MOAIBox2DWheelJoint self
	@out	number jointTranslation		in units, converted from meters
 */
mrb_value MOAIBox2DWheelJoint::_getJointTranslation ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWheelJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2WheelJoint* joint = ( b2WheelJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetJointTranslation () / unitsToMeters );
}

//----------------------------------------------------------------//
/**	@lua	getJointSpeed
	@text	See Box2D documentation.
	
	@in		MOAIBox2DWheelJoint self
	@out	number jointSpeed			in units / s, converted from m/s
 */
mrb_value MOAIBox2DWheelJoint::_getJointSpeed ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWheelJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2WheelJoint* joint = ( b2WheelJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetJointSpeed () / unitsToMeters );
}
//----------------------------------------------------------------//
/**	@lua	isMotorEnabled
 @text	See Box2D documentation.
 
 @in		MOAIBox2DWheelJoint self
 @out	boolean motorEnabled
 */
mrb_value MOAIBox2DWheelJoint::_isMotorEnabled ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWheelJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2WheelJoint* joint = ( b2WheelJoint* )self->mJoint;
	
	return state.ToRValue ( joint->IsMotorEnabled () );
}

//----------------------------------------------------------------//
/**	@lua	getMotorSpeed
	@text	See Box2D documentation.
	
	@in		MOAIBox2DWheelJoint self
	@out	number motorSpeed			in degrees/s, converted from radians/s
 */
mrb_value MOAIBox2DWheelJoint::_getMotorSpeed ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWheelJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2WheelJoint* joint = ( b2WheelJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetMotorSpeed () * ( float )R2D );
}

//----------------------------------------------------------------//
/**	@lua	getSpringDampingRatio
	@text	See Box2D documentation.
	
	@in		MOAIBox2DWheelJoint self
	@out	number dampingRatio
 */
mrb_value MOAIBox2DWheelJoint::_getSpringDampingRatio ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWheelJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2WheelJoint* joint = ( b2WheelJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetSpringDampingRatio () );
}

//----------------------------------------------------------------//
/**	@lua	getMaxMotorTorque
 @text	See Box2D documentation.
 
 @in		MOAIBox2DWheelJoint self
 @out	number maxMotorTorque		in (kg * units / s^2) * units, converted from N-m.
 */
mrb_value MOAIBox2DWheelJoint::_getMaxMotorTorque ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWheelJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2WheelJoint* joint = ( b2WheelJoint* )self->mJoint;
	/* Convert from N-m (kg m / s^2) * m to (kg unit / s^2) * unit */

	return state.ToRValue ( joint->GetMaxMotorTorque () / ( unitsToMeters * unitsToMeters ) );
}

//----------------------------------------------------------------//
/**	@lua	getMotorTorque
 @text	See Box2D documentation.
 
 @in		MOAIBox2DWheelJoint self
 @out	number torque		in (kg * units / s^2) * units, converted from N-m.
 */
mrb_value MOAIBox2DWheelJoint::_getMotorTorque ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWheelJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2WheelJoint* joint = ( b2WheelJoint* )self->mJoint;

	float step = ( float )( 1.0 / MOAISim::Get ().GetStep ());
	/* Convert from N-m (kg m / s^2) * m => (kg unit / s^2) * unit */
	
	return state.ToRValue ( joint->GetMotorTorque (step) / ( unitsToMeters * unitsToMeters ) );
}

//----------------------------------------------------------------//
/**	@lua	getSpringFrequencyHz
	@text	See Box2D documentation.
	
	@in		MOAIBox2DWheelJoint self
	@out	number springFrequency			in Hz
 */
mrb_value MOAIBox2DWheelJoint::_getSpringFrequencyHz ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWheelJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2WheelJoint* joint = ( b2WheelJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetSpringFrequencyHz () );
}

//----------------------------------------------------------------//
/**	@lua	setMotor
	@text	See Box2D documentation.
			If speed is determined to be zero, the motor is disabled, unless forceEnable is set.

	@in		MOAIBox2DWheelJoint self
	@opt	number speed			in degrees/s, converted to radians/s. Default value is 0.
	@opt	number maxMotorTorque		in (kg * units / s^2) * units, converted from N-m. Default value is 0.
	@opt	boolean forceEnable		Default value is false.
	@out	nil
*/
mrb_value MOAIBox2DWheelJoint::_setMotor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWheelJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	float unitsToMeters = self->GetUnitsToMeters ();
	float speed	= state.GetParamValue < float >( 1, 0.0f );
	float max	= state.GetParamValue < float >( 2, 0.0f );
	bool forceEnable = state.GetParamValue < bool >( 3, false );
	
	b2WheelJoint* joint = ( b2WheelJoint* )self->mJoint;
	joint->SetMotorSpeed ( speed * ( float )D2R );
	/* Convert from N-m (kg m / s^2) * m => (kg unit / s^2) * unit */
	joint->SetMaxMotorTorque ( max * unitsToMeters * unitsToMeters );
	joint->EnableMotor ( forceEnable ? true : ( speed != 0.0f ) );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setMotorSpeed
	@text	See Box2D documentation.
 
	@in		MOAIBox2DWheelJoint self
	@opt	number motorSpeed		in degrees/s, converted to radians/s. Default value is 0.
	@out	nil

 */
mrb_value MOAIBox2DWheelJoint::_setMotorSpeed ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWheelJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float speed = state.GetParamValue < float >( 1, 0.0f ) * ( float )D2R;

	b2WheelJoint* joint = ( b2WheelJoint* )self->mJoint;
	joint->SetMotorSpeed ( speed );

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setMotorEnabled
 @text	See Box2D documentation.
 
 @in		MOAIBox2DWheelJoint self
 @opt	boolean enabled			Default value is 'true'
 @out	nil
 */
mrb_value MOAIBox2DWheelJoint::_setMotorEnabled ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWheelJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	bool enabled = state.GetParamValue < bool >( 1, true );
	
	b2WheelJoint* joint = ( b2WheelJoint* )self->mJoint;
	joint->EnableMotor ( enabled );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setMaxMotorTorque
 @text	See Box2D documentation.
 
 @in		MOAIBox2DWheelJoint self
 @opt	number maxMotorTorque		in (kg * units / s^2) * units, converted to N-m. Default value is 0.
 @out	nil
 */
mrb_value MOAIBox2DWheelJoint::_setMaxMotorTorque ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWheelJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	/* Convert to N-m (kg m / s^2) * m from (kg unit / s^2) * unit */
	float maxMotorTorque = state.GetParamValue < float >( 1, 0.0f ) * unitsToMeters * unitsToMeters;
	
	b2WheelJoint* joint = ( b2WheelJoint* )self->mJoint;
	joint->SetMaxMotorTorque ( maxMotorTorque );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setSpringDampingRatio
 @text	See Box2D documentation.
 
 @in		MOAIBox2DWheelJoint self
 @opt	number dampingRatio		Default value is 0.
 @out	nil
 */
mrb_value MOAIBox2DWheelJoint::_setSpringDampingRatio ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWheelJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	float dampingRatio = state.GetParamValue < float >( 1, 0.0f );
	
	b2WheelJoint* joint = ( b2WheelJoint* )self->mJoint;
	joint->SetSpringDampingRatio( dampingRatio );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setSpringFrequencyHz
 @text	See Box2D documentation.
 
 @in		MOAIBox2DWheelJoint self
 @opt	number springFrequencyHz		in Hz. Default value is 0.
 @out	nil
 */
mrb_value MOAIBox2DWheelJoint::_setSpringFrequencyHz ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWheelJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	float springFrequencyHz = state.GetParamValue < float >( 1, 0.0f );
	
	b2WheelJoint* joint = ( b2WheelJoint* )self->mJoint;
	joint->SetSpringFrequencyHz( springFrequencyHz );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
MOAIBox2DWheelJoint::~MOAIBox2DWheelJoint () {
}

//----------------------------------------------------------------//
void MOAIBox2DWheelJoint::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIBox2DWheelJoint::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getJointTranslation",		_getJointTranslation, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getJointSpeed",				_getJointSpeed, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isMotorEnabled",				_isMotorEnabled, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getMotorSpeed",				_getMotorSpeed, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getSpringDampingRatio",		_getSpringDampingRatio, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getMaxMotorTorque",			_getMaxMotorTorque, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getMotorTorque",				_getMotorTorque, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getSpringFrequencyHz",		_getSpringFrequencyHz, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setMotor",					_setMotor, MRB_ARGS_ARG ( 0, 3 ) );
	state.DefineInstanceMethod ( klass, "setMotorEnabled",			_setMotorEnabled, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setMotorSpeed",				_setMotorSpeed, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setMaxMotorTorque",			_setMaxMotorTorque, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setSpringDampingRatio",		_setSpringDampingRatio, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setSpringFrequencyHz",		_setSpringFrequencyHz, MRB_ARGS_ARG ( 0, 1 ) );

}
