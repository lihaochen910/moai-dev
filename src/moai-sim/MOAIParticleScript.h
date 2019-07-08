// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPARTICLESCRIPT_H
#define	MOAIPARTICLESCRIPT_H

class MOAIParticle;
class MOAIParticleState;
class MOAIParticleSystem;

//================================================================//
// MOAIParticleScript
//================================================================//
/**	@lua	MOAIParticleScript
	@text	Particle script. A particle script contains a series of operations, which
	can perform simple computations on values. Values can be hard-coded using packConst
	to create constant values, or stored in registers. There is a set of innate registers,
	accessed through the packReg() function, and a second set of "live" registers which
	allow setting values from external code using the setReg() function.

	@const	PARTICLE_X
	@const	PARTICLE_Y
	@const	PARTICLE_DX
	@const	PARTICLE_DY

	@const	SPRITE_X_LOC
	@const	SPRITE_Y_LOC
	@const	SPRITE_ROT
	@const	SPRITE_X_SCL
	@const	SPRITE_Y_SCL
	@const	SPRITE_RED
	@const	SPRITE_GREEN
	@const	SPRITE_BLUE
	@const	SPRITE_OPACITY
	@const	SPRITE_GLOW
	@const	SPRITE_IDX

*/
class MOAIParticleScript :
	public virtual MOAIRubyObject {
private:
	
	friend class MOAIParticleState;
	
	static const u32 MAX_PARTICLE_REGISTERS = 256;
	static const u32 PARTICLE_REGISTER_MASK = 0x000000ff;
	static const u32 LIVE_REG_COUNT = 16;

	// "temporary" registers
	enum {
		SPRITE_X_LOC,
		SPRITE_Y_LOC,
		SPRITE_ROT,
		SPRITE_X_SCL,
		SPRITE_Y_SCL,
		SPRITE_RED,
		SPRITE_GREEN,
		SPRITE_BLUE,
		SPRITE_OPACITY,
		SPRITE_GLOW,
		SPRITE_IDX,
		
		// not used by sprite drawing, but not stored in particle registers.
		// these get initialized when the script runs. conceptually they are
		// particle registers.
		PARTICLE_AGE,
		PARTICLE_TIME,
		
		TOTAL_SPRITE_REG,
	};
	
	enum {
		END = 0,
		ABS,
		ADD,
		ANGLE_VEC,
		COLOR,
		COS,
		CYCLE,
		DIV,
		EASE,
		EASE_DELTA,
		MUL,
		NORM,
		OSCILLATE,
		RAND,
		RAND_INT,
		RAND_VEC,
		SET,
		SIN,
		SPRITE,
		STEP,
		SUB,
		TAN,
		TIME,
		VEC_ANGLE,
		WRAP,
	};
	
	//----------------------------------------------------------------//
	class Instruction {
	public:
		
		static const u32 MAX_PARAMS = 8;
		
		u32		mOpcode;
		u32		mParams [ MAX_PARAMS ];
		u8		mTypes [ MAX_PARAMS ];
		cc8*	mFormat;
		u32		mSize;
		
		//----------------------------------------------------------------//
		u32		GetSize			();
		void	Init			( u32 opcode, cc8* format );
				Instruction		();
		void	Parse			( MOAIRubyState& state, u32 idx );
		u8*		Write			( u8* cursor );
	};

	typedef STLList < Instruction >::iterator InstructionIt;
	STLList < Instruction > mInstructions;
	
	ZLLeanArray < u8 > mBytecode;

	bool	mCompiled;
	float	mLiveRegisters [ LIVE_REG_COUNT ]; // TODO: OK to let user reserve these?

	//----------------------------------------------------------------//
	static mrb_value		_abs				( mrb_state* M, mrb_value context );
	static mrb_value		_add				( mrb_state* M, mrb_value context );
	static mrb_value		_angleVec			( mrb_state* M, mrb_value context );
	static mrb_value		_color				( mrb_state* M, mrb_value context );
	static mrb_value		_cos				( mrb_state* M, mrb_value context );
	static mrb_value		_cycle				( mrb_state* M, mrb_value context );
	static mrb_value		_div				( mrb_state* M, mrb_value context );
	static mrb_value		_ease				( mrb_state* M, mrb_value context );
	static mrb_value		_easeDelta			( mrb_state* M, mrb_value context );
	static mrb_value		_mul				( mrb_state* M, mrb_value context );
	static mrb_value		_norm				( mrb_state* M, mrb_value context );
	static mrb_value		_oscillate			( mrb_state* M, mrb_value context );
	static mrb_value		_packConst			( mrb_state* M, mrb_value context );
	static mrb_value		_packLiveReg		( mrb_state* M, mrb_value context );
	static mrb_value		_packReg			( mrb_state* M, mrb_value context );
	static mrb_value		_rand				( mrb_state* M, mrb_value context );
	static mrb_value		_randInt			( mrb_state* M, mrb_value context );
	static mrb_value		_randVec			( mrb_state* M, mrb_value context );
	static mrb_value		_set				( mrb_state* M, mrb_value context );
	static mrb_value		_setLiveReg			( mrb_state* M, mrb_value context );
	static mrb_value		_sin				( mrb_state* M, mrb_value context );
	static mrb_value		_sprite				( mrb_state* M, mrb_value context );
	static mrb_value		_step				( mrb_state* M, mrb_value context );
	static mrb_value		_sub				( mrb_state* M, mrb_value context );
	static mrb_value		_tan				( mrb_state* M, mrb_value context );
	static mrb_value		_time				( mrb_state* M, mrb_value context );
	static mrb_value		_vecAngle			( mrb_state* M, mrb_value context );
	static mrb_value		_wrap				( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	static u64		Pack64					( u32 low, u32 hi );
	Instruction&	PushInstruction			( u32 op, cc8* format );
	void			PushSprite				( MOAIParticleSystem& system, float* registers );
	void			ResetRegisters			( float* spriteRegisters, float* particleRegisters, const MOAIParticleSystem& );

public:
	
	DECL_RUBY_FACTORY ( MOAIParticleScript, MOAIRubyObject )
	
	enum {
		PARAM_TYPE_FLAG				= 0x00,
		PARAM_TYPE_CONST			= 0x01,
		PARAM_TYPE_PARTICLE_REG		= 0x02,
		PARAM_TYPE_SPRITE_REG		= 0x04,
		PARAM_TYPE_LIVE_REG			= 0x08,
		
		PARAM_TYPE_REG_MASK			= 0x0E,
		PARAM_TYPE_MASK				= 0x0F,
	};
	
	//----------------------------------------------------------------//
	u8*				Compile					();
					MOAIParticleScript		();
					~MOAIParticleScript		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			Run						( MOAIParticleSystem& system, MOAIParticle& particle, float t0, float t1 );
};

#endif
