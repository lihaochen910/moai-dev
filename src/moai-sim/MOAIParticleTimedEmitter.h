// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPARTICLETIMEDEMITTER_H
#define	MOAIPARTICLETIMEDEMITTER_H

#include <moai-sim/MOAIParticleEmitter.h>

//================================================================//
// MOAIParticleTimedEmitter
//================================================================//
/**	@lua	MOAIParticleTimedEmitter
	@text	Particle emitter.
*/
class MOAIParticleTimedEmitter :
	public MOAIParticleEmitter {
private:
	
	float	mTime;
	float	mEmitTime;
	
	float	mMinFrequency;
	float	mMaxFrequency;

	//----------------------------------------------------------------//
	static mrb_value		_setFrequency			( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	float			GetRandomFrequency		();
	
	//----------------------------------------------------------------//
	void			MOAIAction_Update		( double step );

public:
	
	DECL_RUBY_FACTORY ( MOAIParticleTimedEmitter, MOAIParticleEmitter )


	//----------------------------------------------------------------//
					MOAIParticleTimedEmitter	();
					~MOAIParticleTimedEmitter	();
	void			RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void			SetFrequencyRange			( float min, float max );
};

#endif
