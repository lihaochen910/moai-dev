// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPARTICLEDISTANCEEMITTER_H
#define	MOAIPARTICLEDISTANCEEMITTER_H

#include <moai-sim/MOAIParticleEmitter.h>

//================================================================//
// MOAIParticleDistanceEmitter
//================================================================//
/**	@lua	MOAIParticleDistanceEmitter
	@text	Particle emitter.
*/
class MOAIParticleDistanceEmitter :
	public MOAIParticleEmitter {
private:
	
	bool		mReset;
	
	ZLVec3D		mEmitLoc;			// location of the last emission
	
	float		mMinDistance;
	float		mMaxDistance;
	
	float		mEmitDistance;		// distance at which next emission will occur

	//----------------------------------------------------------------//
	static mrb_value		_reset					( mrb_state* M, mrb_value context );
	static mrb_value		_setDistance			( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	float			GetRandomDistance		();
	
	//----------------------------------------------------------------//
	void			MOAIAction_Update		( double step );
	void			MOAINode_Update			();

public:
	
	DECL_RUBY_FACTORY ( MOAIParticleDistanceEmitter, MOAIParticleEmitter )

	//----------------------------------------------------------------//
					MOAIParticleDistanceEmitter		();
					~MOAIParticleDistanceEmitter	();
	void			RegisterRubyClass				( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs				( MOAIRubyState& state, RClass* klass );
	void			SetDistanceRange				( float min, float max );
};

#endif
