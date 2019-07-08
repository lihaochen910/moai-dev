// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <Box2D/Box2D.h>
#include <moai-box2d/MOAIBox2DArbiter.h>
#include <moai-box2d/MOAIBox2DBody.h>
#include <moai-box2d/MOAIBox2DJoint.h>
#include <moai-box2d/MOAIBox2DWorld.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	destroy
	@text	Schedule joint for destruction.
	
	@in		MOAIBox2DJoint self
	@out	nil
*/
mrb_value MOAIBox2DJoint::_destroy ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DJoint, "U" )
	
	if ( self->mWorld ) {
		self->mWorld->ScheduleDestruction ( *self );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getAnchorA
	@text	See Box2D documentation.
	
	@in		MOAIBox2DJoint self
	@out	number anchorX		in units, in world coordinates, converted to meters
	@out	number anchorY		in units, in world coordinates, converted to meters
*/
mrb_value MOAIBox2DJoint::_getAnchorA ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2Vec2 anchor = self->mJoint->GetAnchorA ();

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( anchor.x / unitsToMeters );
	ret [ 1 ] = state.ToRValue ( anchor.y / unitsToMeters );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getAnchorB
	@text	See Box2D documentation.
	
	@in		MOAIBox2DJoint self
	@out	number anchorX		in units, in world coordinates, converted from meters
	@out	number anchorY		in units, in world coordinates, converted from meters
*/
mrb_value MOAIBox2DJoint::_getAnchorB ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2Vec2 anchor = self->mJoint->GetAnchorB ();

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( anchor.x / unitsToMeters );
	ret [ 1 ] = state.ToRValue ( anchor.y / unitsToMeters );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getBodyA
	@text	See Box2D documentation.
	
	@in		MOAIBox2DJoint self
	@out	MOAIBox2DBody body
*/
mrb_value MOAIBox2DJoint::_getBodyA ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	MOAIBox2DBody* body = ( MOAIBox2DBody* )self->mJoint->GetBodyA ()->GetUserData ();
	return body->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	getBodyB
	@text	See Box2D documentation.
	
	@in		MOAIBox2DJoint self
	@out	MOAIBox2DBody body
*/
mrb_value MOAIBox2DJoint::_getBodyB ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	MOAIBox2DBody* body = ( MOAIBox2DBody* )self->mJoint->GetBodyB ()->GetUserData ();
	return body->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	getReactionForce
	@text	See Box2D documentation.
	
	@in		MOAIBox2DJoint self
	@out	number forceX	in kg * units / s^2 converted from N [kg * m / s^2]
	@out	number forceY	in kg * units / s^2 converted from N [kg * m / s^2]
*/
mrb_value MOAIBox2DJoint::_getReactionForce ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();

	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	float step = ( float )( 1.0 / MOAISim::Get ().GetStep ());
	
	b2Vec2 force = self->mJoint->GetReactionForce ( step );

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( force.x / unitsToMeters );
	ret [ 1 ] = state.ToRValue ( force.y / unitsToMeters );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getReactionTorque
	@text	See Box2D documentation.
	
	@in		MOAIBox2DJoint self
	@out	number reactionTorque	in (kg * units / s^2) * units, converted from N-m.
*/
mrb_value MOAIBox2DJoint::_getReactionTorque ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DJoint_MissingInstance );
		return mrb_nil_value ();
	}
	
	float step = ( float )( 1.0 / MOAISim::Get ().GetStep ());
	
	/* Convert from N-m (kg m / s^2) * m to (kg unit / s^2) * unit */
	float unitsToMeters = self->GetUnitsToMeters();
	float torque = self->mJoint->GetReactionTorque ( step );
	
	return state.ToRValue ( torque / ( unitsToMeters * unitsToMeters ) );
}

//================================================================//
// MOAIBox2DJoint
//================================================================//

//----------------------------------------------------------------//
void MOAIBox2DJoint::Clear () {
	
	if ( this->mJoint ) {
	
		b2Body* b2BodyA = this->mJoint->GetBodyA ();
		b2Body* b2BodyB = this->mJoint->GetBodyB ();
	
		if ( b2BodyA ) {
			MOAIBox2DBody* bodyA = ( MOAIBox2DBody* )b2BodyA->GetUserData ();
			if ( bodyA ) {
				this->RubyRelease ( bodyA );
			}
		}
		
		if ( b2BodyB ) {
			MOAIBox2DBody* bodyB = ( MOAIBox2DBody* )b2BodyB->GetUserData ();
			if ( bodyB ) {
				this->RubyRelease ( bodyB );
			}
		}
		
		this->mJoint = 0;
	}
	this->mWorld = 0;
}

//----------------------------------------------------------------//
void MOAIBox2DJoint::Destroy () {
	
	if ( this->mJoint ) {
		b2World* world = this->mWorld->mWorld;
		world->DestroyJoint ( this->mJoint );
		this->Clear ();
	}
}

//----------------------------------------------------------------//
MOAIBox2DJoint::MOAIBox2DJoint () :
	mJoint ( 0 ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIBox2DJoint::~MOAIBox2DJoint () {
}

//----------------------------------------------------------------//
void MOAIBox2DJoint::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DPrim::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIBox2DJoint::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIBox2DPrim::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "destroy",				_destroy, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getAnchorA",				_getAnchorA, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getAnchorB",				_getAnchorB, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getBodyA",				_getBodyA, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getBodyB",				_getBodyB, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getReactionForce",		_getReactionForce, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getReactionTorque",		_getReactionTorque, MRB_ARGS_NONE () );
}

//----------------------------------------------------------------//
void MOAIBox2DJoint::SetJoint ( b2Joint* joint ) {

	this->mJoint = joint;
	joint->SetUserData ( this );
}
