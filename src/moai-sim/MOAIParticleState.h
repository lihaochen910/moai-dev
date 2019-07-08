// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPARTICLESTATE_H
#define	MOAIPARTICLESTATE_H

#include <moai-sim/MOAIParticle.h>

class MOAIParticleForce;
class MOAIParticlePlugin;
class MOAIParticleScript;
class MOAIParticleSystem;

//================================================================//
// MOAIParticleState
//================================================================//
/**	@lua	MOAIParticleState
	@text	Particle state.
*/
class MOAIParticleState :
	public virtual MOAIRubyObject {
private:
	friend class MOAIParticleScript;
	friend class MOAIParticleSystem;

	typedef ZLLeanLink < MOAIParticleForce* > ForceNode;
	ZLLeanList < MOAIParticleForce* > mForces;

	float mMassRange [ 2 ];
	float mTermRange [ 2 ];

	float mDamping;

	MOAIRubySharedPtr < MOAIParticleScript >		mInit;
	MOAIRubySharedPtr < MOAIParticleScript >		mRender;
	MOAIRubySharedPtr < MOAIParticlePlugin >		mPlugin;
	MOAIRubySharedPtr < MOAIParticleState >			mNext;

	//----------------------------------------------------------------//
	static mrb_value		_clearForces			( mrb_state* M, mrb_value context );
	static mrb_value		_pushForce				( mrb_state* M, mrb_value context );
	static mrb_value		_setDamping				( mrb_state* M, mrb_value context );
	static mrb_value		_setInitScript			( mrb_state* M, mrb_value context );
	static mrb_value		_setMass				( mrb_state* M, mrb_value context );
	static mrb_value		_setNext				( mrb_state* M, mrb_value context );
	static mrb_value		_setPlugin				( mrb_state* M, mrb_value context );
	static mrb_value		_setRenderScript		( mrb_state* M, mrb_value context );
	static mrb_value		_setTerm				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void			GatherForces			( ZLVec3D& loc, ZLVec3D& velocity, float mass, float step );
	void			InitParticle			( MOAIParticleSystem& system, MOAIParticle& particle );
	void			ProcessParticle			( MOAIParticleSystem& system, MOAIParticle& particle, float step );

public:

	DECL_RUBY_FACTORY ( MOAIParticleState, MOAIRubyObject )

	//----------------------------------------------------------------//
	void			ClearForces				();
					MOAIParticleState		();
					~MOAIParticleState		();
	void			PushForce				( MOAIParticleForce& force );
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
