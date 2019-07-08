// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <float.h>
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIParticleState.h>
#include <moai-sim/MOAIParticleSystem.h>
#include <moai-sim/MOAITextureBase.h>

class MOAIDataBuffer;

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	capParticles
	@text	Controls capping vs. wrapping of particles in overflow situation.
			Capping will prevent emission of additional particles when system
			is full. Wrapping will overwrite the oldest particles with new particles.
	
	@in		MOAIParticleSystem self
	@opt	boolean cap					Default value is true.
	@out	nil
*/
mrb_value MOAIParticleSystem::_capParticles ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleSystem, "U" )

	self->mCapParticles = state.GetParamValue < bool >( 1, true );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	capSprites
	@text	Controls capping vs. wrapping of sprites.
	
	@in		MOAIParticleSystem self
	@opt	boolean cap					Default value is true.
	@out	nil
*/
mrb_value MOAIParticleSystem::_capSprites ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleSystem, "U" )

	self->mCapSprites = state.GetParamValue < bool >( 1, true );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	clearSprites
	@text	Flushes any existing sprites in system.
	
	@in		MOAIParticleSystem self
	@out	nil
*/
mrb_value MOAIParticleSystem::_clearSprites ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleSystem, "U" )

	self->mSpriteTop = 0;
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getState
	@text	Returns a particle state for an index or nil if none exists.
	
	@in		MOAIParticleSystem self
	@in		number index
	@out	MOAIParticleState state
*/
mrb_value MOAIParticleSystem::_getState ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleSystem, "UN" )

	u32 idx = state.GetParamValue < u32 >( 1, 1 ) - 1;
	
	MOAIParticleState* particleState = self->GetState ( idx );
	if ( particleState ) {
		return particleState->PushRubyUserdata ( state );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/** @lua	isIdle
	@text	Returns true if the current system is not currently
			processing any particles.
			
	@in		MOAIParticleSystem self
	@out	boolean isIdle				Indicates whether the system is currently idle.
*/
mrb_value MOAIParticleSystem::_isIdle( mrb_state* M, mrb_value context ) {
	
	MOAI_RUBY_SETUP ( MOAIParticleSystem, "U" )

	bool result = !self->mHead;

	return state.ToRValue ( result );
}	


//----------------------------------------------------------------//
/**	@lua	pushParticle
	@text	Adds a particle to the system.
	
	@in		MOAIParticleSystem self
	@opt	number x					Default value is 0.
	@opt	number y					Default value is 0.
	@opt	number dx					Default value is 0.
	@opt	number dy					Default value is 0.
	@opt	number state				Index of initial particle state.
	@out	boolean result				true if particle was added, false if not.
*/
mrb_value MOAIParticleSystem::_pushParticle ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleSystem, "U" )

	float x = state.GetParamValue < float >( 1, 0.0f );
	float y = state.GetParamValue < float >( 2, 0.0f );
	
	float dx = state.GetParamValue < float >( 3, 0.0f );
	float dy = state.GetParamValue < float >( 4, 0.0f );

	u32 stateIdx = state.GetParamValue < u32 >( 5, 1 ) - 1;

	bool result = self->PushParticle ( x, y, dx, dy, stateIdx );
	return state.ToRValue ( result );
}

//----------------------------------------------------------------//
/**	@lua	pushSprite
	@text	Adds a sprite to the system. Sprite will persist until
			particle simulation is begun or 'clearSprites' is called.
	
	@in		MOAIParticleSystem self
	@in		number x
	@in		number y
	@opt	number rot			Rotation in degrees. Default value is 0.
	@opt	number xScale		Default value is 1.
	@opt	number yScale		Default value is 1.
	@out	boolean result		true is sprite was added, false if not.
*/
mrb_value MOAIParticleSystem::_pushSprite ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleSystem, "UNN" )

	AKUParticleSprite sprite;

	sprite.mXLoc		= state.GetParamValue < float >( 1, 0.0f );
	sprite.mYLoc		= state.GetParamValue < float >( 2, 0.0f );
	sprite.mZRot		= state.GetParamValue < float >( 3, 0.0f );
	sprite.mXScl		= state.GetParamValue < float >( 4, 1.0f );
	sprite.mYScl		= state.GetParamValue < float >( 5, 1.0f );
	
	sprite.mRed			= 1.0f;
	sprite.mGreen		= 1.0f;
	sprite.mBlue		= 1.0f;
	sprite.mAlpha		= 1.0f;
	
	sprite.mGfxID = 1;
	
	bool result = self->PushSprite ( sprite );
	
	if ( result ) {
		self->ScheduleUpdate ();
	}
	return state.ToRValue ( result );
}

//----------------------------------------------------------------//
/**	@lua	reserveParticles
	@text	Reserve particle capacity of system.
	
	@in		MOAIParticleSystem self
	@in		number nParticles		Total number of particle records.
	@in		number particleSize		Number of parameters reserved for the particle.
	@out	nil
*/
mrb_value MOAIParticleSystem::_reserveParticles ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleSystem, "UN" )

	u32 total	= state.GetParamValue < u32 >( 1, 0 );
	u32 size	= state.GetParamValue < u32 >( 2, 0 );

	self->ReserveParticles ( total, size );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	reserveSprites
	@text	Reserve sprite capacity of system.
	
	@in		MOAIParticleSystem self
	@in		number nSprites
	@out	nil
*/
mrb_value MOAIParticleSystem::_reserveSprites ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleSystem, "UN" )

	self->ReserveSprites ( state.GetParamValue < u32 >( 1, 0 ));
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	reserveStates
	@text	Reserve total number of states for system.
	
	@in		MOAIParticleSystem self
	@in		number nStates
	@out	nil
*/
mrb_value MOAIParticleSystem::_reserveStates ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleSystem, "UN" )

	self->ClearStates ();
	self->ReserveStates ( state.GetParamValue < u32 >( 1, 0 ));
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setDrawOrder
	@text	Set draw order of sprites in particle system
 
	@in		MOAIParticleSystem self
	@in     number  order		MOAIParticleSystem.ORDER_NORMAL or MOAIParticleSystem.ORDER_REVERSE
	@out	nil
*/
mrb_value MOAIParticleSystem::_setDrawOrder ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleSystem, "U" )

	self->mDrawOrder = state.GetParamValue < u32 >( 1, ORDER_NORMAL );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setComputeBounds
	@text	Set the a flag controlling whether the particle system
			re-computes its bounds every frame.
	
	@in		MOAIParticleSystem self
	@opt	boolean computBounds		Default value is false.
	@out	nil
*/
mrb_value MOAIParticleSystem::_setComputeBounds ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleSystem, "U" )

	self->mComputeBounds = state.GetParamValue < bool >( 1, false );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setSpriteColor
	@text	Set the color of the most recently added sprite.
	
	@in		MOAIParticleSystem self
	@in		number r
	@in		number g
	@in		number b
	@in		number a
	@out	nil
*/
mrb_value MOAIParticleSystem::_setSpriteColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleSystem, "UNNNN" )

	AKUParticleSprite* sprite = self->GetTopSprite ();
	if ( sprite ) {
	
		sprite->mRed	= state.GetParamValue < float >( 1, 1.0f );
		sprite->mGreen	= state.GetParamValue < float >( 2, 1.0f );
		sprite->mBlue	= state.GetParamValue < float >( 3, 1.0f );
		sprite->mAlpha	= state.GetParamValue < float >( 4, 1.0f );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setSpriteDeckIdx
	@text	Set the sprite's deck index.
	
	@in		MOAIParticleSystem self
	@in		number index
	@out	nil
*/
mrb_value MOAIParticleSystem::_setSpriteDeckIdx ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleSystem, "UN" )

	AKUParticleSprite* sprite = self->GetTopSprite ();
	if ( sprite ) {
		sprite->mGfxID = state.GetParamValue < u32 >( 1, sprite->mGfxID );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setState
	@text	Set a particle state.
	
	@in		MOAIParticleSystem self
	@in		number index
	@in		MOAIParticleState state
	@out	nil
*/
mrb_value MOAIParticleSystem::_setState ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleSystem, "UNU" )

	u32 idx = state.GetParamValue < u32 >( 1, 0 ) - 1;

	if ( idx < self->mStates.Size ()) {
	
		MOAIParticleState* particleState =  state.GetRubyObject < MOAIParticleState >( 2, true );
		if ( particleState != self->mStates [ idx ]) {
		
			self->RubyRetain ( particleState );
			self->RubyRelease ( self->mStates [ idx ]);
			self->mStates [ idx ] = particleState;
		}
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	surge
	@text	Release a batch emission or particles into the system.
	
	@in		MOAIParticleSystem self
	@opt	number total			Default value is 1.
	@opt	number x				Default value is 0.
	@opt	number y				Default value is 0.
	@opt	number dx				Default value is 0.
	@opt	number dy				Default value is 0.
	@out	nil
*/
mrb_value MOAIParticleSystem::_surge ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleSystem, "UN" )

	u32 n		= state.GetParamValue < u32 >( 1, 1 );
	float x		= state.GetParamValue < float >( 2, 0.0f );
	float y		= state.GetParamValue < float >( 3, 0.0f );
	float dx	= state.GetParamValue < float >( 4, 0.0f );
	float dy	= state.GetParamValue < float >( 5, 0.0f );
	
	for ( u32 i = 0; i < n; ++i ) {
		self->PushParticle ( x, y, dx, dy );
	}

	return mrb_nil_value ();
}

//================================================================//
// MOAIParticleSystem
//================================================================//

//----------------------------------------------------------------//
void MOAIParticleSystem::ClearStates () {

	for ( u32 i = 0; i < this->mStates.Size (); ++i ) {
		this->RubyRelease ( this->mStates [ i ]);
	}
	this->mStates.Clear ();
}

//----------------------------------------------------------------//
void MOAIParticleSystem::ClearQueue () {

	this->mHead = 0;
	this->mTail = 0;
}

//----------------------------------------------------------------//
void MOAIParticleSystem::EnqueueParticle ( MOAIParticle& particle ) {

	if ( this->mTail ) {
		this->mTail->mNext = &particle;
	}
	else {
		this->mHead = &particle;
	}
	
	particle.mNext = 0;
	this->mTail = &particle;
}

//----------------------------------------------------------------//
MOAIParticleState* MOAIParticleSystem::GetState ( u32 id ) {

	if ( id < this->mStates.Size ()) {
		return this->mStates [ id ];
	}
	return 0;
}

//----------------------------------------------------------------//
AKUParticleSprite* MOAIParticleSystem::GetTopSprite () {

	if ( this->mSpriteTop ) {
		u32 idx = (this->mSpriteTop - 1) % this->mSprites.Size();
		return &this->mSprites [ idx ];
	}
	return 0;
}

//----------------------------------------------------------------//
MOAIParticleSystem::MOAIParticleSystem () :
	mParticleSize ( 0 ),
	mCapParticles ( false ),
	mCapSprites ( false ),
	mHead ( 0 ),
	mTail ( 0 ),
	mFree ( 0 ),
	mSpriteTop ( 0 ),
	mDrawOrder ( ORDER_NORMAL ),
	mComputeBounds ( false ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIGraphicsProp )
		RTTI_EXTEND ( MOAIAction )
	RTTI_END
	
	// prop's index is *added* to particle's index;
	// should be initialized to 0 instead of 1
	this->mIndex = 0;
}

//----------------------------------------------------------------//
MOAIParticleSystem::~MOAIParticleSystem () {

	this->ClearStates ();
}

//----------------------------------------------------------------//
bool MOAIParticleSystem::PushParticle ( float x, float y ) {
	
	return this->PushParticle ( x, y, 0.0f, 0.0f, 0 );
}

//----------------------------------------------------------------//
bool MOAIParticleSystem::PushParticle ( float x, float y, float dx, float dy ) {
	
	return this->PushParticle ( x, y, dx, dy, 0 );
}

//----------------------------------------------------------------//
bool MOAIParticleSystem::PushParticle ( float x, float y, float dx, float dy, u32 stateIdx ) {
	
	if (( !this->mFree ) && this->mCapParticles ) {
		return false;
	}
	
	MOAIParticleState* state = this->GetState ( stateIdx );
	if ( !state ) return false;
	
	MOAIParticle* particle = 0;
	
	if ( this->mFree ) {
		particle = this->mFree;
		this->mFree = particle->mNext;
	}
	else if ( this->mHead ) {
		particle = this->mHead;
		this->mHead = particle->mNext;
	}
	
	if ( particle ) {
		
		float* r = particle->mData;
		
		r [ MOAIParticle::PARTICLE_X ] = x;
		r [ MOAIParticle::PARTICLE_Y ] = y;
		r [ MOAIParticle::PARTICLE_DX ] = dx;
		r [ MOAIParticle::PARTICLE_DY ] = dy;
		
		for ( u32 i = MOAIParticle::TOTAL_PARTICLE_REG; i < this->mParticleSize; ++i ) {
			r [ i ] = 0.0f;
		}
		
		state->InitParticle ( *this, *particle );
		this->EnqueueParticle ( *particle );
		
		return true;
	}
	return false;
}

//----------------------------------------------------------------//
bool MOAIParticleSystem::PushSprite ( const AKUParticleSprite& sprite ) {

	u32 size = ( u32 )this->mSprites.Size ();
	
	if ( size && this->mDeck ) {
	
		if (( this->mSpriteTop >= size ) && this->mCapSprites ) {
			return false;
		}
		
		u32 idx = ( this->mSpriteTop++ ) % size;
		this->mSprites [ idx ] = sprite;
		
		// TODO: need to take rotation into account
		ZLBox bounds = this->mDeck->GetBounds ( sprite.mGfxID );
		
		ZLVec3D offset ( sprite.mXLoc, sprite.mYLoc, 0.0f );
		ZLVec3D scale ( sprite.mXScl, sprite.mYScl, 0.0f );
		
		bounds.Scale ( scale );
		
		float radius = bounds.GetMaxExtent () * 1.4f; // handles case when bounds are rotated

		bounds.mMin.Init ( -radius, -radius, 0.0f );
		bounds.mMax.Init ( radius, radius, 0.0f );
		
		bounds.Offset ( offset );
		
		if ( this->mSpriteTop == 1 ) {
			this->mParticleBounds = bounds;
		}
		else {
			this->mParticleBounds.Grow ( bounds );
		}
		return true;
	}
	return false;
}

//----------------------------------------------------------------//
void MOAIParticleSystem::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIGraphicsProp::RegisterRubyClass ( state, klass );
	MOAIAction::RegisterRubyClass ( state, klass );

	state.DefineClassConst ( klass, "ORDER_NORMAL",	( u32 )ORDER_NORMAL );
	state.DefineClassConst ( klass, "ORDER_REVERSE",	( u32 )ORDER_REVERSE );
}

//----------------------------------------------------------------//
void MOAIParticleSystem::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIGraphicsProp::RegisterRubyFuncs ( state, klass );
	MOAIAction::RegisterRubyFuncs ( state, klass );
	
	state.DefineInstanceMethod ( klass, "capParticles",		_capParticles, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "capSprites",		_capSprites, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "clearSprites",		_clearSprites, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isIdle",			_isIdle, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getState",			_getState, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "pushParticle",		_pushParticle, MRB_ARGS_ARG ( 0, 5 ) );
	state.DefineInstanceMethod ( klass, "pushSprite",		_pushSprite, MRB_ARGS_ARG ( 2, 3 ) );
	state.DefineInstanceMethod ( klass, "reserveParticles",	_reserveParticles, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "reserveSprites",	_reserveSprites, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "reserveStates",	_reserveStates, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setDrawOrder",		_setDrawOrder, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setComputeBounds",	_setComputeBounds, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setSpriteColor",	_setSpriteColor, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "setSpriteDeckIdx",	_setSpriteDeckIdx, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setState",			_setState, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "surge",			_surge, MRB_ARGS_ARG ( 0, 5 ) );

}

//----------------------------------------------------------------//
void MOAIParticleSystem::ReserveParticles ( u32 maxParticles, u32 particleSize ) {
	
	particleSize += MOAIParticle::TOTAL_PARTICLE_REG;
	
	this->mHead = 0;
	this->mTail = 0;
	this->mFree = 0;
	
	this->mParticleSize = particleSize;
	
	this->mParticles.Init ( maxParticles );
	this->mParticleData.Init ( maxParticles * particleSize );
	this->mParticleData.Fill ( 0.0f );
	
	for ( u32 i = 0; i < maxParticles; ++i ) {
		MOAIParticle& particle = this->mParticles [ i ];
		particle.mNext = this->mFree;
		this->mFree = &particle;
		
		if ( particleSize ) {
			particle.mData = &this->mParticleData [ i * particleSize ];
		}
		else {
			particle.mData = 0;
		}
	}
}

//----------------------------------------------------------------//
void MOAIParticleSystem::ReserveSprites ( u32 maxSprites ) {

	this->mSprites.Init ( maxSprites );
}

//----------------------------------------------------------------//
void MOAIParticleSystem::ReserveStates ( u32 total ) {

	this->mStates.Init ( total );
	this->mStates.Fill ( 0 );
}

//----------------------------------------------------------------//
void MOAIParticleSystem::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {

	MOAIGraphicsProp::SerializeIn ( state, serializer );
	MOAIAction::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIParticleSystem::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {

	MOAIGraphicsProp::SerializeOut ( state, serializer );
	MOAIAction::SerializeOut ( state, serializer );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAIParticleSystem::MOAIAction_IsDone () {

	return false;
}

//----------------------------------------------------------------//
void MOAIParticleSystem::MOAIAction_Update ( double step ) {

	bool schedule = ( this->mSpriteTop > 0 );

	// clear out the sprites
	this->mSpriteTop = 0;

	this->mParticleBounds.Init ( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );

	// if particles
	if ( this->mHead ) {

		// grab the head then clear out the queue
		MOAIParticle* cursor = this->mHead;
		this->ClearQueue ();
		
		// update the particles and rebuild the queue
		while ( cursor ) {
			MOAIParticle* particle = cursor;
			cursor = cursor->mNext;
			
			// update the particle
			if ( particle->mState ) {
				particle->mState->ProcessParticle ( *this, *particle,( float )step );
			}
			
			// if is still to be killed, move it to the free list, else put it back in the queue
			if ( !particle->mState ) {
				particle->mNext = this->mFree;
				this->mFree = particle;
			}
			else {
				// and put it back in the queue
				this->EnqueueParticle ( *particle );
			}
		}
	}
	
	if ( schedule || this->mSpriteTop ) {
		this->ScheduleUpdate ();
	}
}

//----------------------------------------------------------------//
void MOAIParticleSystem::MOAIDrawable_Draw ( int subPrimID ) {
	UNUSED ( subPrimID );

	if ( !this->IsVisible ()) return;
	if ( !this->mDeck ) return;
	if ( this->IsClear ()) return;

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	this->PushGfxState ();
	this->LoadUVTransform ();

	ZLAffine3D drawingMtx;
	ZLAffine3D spriteMtx;
	
	u32 maxSprites = ( u32 )this->mSprites.Size ();
	u32 total = this->mSpriteTop;
	u32 base = 0;
	if ( total > maxSprites ) {
		base = total % maxSprites;
		total = maxSprites;
	}
	
	for ( u32 i = 0; i < total; ++i ) {

		u32 idx;
		if ( this->mDrawOrder == ORDER_NORMAL ) {
			idx = ( base + i ) % maxSprites;
		}
		else {
			idx = ( base + ( total - 1 - i )) % maxSprites;
		}
				
		AKUParticleSprite& sprite = this->mSprites [ idx ];
		gfxState.SetPenColor ( sprite.mRed, sprite.mGreen, sprite.mBlue, sprite.mAlpha );
		
		spriteMtx.ScRoTr ( sprite.mXScl, sprite.mYScl, 1.0f, 0.0f, 0.0f, sprite.mZRot * ( float )D2R, sprite.mXLoc, sprite.mYLoc, 0.0f );
		
		drawingMtx = this->GetLocalToWorldMtx ();
		drawingMtx.Prepend ( spriteMtx );
		
		gfxState.SetMtx ( MOAIGfxState::MODEL_TO_WORLD_MTX, drawingMtx );
		
		//this->mDeck->Draw ( this->mIndex + ( u32 )sprite.mGfxID, this->mMaterialBatch );
		this->mDeck->Draw ( this->mIndex + ( u32 )sprite.mGfxID );
	}
	
	this->PopGfxState ();
}

//----------------------------------------------------------------//
ZLBounds MOAIParticleSystem::MOAIPartitionHull_GetModelBounds () {

	if ( this->mSpriteTop ) {
		ZLBounds bounds;
		bounds.Init ( this->mParticleBounds );
		return bounds;
	}
	return ZLBounds::EMPTY;
}
