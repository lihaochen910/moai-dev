// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <Box2D/Box2D.h>
#include <moai-box2d/MOAIBox2DArbiter.h>
#include <moai-box2d/MOAIBox2DBody.h>
#include <moai-box2d/MOAIBox2DPrismaticJoint.h>
#include <moai-box2d/MOAIBox2DWorld.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getJointSpeed
	@text	See Box2D documentation.
	
	@in		MOAIBox2DPrismaticJoint self
	@out	number jointSpeed				in units/s, converted from m/s
*/
mrb_value MOAIBox2DPrismaticJoint::_getJointSpeed ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPrismaticJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2PrismaticJoint* joint = ( b2PrismaticJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetJointSpeed () / unitsToMeters );
}

//----------------------------------------------------------------//
/**	@lua	getJointTranslation
	@text	See Box2D documentation.
	
	@in		MOAIBox2DPrismaticJoint self
	@out	number jointTranslation			in units, converted from meters.
*/
mrb_value MOAIBox2DPrismaticJoint::_getJointTranslation ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPrismaticJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2PrismaticJoint* joint = ( b2PrismaticJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetJointTranslation () / unitsToMeters );
}

//----------------------------------------------------------------//
/**	@lua	getLowerLimit
	@text	See Box2D documentation.
	
	@in		MOAIBox2DPrismaticJoint self
	@out	number lowerLimit				in units, converted from meters.
*/
mrb_value MOAIBox2DPrismaticJoint::_getLowerLimit ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPrismaticJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2PrismaticJoint* joint = ( b2PrismaticJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetLowerLimit () / unitsToMeters );
}

//----------------------------------------------------------------//
/**	@lua	getMotorForce
	@text	See Box2D documentation.
	
	@in		MOAIBox2DPrismaticJoint self
	@out	number motorForce				in kg * units / s^2, converted from N [kg * m / s^2]
*/
mrb_value MOAIBox2DPrismaticJoint::_getMotorForce ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPrismaticJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2PrismaticJoint* joint = ( b2PrismaticJoint* )self->mJoint;

	float step = ( float )( 1.0 / MOAISim::Get ().GetStep ());
	
	return state.ToRValue ( joint->GetMotorForce (step) / unitsToMeters );
}

//----------------------------------------------------------------//
/**	@lua	getMotorSpeed
	@text	See Box2D documentation.
	
	@in		MOAIBox2DPrismaticJoint self
	@out	number motorSpeed				in units/s, converted from m/s
*/
mrb_value MOAIBox2DPrismaticJoint::_getMotorSpeed ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPrismaticJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2PrismaticJoint* joint = ( b2PrismaticJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetMotorSpeed () / unitsToMeters );
}

//----------------------------------------------------------------//
/**	@lua	getUpperLimit
	@text	See Box2D documentation.
	
	@in		MOAIBox2DPrismaticJoint self
	@out	number upperLimit				in units, converted from meters.
*/
mrb_value MOAIBox2DPrismaticJoint::_getUpperLimit ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPrismaticJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2PrismaticJoint* joint = ( b2PrismaticJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetUpperLimit () / unitsToMeters );
}

//----------------------------------------------------------------//
/**	@lua	isLimitEnabled
	@text	See Box2D documentation.
	
	@in		MOAIBox2DPrismaticJoint self
	@out	boolean limitEnabled
*/
mrb_value MOAIBox2DPrismaticJoint::_isLimitEnabled ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPrismaticJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2PrismaticJoint* joint = ( b2PrismaticJoint* )self->mJoint;
	
	return state.ToRValue ( joint->IsLimitEnabled () );
}

//----------------------------------------------------------------//
/**	@lua	isMotorEnabled
	@text	See Box2D documentation.
	
	@in		MOAIBox2DPrismaticJoint self
	@out	boolean motorEnabled
*/
mrb_value MOAIBox2DPrismaticJoint::_isMotorEnabled ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPrismaticJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2PrismaticJoint* joint = ( b2PrismaticJoint* )self->mJoint;
	
	return state.ToRValue ( joint->IsMotorEnabled () );
}

//----------------------------------------------------------------//
/**	@lua	setLimit
	@text	See Box2D documentation.
	
	@in		MOAIBox2DPrismaticJoint self
	@opt	number lower			in units, converted to meters. Default value is 0.
	@opt	number upper			in units, converted to meters. Default value is 0.
	@out	nil
*/
mrb_value MOAIBox2DPrismaticJoint::_setLimit ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPrismaticJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	float lower	= state.GetParamValue < float >( 1, 0.0f );
	float upper	= state.GetParamValue < float >( 2, 0.0f );
	
	float unitsToMeters = self->GetUnitsToMeters ();
	b2PrismaticJoint* joint = ( b2PrismaticJoint* )self->mJoint;
	
	joint->SetLimits ( lower * unitsToMeters, upper * unitsToMeters );
	joint->EnableLimit ( true );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setLimitEnabled
	@text	See Box2D documentation.
	
	@in		MOAIBox2DPrismaticJoint self
	@opt	boolean enabled			Default value is 'true'
	@out	nil
*/
mrb_value MOAIBox2DPrismaticJoint::_setLimitEnabled ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPrismaticJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	bool enabled = state.GetParamValue < bool >( 1, true );
	
	b2PrismaticJoint* joint = ( b2PrismaticJoint* )self->mJoint;
	joint->EnableLimit ( enabled );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setMaxMotorForce
	@text	See Box2D documentation.
	
	@in		MOAIBox2DPrismaticJoint self
	@opt	number maxMotorForce		in kg * units / s^2, converted to N [kg * m / s^2]. Default value is 0.
	@out	nil
*/
mrb_value MOAIBox2DPrismaticJoint::_setMaxMotorForce ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPrismaticJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float maxMotorForce = state.GetParamValue < float >( 1, 0.0f );

	b2PrismaticJoint* joint = ( b2PrismaticJoint* )self->mJoint;
	joint->SetMaxMotorForce ( maxMotorForce * unitsToMeters );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setMotor
	@text	See Box2D documentation.
			If speed is determined to be zero, the motor is disabled, unless forceEnable is set.
	
	@in		MOAIBox2DPrismaticJoint self
	@opt	number speed			in units/s converted to m/s. Default value is 0.
	@opt	number maxForce			in kg * units / s^2, converted to N [kg * m / s^2]. Default value is 0.
	@opt	boolean forceEnable		Default value is false.
	@out	nil
*/
mrb_value MOAIBox2DPrismaticJoint::_setMotor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPrismaticJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	float speed	= state.GetParamValue < float >( 1, 0.0f );
	float max	= state.GetParamValue < float >( 2, 0.0f );
	bool forceEnable = state.GetParamValue < bool >( 3, false );
	
	float unitsToMeters = self->GetUnitsToMeters ();
	b2PrismaticJoint* joint = ( b2PrismaticJoint* )self->mJoint;
	
	joint->SetMotorSpeed ( speed * unitsToMeters );
	joint->SetMaxMotorForce ( max * unitsToMeters );
	joint->EnableMotor ( forceEnable ? true : ( speed != 0.0f ) );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setMotorEnabled
	@text	See Box2D documentation.
	
	@in		MOAIBox2DPrismaticJoint self
	@opt	boolean enabled			Default value is 'true'
	@out	nil
*/
mrb_value MOAIBox2DPrismaticJoint::_setMotorEnabled ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPrismaticJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	bool enabled = state.GetParamValue < bool >( 1, true );
	
	b2PrismaticJoint* joint = ( b2PrismaticJoint* )self->mJoint;
	joint->EnableMotor ( enabled );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setMotorSpeed
	@text	See Box2D documentation.

	@in		MOAIBox2DPrismaticJoint self
	@opt	number motorSpeed		in units/s, converted to m/s. Default value is 0.
	@out	nil

 */
mrb_value MOAIBox2DPrismaticJoint::_setMotorSpeed ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPrismaticJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float unitsToMeters = self->GetUnitsToMeters ();
	float speed = state.GetParamValue < float >( 1, 0.0f ) * unitsToMeters;

	b2PrismaticJoint* joint = ( b2PrismaticJoint* )self->mJoint;
	joint->SetMotorSpeed ( speed );

	return mrb_nil_value ();
}


//================================================================//
// MOAIBox2DPrismaticJoint
//================================================================//

//----------------------------------------------------------------//
MOAIBox2DPrismaticJoint::MOAIBox2DPrismaticJoint () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIBox2DJoint )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIBox2DPrismaticJoint::~MOAIBox2DPrismaticJoint () {
}

//----------------------------------------------------------------//
void MOAIBox2DPrismaticJoint::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIBox2DPrismaticJoint::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getJointSpeed",				_getJointSpeed, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getJointTranslation",		_getJointTranslation, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getLowerLimit",				_getLowerLimit, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getMotorForce",				_getMotorForce, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getMotorSpeed",				_getMotorSpeed, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getUpperLimit",				_getUpperLimit, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isLimitEnabled",				_isLimitEnabled, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isMotorEnabled",				_isMotorEnabled, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setLimit",					_setLimit, MRB_ARGS_ARG ( 0, 2 ) );
	state.DefineInstanceMethod ( klass, "setLimitEnabled",			_setLimitEnabled, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setMaxMotorForce",			_setMaxMotorForce, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setMotor",					_setMotor, MRB_ARGS_ARG ( 0, 3 ) );
	state.DefineInstanceMethod ( klass, "setMotorSpeed",				_setMotorSpeed, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setMotorEnabled",			_setMotorEnabled, MRB_ARGS_ARG ( 0, 1 ) );

}
