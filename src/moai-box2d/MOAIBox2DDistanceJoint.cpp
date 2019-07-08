// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <Box2D/Box2D.h>
#include <moai-box2d/MOAIBox2DArbiter.h>
#include <moai-box2d/MOAIBox2DBody.h>
#include <moai-box2d/MOAIBox2DDistanceJoint.h>
#include <moai-box2d/MOAIBox2DWorld.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getDampingRatio
	@text	See Box2D documentation.
	
	@in		MOAIBox2DDistanceJoint self
	@out	number dampingRatio
*/
mrb_value MOAIBox2DDistanceJoint::_getDampingRatio ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DDistanceJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2DistanceJoint* joint = ( b2DistanceJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetDampingRatio ());
}

//----------------------------------------------------------------//
/**	@lua	getFrequency
	@text	See Box2D documentation.
	
	@in		MOAIBox2DDistanceJoint self
	@out	number frequency				In Hz.
*/
mrb_value MOAIBox2DDistanceJoint::_getFrequency ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DDistanceJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2DistanceJoint* joint = ( b2DistanceJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetFrequency ());
}

//----------------------------------------------------------------//
/**	@lua	getLength
	@text	See Box2D documentation.
	
	@in		MOAIBox2DDistanceJoint self
	@out	number length					In units, converted from meters.
*/
mrb_value MOAIBox2DDistanceJoint::_getLength ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DDistanceJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2DistanceJoint* joint = ( b2DistanceJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetLength () / unitsToMeters );
}

//----------------------------------------------------------------//
/**	@lua	setDampingRatio
	@text	See Box2D documentation.
	
	@in		MOAIBox2DDistanceJoint self
	@opt	number dampingRatio		Default value is 0.
	@out	nil
*/
mrb_value MOAIBox2DDistanceJoint::_setDampingRatio ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DDistanceJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float dampingRatio = state.GetParamValue < float >( 1, 0.0f );

	b2DistanceJoint* joint = ( b2DistanceJoint* )self->mJoint;
	joint->SetDampingRatio ( dampingRatio );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setFrequency
	@text	See Box2D documentation.
	
	@in		MOAIBox2DDistanceJoint self
	@opt	number frequency		In Hz. Default value is 0.
	@out	nil
*/
mrb_value MOAIBox2DDistanceJoint::_setFrequency ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DDistanceJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float frequency = state.GetParamValue < float >( 1, 0.0f );

	b2DistanceJoint* joint = ( b2DistanceJoint* )self->mJoint;
	joint->SetFrequency ( frequency );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setLength
	@text	See Box2D documentation.
	
	@in		MOAIBox2DDistanceJoint self
	@opt	number length		in units, converted to meters. Default value is 0.
	@out	nil
*/
mrb_value MOAIBox2DDistanceJoint::_setLength ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DDistanceJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float length = state.GetParamValue < float >( 1, 0.0f ) * unitsToMeters;

	b2DistanceJoint* joint = ( b2DistanceJoint* )self->mJoint;
	joint->SetLength ( length );
	
	return mrb_nil_value ();
}

//================================================================//
// MOAIBox2DDistanceJoint
//================================================================//

//----------------------------------------------------------------//
MOAIBox2DDistanceJoint::MOAIBox2DDistanceJoint () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIBox2DJoint )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIBox2DDistanceJoint::~MOAIBox2DDistanceJoint () {
}

//----------------------------------------------------------------//
void MOAIBox2DDistanceJoint::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIBox2DDistanceJoint::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getDampingRatio",		_getDampingRatio, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getFrequency",			_getFrequency, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getLength",				_getLength, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setDampingRatio",		_setDampingRatio, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setFrequency",			_setFrequency, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setLength",				_setLength, MRB_ARGS_ARG ( 0, 1 ) );

}
