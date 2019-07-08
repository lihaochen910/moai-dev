// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <Box2D/Box2D.h>
#include <moai-box2d/MOAIBox2DArbiter.h>
#include <moai-box2d/MOAIBox2DBody.h>
#include <moai-box2d/MOAIBox2DMotorJoint.h>
#include <moai-box2d/MOAIBox2DWorld.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	setLinearOffset
	@text	See Box2D documentation.
	
	@in		MOAIBox2DMotorJoint self
	@opt	number x	target linear offset, in frame A, in meters
	@opt	number y	target linear offset, in frame A, in meters
	@out	nil	
*/
mrb_value MOAIBox2DMotorJoint::_setLinearOffset ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMotorJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float unitsToMeters = self->GetUnitsToMeters();

	float x = state.GetParamValue < float >( 1, 0.0f );
	float y = state.GetParamValue < float >( 2, 0.0f );

	b2MotorJoint* joint = ( b2MotorJoint* )self->mJoint;
	b2Vec2 offset( x * unitsToMeters, y * unitsToMeters );
	joint->SetLinearOffset( offset );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getLinearOffset
	@text	See Box2D documentation.
	
	@in		MOAIBox2DMotorJoint self
	@out	number x	target linear offset, in frame A, in meters 
	@out	number y	target linear offset, in frame A, in meters
*/

mrb_value MOAIBox2DMotorJoint::_getLinearOffset ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMotorJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float unitsToMeters = self->GetUnitsToMeters();

	b2MotorJoint* joint = ( b2MotorJoint* )self->mJoint;
	const b2Vec2& offset = joint->GetLinearOffset();
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( offset.x / unitsToMeters );
	ret [ 1 ] = state.ToRValue ( offset.y / unitsToMeters );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	setAngularOffset
	@text	See Box2D documentation.
	
	@in		MOAIBox2DMotorJoint self
	@opt	number angle	in degrees
	@out	nil	
*/
mrb_value	MOAIBox2DMotorJoint::_setAngularOffset ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMotorJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float angle = state.GetParamValue < float >( 1, 0.0f );

	b2MotorJoint* joint = ( b2MotorJoint* )self->mJoint;
	joint->SetAngularOffset( angle * ( float )D2R );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getAngularOffset
	@text	See Box2D documentation.
	
	@in		MOAIBox2DMotorJoint self
	@out	number angle	in degrees, converted from radians
*/
mrb_value	MOAIBox2DMotorJoint::_getAngularOffset ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMotorJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2MotorJoint* joint = ( b2MotorJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetAngularOffset () * ( float )R2D );
}

//----------------------------------------------------------------//
/**	@lua	setMaxForce
	@text	See Box2D documentation.
	
	@in		MOAIBox2DMotorJoint self
	@opt	number force	maximum friction force in kg * units / s^2, converted to N [kg * m / s^2]
	@out	nil
*/
mrb_value	MOAIBox2DMotorJoint::_setMaxForce ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMotorJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float unitsToMeters = self->GetUnitsToMeters();
	float force = state.GetParamValue < float >( 2, 0.0f ) * unitsToMeters;

	b2MotorJoint* joint = ( b2MotorJoint* )self->mJoint;
	joint->SetMaxForce( force );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getMaxForce
	@text	See Box2D documentation.
	
	@in		MOAIBox2DMotorJoint self
	@out	number force	maximum friction force in kg * units / s^2, converted from N [kg * m / s^2].
*/
mrb_value	MOAIBox2DMotorJoint::_getMaxForce ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMotorJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float unitsToMeters = self->GetUnitsToMeters ();

	b2MotorJoint* joint = ( b2MotorJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetMaxForce () / unitsToMeters );
}

//----------------------------------------------------------------//
/** @lua	setMaxTorque
    @text	See Box2D documentation.
    
	@in		MOAIBox2DMotorJoint self
    @opt	number	torque	maximum friction torque in kg * units / s^2 * units, converted to N-m [kg * m / s^2 * m].		
	@out	nil
*/
mrb_value	MOAIBox2DMotorJoint::_setMaxTorque ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMotorJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float unitsToMeters = self->GetUnitsToMeters();
	/* Convert to/from N-m (kg m / s^2) * m from/to (kg unit / s^2) * unit */
	float torque = state.GetParamValue < float >( 2, 0.0f ) * unitsToMeters * unitsToMeters;

	b2MotorJoint* joint = ( b2MotorJoint* )self->mJoint;
	joint->SetMaxTorque( torque );

	return mrb_nil_value ();
}
 
//----------------------------------------------------------------//
/** @lua	getMaxTorque
    @text	See Box2D documentation.
    
	@in		MOAIBox2DMotorJoint self
    @out	number	torque	maximum friction torque in (kg * units / s^2) * units, converted from N-m.		
*/
mrb_value MOAIBox2DMotorJoint::_getMaxTorque ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMotorJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2MotorJoint* joint = ( b2MotorJoint* )self->mJoint;
	float unitsToMeters = self->GetUnitsToMeters();
	/* Convert to/from N-m (kg m / s^2) * m from/to (kg unit / s^2) * unit */
	
	return state.ToRValue ( joint->GetMaxTorque () / ( unitsToMeters * unitsToMeters ) );
}

//----------------------------------------------------------------//
/** @lua	setCorrectionFactor
    @text	See Box2D documentation.
    
	@in		MOAIBox2DMotorJoint self
    @opt	number	factor		position correction factor in the range [0,1]
	@out	nil
*/
mrb_value MOAIBox2DMotorJoint::_setCorrectionFactor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMotorJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float factor = state.GetParamValue < float >( 2, 0.0f );

	b2MotorJoint* joint = ( b2MotorJoint* )self->mJoint;
	joint->SetCorrectionFactor( factor );
		
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/** @lua	getCorrectionFactor
    @text	See Box2D documentation.
    
	@in		MOAIBox2DMotorJoint self
    @out	number	factor		position correction factor in the range [0,1]
*/
mrb_value MOAIBox2DMotorJoint::_getCorrectionFactor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DMotorJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2MotorJoint* joint = ( b2MotorJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetCorrectionFactor () );
}

//================================================================//
// MOAIBox2DRevoluteJoint
//================================================================//

//----------------------------------------------------------------//
MOAIBox2DMotorJoint::MOAIBox2DMotorJoint () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIBox2DJoint )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIBox2DMotorJoint::~MOAIBox2DMotorJoint () {
}

//----------------------------------------------------------------//
void MOAIBox2DMotorJoint::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIBox2DMotorJoint::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "setLinearOffset", 		_setLinearOffset, MRB_ARGS_ARG ( 0, 2 ) );
	state.DefineInstanceMethod ( klass, "getLinearOffset", 		_getLinearOffset, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setAngularOffset", 		_setAngularOffset, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "getAngularOffset", 		_getAngularOffset, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setMaxForce", 			_setMaxForce, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "getMaxForce", 			_getMaxForce, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setMaxTorque", 			_setMaxTorque, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "getMaxTorque", 			_getMaxTorque, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setCorrectionFactor",	_setCorrectionFactor, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "getCorrectionFactor", 	_getCorrectionFactor, MRB_ARGS_NONE () );

}
