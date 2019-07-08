// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <Box2D/Box2D.h>
#include <moai-box2d/MOAIBox2DArbiter.h>
#include <moai-box2d/MOAIBox2DBody.h>
#include <moai-box2d/MOAIBox2DPulleyJoint.h>
#include <moai-box2d/MOAIBox2DWorld.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getGroundAnchorA
	@text	See Box2D documentation.
	
	@in		MOAIBox2DPulleyJoint self
	@out	number x					in units, world coordinates, converted from meters
	@out	number y					in units, world coordinates, converted from meters
*/
mrb_value MOAIBox2DPulleyJoint::_getGroundAnchorA ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPulleyJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2PulleyJoint* joint = ( b2PulleyJoint* )self->mJoint;
	
	b2Vec2 groundAnchorA = joint->GetGroundAnchorA ();
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( groundAnchorA.x / unitsToMeters );
	ret [ 1 ] = state.ToRValue ( groundAnchorA.y / unitsToMeters );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getGroundAnchorB
	@text	See Box2D documentation.
	
	@in		MOAIBox2DPulleyJoint self
	@out	number x					in units, world coordinates, converted from meters
	@out	number y					in units, world coordinates, converted from meters
*/
mrb_value MOAIBox2DPulleyJoint::_getGroundAnchorB ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPulleyJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2PulleyJoint* joint = ( b2PulleyJoint* )self->mJoint;
	
	b2Vec2 groundAnchorB = joint->GetGroundAnchorB ();
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( groundAnchorB.x / unitsToMeters );
	ret [ 1 ] = state.ToRValue ( groundAnchorB.y / unitsToMeters );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getLength1
	@text	See Box2D documentation.
	
	@in		MOAIBox2DPulleyJoint self
	@out	number length1				in units, converted from meters.
*/
mrb_value MOAIBox2DPulleyJoint::_getLength1 ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPulleyJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2PulleyJoint* joint = ( b2PulleyJoint* )self->mJoint;

	return state.ToRValue ( joint->GetLengthA () / unitsToMeters );
}

//----------------------------------------------------------------//
/**	@lua	getLength2
	@text	See Box2D documentation.
	
	@in		MOAIBox2DPulleyJoint self
	@out	number length2				in units, converted from meters.
*/
mrb_value MOAIBox2DPulleyJoint::_getLength2 ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPulleyJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2PulleyJoint* joint = ( b2PulleyJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetLengthB () / unitsToMeters );
}

//----------------------------------------------------------------//
/**	@lua	getRatio
	@text	See Box2D documentation.
	
	@in		MOAIBox2DPulleyJoint self
	@out	number ratio
*/
mrb_value MOAIBox2DPulleyJoint::_getRatio ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DPulleyJoint, "U" )

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}

	b2PulleyJoint* joint = ( b2PulleyJoint* )self->mJoint;
	
	return state.ToRValue ( joint->GetRatio () );
}

//================================================================//
// MOAIBox2DPulleyJoint
//================================================================//

//----------------------------------------------------------------//
MOAIBox2DPulleyJoint::MOAIBox2DPulleyJoint () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIBox2DJoint )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIBox2DPulleyJoint::~MOAIBox2DPulleyJoint () {
}

//----------------------------------------------------------------//
void MOAIBox2DPulleyJoint::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIBox2DPulleyJoint::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DJoint::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getGroundAnchorA",		_getGroundAnchorA, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getGroundAnchorB",		_getGroundAnchorB, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getLength1",				_getLength1, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getLength2",				_getLength2, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getRatio",				_getRatio, MRB_ARGS_NONE () );

}
