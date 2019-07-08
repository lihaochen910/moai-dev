// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPARTICLEPLUGIN_H
#define	MOAIPARTICLEPLUGIN_H

#include <moai-sim/host_particles.h>

//================================================================//
// MOAIParticlePlugin
//================================================================//
/**	@lua	MOAIParticlePlugin
	@text	Allows custom particle processing.
*/
class MOAIParticlePlugin :
	public virtual MOAIRubyObject {
protected:

	int				mSize;

	//----------------------------------------------------------------//
	static mrb_value		_getSize			( mrb_state* M, mrb_value context );

public:
	
	//----------------------------------------------------------------//
					MOAIParticlePlugin			();
					~MOAIParticlePlugin			();	
	virtual void	OnInit						( float* particle, float* registers ) = 0;
	virtual void	OnRender					( float* particle, float* registers, AKUParticleSprite* sprite, float t0, float t1, float term ) = 0;
	void			RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
};

#endif
