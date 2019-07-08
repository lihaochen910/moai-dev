// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <Box2D/Box2D.h>
#include <moai-box2d/MOAIBox2DArbiter.h>
#include <moai-box2d/MOAIBox2DFixture.h>
#include <moai-box2d/MOAIBox2DWorld.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getContactNormal
 @text	Returns the normal for the contact.
 
 @in	MOAIBox2DArbiter self
 @out	number normal.x
 @out	number normal.y
 */
mrb_value MOAIBox2DArbiter::_getContactNormal ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DArbiter, "U" )
	
	self->AffirmContactData ();
	mrb_value ary = mrb_ary_new ( M );
	mrb_ary_push ( M, ary, state.ToRValue ( self->mContactNormal.x ));
	mrb_ary_push ( M, ary, state.ToRValue ( self->mContactNormal.y ));
	return ary;
}

//----------------------------------------------------------------//
/**	@lua	getContactPoints
	@text	Returns the contact points in world space. There can be 0, 1 or 2 points (see box2d manual section 4.11)
	
	@in		MOAIBox2DArbiter self
	@out	number p1.x			in units, world coordinates, converted from meters
	@out	number p1.y			in units, world coordinates, converted from meters
	@out	number p2.x
	@out	number p2.y
*/
mrb_value MOAIBox2DArbiter::_getContactPoints ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DArbiter, "U" )
	
	self->AffirmContactData ();

	float u2m = self->GetUnitsToMeters();
	mrb_value ary = mrb_ary_new ( M );

	for ( u32 i = 0; i < self->mTotalPoints; ++i ) {
		mrb_ary_push ( M, ary, state.ToRValue ( self->mContactPoints [ i ].x / u2m ));
		mrb_ary_push ( M, ary, state.ToRValue ( self->mContactPoints [ i ].y / u2m ));
	}
	return ary;
}

//----------------------------------------------------------------//
/**	@lua	getNormalImpulse
	@text	Returns total normal impulse for contact.
	
	@in		MOAIBox2DArbiter self
	@out	number impulse			Impulse in kg * units / s converted from kg * m / s
*/
mrb_value MOAIBox2DArbiter::_getNormalImpulse ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DArbiter, "U" )

	self->AffirmContactData ();
	float impulse = self->mNormalImpulse;
	const float metersToUnits = 1 / self->GetUnitsToMeters();
	impulse = impulse * metersToUnits;

	return state.ToRValue ( impulse );
}

//----------------------------------------------------------------//
/**	@lua	getTangentImpulse
	@text	Returns total tangent impulse for contact.
	
	@in		MOAIBox2DArbiter self
	@out	number impulse			Impulse in kg * units / s converted from kg * m / s
*/
mrb_value MOAIBox2DArbiter::_getTangentImpulse ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DArbiter, "U" )

	self->AffirmContactData ();
	float impulse = self->mTangentImpulse;
	const float metersToUnits = 1 / self->GetUnitsToMeters();
	impulse = impulse * metersToUnits;

	return state.ToRValue ( impulse );
}

//----------------------------------------------------------------//
/**	@lua	setContactEnabled
	@text	Enabled or disable the contact.
	
	@in		MOAIBox2DArbiter self
	@in		boolean enabled
	@out	nil
*/
mrb_value MOAIBox2DArbiter::_setContactEnabled ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DArbiter, "U" )
	
	bool enabled = state.GetParamValue < bool >( 1, false );
	self->mContact->SetEnabled ( enabled );
	
	return context;
}

//================================================================//
// MOAIBox2DArbiter
//================================================================//

//----------------------------------------------------------------//
void MOAIBox2DArbiter::AffirmContactData () {
	
	if ( !this->mContactDirty ) {
		return;
	}
	
	this->mNormalImpulse = 0.0f;
	this->mTangentImpulse = 0.0f;
	this->mTotalPoints = 0;
	
	if ( this->mContact ) {
		b2Manifold* manifold = this->mContact->GetManifold ();
		u32 totalPoints = manifold->pointCount;
		
		b2WorldManifold worldManifold;
		this->mContact->GetWorldManifold ( &worldManifold );
		
		this->mContactNormal = worldManifold.normal;
		this->mTotalPoints = totalPoints;
		for ( u32 i = 0; i < totalPoints; ++i ) {
			this->mContactPoints [ i ] = worldManifold.points [ i ];
		}
	}
	else {
		this->mContactNormal = b2Vec2 ();
	}
	
	if ( this->mImpulse ) {
		for ( u32 i = 0; i < this->mTotalPoints; ++i ) {
			this->mNormalImpulse += this->mImpulse->normalImpulses [ i ];
			this->mTangentImpulse += this->mImpulse->tangentImpulses [ i ];
		}
	}
	
	this->mContactDirty = false;
}

//----------------------------------------------------------------//
void MOAIBox2DArbiter::BeginContact ( b2Contact* contact ) {
	
	this->mContact = contact;
	this->mImpulse = 0;
	this->mContactDirty = true;
	
	b2Fixture* fixtureA = contact->GetFixtureA ();
	b2Fixture* fixtureB = contact->GetFixtureB ();
	
	MOAIBox2DFixture* moaiFixtureA = ( MOAIBox2DFixture* )fixtureA->GetUserData ();
	MOAIBox2DFixture* moaiFixtureB = ( MOAIBox2DFixture* )fixtureB->GetUserData ();
	
	moaiFixtureA->HandleCollision ( BEGIN, moaiFixtureB, this );
	moaiFixtureB->HandleCollision ( BEGIN, moaiFixtureA, this );
}

//----------------------------------------------------------------//
void MOAIBox2DArbiter::EndContact ( b2Contact* contact ) {
	
	this->mContact = contact;
	this->mImpulse = 0;
	this->mContactDirty = true;
	
	b2Fixture* fixtureA = contact->GetFixtureA ();
	b2Fixture* fixtureB = contact->GetFixtureB ();
	
	MOAIBox2DFixture* moaiFixtureA = ( MOAIBox2DFixture* )fixtureA->GetUserData ();
	MOAIBox2DFixture* moaiFixtureB = ( MOAIBox2DFixture* )fixtureB->GetUserData ();
	
	moaiFixtureA->HandleCollision ( END, moaiFixtureB, this );
	moaiFixtureB->HandleCollision ( END, moaiFixtureA, this );
}

//----------------------------------------------------------------//
float MOAIBox2DArbiter::GetUnitsToMeters ( ) const {
	if (this->mWorld) {
		return this->mWorld->GetUnitsToMeters();
	} else {
		return 1.0f;
	}
}

//----------------------------------------------------------------//
MOAIBox2DArbiter::MOAIBox2DArbiter ( ) :
	mContact ( 0 ),
	mImpulse ( 0 ),
	mWorld ( NULL ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIBox2DArbiter::MOAIBox2DArbiter ( const MOAIBox2DWorld& world ) :
	mContact ( 0 ),
	mImpulse ( 0 ),
	mWorld ( &world ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIBox2DArbiter::~MOAIBox2DArbiter () {
}

//----------------------------------------------------------------//
void MOAIBox2DArbiter::PostSolve ( b2Contact* contact, const b2ContactImpulse* impulse ) {

	this->mContact = contact;
	this->mImpulse = impulse;
	this->mContactDirty = true;

	b2Fixture* fixtureA = contact->GetFixtureA ();
	b2Fixture* fixtureB = contact->GetFixtureB ();
	
	MOAIBox2DFixture* moaiFixtureA = ( MOAIBox2DFixture* )fixtureA->GetUserData ();
	MOAIBox2DFixture* moaiFixtureB = ( MOAIBox2DFixture* )fixtureB->GetUserData ();
	
	moaiFixtureA->HandleCollision ( POST_SOLVE, moaiFixtureB, this );
	moaiFixtureB->HandleCollision ( POST_SOLVE, moaiFixtureA, this );
}

//----------------------------------------------------------------//
void MOAIBox2DArbiter::PreSolve ( b2Contact* contact, const b2Manifold* oldManifold ) {
	UNUSED ( oldManifold );
	
	this->mContact = contact;
	this->mImpulse = 0;
	this->mContactDirty = true;
	
	b2Fixture* fixtureA = contact->GetFixtureA ();
	b2Fixture* fixtureB = contact->GetFixtureB ();
	
	MOAIBox2DFixture* moaiFixtureA = ( MOAIBox2DFixture* )fixtureA->GetUserData ();
	MOAIBox2DFixture* moaiFixtureB = ( MOAIBox2DFixture* )fixtureB->GetUserData ();
	
	moaiFixtureA->HandleCollision ( PRE_SOLVE, moaiFixtureB, this );
	moaiFixtureB->HandleCollision ( PRE_SOLVE, moaiFixtureA, this );
}

//----------------------------------------------------------------//
void MOAIBox2DArbiter::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "ALL", ( u32 )ALL );
	state.DefineClassConst ( klass, "BEGIN", ( u32 )BEGIN );
	state.DefineClassConst ( klass, "END", ( u32 )END );
	state.DefineClassConst ( klass, "PRE_SOLVE", ( u32 )PRE_SOLVE );
	state.DefineClassConst ( klass, "POST_SOLVE", ( u32 )POST_SOLVE );

}

//----------------------------------------------------------------//
void MOAIBox2DArbiter::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "getContactNormal", _getContactNormal, MRB_ARGS_NONE ());
	state.DefineInstanceMethod ( klass, "getContactPoints", _getContactPoints, MRB_ARGS_NONE ());
	state.DefineInstanceMethod ( klass, "getNormalImpulse", _getNormalImpulse, MRB_ARGS_NONE ());
	state.DefineInstanceMethod ( klass, "getTangentImpulse", _getTangentImpulse, MRB_ARGS_NONE ());
	state.DefineInstanceMethod ( klass, "setContactEnabled", _setContactEnabled, MRB_ARGS_REQ ( 1 ));
	
}

//----------------------------------------------------------------//
void MOAIBox2DArbiter::SetWorld ( const MOAIBox2DWorld& world ) {
	
	this->mWorld = &world;
}
