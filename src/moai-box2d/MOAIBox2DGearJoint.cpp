// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <Box2D/Box2D.h>
#include <moai-box2d/MOAIBox2DArbiter.h>
#include <moai-box2d/MOAIBox2DBody.h>
#include <moai-box2d/MOAIBox2DGearJoint.h>
#include <moai-box2d/MOAIBox2DWorld.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getJointA
	@text	See Box2D documentation.
	
	@in		MOAIBox2DGearJoint self
	@out	MOAIBox2DJoint jointA
*/
mrb_value MOAIBox2DGearJoint::_getJointA ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DGearJoint, "U" )

	return self->mJointA->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	getJointB
	@text	See Box2D documentation.
	
	@in		MOAIBox2DGearJoint self
	@out	MOAIBox2DJoint jointB
*/
mrb_value MOAIBox2DGearJoint::_getJointB ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DGearJoint, "U" )

	return self->mJointB->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	getRatio
	@text	See Box2D documentation.
	
	@in		MOAIBox2DGearJoint self
	@out	number ratio
*/
mrb_value MOAIBox2DGearJoint::_getRatio ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DGearJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2GearJoint* joint = ( b2GearJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetRatio ());
}

//----------------------------------------------------------------//
/**	@lua	setRatio
	@text	See Box2D documentation.
	
	@in		MOAIBox2DGearJoint self
	@opt	number ratio		Default value is 0.
	@out	nil
*/
mrb_value MOAIBox2DGearJoint::_setRatio ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DGearJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	float ratio = state.GetParamValue < float >( 2, 0.0f );

	b2GearJoint* joint = ( b2GearJoint* )self->mJoint;
	joint->SetRatio ( ratio );
	
	return mrb_nil_value ();
}

//================================================================//
// MOAIBox2DGearJoint
//================================================================//

//----------------------------------------------------------------//
MOAIBox2DGearJoint::MOAIBox2DGearJoint () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIBox2DJoint )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIBox2DGearJoint::~MOAIBox2DGearJoint () {

	this->mJointA.Set ( *this, 0 );
	this->mJointB.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAIBox2DGearJoint::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIBox2DGearJoint::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getJointA",				_getJointA, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getJointB",				_getJointB, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getRatio",				_getRatio, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setRatio",				_setRatio, MRB_ARGS_ARG ( 0, 1 ) );
	
}
