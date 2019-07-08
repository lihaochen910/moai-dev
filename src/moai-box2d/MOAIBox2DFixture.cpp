// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <Box2D/Box2D.h>
#include <moai-box2d/MOAIBox2DArbiter.h>
#include <moai-box2d/MOAIBox2DBody.h>
#include <moai-box2d/MOAIBox2DFixture.h>
#include <moai-box2d/MOAIBox2DWorld.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	destroy
	@text	Schedule fixture for destruction.
	
	@in		MOAIBox2DFixture self
	@out	nil
*/
mrb_value MOAIBox2DFixture::_destroy ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DFixture, "U" )
	
	if ( self->mWorld ) {
		self->mWorld->ScheduleDestruction ( *self );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getBody
	@text	Returns the body that owns the fixture.
	
	@in		MOAIBox2DFixture self
	@out	MOAIBox2DBody body
*/
mrb_value MOAIBox2DFixture::_getBody ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DFixture, "U" )
	
	if ( !self->mFixture ) return mrb_nil_value ();
	
	b2Body* body = self->mFixture->GetBody ();
	if ( body ) {
		MOAIBox2DBody* moaiBody = ( MOAIBox2DBody* )body->GetUserData ();
		if ( moaiBody ) {
			return moaiBody->PushRubyUserdata ( state );
		}
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getFilter
	@text	See Box2D documentation.
	
	@in		MOAIBox2DFixture self
	@out	number categoryBits
	@out	number maskBits
	@out	number groupIndex
*/
mrb_value MOAIBox2DFixture::_getFilter ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DFixture, "U" )
	
	if ( !self->mFixture ) return mrb_nil_value ();
	
	const b2Filter& filterData = self->mFixture->GetFilterData ();

	mrb_value ret [ 3 ];
	ret [ 0 ] = state.ToRValue ( filterData.categoryBits );
	ret [ 1 ] = state.ToRValue ( filterData.maskBits );
	ret [ 2 ] = state.ToRValue ( filterData.groupIndex );

	return mrb_ary_new_from_values ( state, 3, ret );
}

//----------------------------------------------------------------//
/**	@lua	setCollisionHandler
	@text	Sets a Ruby function to call when collisions occur. The handler should
			accept the following parameters: ( phase, fixtureA, fixtureB, arbiter ). 'phase' will
			be one of the phase masks. 'fixtureA' will be the fixture receiving the collision.
			'fixtureB' will be the other fixture in the collision. 'arbiter' will be the
			MOAIArbiter. Note that the arbiter is only good for the current collision: do
			not keep references to it for later use.
	
	@in		MOAIBox2DFixture self
	@in		function handler
	@opt	number phaseMask		Any bitwise combination of MOAIBox2DArbiter.BEGIN, MOAIBox2DArbiter.END,
									MOAIBox2DArbiter.POST_SOLVE, MOAIBox2DArbiter.PRE_SOLVE,
									MOAIBox2DArbiter.ALL
	@opt	number categoryMask		Check against opposing fixture's category bits and generate collision events if match.
	@out	nil
*/
mrb_value MOAIBox2DFixture::_setCollisionHandler ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DFixture, "UF" )
	
	self->mCollisionHandler.SetRef ( state.GetParamValue ( 1 ) );
	self->mCollisionPhaseMask = state.GetParamValue < u32 >( 2, MOAIBox2DArbiter::ALL );
	self->mCollisionCategoryMask = state.GetParamValue < u32 >( 3, 0xffffffff );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setDensity
	@text	See Box2D documentation.
	
	@in		MOAIBox2DFixture self
	@in		number density			In kg/units^2, converted to kg/m^2
	@out	nil
*/
mrb_value MOAIBox2DFixture::_setDensity ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DFixture, "UN" )
	
	if ( !self->mFixture ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DFixture_MissingInstance );
		return mrb_nil_value ();
	}
	
	float unitsToMeters = self->GetUnitsToMeters();
	float density = state.GetParamValue < float >( 1, 0.0f ) / (unitsToMeters * unitsToMeters);
	self->mFixture->SetDensity ( density );

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setFilter
	@text	See Box2D documentation.
	
	@in		MOAIBox2DFixture self
	@in		number categoryBits
	@opt	number maskBits
	@opt	number groupIndex
	@out	nil
*/
mrb_value MOAIBox2DFixture::_setFilter ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DFixture, "UN" )
	
	if ( !self->mFixture ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DFixture_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2Filter filter = self->mFixture->GetFilterData ();
	
	filter.categoryBits		= ( uint16 )state.GetParamValue < u32 >( 1, 0 );
	filter.maskBits			= ( uint16 )state.GetParamValue < u32 >( 2, 0xffffffff );
	filter.groupIndex		= ( int16 )state.GetParamValue < int >( 3, ( int )filter.groupIndex );
	
	self->mFixture->SetFilterData ( filter );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setFriction
	@text	See Box2D documentation.
	
	@in		MOAIBox2DFixture self
	@in		number friction
	@out	nil
*/
mrb_value MOAIBox2DFixture::_setFriction ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DFixture, "UN" )
	
	if ( !self->mFixture ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DFixture_MissingInstance );
		return mrb_nil_value ();
	}
	
	float friction = state.GetParamValue < float >( 1, 0.0f );
	self->mFixture->SetFriction ( friction );

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setRestitution
	@text	See Box2D documentation.
	
	@in		MOAIBox2DFixture self
	@in		number restitution
	@out	nil
*/
mrb_value MOAIBox2DFixture::_setRestitution ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DFixture, "UN" )
	
	if ( !self->mFixture ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DFixture_MissingInstance );
		return mrb_nil_value ();
	}
	
	float restitution = state.GetParamValue < float >( 1, 0.0f );
	self->mFixture->SetRestitution ( restitution );

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setSensor
	@text	See Box2D documentation.
	
	@in		MOAIBox2DFixture self
	@opt	boolean isSensor		Default value is 'true'
	@out	nil
*/
mrb_value MOAIBox2DFixture::_setSensor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DFixture, "U" )
	
	if ( !self->mFixture ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DFixture_MissingInstance );
		return mrb_nil_value ();
	}
	
	bool isSensor = state.GetParamValue < bool >( 1, true );
	self->mFixture->SetSensor ( isSensor );

	return mrb_nil_value ();
}

//================================================================//
// MOAIBox2DFixture
//================================================================//

//----------------------------------------------------------------//
void MOAIBox2DFixture::Clear () {

	if ( this->mFixture ) {
		this->mFixture = 0;
	}
	this->mWorld = 0;
}

//----------------------------------------------------------------//
void MOAIBox2DFixture::Destroy () {

	if ( this->mFixture ) {
		b2Body* body = this->mFixture->GetBody ();
		body->DestroyFixture ( this->mFixture );
		this->Clear ();
	}
}

//----------------------------------------------------------------//
void MOAIBox2DFixture::HandleCollision ( u32 eventType, MOAIBox2DFixture* other, MOAIBox2DArbiter* arbiter ) {
	
	if ( this->mCollisionPhaseMask & eventType ) {
		
		const b2Filter& otherFilter = other->mFixture->GetFilterData ();
		
		if ( this->mCollisionCategoryMask & otherFilter.categoryBits ) {
		
			if ( this->mCollisionHandler ) {
			
				MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
				if ( this->mCollisionHandler ) {

					mrb_value argv [ 4 ];
					argv [ 0 ] = state.ToRValue ( eventType );
					argv [ 1 ] = this->PushRubyUserdata ( state );
					argv [ 2 ] = other->PushRubyUserdata ( state );
					argv [ 3 ] = arbiter->PushRubyUserdata ( state );
					
					state.FuncCall ( this->mCollisionHandler, "call", 4, argv );
				}
			}
		}
	}
}

//----------------------------------------------------------------//
u32 MOAIBox2DFixture::LoadVerts ( MOAIRubyState& state, int idx, b2Vec2* verts, u32 max, float unitsToMeters  ) {
	
	//int itr = state.PushTableItr ( idx );
	//idx = 0;
	//
	//u32 total = 0;
	//for ( ; state.TableItrNext ( itr ) && ( total < max ); ++idx ) {
	//	
	//	float val = state.GetParamValue < float >( -1, 0 ); // TODO: add error checking
	//	
	//	if ( idx & 0x01 ) {
	//		verts [ total ].y = val * unitsToMeters;
	//		total++;
	//	}
	//	else {
	//		verts [ total ].x = val * unitsToMeters;
	//	}
	//}
	//return total;
	return 0;
}

//----------------------------------------------------------------//
MOAIBox2DFixture::MOAIBox2DFixture () :
	mFixture ( 0 ),
	mCollisionPhaseMask ( 0 ),
	mCollisionCategoryMask ( 0 ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIBox2DFixture::~MOAIBox2DFixture () {
}

//----------------------------------------------------------------//
void MOAIBox2DFixture::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
}

//----------------------------------------------------------------//
void MOAIBox2DFixture::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	state.DefineInstanceMethod ( klass, "destroy",				_destroy, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getBody",				_getBody, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getFilter",				_getFilter, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setCollisionHandler",	_setCollisionHandler, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setDensity",				_setDensity, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setFilter",				_setFilter, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setFriction",			_setFriction, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setRestitution",			_setRestitution, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setSensor",				_setSensor, MRB_ARGS_NONE () );
}

//----------------------------------------------------------------//
void MOAIBox2DFixture::SetFixture ( b2Fixture* fixture ) {

	this->mFixture = fixture;
	if ( fixture ) {
		fixture->SetUserData ( this );
	}
}
