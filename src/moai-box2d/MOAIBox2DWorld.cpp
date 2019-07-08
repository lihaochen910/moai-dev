// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <Box2D/Box2D.h>
#include <moai-box2d/MOAIBox2DArbiter.h>
#include <moai-box2d/MOAIBox2DBody.h>
#include <moai-box2d/MOAIBox2DDebugDraw.h>
#include <moai-box2d/MOAIBox2DDistanceJoint.h>
#include <moai-box2d/MOAIBox2DFixture.h>
#include <moai-box2d/MOAIBox2DFrictionJoint.h>
#include <moai-box2d/MOAIBox2DGearJoint.h>
#include <moai-box2d/MOAIBox2DJoint.h>
#include <moai-box2d/MOAIBox2DMotorJoint.h>
#include <moai-box2d/MOAIBox2DMouseJoint.h>
#include <moai-box2d/MOAIBox2DPrismaticJoint.h>
#include <moai-box2d/MOAIBox2DPulleyJoint.h>
#include <moai-box2d/MOAIBox2DRevoluteJoint.h>
#include <moai-box2d/MOAIBox2DRopeJoint.h>
#include <moai-box2d/MOAIBox2DWeldJoint.h>
#include <moai-box2d/MOAIBox2DWheelJoint.h>
#include <moai-box2d/MOAIBox2DWorld.h>

//================================================================//
// MOAIBox2DRayCastCallback
//================================================================//
class MOAIBox2DRayCastCallback :
	public b2RayCastCallback {
private:

	friend class MOAIBox2DWorld;

	b2Fixture*		m_fixture;
	b2Vec2			m_point;
	b2Vec2			m_normal;

public:

	//----------------------------------------------------------------//
	MOAIBox2DRayCastCallback () {
		m_fixture = NULL;
		m_point.SetZero();
		m_normal.SetZero();
	}

	//----------------------------------------------------------------//
	float32 ReportFixture ( b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction ) {
		m_fixture = fixture;
		m_point = point;
		m_normal = normal;
		return fraction;
	}
};

//================================================================//
// MOAIBox2DPrim
//================================================================//

//----------------------------------------------------------------//
float MOAIBox2DPrim::GetUnitsToMeters () {

	if ( this->mWorld ) {
		return this->mWorld->GetUnitsToMeters ();
	}
	return 1.0f;
}

//----------------------------------------------------------------//
MOAIBox2DPrim::MOAIBox2DPrim () :
	mWorld ( 0 ),
	mDestroy ( false ),
	mDestroyNext ( 0 ) {
}

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	addBody
	@text	Create and add a body to the world.
	
	@in		MOAIBox2DWorld self
	@in		number type		One of MOAIBox2DBody.DYNAMIC, MOAIBox2DBody.KINEMATIC, MOAIBox2DBody.STATIC
	@opt	number x	in units, in world coordinates, converted to meters
	@opt	number y	in units, in world coordinates, converted to meters
	@out	MOAIBox2DBody joint
*/
mrb_value MOAIBox2DWorld::_addBody ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "UN" )
	
	if ( self->IsLocked ()) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DWorld_IsLocked );
		return mrb_nil_value ();
	}
	
	u32 type	= state.GetParamValue < u32 >( 1, 0 );
	float x		= state.GetParamValue < float >( 2, 0.0f ) * self->mUnitsToMeters;
	float y		= state.GetParamValue < float >( 3, 0.0f ) * self->mUnitsToMeters;
	
	b2BodyDef groundBodyDef;
	groundBodyDef.type = ( b2BodyType )type;
	groundBodyDef.position.Set ( x, y );
	
	MOAIBox2DBody* body = state.CreateClassInstance < MOAIBox2DBody >();
	body->SetBody ( self->mWorld->CreateBody ( &groundBodyDef ));
	body->SetWorld ( self );
	self->RubyRetain ( body );
	
	return body->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	addDistanceJoint
	@text	Create and add a joint to the world. See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@in		MOAIBox2DBody bodyA
	@in		MOAIBox2DBody bodyB
	@in		number anchorA_X			in units, in world coordinates, converted to meters
	@in		number anchorA_Y			in units, in world coordinates, converted to meters
	@in		number anchorB_X			in units, in world coordinates, converted to meters
	@in		number anchorB_Y			in units, in world coordinates, converted to meters
	@opt	number frequencyHz			in Hz. Default value determined by Box2D
	@opt	number dampingRatio			Default value determined by Box2D
	@opt	boolean collideConnected	Default value is false
	@out	MOAIBox2DJoint joint
*/
mrb_value	MOAIBox2DWorld::_addDistanceJoint ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "UUUNNNN" )
	
	if ( self->IsLocked ()) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DWorld_IsLocked );
		return mrb_nil_value ();
	}
	
	MOAIBox2DBody* bodyA = state.GetRubyObject < MOAIBox2DBody >( 1, true );
	MOAIBox2DBody* bodyB = state.GetRubyObject < MOAIBox2DBody >( 2, true );
	
	if ( !( bodyA && bodyB )) return mrb_nil_value ();
	
	b2Vec2 anchorA;
	anchorA.x	= state.GetParamValue < float >( 3, 0 ) * self->mUnitsToMeters;
	anchorA.y	= state.GetParamValue < float >( 4, 0 ) * self->mUnitsToMeters;
	
	b2Vec2 anchorB;
	anchorB.x	= state.GetParamValue < float >( 5, 0 ) * self->mUnitsToMeters;
	anchorB.y	= state.GetParamValue < float >( 6, 0 ) * self->mUnitsToMeters;
	
	b2DistanceJointDef jointDef;
	jointDef.Initialize ( bodyA->mBody, bodyB->mBody, anchorA, anchorB );
	
	jointDef.frequencyHz	= state.GetParamValue < float >( 7, jointDef.frequencyHz );
	jointDef.dampingRatio	= state.GetParamValue < float >( 8, jointDef.dampingRatio );
	jointDef.collideConnected = state.GetParamValue < bool >( 9, false );
	
	MOAIBox2DDistanceJoint* joint = state.CreateClassInstance < MOAIBox2DDistanceJoint >();
	joint->SetJoint ( self->mWorld->CreateJoint ( &jointDef ));
	joint->SetWorld ( self );
	joint->RubyRetain ( bodyA );
	joint->RubyRetain ( bodyB );
	self->RubyRetain ( joint );
	
	return joint->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	addFrictionJoint
	@text	Create and add a joint to the world. See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@in		MOAIBox2DBody bodyA
	@in		MOAIBox2DBody bodyB
	@in		number anchorX				in units, in world coordinates, converted to meters
	@in		number anchorY				in units, in world coordinates, converted to meters
	@opt	number maxForce				in kg * units / s^2, converted to N [kg * m / s^2]. 	Default value determined by Box2D
	@opt	number maxTorque			in kg * units / s^2 * units, converted to N-m [kg * m / s^2 * m].	Default value determined by Box2D
	@opt	boolean collideConnected	Default value is false
	@out	MOAIBox2DJoint joint
*/
mrb_value	MOAIBox2DWorld::_addFrictionJoint ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "UUUNN" )
	
	if ( self->IsLocked ()) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DWorld_IsLocked );
		return mrb_nil_value ();
	}
	
	MOAIBox2DBody* bodyA = state.GetRubyObject < MOAIBox2DBody >( 1, true );
	MOAIBox2DBody* bodyB = state.GetRubyObject < MOAIBox2DBody >( 2, true );
	
	if ( !( bodyA && bodyB )) return mrb_nil_value ();
	
	b2Vec2 anchor;
	anchor.x	= state.GetParamValue < float >( 3, 0 ) * self->mUnitsToMeters;
	anchor.y	= state.GetParamValue < float >( 4, 0 ) * self->mUnitsToMeters;
	
	b2FrictionJointDef jointDef;
	jointDef.Initialize ( bodyA->mBody, bodyB->mBody, anchor );

	float unitsToMeters = self->GetUnitsToMeters();

	jointDef.maxForce	= state.GetParamValue < float >( 5, jointDef.maxForce / unitsToMeters ) * unitsToMeters;
	/* Convert to/from N-m (kg m / s^2) * m from/to (kg unit / s^2) * unit */
	jointDef.maxTorque	= state.GetParamValue < float >( 6, jointDef.maxTorque / ( unitsToMeters * unitsToMeters ) ) * unitsToMeters * unitsToMeters;
	
	jointDef.collideConnected = state.GetParamValue < bool >( 7, false );
	
	MOAIBox2DFrictionJoint* joint = state.CreateClassInstance < MOAIBox2DFrictionJoint >();
	joint->SetJoint ( self->mWorld->CreateJoint ( &jointDef ));
	joint->SetWorld ( self );
	joint->RubyRetain ( bodyA );
	joint->RubyRetain ( bodyB );
	self->RubyRetain ( joint );
	
	return joint->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	addGearJoint
	@text	Create and add a joint to the world. See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@in		MOAIBox2DJoint jointA
	@in		MOAIBox2DJoint jointB
	@in		number ratio
	@opt	boolean collideConnected	Default value is false
	@out	MOAIBox2DJoint joint
*/
mrb_value	MOAIBox2DWorld::_addGearJoint ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "UUUN" )
	
	if ( self->IsLocked ()) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DWorld_IsLocked );
		return mrb_nil_value ();
	}
	
	MOAIBox2DJoint* jointA = state.GetRubyObject < MOAIBox2DJoint >( 1, true );
	MOAIBox2DJoint* jointB = state.GetRubyObject < MOAIBox2DJoint >( 2, true );
	
	if ( !( jointA && jointB )) return mrb_nil_value ();
	
	b2GearJointDef jointDef;
	
	jointDef.bodyA				= jointA->mJoint->GetBodyA ();
	jointDef.bodyB				= jointB->mJoint->GetBodyB ();
	jointDef.joint1				= jointA->mJoint;
	jointDef.joint2				= jointB->mJoint;
	jointDef.ratio				= state.GetParamValue < float >( 3, 0.0f );
	jointDef.collideConnected	= state.GetParamValue < bool >( 4, false );
	
	MOAIBox2DGearJoint* joint = state.CreateClassInstance < MOAIBox2DGearJoint >();
	joint->SetJoint ( self->mWorld->CreateJoint ( &jointDef ));
	joint->SetWorld ( self );
	self->RubyRetain ( joint );
	
	joint->mJointA.Set ( *self, jointA );
	joint->mJointB.Set ( *self, jointB );
	
	return joint->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/** @lua	addMotorJoint
	@text	Create and add a joint to the world. See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@in		MOAIBox2DBody bodyA
	@in		MOAIBox2DBody bodyB
	@opt	boolean collideConnected	Default value is false
	@out	MOAIBox2DJoint joint
*/
mrb_value MOAIBox2DWorld::_addMotorJoint ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "UUU" )

	if ( self->IsLocked ()) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DWorld_IsLocked );
		return mrb_nil_value ();
	}
	
	MOAIBox2DBody* bodyA = state.GetRubyObject < MOAIBox2DBody >( 1, true );
	MOAIBox2DBody* bodyB = state.GetRubyObject < MOAIBox2DBody >( 2, true );
	
	if ( !( bodyA && bodyB )) return mrb_nil_value ();
	
	b2MotorJointDef jointDef;
	jointDef.Initialize ( bodyA->mBody, bodyB->mBody );
	
	jointDef.collideConnected = state.GetParamValue < bool >( 3, false );
	
	MOAIBox2DMotorJoint* joint = state.CreateClassInstance < MOAIBox2DMotorJoint >();
	joint->SetJoint ( self->mWorld->CreateJoint ( &jointDef ));
	joint->SetWorld ( self );
	joint->RubyRetain ( bodyA );
	joint->RubyRetain ( bodyB );
	self->RubyRetain ( joint );
	
	return joint->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	addMouseJoint
	@text	Create and add a joint to the world. See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@in		MOAIBox2DBody bodyA
	@in		MOAIBox2DBody bodyB
	@in		number targetX				in units, in world coordinates, converted to meters
	@in		number targetY				in units, in world coordinates, converted to meters
	@in		number maxForce				in kg * units / s^2, converted to N [kg * m / s^2].
	@opt	number frequencyHz			in Hz. Default value determined by Box2D
	@opt	number dampingRatio			Default value determined by Box2D
	@opt	boolean collideConnected	Default value is false
	@out	MOAIBox2DJoint joint
*/
mrb_value	MOAIBox2DWorld::_addMouseJoint ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "UUUNNN" )
	
	if ( self->IsLocked ()) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DWorld_IsLocked );
		return mrb_nil_value ();
	}
	
	MOAIBox2DBody* bodyA = state.GetRubyObject < MOAIBox2DBody >( 1, true );
	MOAIBox2DBody* bodyB = state.GetRubyObject < MOAIBox2DBody >( 2, true );
	
	if ( !( bodyA && bodyB )) return mrb_nil_value ();
	
	b2Vec2 target;
	target.x	= state.GetParamValue < float >( 3, 0 ) * self->mUnitsToMeters;
	target.y	= state.GetParamValue < float >( 4, 0 ) * self->mUnitsToMeters;
	
	b2MouseJointDef jointDef;
	jointDef.bodyA				= bodyA->mBody;
	jointDef.bodyB				= bodyB->mBody;
	jointDef.target				= target;
	jointDef.maxForce			= state.GetParamValue < float >( 5, 0.0f ) * self->mUnitsToMeters;
	jointDef.frequencyHz		= state.GetParamValue < float >( 6, jointDef.frequencyHz );
	jointDef.dampingRatio		= state.GetParamValue < float >( 7, jointDef.dampingRatio );
	jointDef.collideConnected	= state.GetParamValue < bool >( 8, false );
	
	MOAIBox2DMouseJoint* joint = state.CreateClassInstance < MOAIBox2DMouseJoint >();
	joint->SetJoint ( self->mWorld->CreateJoint ( &jointDef ));
	joint->SetWorld ( self );
	joint->RubyRetain ( bodyA );
	joint->RubyRetain ( bodyB );
	self->RubyRetain ( joint );
	
	return joint->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	addPrismaticJoint
	@text	Create and add a joint to the world. See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@in		MOAIBox2DBody bodyA
	@in		MOAIBox2DBody bodyB
	@in		number anchorA				in units, in world coordinates, converted to meters
	@in		number anchorB				in units, in world coordinates, converted to meters
	@in		number axisA				translation axis vector X component (no units)
	@in		number axisB				translation axis vector Y component (no units)
	@opt	boolean collideConnected	Default value is false
	@out	MOAIBox2DJoint joint
*/
mrb_value	MOAIBox2DWorld::_addPrismaticJoint ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "UUUNNNN" )
	
	if ( self->IsLocked ()) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DWorld_IsLocked );
		return mrb_nil_value ();
	}
	
	MOAIBox2DBody* bodyA = state.GetRubyObject < MOAIBox2DBody >( 1, true );
	MOAIBox2DBody* bodyB = state.GetRubyObject < MOAIBox2DBody >( 2, true );
	
	if ( !( bodyA && bodyB )) return mrb_nil_value ();
	
	b2Vec2 anchor;
	anchor.x	= state.GetParamValue < float >( 3, 0 ) * self->mUnitsToMeters;
	anchor.y	= state.GetParamValue < float >( 4, 0 ) * self->mUnitsToMeters;
	
	b2Vec2 axis;
	axis.x		= state.GetParamValue < float >( 5, 0 );
	axis.y		= state.GetParamValue < float >( 6, 0 );
	
	b2PrismaticJointDef jointDef;
	jointDef.Initialize ( bodyA->mBody, bodyB->mBody, anchor, axis );
	
	jointDef.collideConnected = state.GetParamValue < bool >( 7, false );
	
	MOAIBox2DPrismaticJoint* joint = state.CreateClassInstance < MOAIBox2DPrismaticJoint >();
	joint->SetJoint ( self->mWorld->CreateJoint ( &jointDef ));
	joint->SetWorld ( self );
	joint->RubyRetain ( bodyA );
	joint->RubyRetain ( bodyB );
	self->RubyRetain ( joint );
	
	return joint->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	addPulleyJoint
	@text	Create and add a joint to the world. See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@in		MOAIBox2DBody bodyA
	@in		MOAIBox2DBody bodyB
	@in		number groundAnchorA_X		in units, in world coordinates, converted to meters
	@in		number groundAnchorA_Y		in units, in world coordinates, converted to meters
	@in		number groundAnchorB_X		in units, in world coordinates, converted to meters
	@in		number groundAnchorB_Y		in units, in world coordinates, converted to meters
	@in		number anchorA_X			in units, in world coordinates, converted to meters
	@in		number anchorA_Y			in units, in world coordinates, converted to meters
	@in		number anchorB_X			in units, in world coordinates, converted to meters
	@in		number anchorB_Y			in units, in world coordinates, converted to meters
	@in		number ratio
	@in		number maxLengthA			in units, converted to meters
	@in		number maxLengthB			in units, converted to meters
	@opt	boolean collideConnected	Default value is false
	@out	MOAIBox2DJoint joint
*/
mrb_value	MOAIBox2DWorld::_addPulleyJoint ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "UUUNNNNNNNNNNN" )
	
	if ( self->IsLocked ()) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DWorld_IsLocked );
		return mrb_nil_value ();
	}
	
	MOAIBox2DBody* bodyA = state.GetRubyObject < MOAIBox2DBody >( 1, true );
	MOAIBox2DBody* bodyB = state.GetRubyObject < MOAIBox2DBody >( 2, true );
	
	if ( !( bodyA && bodyB )) return mrb_nil_value ();
	
	b2Vec2 groundAnchorA;
	groundAnchorA.x		= state.GetParamValue < float >( 3, 0 ) * self->mUnitsToMeters;
	groundAnchorA.y		= state.GetParamValue < float >( 4, 0 ) * self->mUnitsToMeters;
	
	b2Vec2 groundAnchorB;
	groundAnchorB.x		= state.GetParamValue < float >( 5, 0 ) * self->mUnitsToMeters;
	groundAnchorB.y		= state.GetParamValue < float >( 6, 0 ) * self->mUnitsToMeters;
	
	b2Vec2 anchorA;
	anchorA.x			= state.GetParamValue < float >( 7, 0 ) * self->mUnitsToMeters;
	anchorA.y			= state.GetParamValue < float >( 8, 0 ) * self->mUnitsToMeters;
	
	b2Vec2 anchorB;
	anchorB.x			= state.GetParamValue < float >( 9, 0 ) * self->mUnitsToMeters;
	anchorB.y			= state.GetParamValue < float >( 10, 0 ) * self->mUnitsToMeters;
	
	float ratio			= state.GetParamValue < float >( 11, 0 );
	
	b2PulleyJointDef jointDef;
	jointDef.Initialize ( bodyA->mBody, bodyB->mBody, groundAnchorA, groundAnchorB, anchorA, anchorB, ratio );
	
	jointDef.lengthA	= state.GetParamValue < float >( 12, 0 ) * self->mUnitsToMeters;
	jointDef.lengthB	= state.GetParamValue < float >( 13, 0 ) * self->mUnitsToMeters;
	
	jointDef.collideConnected = state.GetParamValue < bool >( 15, false );
	
	MOAIBox2DPulleyJoint* joint = state.CreateClassInstance < MOAIBox2DPulleyJoint >();
	joint->SetJoint ( self->mWorld->CreateJoint ( &jointDef ));
	joint->SetWorld ( self );
	joint->RubyRetain ( bodyA );
	joint->RubyRetain ( bodyB );
	self->RubyRetain ( joint );
	
	return joint->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	addRevoluteJoint
	@text	Create and add a joint to the world. See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@in		MOAIBox2DBody bodyA
	@in		MOAIBox2DBody bodyB
	@in		number anchorX				in units, in world coordinates, converted to meters
	@in		number anchorY				in units, in world coordinates, converted to meters
	@opt	boolean collideConnected	Default value is false
	@out	MOAIBox2DJoint joint
*/
mrb_value	MOAIBox2DWorld::_addRevoluteJoint ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "UUUNN" )
	
	if ( self->IsLocked ()) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DWorld_IsLocked );
		return mrb_nil_value ();
	}
	
	MOAIBox2DBody* bodyA = state.GetRubyObject < MOAIBox2DBody >( 1, true );
	MOAIBox2DBody* bodyB = state.GetRubyObject < MOAIBox2DBody >( 2, true );
	
	if ( !( bodyA && bodyB )) return mrb_nil_value ();
	
	b2Vec2 anchor;
	anchor.x	= state.GetParamValue < float >( 3, 0 ) * self->mUnitsToMeters;
	anchor.y	= state.GetParamValue < float >( 4, 0 ) * self->mUnitsToMeters;
	
	b2RevoluteJointDef jointDef;
	jointDef.Initialize ( bodyA->mBody, bodyB->mBody, anchor );
	
	jointDef.collideConnected = state.GetParamValue < bool >( 5, false );
	
	MOAIBox2DRevoluteJoint* joint = state.CreateClassInstance < MOAIBox2DRevoluteJoint >();
	joint->SetJoint ( self->mWorld->CreateJoint ( &jointDef ));
	joint->SetWorld ( self );
	joint->RubyRetain ( bodyA );
	joint->RubyRetain ( bodyB );
	self->RubyRetain ( joint );
	
	return joint->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	addRevoluteJointLocal
	@text	Create and add a joint to the world, using local anchors. See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@in		MOAIBox2DBody bodyA
	@in		MOAIBox2DBody bodyB
	@in		number anchorA_X	in units, in world coordinates, converted to meters
	@in		number anchorA_Y	in units, in world coordinates, converted to meters
	@in		number anchorB_X	in units, in world coordinates, converted to meters
	@in		number anchorB_Y	in units, in world coordinates, converted to meters
	@out	MOAIBox2DJoint joint
*/
mrb_value	MOAIBox2DWorld::_addRevoluteJointLocal( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "UUUNNNN" )
	
	if ( self->IsLocked ()) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DWorld_IsLocked );
		return mrb_nil_value ();
	}
	
	MOAIBox2DBody* bodyA = state.GetRubyObject < MOAIBox2DBody >( 1, true );
	MOAIBox2DBody* bodyB = state.GetRubyObject < MOAIBox2DBody >( 2, true );
	
	if ( !( bodyA && bodyB )) return mrb_nil_value ();
		
	b2RevoluteJointDef jointDef;
	jointDef.bodyA = bodyA->mBody;
	jointDef.bodyB = bodyB->mBody;
	
	jointDef.localAnchorA.Set(
		state.GetParamValue < float >( 3, 0 ) * self->mUnitsToMeters, 
		state.GetParamValue < float >( 4, 0 ) * self->mUnitsToMeters
	);
		
	jointDef.localAnchorB.Set(
		state.GetParamValue < float >( 5, 0 ) * self->mUnitsToMeters, 
		state.GetParamValue < float >( 6, 0 ) * self->mUnitsToMeters
	);
		
	MOAIBox2DRevoluteJoint* joint = state.CreateClassInstance < MOAIBox2DRevoluteJoint >();
	joint->SetJoint ( self->mWorld->CreateJoint ( &jointDef ));
	joint->SetWorld ( self );
	joint->RubyRetain ( bodyA );
	joint->RubyRetain ( bodyB );
	self->RubyRetain ( joint );
	
	return joint->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	addRopeJoint
	@text	Create and add a rope joint to the world. See Box2D documentation.
 
	@in		MOAIBox2DWorld self
	@in		MOAIBox2DBody bodyA
	@in		MOAIBox2DBody bodyB
	@in		number maxLength				in units, converted to meters
	@opt	number anchorAX					in units, in world coordinates, converted to meters
	@opt	number anchorAY					in units, in world coordinates, converted to meters
	@opt	number anchorBX					in units, in world coordinates, converted to meters
	@opt	number anchorBY					in units, in world coordinates, converted to meters
	@opt	boolean collideConnected		Default value is false
	@out	MOAIBox2DJoint joint
 */
mrb_value	MOAIBox2DWorld::_addRopeJoint ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "UUU" )
	
	if ( self->IsLocked ()) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DWorld_IsLocked );
		return mrb_nil_value ();
	}
	
	MOAIBox2DBody* bodyA = state.GetRubyObject < MOAIBox2DBody >( 1, true );
	MOAIBox2DBody* bodyB = state.GetRubyObject < MOAIBox2DBody >( 2, true );
	
	if ( !( bodyA && bodyB )) return mrb_nil_value ();
	
	float maxLength = state.GetParamValue < float >( 3, 1 ) * self->mUnitsToMeters;
	
	b2Vec2 anchorA;
	anchorA.x	= state.GetParamValue < float >( 4, 0 ) * self->mUnitsToMeters;
	anchorA.y	= state.GetParamValue < float >( 5, 0 ) * self->mUnitsToMeters;

	b2Vec2 anchorB;
	anchorB.x	= state.GetParamValue < float >( 6, 0 ) * self->mUnitsToMeters;
	anchorB.y	= state.GetParamValue < float >( 7, 0 ) * self->mUnitsToMeters;

	bool collideConnected = state.GetParamValue < bool >( 8, false );
	
	b2RopeJointDef jointDef;
	jointDef.bodyA = bodyA->mBody;
	jointDef.bodyB = bodyB->mBody;
	jointDef.collideConnected = collideConnected;
	jointDef.localAnchorA = bodyA->mBody->GetLocalPoint(anchorA);
	jointDef.localAnchorB = bodyB->mBody->GetLocalPoint(anchorB);
	jointDef.maxLength = maxLength;
	
	MOAIBox2DRopeJoint* joint = state.CreateClassInstance < MOAIBox2DRopeJoint >();
	joint->SetJoint ( self->mWorld->CreateJoint ( &jointDef ));
	joint->SetWorld ( self );
	joint->RubyRetain ( bodyA );
	joint->RubyRetain ( bodyB );
	self->RubyRetain ( joint );
	
	return joint->PushRubyUserdata ( state );
}


//----------------------------------------------------------------//
/**	@lua	addWeldJoint
	@text	Create and add a joint to the world. See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@in		MOAIBox2DBody bodyA
	@in		MOAIBox2DBody bodyB
	@in		number anchorX				in units, in world coordinates, converted to meters
	@in		number anchorY				in units, in world coordinates, converted to meters
	@opt	boolean collideConnected	Default value is false
	@out	MOAIBox2DJoint joint
*/
mrb_value	MOAIBox2DWorld::_addWeldJoint ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "UUUNN" )
	
	if ( self->IsLocked ()) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DWorld_IsLocked );
		return mrb_nil_value ();
	}
	
	MOAIBox2DBody* bodyA = state.GetRubyObject < MOAIBox2DBody >( 1, true );
	MOAIBox2DBody* bodyB = state.GetRubyObject < MOAIBox2DBody >( 2, true );
	
	if ( !( bodyA && bodyB )) return mrb_nil_value ();
	
	b2Vec2 anchor;
	anchor.x	= state.GetParamValue < float >( 3, 0 ) * self->mUnitsToMeters;
	anchor.y	= state.GetParamValue < float >( 4, 0 ) * self->mUnitsToMeters;
	
	b2WeldJointDef jointDef;
	jointDef.Initialize ( bodyA->mBody, bodyB->mBody, anchor );
	
	jointDef.collideConnected = state.GetParamValue < bool >( 5, false );
	
	MOAIBox2DWeldJoint* joint = state.CreateClassInstance < MOAIBox2DWeldJoint >();
	joint->SetJoint ( self->mWorld->CreateJoint ( &jointDef ));
	joint->SetWorld ( self );
	joint->RubyRetain ( bodyA );
	joint->RubyRetain ( bodyB );
	self->RubyRetain ( joint );
	
	return joint->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	addWheelJoint
	@text	Create and add a joint to the world. See Box2D documentation.
 
	@in		MOAIBox2DWorld self
	@in		MOAIBox2DBody bodyA
	@in		MOAIBox2DBody bodyB
	@in		number anchorX				in units, in world coordinates, converted to meters
	@in		number anchorY				in units, in world coordinates, converted to meters
	@in		number axisX				translation axis vector X component (no units)
	@in		number axisY				translation axis vector Y component (no units)
	@opt	boolean collideConnected	Default value is false
	@out	MOAIBox2DJoint joint
 */
mrb_value	MOAIBox2DWorld::_addWheelJoint ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "UUUNNNN" )
	
	if ( self->IsLocked ()) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DWorld_IsLocked );
		return mrb_nil_value ();
	}
	
	MOAIBox2DBody* bodyA = state.GetRubyObject < MOAIBox2DBody >( 1, true );
	MOAIBox2DBody* bodyB = state.GetRubyObject < MOAIBox2DBody >( 2, true );
	
	if ( !( bodyA && bodyB )) return mrb_nil_value ();
	
	b2Vec2 anchor;
	anchor.x	= state.GetParamValue < float >( 3, 0 ) * self->mUnitsToMeters;
	anchor.y	= state.GetParamValue < float >( 4, 0 ) * self->mUnitsToMeters;
	
	b2Vec2 axis;
	axis.x		= state.GetParamValue < float >( 5, 0 );
	axis.y      = state.GetParamValue < float >( 6, 0 );
	
	b2WheelJointDef jointDef;
	jointDef.Initialize ( bodyA->mBody, bodyB->mBody, anchor, axis );
	
	jointDef.collideConnected = state.GetParamValue < bool >( 7, false );
	
	MOAIBox2DWheelJoint* joint = state.CreateClassInstance < MOAIBox2DWheelJoint >();
	joint->SetJoint ( self->mWorld->CreateJoint ( &jointDef ));
	joint->SetWorld ( self );
	joint->RubyRetain ( bodyA );
	joint->RubyRetain ( bodyB );
	self->RubyRetain ( joint );
	
	return joint->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	getAngularSleepTolerance
	@text	See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@out	number angularSleepTolerance	in degrees/s, converted from radians/s
*/
mrb_value MOAIBox2DWorld::_getAngularSleepTolerance ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "U" )
	
	float tolerance = self->mWorld->GetAngularSleepTolerance();
	tolerance *= ( float )R2D;
	return state.ToRValue ( tolerance );
}

//----------------------------------------------------------------//
/**	@lua	getAutoClearForces
	@text	See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@out	boolean autoClearForces
*/
mrb_value MOAIBox2DWorld::_getAutoClearForces ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "U" )
	
	bool autoClearForces = self->mWorld->GetAutoClearForces ();
	
	return state.ToRValue ( autoClearForces );
}

//----------------------------------------------------------------//
/**	@lua	getGravity
	@text	See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@out	number gravityX		in units/s^2, converted from m/s^2
	@out	number gravityY		in units/s^2, converted from m/s^2
*/
mrb_value MOAIBox2DWorld::_getGravity ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "U" )
	
	b2Vec2 gravity = self->mWorld->GetGravity ();

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( gravity.x / self->mUnitsToMeters );
	ret [ 1 ] = state.ToRValue ( gravity.y / self->mUnitsToMeters );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getLinearSleepTolerance
	@text	See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@out	number linearSleepTolerance	in units/s, converted from m/s
*/
mrb_value MOAIBox2DWorld::_getLinearSleepTolerance ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "U" )
	
	return state.ToRValue ( self->mWorld->GetLinearSleepTolerance () / self->mUnitsToMeters );
}

//----------------------------------------------------------------//
/** @lua    getPerformace
	@text   Returns profiler numbers from box2d
	
	@in		MOAIBox2DWorld self
	@out	number	step
	@out	number	collide
	@out	number	solve
	@out	number	solveInit
	@out	number	solveVelocity
	@out	number	solvePosition
	@out	number	broadphase
	@out	number	solveTOI
*/
mrb_value MOAIBox2DWorld::_getPerformance ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "U" )

	b2Profile p = self->mWorld->GetProfile ();

	mrb_value ret [ 8 ];
	ret [ 0 ] = state.ToRValue ( p.step );
	ret [ 1 ] = state.ToRValue ( p.collide );
	ret [ 2 ] = state.ToRValue ( p.solve );
	ret [ 3 ] = state.ToRValue ( p.solveInit );
	ret [ 4 ] = state.ToRValue ( p.solveVelocity );
	ret [ 5 ] = state.ToRValue ( p.solvePosition );
	ret [ 6 ] = state.ToRValue ( p.broadphase );
	ret [ 7 ] = state.ToRValue ( p.solveTOI );

	return mrb_ary_new_from_values ( state, 8, ret );
}

//----------------------------------------------------------------//
/**	@lua   getRayCast
	@text   return RayCast 1st point hit
       
	@in		MOAIBox2DWorld self
	@in		number p1x
	@in		number p1y
	@in		number p2x
	@in		number p2y
	@out	boolean hit					true if hit, false otherwise
	@out	MOAIBox2DFixture fixture	the fixture that was hit, or nil
	@out	number hitpointX
	@out	number hitpointY
*/
mrb_value MOAIBox2DWorld::_getRayCast ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "U" )
	float p1x=state.GetParamValue < float >( 1, 0 ) * self->mUnitsToMeters;
	float p1y=state.GetParamValue < float >( 2, 0 ) * self->mUnitsToMeters;
	float p2x=state.GetParamValue < float >( 3, 0 ) * self->mUnitsToMeters;
	float p2y=state.GetParamValue < float >( 4, 0 ) * self->mUnitsToMeters;
 
	b2Vec2 p1(p1x,p1y);
	b2Vec2 p2(p2x,p2y);
   
	MOAIBox2DRayCastCallback callback;
	self->mWorld->RayCast(&callback, p1, p2);
 
	if (NULL != callback.m_fixture) {
		b2Vec2 hitpoint = callback.m_point;

		mrb_value ary = mrb_ary_new ( M );
		mrb_ary_push ( M, ary, state.ToRValue ( true ));
		mrb_ary_push ( M, ary, state.ToRValue ( hitpoint.x / self->mUnitsToMeters ));
		mrb_ary_push ( M, ary, state.ToRValue ( hitpoint.y / self->mUnitsToMeters ));

		// Raycast hit a fixture
		MOAIBox2DFixture* moaiFixture = ( MOAIBox2DFixture* ) callback.m_fixture->GetUserData ();
		if ( moaiFixture ) {
			mrb_ary_push ( M, ary, moaiFixture->PushRubyUserdata ( state ));
			return ary;
		} else {
			return ary;
		}

	} else {

		// Raycast did not hit a fixture
		return state.ToRValue ( false );
	}
}

//----------------------------------------------------------------//
/**	@lua	getTimeToSleep
	@text	See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@out	number timeToSleep
*/
mrb_value MOAIBox2DWorld::_getTimeToSleep ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "U" )
	
	return state.ToRValue ( self->mWorld->GetTimeToSleep ());
}

//----------------------------------------------------------------//
/**	@lua	setAngularSleepTolerance
	@text	See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@opt	number angularSleepTolerance		in degrees/s, converted to radians/s. Default value is 0.0f.
	@out	nil
*/
mrb_value MOAIBox2DWorld::_setAngularSleepTolerance ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "U" )
	
	float tolerance = state.GetParamValue< float >( 1, 0.0f );
	tolerance *= ( float )R2D;
	self->mWorld->SetAngularSleepTolerance ( tolerance );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setAutoClearForces
	@text	See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@opt	boolean autoClearForces		Default value is 'true'
	@out	nil
*/
mrb_value MOAIBox2DWorld::_setAutoClearForces ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "U" )
	
	bool autoClearForces = state.GetParamValue < bool >( 1, true );
	
	self->mWorld->SetAutoClearForces ( autoClearForces );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setDebugDrawEnabled
	@text	enable/disable debug drawing.
 
	@in		MOAIBox2DWorld self
	@in		boolean enable
	@out	nil
*/
mrb_value MOAIBox2DWorld::_setDebugDrawEnabled ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "U" )
 
	bool enabled = state.GetParamValue < bool >( 1, false );
 
	if(enabled)
	{
		self->mDebugDraw->SetFlags (
			b2Draw::e_shapeBit			|
			b2Draw::e_jointBit			|
			b2Draw::e_centerOfMassBit
		);
	}
	else
	{
		self->mDebugDraw->ClearFlags(0);
		self->mDebugDraw->SetFlags(0);
	}
 
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setDebugDrawFlags
	@text	Sets mask for debug drawing.
	
	@in		MOAIBox2DWorld self
	@opt	number flags		One of MOAIBox2DWorld.DEBUG_DRAW_SHAPES, MOAIBox2DWorld.DEBUG_DRAW_JOINTS,
								MOAIBox2DWorld.DEBUG_DRAW_BOUNDS, MOAIBox2DWorld.DEBUG_DRAW_PAIRS,
								MOAIBox2DWorld.DEBUG_DRAW_CENTERS. Default value is MOAIBox2DWorld.DEBUG_DRAW_DEFAULT.
	@out	nil
*/
mrb_value MOAIBox2DWorld::_setDebugDrawFlags ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "U" )
	
	u32 flags = state.GetParamValue < u32 >( 1, DEBUG_DRAW_DEFAULT );
	if ( self->mDebugDraw ) {
		self->mDebugDraw->SetFlags ( flags );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setGravity
	@text	See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@opt	number gravityX			in units/s^2, converted to m/s^2. Default value is 0.
	@opt	number gravityY			in units/s^2, converted to m/s^2. Default value is 0.
	@out	nil
*/
mrb_value MOAIBox2DWorld::_setGravity ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "U" )
	
	b2Vec2 gravity;
	
	gravity.x	= state.GetParamValue < float >( 1, 0.0f ) * self->mUnitsToMeters;
	gravity.y	= state.GetParamValue < float >( 2, 0.0f ) * self->mUnitsToMeters;
	
	self->mWorld->SetGravity ( gravity );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setIterations
	@text	See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@opt	number velocityIteratons		Default value is current value of velocity iterations.
	@opt	number positionIterations		Default value is current value of positions iterations.
	@out	nil
*/
mrb_value MOAIBox2DWorld::_setIterations ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "U" )
	
	self->mVelocityIterations = state.GetParamValue < u32 >( 1, self->mVelocityIterations );
	self->mPositionIterations = state.GetParamValue < u32 >( 2, self->mPositionIterations );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setLinearSleepTolerance
	@text	See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@opt	number linearSleepTolerance		in units/s, converted to m/s. Default value is 0.0f.
	@out	nil
*/
mrb_value MOAIBox2DWorld::_setLinearSleepTolerance ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "U" )
	
	self->mWorld->SetLinearSleepTolerance ( state.GetParamValue < float >( 1, 0.0f ) * self->mUnitsToMeters );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setTimeToSleep
	@text	See Box2D documentation.
	
	@in		MOAIBox2DWorld self
	@opt	number timeToSleep				Default value is 0.0f.
	@out	nil
*/
mrb_value MOAIBox2DWorld::_setTimeToSleep ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "U" )
	
	self->mWorld->SetTimeToSleep ( state.GetParamValue < float >( 1, 0.0f ));
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setUnitsToMeters
	@text	Sets a scale factor for converting game world units to Box2D meters.
	
	@in		MOAIBox2DWorld self
	@opt	number unitsToMeters			Default value is 1.
	@out	nil
*/
mrb_value MOAIBox2DWorld::_setUnitsToMeters ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DWorld, "U" )
	
	self->mUnitsToMeters = state.GetParamValue ( 1, 1.0f );
	
	return context;
}

//================================================================//
// MOAIBox2DWorld
//================================================================//

//----------------------------------------------------------------//
void MOAIBox2DWorld::Destroy () {

	if ( this->mLock ) return;
	this->mLock = true;

	while ( this->mDestroyFixtures ) {
		MOAIBox2DPrim* prim = this->mDestroyFixtures;
		this->mDestroyFixtures = this->mDestroyFixtures->mDestroyNext;
		if ( prim->IsValid ()) {
			prim->Destroy ();
			this->RubyRelease ( prim );
		}
	}
	
	while ( this->mDestroyJoints ) {
		MOAIBox2DPrim* prim = this->mDestroyJoints;
		this->mDestroyJoints = this->mDestroyJoints->mDestroyNext;
		if ( prim->IsValid ()) {
			prim->Destroy ();
			this->RubyRelease ( prim );
		}
	}
	
	while ( this->mDestroyBodies ) {
		MOAIBox2DPrim* prim = this->mDestroyBodies;
		this->mDestroyBodies = this->mDestroyBodies->mDestroyNext;
		prim->Destroy ();
		this->RubyRelease ( prim );
	}
	
	this->mLock = false;
}

//----------------------------------------------------------------//
bool MOAIBox2DWorld::IsLocked () {

	if ( this->mWorld ) {
		return this->mWorld->IsLocked ();
	}
	return false;
}

//----------------------------------------------------------------//
MOAIBox2DWorld::MOAIBox2DWorld () :
	mLock ( false ),
	mVelocityIterations ( 10 ),
	mPositionIterations ( 10 ),
	mUnitsToMeters ( 1.0f ),
	mDestroyBodies ( 0 ),
	mDestroyFixtures ( 0 ),
	mDestroyJoints ( 0 ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIAction )
		RTTI_EXTEND ( MOAIDrawable )
	RTTI_END

	MOAIBox2DArbiter* arbiter = MOAIRubyRuntime::Get ().State ().CreateClassInstance < MOAIBox2DArbiter >();
	arbiter->SetWorld ( *this );
	
	this->mArbiter.Set ( *this, arbiter );
	
	b2Vec2 gravity ( 0.0f, 0.0f );
	this->mWorld = new b2World ( gravity );
	this->mWorld->SetContactListener ( this->mArbiter );
	this->mWorld->SetDestructionListener ( this );
	this->mWorld->SetAllowSleeping ( true );
	this->mDebugDraw = new MOAIBox2DDebugDraw ();
	this->mWorld->SetDebugDraw ( this->mDebugDraw );
	
	this->mDebugDraw->SetFlags ( DEBUG_DRAW_DEFAULT );
}

//----------------------------------------------------------------//
MOAIBox2DWorld::~MOAIBox2DWorld () {

	this->mWorld->SetContactListener ( 0 );

	while ( b2Body* body = this->mWorld->GetBodyList ()) {
		MOAIBox2DBody* moaiBody = ( MOAIBox2DBody* )body->GetUserData ();
		moaiBody->Destroy ();
		this->RubyRelease ( moaiBody );
	}
	
	this->mArbiter.Set ( *this, 0 );
	
	delete ( this->mDebugDraw );
	delete ( this->mWorld );
}

//----------------------------------------------------------------//
void MOAIBox2DWorld::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIAction::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "DEBUG_DRAW_SHAPES", ( u32 )DEBUG_DRAW_SHAPES );
	state.DefineClassConst ( klass, "DEBUG_DRAW_JOINTS", ( u32 )DEBUG_DRAW_JOINTS );
	state.DefineClassConst ( klass, "DEBUG_DRAW_BOUNDS", ( u32 )DEBUG_DRAW_BOUNDS );
	state.DefineClassConst ( klass, "DEBUG_DRAW_PAIRS", ( u32 )DEBUG_DRAW_PAIRS );
	state.DefineClassConst ( klass, "DEBUG_DRAW_CENTERS", ( u32 )DEBUG_DRAW_CENTERS );
	
	state.DefineClassConst ( klass, "DEBUG_DRAW_DEFAULT", ( u32 )DEBUG_DRAW_DEFAULT );
}

//----------------------------------------------------------------//
void MOAIBox2DWorld::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIAction::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "addBody", _addBody, MRB_ARGS_ARG ( 1, 2 ) );
	state.DefineInstanceMethod ( klass, "addDistanceJoint", _addDistanceJoint, MRB_ARGS_ARG ( 6, 3 ) );
	state.DefineInstanceMethod ( klass, "addFrictionJoint", _addFrictionJoint, MRB_ARGS_ARG ( 4, 3 ) );
	state.DefineInstanceMethod ( klass, "addGearJoint", _addGearJoint, MRB_ARGS_ARG ( 3, 1 ) );
	state.DefineInstanceMethod ( klass, "addMotorJoint", _addMotorJoint, MRB_ARGS_ARG ( 2, 1 ) );
	state.DefineInstanceMethod ( klass, "addMouseJoint", _addMouseJoint, MRB_ARGS_ARG ( 5, 3 ) );
	state.DefineInstanceMethod ( klass, "addPrismaticJoint", _addPrismaticJoint, MRB_ARGS_ARG ( 6, 1 ) );
	state.DefineInstanceMethod ( klass, "addPulleyJoint", _addPulleyJoint, MRB_ARGS_ARG ( 13, 1 ) );
	state.DefineInstanceMethod ( klass, "addRevoluteJoint", _addRevoluteJoint, MRB_ARGS_ARG ( 4, 1 ) );
	state.DefineInstanceMethod ( klass, "addRevoluteJointLocal", _addRevoluteJointLocal, MRB_ARGS_REQ ( 6 ) );
	state.DefineInstanceMethod ( klass, "addRopeJoint", _addRopeJoint, MRB_ARGS_ARG ( 3, 5 ) );
	state.DefineInstanceMethod ( klass, "addWeldJoint", _addWeldJoint, MRB_ARGS_ARG ( 4, 1 ) );
	state.DefineInstanceMethod ( klass, "addWheelJoint", _addWheelJoint, MRB_ARGS_ARG ( 6, 1 ) );
	state.DefineInstanceMethod ( klass, "getAngularSleepTolerance", _getAngularSleepTolerance, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getAutoClearForces", _getAutoClearForces, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getGravity", _getGravity, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getLinearSleepTolerance", _getLinearSleepTolerance, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getRayCast", _getRayCast, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "getTimeToSleep", _getTimeToSleep, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setAngularSleepTolerance", _setAngularSleepTolerance, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setDebugDrawEnabled", _setDebugDrawEnabled, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setDebugDrawFlags", _setDebugDrawFlags, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setGravity", _setGravity, MRB_ARGS_ARG ( 0, 2 ) );
	state.DefineInstanceMethod ( klass, "setIterations", _setIterations, MRB_ARGS_ARG ( 0, 2 ) );
	state.DefineInstanceMethod ( klass, "setLinearSleepTolerance", _setLinearSleepTolerance, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setTimeToSleep", _setTimeToSleep, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setUnitsToMeters", _setUnitsToMeters, MRB_ARGS_ARG ( 0, 1 ) );

}

//----------------------------------------------------------------//
void MOAIBox2DWorld::SayGoodbye ( b2Fixture* fixture ) {

	MOAIBox2DFixture* moaiFixture = ( MOAIBox2DFixture* )fixture->GetUserData ();
	if ( moaiFixture->IsValid ()) {
		moaiFixture->Clear ();
		this->RubyRelease ( moaiFixture );
	}
}

//----------------------------------------------------------------//
void MOAIBox2DWorld::SayGoodbye ( b2Joint* joint ) {

	MOAIBox2DJoint* moaiJoint = ( MOAIBox2DJoint* )joint->GetUserData ();
	if ( moaiJoint->IsValid ()) {
		moaiJoint->Clear ();
		this->RubyRelease ( moaiJoint );
	}
}

//----------------------------------------------------------------//
void MOAIBox2DWorld::ScheduleDestruction ( MOAIBox2DBody& body ) {

	if ( !body.mDestroy ) {
		body.mDestroyNext = this->mDestroyBodies;
		this->mDestroyBodies = &body;
		body.mDestroy = true;
	}
	this->Destroy ();
}

//----------------------------------------------------------------//
void MOAIBox2DWorld::ScheduleDestruction ( MOAIBox2DFixture& fixture ) {

	if ( fixture.IsValid () && !fixture.mDestroy ) {
		fixture.mDestroyNext = this->mDestroyFixtures;
		this->mDestroyFixtures = &fixture;
		fixture.mDestroy = true;
	}
	this->Destroy ();
}

//----------------------------------------------------------------//
void MOAIBox2DWorld::ScheduleDestruction ( MOAIBox2DJoint& joint ) {

	if ( joint.IsValid () && !joint.mDestroy ) {
		joint.mDestroyNext = this->mDestroyJoints;
		this->mDestroyJoints = &joint;
		joint.mDestroy = true;
	}
	this->Destroy ();
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAIBox2DWorld::MOAIAction_IsDone () {

	return false;
}

//----------------------------------------------------------------//
void MOAIBox2DWorld::MOAIAction_Update ( double step ) {
	
	this->mLock = true;
	this->mWorld->Step (( float32 )step, this->mVelocityIterations, this->mPositionIterations );
	this->mLock = false;
	
	this->Destroy ();
	
	b2Body* body = this->mWorld->GetBodyList ();
	for ( ; body; body = body->GetNext ()) {
		if ( body->IsActive () && body->IsAwake ()) {
			MOAIBox2DBody* moaiBody = ( MOAIBox2DBody* )body->GetUserData ();
			moaiBody->ScheduleUpdate ();
		}
	}
}

//----------------------------------------------------------------//
void MOAIBox2DWorld::MOAIDrawable_Draw ( int subPrimID ) {
	UNUSED ( subPrimID );

	if ( this->mDebugDraw && MOAIDraw::Get ().Bind ()) {
		
		MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
		
		gfxState.SetMtx ( MOAIGfxState::MODEL_TO_WORLD_MTX );
		
		this->mDebugDraw->mScale = 1.0f / this->mUnitsToMeters;
		this->mWorld->DrawDebugData ();
	}
}
