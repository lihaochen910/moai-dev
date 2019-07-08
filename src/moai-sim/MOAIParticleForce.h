// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPARTICLEFORCE_H
#define	MOAIPARTICLEFORCE_H

#include <moai-sim/MOAITransform.h>

class MOAIParticle;
class MOAIParticleSystem;

//================================================================//
// MOAIParticleForce
//================================================================//
/**	@lua	MOAIParticleForce
	@text	Particle force.

	@const	FORCE
	@const	GRAVITY
	@const	OFFSET
*/
class MOAIParticleForce :
	public MOAITransform {
private:

	u32			mShape;
	u32			mType;
	ZLVec3D		mVec;

	ZLVec3D		mWorldLoc;	
	ZLVec3D		mWorldVec;

	// attractor coefficients
	float		mRadius;
	float		mPull;

	//----------------------------------------------------------------//
	static mrb_value		_initAttractor			( mrb_state* M, mrb_value context );
	static mrb_value		_initBasin				( mrb_state* M, mrb_value context );
	static mrb_value		_initLinear				( mrb_state* M, mrb_value context );
	static mrb_value		_initRadial				( mrb_state* M, mrb_value context );
	static mrb_value		_setType				( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	void			MOAINode_Update			();

public:
	
	enum {
		ATTRACTOR,
		BASIN,
		LINEAR,
		RADIAL,
	};
	
	enum {
		FORCE,
		GRAVITY,
		OFFSET,
	};
	
	DECL_RUBY_FACTORY ( MOAIParticleForce, MOAITransform )

	//----------------------------------------------------------------//
	void			Eval					( const ZLVec3D& loc, float mass, ZLVec3D& acceleration, ZLVec3D& offset );
					MOAIParticleForce		();
					~MOAIParticleForce		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
