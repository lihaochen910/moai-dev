// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <Box2D/Box2D.h>
#include <moai-box2d/MOAIBox2DBody.h>
#include <moai-box2d/MOAIBox2DFixture.h>
#include <moai-box2d/MOAIBox2DWorld.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/** @lua addChain
	@text Create and add a set of collision edges to the body.
	
	@in		MOAIBox2DBody self
	@in		table verts					Array containing vertex coordinate components ( t[1] = x0, t[2] = y0, t[3] = x1, t[4] = y1... )
	@opt	boolean closeChain			Default value is false.
	@out	MOAIBox2DFixture fixture	Returns nil on failure.
*/
mrb_value MOAIBox2DBody::_addChain ( mrb_state* M, mrb_value context ) {
    MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
 
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
 
	float unitsToMeters = self->GetUnitsToMeters ();
	u32 totalCoords = ( u32 )RARRAY_LEN ( state.GetParamValue ( 1 ) );
	if( totalCoords < 4 || totalCoords % 2 != 0 ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_InvalidVertexCount_D, totalCoords );
		return mrb_nil_value ();
	}
 
	u32 totalVerts = totalCoords / 2;                       
	b2Vec2* verts = ( b2Vec2* )alloca ( sizeof ( b2Vec2 ) * totalVerts );
	int numVerts = MOAIBox2DFixture::LoadVerts ( state, 1, verts, totalVerts, unitsToMeters );
	if ( numVerts ) {
		bool closeChain = state.GetParamValue < bool >( 2, false );
 
		b2ChainShape chainShape;                               
		if( closeChain ) {
			chainShape.CreateLoop(verts, numVerts);
		}
		else {
			chainShape.CreateChain(verts, numVerts);
		}
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &chainShape;
		MOAIBox2DFixture* fixture = state.CreateClassInstance < MOAIBox2DFixture >();
		fixture->SetFixture ( self->mBody->CreateFixture ( &fixtureDef ));
		fixture->SetWorld ( self->mWorld );
		self->mWorld->RubyRetain ( fixture );
		return fixture->PushRubyUserdata ( state );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	addCircle
	@text	Create and add circle fixture to the body.
	
	@in		MOAIBox2DBody self
	@in		number x	in units, world coordinates, converted to meters
	@in		number y	in units, world coordinates, converted to meters
	@in		number radius	in units, converted to meters
	@out	MOAIBox2DFixture fixture
*/
mrb_value MOAIBox2DBody::_addCircle ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "UNNN" )
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2CircleShape circleShape;

	circleShape.m_p.x		= state.GetParamValue < float >( 1, 0.0f ) * unitsToMeters;
	circleShape.m_p.y		= state.GetParamValue < float >( 2, 0.0f ) * unitsToMeters;
	circleShape.m_radius	= state.GetParamValue < float >( 3, 1.0f ) * unitsToMeters;
	
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &circleShape;
	
	MOAIBox2DFixture* fixture = state.CreateClassInstance < MOAIBox2DFixture >();
	fixture->SetFixture ( self->mBody->CreateFixture ( &fixtureDef ));
	fixture->SetWorld ( self->mWorld );
	self->mWorld->RubyRetain ( fixture );

	return fixture->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	addEdges
 @text	Create and add a polygon fixture to the body.
 
 @in		MOAIBox2DBody self
 @in		table verts				Array containing vertex coordinate components in units, world coordinates, converted to meters ( t[1] = x0, t[2] = y0, t[3] = x1, t[4] = y1... )
 @out	    table fixtures			Array containing MOAIBox2DFixture fixtures. Returns nil on failure.
 */
mrb_value MOAIBox2DBody::_addEdges ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	float unitsToMeters = self->GetUnitsToMeters ();
	u32 totalVerts = ( u32 )RARRAY_LEN ( state.GetParamValue ( 1 )) / 2;
	
	if (totalVerts) {
		
		b2Vec2* verts = ( b2Vec2* )alloca ( sizeof ( b2Vec2 ) * totalVerts );
		int numVerts = MOAIBox2DFixture::LoadVerts ( state, 2, verts, totalVerts, unitsToMeters );
		
		if ( numVerts ) {
			MOAIRubyState retstate ( M );

			mrb_value ary = mrb_ary_new ( M );
			mrb_ary_resize ( M, ary, numVerts );

			b2EdgeShape edgeShape;
			for ( u32 i = 0; i < totalVerts; i+=2) {
				edgeShape.Set(verts[i], verts[i+1]);
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &edgeShape;
				MOAIBox2DFixture* fixture = state.CreateClassInstance < MOAIBox2DFixture >();
				fixture->SetFixture ( self->mBody->CreateFixture ( &fixtureDef ));
				fixture->SetWorld ( self->mWorld );
				self->mWorld->RubyRetain ( fixture );
				//lua_pushnumber ( retstate, idx );
				mrb_ary_push ( M, ary, fixture->PushRubyUserdata ( retstate ));
				//lua_settable ( retstate, -3 );
			}
			
			return ary;
		}
	}	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	addPolygon
	@text	Create and add a polygon fixture to the body.
	
	@in		MOAIBox2DBody self
	@in		table verts					Array containing vertex coordinate components in units, world coordinates, converted to meters. ( t[1] = x0, t[2] = y0, t[3] = x1, t[4] = y1... )
	@out	MOAIBox2DFixture fixture	Returns nil on failure.
*/
mrb_value MOAIBox2DBody::_addPolygon ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )

	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}

	float unitsToMeters = self->GetUnitsToMeters ();

	b2Vec2 verts [ b2_maxPolygonVertices ];
	
	// TODO: add error checking
	int numVerts = MOAIBox2DFixture::LoadVerts ( state, 1, verts, b2_maxPolygonVertices, unitsToMeters );
	
	if (( numVerts < 3 ) || ( numVerts > b2_maxPolygonVertices )) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_InvalidVertexCount_D, numVerts );
		return mrb_nil_value ();
	}
	
	if ( numVerts ) {
		
		b2PolygonShape polyShape;
		polyShape.Set ( verts, numVerts );
		
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &polyShape;
		
		MOAIBox2DFixture* fixture = state.CreateClassInstance < MOAIBox2DFixture >();
		fixture->SetFixture ( self->mBody->CreateFixture ( &fixtureDef ));
		fixture->SetWorld ( self->mWorld );
		self->mWorld->RubyRetain ( fixture );

		return fixture->PushRubyUserdata ( state );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	addRect
	@text	Create and add a rect fixture to the body.
	
	@in		MOAIBox2DBody self
	@in		number xMin	in units, world coordinates, converted to meters
	@in		number yMin	in units, world coordinates, converted to meters
	@in		number xMax	in units, world coordinates, converted to meters
	@in		number yMax	in units, world coordinates, converted to meters
	@in		number angle
	@out	MOAIBox2DFixture fixture
*/
mrb_value MOAIBox2DBody::_addRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "UNNNN" )
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	ZLRect rect = state.GetRect < float >( 1 );
	rect.Bless ();
	
	float angle = state.GetParamValue < float >( 4, 0.0f );
	
	float hx = rect.Width () * 0.5f * unitsToMeters;
	float hy = rect.Height () * 0.5f * unitsToMeters;
	
	b2Vec2 center;
	center.x = ( rect.mXMin * unitsToMeters ) + hx;
	center.y = ( rect.mYMin * unitsToMeters ) + hy;
	
	b2PolygonShape polygonShape;
	polygonShape.SetAsBox ( hx, hy, center, angle );
	
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &polygonShape;
	
	MOAIBox2DFixture* fixture = state.CreateClassInstance < MOAIBox2DFixture >();
	fixture->SetFixture ( self->mBody->CreateFixture ( &fixtureDef ));
	fixture->SetWorld ( self->mWorld );
	self->mWorld->RubyRetain ( fixture );

	return fixture->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	applyAngularImpulse
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@in		number angularImpulse	in kg * units / s, converted to kg * m / s
	@opt	boolean wake			wake this body. Default is true
	@out	nil
*/
mrb_value MOAIBox2DBody::_applyAngularImpulse ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "UN" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	float impulse = state.GetParamValue < float >( 1, 0.0f ) * self->GetUnitsToMeters();	
	bool wake = state.GetParamValue < bool >( 2, true );
	
	self->mBody->ApplyAngularImpulse ( impulse, wake );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	applyForce
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@in		number forceX	in kg * units / s^2, converted to N [kg * m / s^2]
	@in		number forceY	in kg * units / s^2, converted to N [kg * m / s^2]
	@opt	number pointX	in units, world coordinates, converted to meters
	@opt	number pointY	in units, world coordinates, converted to meters
	@opt	boolean wake	wake this body. Default is true
	@out	nil
*/
mrb_value MOAIBox2DBody::_applyForce ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "UNN" )
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2Vec2 force;
	force.x = state.GetParamValue < float >( 1, 0.0f ) * unitsToMeters;
	force.y = state.GetParamValue < float >( 2, 0.0f ) * unitsToMeters;
	
	b2Vec2 point;
	point.x = state.GetParamValue < float >( 3, 0.0f ) * unitsToMeters;
	point.y = state.GetParamValue < float >( 4, 0.0f ) * unitsToMeters;	
	
	bool wake = state.GetParamValue < bool >( 5, true );
	
	self->mBody->ApplyForce ( force, point, wake );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	applyLinearImpulse
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@in		number impulseX	in kg * units / s, converted to kg * m / s
	@in		number impulseY	in kg * units / s, converted to kg * m / s
	@opt	number pointX	in units, world coordinates, converted to meters
	@opt	number pointY	in units, world coordinates, converted to meters
	@opt	boolean wake	wake this body. Default is true
	@out	nil
*/
mrb_value MOAIBox2DBody::_applyLinearImpulse ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "UNN" )
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2Vec2 impulse;
	impulse.x = state.GetParamValue < float >( 1, 0.0f ) * unitsToMeters;
	impulse.y = state.GetParamValue < float >( 2, 0.0f ) * unitsToMeters;
	
	b2Vec2 point;
	point.x = state.GetParamValue < float >( 3, 0.0f ) * unitsToMeters;
	point.y = state.GetParamValue < float >( 4, 0.0f ) * unitsToMeters;
	bool wake = state.GetParamValue < bool >( 5, true );
	
	self->mBody->ApplyLinearImpulse ( impulse, point, wake );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	applyTorque
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@opt	number torque	in (kg * units / s^2) * units, converted to N-m. Default value is 0.
	@opt	boolean wake	wake this body. Default is true
	@out	nil
*/
mrb_value MOAIBox2DBody::_applyTorque ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	float unitsToMeters = self->GetUnitsToMeters();
	/* Convert from N-m (kg m / s^2) * m => (kg unit / s^2) * unit */
	float torque = state.GetParamValue < float >( 1, 0.0f ) * unitsToMeters * unitsToMeters;
	bool wake = state.GetParamValue < bool >( 2, true );

	self->mBody->ApplyTorque ( torque, wake );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	destroy
	@text	Schedule body for destruction.
	
	@in		MOAIBox2DBody self
	@out	nil
*/
mrb_value MOAIBox2DBody::_destroy ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	if ( self->mWorld ) {
		self->mWorld->ScheduleDestruction ( *self );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getAngle
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@out	number angle		Angle in degrees, converted from radians
*/
mrb_value MOAIBox2DBody::_getAngle ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	float angle = self->mBody->GetAngle () * ( float )R2D;
	return state.ToRValue ( angle );
}

//----------------------------------------------------------------//
/**	@lua	getAngularVelocity
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@out	number omega		Angular velocity in degrees/s, converted from radians/s
*/
mrb_value MOAIBox2DBody::_getAngularVelocity ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	float omega = self->mBody->GetAngularVelocity () * ( float )R2D;
	return state.ToRValue ( omega );
}

//----------------------------------------------------------------//
/**	@lua	getContactList
	@text	Returns list of MOAIBox2DBody that are in contact with this body
	
	@in		MOAIBox2DBody self
	@in 	boolean	touching
	@out	... bodies
*/
mrb_value MOAIBox2DBody::_getContactList ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	b2Body* body = self->mBody;
	if ( !body ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	bool touching = state.GetParamValue < bool >( 2, false );

	mrb_value ary = mrb_ary_new ( M );
	for ( b2ContactEdge* ce = body->GetContactList (); ce != NULL; ce = ce->next ) {

		if ( !touching || ce->contact->IsTouching () ) {
			MOAIBox2DBody* moaiBody = ( MOAIBox2DBody* )ce->other->GetUserData ();
			if ( moaiBody ) {
				mrb_ary_push ( M, ary, moaiBody->PushRubyUserdata ( state ));
			}
		}
	}

	return ary;
}

//----------------------------------------------------------------//
/**	@lua	getInertia
	@text   See Box2D documentation.

	@in		MOAIBox2DBody self
	@out	number inertia			Calculated inertia (based on last call to resetMassData()). In kg * unit/s^s, converted from kg*m/s^2.
*/
mrb_value MOAIBox2DBody::_getInertia ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}

	float unitsToMeters = self->GetUnitsToMeters();
	float inertia = self->mBody->GetInertia();
	inertia /= unitsToMeters;
	
	return state.ToRValue ( inertia );
}

//----------------------------------------------------------------//
/**	@lua	getGravityScale
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@out	number gravityScale
*/
mrb_value MOAIBox2DBody::_getGravityScale ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	float scale = self->mBody->GetGravityScale ();

	return state.ToRValue ( scale );
}

//----------------------------------------------------------------//
/**	@lua	getLinearVelocity
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@out	number velocityX	in unit/s, converted from m/s
	@out	number velocityY	in unit/s, converted from m/s
*/
mrb_value MOAIBox2DBody::_getLinearVelocity ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2Vec2 velocity = self->mBody->GetLinearVelocity ();

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( velocity.x / unitsToMeters );
	ret [ 1 ] = state.ToRValue ( velocity.y / unitsToMeters );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getLocalCenter
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@out	number centerX	in units, local coordinates, converted from meters
	@out	number centerY	in units, local coordinates, converted from meters
*/
mrb_value MOAIBox2DBody::_getLocalCenter ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2Vec2 center = self->mBody->GetLocalCenter ();

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( center.x / unitsToMeters );
	ret [ 1 ] = state.ToRValue ( center.y / unitsToMeters );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getMass
	@text   See Box2D documentation.

	@in		MOAIBox2DBody self
	@out	number mass				Calculated mass in kg (based on last call to resetMassData()).
*/
mrb_value MOAIBox2DBody::_getMass ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	return state.ToRValue ( self->mBody->GetMass () );
}

//----------------------------------------------------------------//
/**	@lua	getPosition
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@out	number positionX	in units, world coordinates, converted from meters
	@out	number positionY	in units, world coordinates, converted from meters
*/
mrb_value MOAIBox2DBody::_getPosition ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2Vec2 position = self->mBody->GetPosition ();

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( position.x / unitsToMeters );
	ret [ 1 ] = state.ToRValue ( position.y / unitsToMeters );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getWorldCenter
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@out	number worldX	in units, world coordinates, converted from meters
	@out	number worldY	in units, world coordinates, converted from meters
*/
mrb_value MOAIBox2DBody::_getWorldCenter ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2Vec2 center = self->mBody->GetWorldCenter ();

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( center.x / unitsToMeters );
	ret [ 1 ] = state.ToRValue ( center.y / unitsToMeters );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	isActive
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@out	boolean isActive
*/
mrb_value MOAIBox2DBody::_isActive ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	bool isActive = self->mBody->IsActive ();
	
	return state.ToRValue ( isActive );
}

//----------------------------------------------------------------//
/**	@lua	isAwake
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@out	boolean isAwake
*/
mrb_value MOAIBox2DBody::_isAwake ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	bool isAwake = self->mBody->IsAwake ();
	
	return state.ToRValue ( isAwake );
}

//----------------------------------------------------------------//
/**	@lua	isBullet
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@out	boolean isBullet
*/
mrb_value MOAIBox2DBody::_isBullet ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	bool isBullet = self->mBody->IsBullet ();
	
	return state.ToRValue ( isBullet );
}

//----------------------------------------------------------------//
/**	@lua	isFixedRotation
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@out	boolean isFixedRotation
*/
mrb_value MOAIBox2DBody::_isFixedRotation ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	bool isFixedRotation = self->mBody->IsFixedRotation ();
	
	return state.ToRValue ( isFixedRotation );
}

//----------------------------------------------------------------//
/**	@lua	resetMassData
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@out	nil
*/
mrb_value MOAIBox2DBody::_resetMassData ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	self->mBody->ResetMassData ();
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setActive
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@opt	boolean active			Default value is false.
	@out	nil
*/
mrb_value MOAIBox2DBody::_setActive ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )

	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}

	bool active = state.GetParamValue < bool >( 1, false );
	self->mBody->SetActive ( active );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setAngularDamping
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@in		number damping
	@out	nil
*/
mrb_value MOAIBox2DBody::_setAngularDamping ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "UN" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	float damping = state.GetParamValue < float >( 2, 0.0f );
	self->mBody->SetAngularDamping ( damping );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setAngularVelocity
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@opt	number omega		Angular velocity in degrees/s, converted to radians/s. Default value is 0.
	@out	nil
*/
mrb_value MOAIBox2DBody::_setAngularVelocity ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	float omega = state.GetParamValue < float >( 2, 0.0f ) * ( float )D2R;
	self->mBody->SetAngularVelocity ( omega );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setAwake
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@opt	boolean awake		Default value is true.
	@out	nil
*/
mrb_value MOAIBox2DBody::_setAwake ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	bool awake = state.GetParamValue < bool >( 2, true );
	self->mBody->SetAwake ( awake );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setBullet
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@opt	boolean bullet		Default value is true.
	@out	nil
*/
mrb_value MOAIBox2DBody::_setBullet ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	bool bullet = state.GetParamValue < bool >( 2, true );
	self->mBody->SetBullet ( bullet );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setFixedRotation
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@opt	boolean fixedRotation		Default value is true.
	@out	nil
*/
mrb_value MOAIBox2DBody::_setFixedRotation ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	bool fixedRotation = state.GetParamValue < bool >( 2, true );
	self->mBody->SetFixedRotation ( fixedRotation );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setGravityScale
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@opt	number gravityScale.
	@out	nil
*/
mrb_value MOAIBox2DBody::_setGravityScale ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "UN" );

	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}

	float scale = state.GetParamValue < float >( 2, 0.0f );
	self->mBody->SetGravityScale ( scale );

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setLinearDamping
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@opt	number damping
	@out	nil
*/
mrb_value MOAIBox2DBody::_setLinearDamping ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "UN" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	float damping = state.GetParamValue < float >( 2, 0.0f );
	self->mBody->SetLinearDamping ( damping );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setLinearVelocity
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@opt	number velocityX	in unit/s, converted to m/s. Default is 0.
	@opt	number velocityY	in unit/s, converted to m/s. Default is 0.
	@out	nil
*/
mrb_value MOAIBox2DBody::_setLinearVelocity ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	b2Vec2 v;
	v.x		= state.GetParamValue < float >( 2, 0.0f ) * unitsToMeters;
	v.y		= state.GetParamValue < float >( 3, 0.0f ) * unitsToMeters;
	
	self->mBody->SetLinearVelocity ( v );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setMassData
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@in		number mass			in kg.
	@opt	number I			in kg*units^2, converted to kg * m^2. Default is previous value for I.
	@opt	number centerX		in units, local coordinates, converted to meters. Default is previous value for centerX.
	@opt	number centerY		in units, local coordinates, converted to meters. Default is previous value for centerY.
	@out	nil
*/
mrb_value MOAIBox2DBody::_setMassData ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "UN" )
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	if ( self->mWorld->IsLocked ()) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DWorld_IsLocked );
		return mrb_nil_value ();
	}
	
	b2MassData massData;
	self->mBody->GetMassData ( &massData );
	
	massData.mass		= state.GetParamValue < float >( 2, massData.mass );
	massData.I			= state.GetParamValue < float >( 3, massData.I ) * unitsToMeters * unitsToMeters;
	massData.center.x	= state.GetParamValue < float >( 4, massData.center.x ) * unitsToMeters;
	massData.center.y	= state.GetParamValue < float >( 5, massData.center.y ) * unitsToMeters;
	
	self->mBody->SetMassData ( &massData );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setTransform
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@opt	number positionX		in units, world coordinates, converted to meters. Default is 0.
	@opt	number positionY		in units, world coordinates, converted to meters. Default is 0.
	@opt	number angle			In degrees, converted to radians. Default is 0.
	@out	nil
*/
mrb_value MOAIBox2DBody::_setTransform ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return mrb_nil_value ();
	}
	
	if ( self->mWorld->IsLocked ()) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DWorld_IsLocked );
		return mrb_nil_value ();
	}
	
	b2Vec2 position;
	position.x		= state.GetParamValue < float >( 2, 0.0f ) * unitsToMeters;
	position.y		= state.GetParamValue < float >( 3, 0.0f ) * unitsToMeters;
	float angle		= state.GetParamValue < float >( 4, 0.0f ) * ( float )D2R;
	
	self->mBody->SetTransform ( position, angle );
	self->ScheduleUpdate ();
	
	return mrb_nil_value ();
}


//----------------------------------------------------------------//
/**	@lua	setType
	@text	See Box2D documentation.
	
	@in		MOAIBox2DBody self
	@in		number type		One of MOAIBox2DBody.DYNAMIC, MOAIBox2DBody.KINEMATIC, MOAIBox2DBody.STATIC
	@out	nil
*/
mrb_value MOAIBox2DBody::_setType ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIBox2DBody, "UN" )
	
	if ( !self->mBody ) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DBody_MissingInstance );
		return context;
	}
	
	if ( self->mWorld->IsLocked ()) {
		MOAILogF ( state, ZLLog::LOG_ERROR, MOAISTRING_MOAIBox2DWorld_IsLocked );
		return context;
	}
	
	u32 type	= state.GetParamValue < u32 >( 1, 0 );
	
	self->mBody->SetType ( ( b2BodyType ) type );
	self->ScheduleUpdate ();
	
	return context;
}

//================================================================//
// MOAIBox2DBody
//================================================================//

//----------------------------------------------------------------//
void MOAIBox2DBody::Destroy () {

	b2World* world = this->mWorld->mWorld;
	world->DestroyBody ( this->mBody );
	this->mBody = 0;
}

//----------------------------------------------------------------//
MOAIBox2DBody::MOAIBox2DBody () :
	mBody ( 0 ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAITransformBase )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIBox2DBody::~MOAIBox2DBody () {
}

//----------------------------------------------------------------//
void MOAIBox2DBody::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAITransformBase::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "DYNAMIC",		( u32 )b2_dynamicBody );
	state.DefineClassConst ( klass, "KINEMATIC",	( u32 )b2_kinematicBody );
	state.DefineClassConst ( klass, "STATIC",		( u32 )b2_staticBody );
}

//----------------------------------------------------------------//
void MOAIBox2DBody::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAITransformBase::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "addChain",				_addChain, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "addCircle",				_addCircle, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "addEdges",               _addEdges, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "addPolygon",				_addPolygon, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "addRect",				_addRect, MRB_ARGS_REQ ( 5 ) );
	state.DefineInstanceMethod ( klass, "applyAngularImpulse",	_applyAngularImpulse, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "applyForce",				_applyForce, MRB_ARGS_ARG ( 2, 3 ) );
	state.DefineInstanceMethod ( klass, "applyLinearImpulse",		_applyLinearImpulse, MRB_ARGS_ARG ( 2, 3 ) );
	state.DefineInstanceMethod ( klass, "applyTorque",			_applyTorque, MRB_ARGS_ARG ( 0, 2 ) );
	state.DefineInstanceMethod ( klass, "destroy",				_destroy, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getAngle",				_getAngle, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getAngularVelocity",		_getAngularVelocity, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getContactList",			_getContactList, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getInertia",				_getInertia, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getGravityScale",		_getGravityScale, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getLinearVelocity",		_getLinearVelocity, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getLocalCenter",			_getLocalCenter, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getMass",				_getMass, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getPosition",			_getPosition, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getWorldCenter",			_getWorldCenter, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isActive",				_isActive, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isAwake",				_isAwake, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isBullet",				_isBullet, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isFixedRotation",		_isFixedRotation, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "resetMassData",			_resetMassData, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setActive",				_setActive, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setAngularDamping",		_setAngularDamping, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setAngularVelocity",		_setAngularVelocity, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setAwake",				_setAwake, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setBullet",				_setBullet, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setFixedRotation",		_setFixedRotation, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setGravityScale",		_setGravityScale, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setLinearDamping",		_setLinearDamping, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setLinearVelocity",		_setLinearVelocity, MRB_ARGS_ARG ( 0, 2 ) );
	state.DefineInstanceMethod ( klass, "setMassData",			_setMassData, MRB_ARGS_ARG ( 1, 3 ) );
	state.DefineInstanceMethod ( klass, "setTransform",			_setTransform, MRB_ARGS_ARG ( 0, 3 ) );
	state.DefineInstanceMethod ( klass, "setType",				_setType, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAIBox2DBody::SetBody ( b2Body* body ) {

	this->mBody = body;
	body->SetUserData ( this );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAIBox2DBody::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	// TODO: these values may need to be cached for performance reasons
	if ( MOAITransform::MOAITransformAttr::Check ( attrID )) {
	
		const b2Transform & xform = mBody->GetTransform();
		
		switch ( UNPACK_ATTR ( attrID )) {
		
			case MOAITransform::ATTR_X_LOC: {
				float x = attr.Apply ( xform.p.x, op, MOAIAttribute::ATTR_READ_WRITE ) * this->GetUnitsToMeters ();
				mBody->SetTransform ( b2Vec2( x, xform.p.y), xform.q.GetAngle() );
				return true;
			}
				
			case MOAITransform::ATTR_Y_LOC: {
				float y = attr.Apply ( xform.p.y, op, MOAIAttribute::ATTR_READ_WRITE ) * this->GetUnitsToMeters ();
				mBody->SetTransform ( b2Vec2( xform.p.x, y ), xform.q.GetAngle() );
				return true;
			}
				
			case MOAITransform::ATTR_Z_ROT: {
				float angle = attr.Apply ( xform.q.GetAngle(), op, MOAIAttribute::ATTR_READ_WRITE );
				mBody->SetTransform ( xform.p,  ( float )((angle * D2R) + M_PI_4 ));
				return true;
			}
		}
	}
	return MOAITransformBase::MOAINode_ApplyAttrOp (attrID, attr, op );
}

//----------------------------------------------------------------//
void MOAIBox2DBody::MOAINode_Update () {

	if ( this->mBody ) {
		
		b2Transform transform = this->mBody->GetTransform ();
		float scale = 1.0f / this->GetUnitsToMeters ();
		
		float* m = this->mLocalToWorldMtx.m;
		
		m [ ZLAffine3D::C0_R0 ] = ( float )transform.q.GetXAxis().x;
		m [ ZLAffine3D::C0_R1 ] = ( float )transform.q.GetXAxis().y;

		m [ ZLAffine3D::C1_R0 ] = ( float )transform.q.GetYAxis().x;
		m [ ZLAffine3D::C1_R1 ] = ( float )transform.q.GetYAxis().y;

		m [ ZLAffine3D::C3_R0 ] = ( float )transform.p.x * scale;
		m [ ZLAffine3D::C3_R1 ] = ( float )transform.p.y * scale;
		
		this->mWorldToLocalMtx.Inverse ( this->mLocalToWorldMtx );
	}
}

//----------------------------------------------------------------//
void MOAIBox2DBody::MOAITransformBase_BuildLocalToWorldMtx ( ZLAffine3D& localToWorldMtx ) {
	UNUSED ( localToWorldMtx );
}
