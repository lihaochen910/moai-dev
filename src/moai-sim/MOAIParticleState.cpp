// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIParticleForce.h>
#include <moai-sim/MOAIParticlePlugin.h>
#include <moai-sim/MOAIParticleScript.h>
#include <moai-sim/MOAIParticleState.h>
#include <moai-sim/MOAIParticleSystem.h>

class MOAIDataBuffer;

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	clearForces
	@text	Removes all particle forces from the state.
	
	@in		MOAIParticleState self
	@out	nil
*/
mrb_value MOAIParticleState::_clearForces ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleState, "U" )
	
	self->ClearForces ();
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	pushForce
	@text	Adds a force to the state.
	
	@in		MOAIParticleState self
	@in		MOAIParticleForce force
	@out	nil
*/
mrb_value MOAIParticleState::_pushForce ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleState, "UU" )
	
	MOAIParticleForce* force = state.GetRubyObject < MOAIParticleForce >( 1, true );
	if ( force ) {
		self->PushForce ( *force );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setDamping
	@text	Sets damping for particle physics model.
	
	@in		MOAIParticleState self
	@in		number damping
	@out	nil
*/
mrb_value MOAIParticleState::_setDamping ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleState, "UN" )

	self->mDamping = state.GetParamValue < float >( 1, 0.0f );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setInitScript
	@text	Sets the particle script to use for initializing new particles.
	
	@in		MOAIParticleState self
	@opt	MOAIParticleScript script
	@out	nil
*/
mrb_value MOAIParticleState::_setInitScript ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleState, "U" )

	MOAIParticleScript* init = state.GetRubyObject < MOAIParticleScript >( 1, true );

	if ( init ) {
		init->Compile ();
	}
	self->mInit.Set ( *self, init );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setMass
	@text	Sets range of masses (chosen randomly) for particles
			initialized by the state.
	
	@in		MOAIParticleState self
	@in		number minMass
	@opt	number maxMass			Default value is minMass.
	@out	nil
*/
mrb_value MOAIParticleState::_setMass ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleState, "UN" )

	float m0 = state.GetParamValue < float >( 1, 0.0f );
	float m1 = state.GetParamValue < float >( 2, m0 );

	self->mMassRange [ 0 ] = m0;
	self->mMassRange [ 1 ] = m1;

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setNext
	@text	Sets the next state (if any).
	
	@in		MOAIParticleState self
	@opt	MOAIParticleState next		Default value is nil.
	@out	nil
*/
mrb_value MOAIParticleState::_setNext ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleState, "U" )
	
	self->mNext.Set ( *self, state.GetRubyObject < MOAIParticleState >( 1, true ));
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setPlugin
	@text	Sets the particle plugin to use for initializing and updating particles.
	
	@in		MOAIParticleState self
	@opt	MOAIParticlePlugin plugin
	@out	nil
*/
mrb_value MOAIParticleState::_setPlugin ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleState, "U" )

	self->mPlugin.Set ( *self, state.GetRubyObject < MOAIParticlePlugin >( 1, true ));
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setRenderScript
	@text	Sets the particle script to use for rendering particles.
	
	@in		MOAIParticleState self
	@opt	MOAIParticleScript script
	@out	nil
*/
mrb_value MOAIParticleState::_setRenderScript ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleState, "U" )

	MOAIParticleScript* render = state.GetRubyObject < MOAIParticleScript >( 1, true );

	if ( render ) {
		render->Compile ();
	}
	self->mRender.Set ( *self, render );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setTerm
	@text	Sets range of terms (chosen randomly) for particles
			initialized by the state.
	
	@in		MOAIParticleState self
	@in		number minTerm
	@opt	number maxTerm			Default value is minTerm.
	@out	nil
*/
mrb_value MOAIParticleState::_setTerm ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleState, "UN" )

	float t0 = state.GetParamValue < float >( 1, 0.0f );
	float t1 = state.GetParamValue < float >( 2, t0 );

	self->mTermRange [ 0 ] = t0;
	self->mTermRange [ 1 ] = t1;

	return mrb_nil_value ();
}

//================================================================//
// MOAIParticleState
//================================================================//

//----------------------------------------------------------------//
void MOAIParticleState::ClearForces () {

	while ( this->mForces.Count ()) {
	
		ForceNode* forceNode = this->mForces.Head ();
		this->mForces.PopFront ();
		
		this->RubyRelease ( forceNode->Data ());
		delete forceNode;
	}
}

//----------------------------------------------------------------//
void MOAIParticleState::GatherForces ( ZLVec3D& loc, ZLVec3D& velocity, float mass, float step ) {

	ZLVec3D result;

	ZLVec3D acceleration ( 0.0f, 0.0f, 0.0f );
	ZLVec3D offset ( 0.0f, 0.0f, 0.0f );
		
	ForceNode* forceNode = this->mForces.Head ();
	for ( ; forceNode; forceNode = forceNode->Next ()) {
		MOAIParticleForce* particleForce = forceNode->Data ();
		particleForce->Eval ( loc, mass, acceleration, offset );
	}
	
	velocity.mX += acceleration.mX * step;
	velocity.mY += acceleration.mY * step;
	
	velocity.Scale ( ZLFloat::Clamp ( 1.0f - ( this->mDamping * step ), 0.0f, 1.0f ));
	
	loc.mX += ( velocity.mX + offset.mX ) * step;
	loc.mY += ( velocity.mY + offset.mY ) * step;
}

//----------------------------------------------------------------//
void MOAIParticleState::InitParticle ( MOAIParticleSystem& system, MOAIParticle& particle ) {

	if ( this->mInit ) {
		this->mInit->Run ( system, particle, 0.0f, 0.0f );
	}
	
	MOAIParticlePlugin* plugin = this->mPlugin;
	if ( plugin ) {
		plugin->OnInit ( particle.mData, &particle.mData [ MOAIParticle::TOTAL_PARTICLE_REG ]);
	}
	
	particle.mAge = 0.0f;
	particle.mTerm = ZLFloat::Rand ( this->mTermRange [ 0 ], this->mTermRange [ 1 ]);
	particle.mMass = ZLFloat::Rand ( this->mMassRange [ 0 ], this->mMassRange [ 1 ]);
	particle.mState = this;
}

//----------------------------------------------------------------//
MOAIParticleState::MOAIParticleState () :
	mDamping ( 0.0f ) {

	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END
	
	this->mMassRange [ 0 ] = 1.0f;
	this->mMassRange [ 1 ] = 1.0f;
	
	this->mTermRange [ 0 ] = 1.0f;
	this->mTermRange [ 1 ] = 1.0f;
}

//----------------------------------------------------------------//
MOAIParticleState::~MOAIParticleState () {

	this->ClearForces ();
	
	this->mInit.Set ( *this, 0 );
	this->mRender.Set ( *this, 0 );
	this->mPlugin.Set ( *this, 0 );
	this->mNext.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAIParticleState::PushForce ( MOAIParticleForce& force ) {

	this->RubyRetain ( &force );

	ForceNode* forceNode = new ForceNode ();
	forceNode->Data ( &force );
	
	this->mForces.PushBack ( *forceNode );
}

//----------------------------------------------------------------//
void MOAIParticleState::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIParticleState::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "clearForces",			_clearForces, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "pushForce",			_pushForce, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setDamping",			_setDamping, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setInitScript",		_setInitScript, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setMass",				_setMass, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "setPlugin",			_setPlugin, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setNext",				_setNext, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setRenderScript",		_setRenderScript, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setTerm",				_setTerm, MRB_ARGS_ARG ( 1, 1 ) );

}

//----------------------------------------------------------------//
void MOAIParticleState::ProcessParticle ( MOAIParticleSystem& system, MOAIParticle& particle, float step ) {

	float t0 = particle.mAge / particle.mTerm;
	particle.mAge += step;
	
	if ( particle.mAge > particle.mTerm ) {
		particle.mAge = particle.mTerm;
	}
	
	float t1 = particle.mAge / particle.mTerm;
	
	float* r = particle.mData;
	
	ZLVec3D loc;
	ZLVec3D vel;
	
	loc.mX = r [ MOAIParticle::PARTICLE_X ];
	loc.mY = r [ MOAIParticle::PARTICLE_Y ];
	loc.mZ = 0.0f;
	
	vel.mX = r [ MOAIParticle::PARTICLE_DX ];
	vel.mY = r [ MOAIParticle::PARTICLE_DY ];
	vel.mZ = 0.0f;
	
	this->GatherForces ( loc, vel, particle.mMass, step );
	
	r [ MOAIParticle::PARTICLE_X ]	= loc.mX;
	r [ MOAIParticle::PARTICLE_Y ]	= loc.mY;
	r [ MOAIParticle::PARTICLE_DX ]	= vel.mX;
	r [ MOAIParticle::PARTICLE_DY ]	= vel.mY;

	if ( this->mRender ) {
		this->mRender->Run ( system, particle, t0, t1 );
	}
	
	MOAIParticlePlugin* plugin = this->mPlugin;
	if ( plugin ) {
		
		AKUParticleSprite sprite;
		plugin->OnRender ( particle.mData, &particle.mData [ MOAIParticle::TOTAL_PARTICLE_REG ], &sprite, t0, t1, particle.mTerm );
		system.PushSprite ( sprite );
	}

	if ( particle.mAge >= particle.mTerm ) {
		
		if ( this->mNext ) {
			this->mNext->InitParticle ( system, particle );
		}
		else {
			particle.mState = 0;
		}
	}
}
