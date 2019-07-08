// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPARTICLESYSTEM_H
#define	MOAIPARTICLESYSTEM_H

#include <moai-sim/host_particles.h>
#include <moai-sim/MOAIAction.h>
#include <moai-sim/MOAIGraphicsProp.h>
#include <moai-sim/MOAIParticle.h>

class MOAIDeck;
class MOAIParticleScript;
class MOAIParticleState;

//================================================================//
// MOAIParticleSystem
//================================================================//
/**	@lua	MOAIParticleSystem
	@text	Particle system.
*/
class MOAIParticleSystem :
	public MOAIGraphicsProp,
	public MOAIAction {
private:

	ZLLeanArray < MOAIParticleState* >	mStates;
	ZLLeanArray < MOAIParticle >		mParticles;
	ZLLeanArray < float >				mParticleData;
	u32									mParticleSize;
	
	bool								mCapParticles;
	bool								mCapSprites;
	
	MOAIParticle*						mHead;
	MOAIParticle*						mTail;
	MOAIParticle*						mFree;
	
	ZLLeanArray < AKUParticleSprite >	mSprites;
	u32									mSpriteTop;
	u32									mDrawOrder;
	
	bool								mComputeBounds;
	ZLBox								mParticleBounds;
	
	//----------------------------------------------------------------//
	static mrb_value		_capParticles			( mrb_state* M, mrb_value context );
	static mrb_value		_capSprites				( mrb_state* M, mrb_value context );
	static mrb_value		_clearSprites			( mrb_state* M, mrb_value context );
	static mrb_value		_isIdle					( mrb_state* M, mrb_value context );
	static mrb_value		_getState				( mrb_state* M, mrb_value context );
	static mrb_value		_pushParticle			( mrb_state* M, mrb_value context );
	static mrb_value		_pushSprite				( mrb_state* M, mrb_value context );
	static mrb_value		_reserveParticles		( mrb_state* M, mrb_value context );
	static mrb_value		_reserveRects			( mrb_state* M, mrb_value context );
	static mrb_value		_reserveSprites			( mrb_state* M, mrb_value context );
	static mrb_value		_reserveStates			( mrb_state* M, mrb_value context );
	static mrb_value		_setComputeBounds		( mrb_state* M, mrb_value context );
	static mrb_value		_setDrawOrder			( mrb_state* M, mrb_value context );
	static mrb_value		_setSpriteColor			( mrb_state* M, mrb_value context );
	static mrb_value		_setSpriteDeckIdx		( mrb_state* M, mrb_value context );
	static mrb_value		_setState				( mrb_state* M, mrb_value context );
	static mrb_value		_surge					( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	void					ClearStates				();
	void					ClearQueue				();
	void					EnqueueParticle			( MOAIParticle& particle );
	AKUParticleSprite*		GetTopSprite			();
	MOAIParticleState*		GetState				( u32 id );
	
	//----------------------------------------------------------------//
	bool					MOAIAction_IsDone						();
	void					MOAIAction_Update						( double step );
	void					MOAIDrawable_Draw						( int subPrimID );
	ZLBounds				MOAIPartitionHull_GetModelBounds		();

public:

	enum {
		ORDER_NORMAL,
		ORDER_REVERSE,
	};

	friend class MOAIParticleEngine;
	friend class MOAIParticleScript;
	friend class MOAIParticleState;
	
	DECL_RUBY_FACTORY ( MOAIParticleSystem, MOAIAction )

	//----------------------------------------------------------------//
					MOAIParticleSystem		();
					~MOAIParticleSystem		();
	bool			PushParticle			( float x, float y );
	bool			PushParticle			( float x, float y, float dx, float dy );
	bool			PushParticle			( float x, float y, float dx, float dy, u32 stateIdx );
	bool			PushSprite				( const AKUParticleSprite& sprite );
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			ReserveParticles		( u32 maxParticles, u32 particleSize );
	void			ReserveRects			( u32 total );
	void			ReserveSprites			( u32 maxSprites );
	void			ReserveStates			( u32 total );
	void			SerializeIn				( MOAIRubyState& state, MOAIDeserializer& serializer );
	void			SerializeOut			( MOAIRubyState& state, MOAISerializer& serializer );
	void			SetConstant				( u32 idx, float value );
	void			SetRect					( u32 idx, ZLRect& rect );
};

#endif
