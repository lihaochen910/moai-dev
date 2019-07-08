// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIParticle.h>
#include <moai-sim/MOAIParticleState.h>
#include <moai-sim/MOAIParticleScript.h>
#include <moai-sim/MOAIParticleSystem.h>

#define IMPL_RUBY_PARTICLE_OP(opcode,format)								\
	MOAI_RUBY_SETUP ( MOAIParticleScript, "U" )								\
	Instruction& instruction = self->PushInstruction ( opcode, format );	\
	instruction.Parse ( state, 1 );											\
	return mrb_nil_value ();

#define READ_ADDR(reg,bytecode)									\
	type = *( bytecode++ );										\
	regIdx = *( bytecode++ );									\
	reg = 0;													\
																\
	if ( type & PARAM_TYPE_REG_MASK ) {							\
		if ( type == PARAM_TYPE_SPRITE_REG ) {					\
			reg = &spriteRegisters [ regIdx ];					\
		}														\
		else if ( type == PARAM_TYPE_LIVE_REG ) {				\
			reg = &this->mLiveRegisters [ regIdx ];				\
		}														\
		else {													\
			reg = &particleRegisters [ regIdx ];				\
		}														\
	}

#define READ_VALUE(var,bytecode)								\
	type = *( bytecode++ );										\
	if ( type == PARAM_TYPE_CONST ) {							\
		dst = ( u8* )&var;										\
		*( dst++ ) = *( bytecode++ );							\
		*( dst++ ) = *( bytecode++ );							\
		*( dst++ ) = *( bytecode++ );							\
		*( dst++ ) = *( bytecode++ );							\
	}															\
	else {														\
		regIdx = *( bytecode++ );								\
		var = 0.0f;												\
																\
		if ( type & PARAM_TYPE_REG_MASK ) {						\
			if ( type == PARAM_TYPE_SPRITE_REG ) {				\
				var = spriteRegisters [ regIdx ];				\
			}													\
			else if ( type == PARAM_TYPE_LIVE_REG ) {			\
				var = this->mLiveRegisters [ regIdx ];			\
			}													\
			else {												\
				var = particleRegisters [ regIdx ];				\
			}													\
		}														\
	}

#define READ_INT(var,bytecode)									\
		dst = ( u8* )&var;										\
		*( dst++ ) = *( bytecode++ );							\
		*( dst++ ) = *( bytecode++ );							\
		*( dst++ ) = *( bytecode++ );							\
		*( dst++ ) = *( bytecode++ );

//================================================================//
// MOAIParticleScript
//================================================================//

//----------------------------------------------------------------//
u32 MOAIParticleScript::Instruction::GetSize () {

	return this->mSize;
}

//----------------------------------------------------------------//
void MOAIParticleScript::Instruction::Init ( u32 opcode, cc8* format ) {
	this->mOpcode = opcode;
	this->mFormat = format;
}

//----------------------------------------------------------------//
MOAIParticleScript::Instruction::Instruction () :
	mOpcode ( MOAIParticleScript::END ),
	mFormat ( 0 ),
	mSize ( 1 ) {
}

//----------------------------------------------------------------//
void MOAIParticleScript::Instruction::Parse ( MOAIRubyState& state, u32 idx ) {

	u64 bits;
	u8 type;

	if ( this->mFormat ) {
		for ( u32 i = 0; this->mFormat [ i ]; ++i ) {
		
			cc8 c = this->mFormat [ i ];
			
			switch ( c ) {
				
				case 'I':
					
					this->mSize += sizeof ( u32 );
					
					this->mTypes [ i ] = PARAM_TYPE_FLAG;
					this->mParams [ i ] = state.GetParamValue < u32 >( idx++, 0 );
					break;
				
				case 'R':
					
					this->mSize += sizeof ( u8 );
					this->mSize += sizeof ( u8 );
					
					bits = state.GetParamValue < u64 >( idx++, 0 );
					type = ( bits >> 32 ) & PARAM_TYPE_MASK;

					if ( !( type & PARAM_TYPE_REG_MASK )) {
					
						this->mTypes [ i ] = PARAM_TYPE_CONST; // force load of 0 on decode
						this->mParams [ i ] = 0;
					}
					else {
					
						this->mTypes [ i ] = type;
						this->mParams [ i ] = ( u32 )( bits & 0xff );
					}
					break;
				
				case 'V':
					
					this->mSize += sizeof ( u8 );
					
					bits = state.GetParamValue < u64 >( idx++, 0 );
					
					type = ( bits >> 32 ) & PARAM_TYPE_MASK;
					this->mTypes [ i ] = type;
					
					if ( type & PARAM_TYPE_REG_MASK ) {
						
						this->mParams [ i ] = ( u32 )( bits & 0xff );
						this->mSize += sizeof ( u8 );
					}
					else if ( type == PARAM_TYPE_CONST ) {
					
						this->mParams [ i ] = ( u32 )bits;
						this->mSize += sizeof ( u32 );
					}
					else {
						
						// unrecognized; treat as const
						// TODO: issue warning
						this->mTypes [ i ] = PARAM_TYPE_CONST;
						this->mParams [ i ] = 0;
						this->mSize += sizeof ( u32 );
					}
					break;
			}
		}
	}
}

//----------------------------------------------------------------//
u8* MOAIParticleScript::Instruction::Write ( u8* cursor ) {

	*( cursor++ ) = ( u8 )this->mOpcode;
	u8* src;

	if ( this->mFormat ) {
	
		for ( u32 i = 0; this->mFormat [ i ]; ++i ) {
				
			cc8 c = this->mFormat [ i ];
			
			switch ( c ) {
				
				case 'I':
					
					src = ( u8* )&this->mParams [ i ];
					*( cursor++ ) = *( src++ );
					*( cursor++ ) = *( src++ );
					*( cursor++ ) = *( src++ );
					*( cursor++ ) = *( src++ );
					break;
				
				case 'R':
					
					*( cursor++ ) = this->mTypes [ i ];
					*( cursor++ ) = ( u8 )this->mParams [ i ];
					break;
				
				case 'V':
					
					*( cursor++ ) = this->mTypes [ i ];
					
					if ( this->mTypes [ i ] == PARAM_TYPE_CONST ) {
						
						src = ( u8* )&this->mParams [ i ];
						*( cursor++ ) = *( src++ );
						*( cursor++ ) = *( src++ );
						*( cursor++ ) = *( src++ );
						*( cursor++ ) = *( src++ );
					}
					else {
						
						*( cursor++ ) = ( u8 )this->mParams [ i ];
					}
					break;
			}
		}
	}
	return cursor;
}

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	abs
    @text	r0 = abs(v0)
 
    @in		MOAIParticleScript self
    @in		number r0
    @in		number v0
    @out	nil
*/
mrb_value MOAIParticleScript::_abs ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( ABS, "RV" )
}

//----------------------------------------------------------------//
/**	@lua	add
	@text	r0 = v0 + v1
	
	@in		MOAIParticleScript self
	@in		number r0
	@in		number v0
	@in		number v1
	@out	nil
*/
mrb_value MOAIParticleScript::_add ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( ADD, "RVV" )
}

//----------------------------------------------------------------//
/**	@lua	angleVec
	@text	Load two registers with the X and Y components of a unit
			vector with a given angle.
	
	@in		MOAIParticleScript self
	@in		number r0			Register to store result X.
	@in		number r1			Register to store result Y.
	@in		number v0			Angle of vector (in degrees).
	@out	nil
*/
mrb_value MOAIParticleScript::_angleVec ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( ANGLE_VEC, "RRV" )
}

//----------------------------------------------------------------//
/**	@lua	color
 @text	r0, r1, r2, r3 = color (of the MOAIParticleSystem)

 Note that if you do not specify SPRITE_RED and related values,
 sprites are rendered with the current values. This function is
 useful to store the values when the initialization script is
 run in registers.
 
 @in		MOAIParticleScript self
 @in		number r0	r
 @in		number r1	g
 @in		number r2	b
 @opt		number r3	a
 @out	nil
 */
mrb_value MOAIParticleScript::_color ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( COLOR, "RRRR" )
}

//----------------------------------------------------------------//
/**	@lua	cos
 @text	r0 = cos(v0)
 
 @in		MOAIParticleScript self
 @in		number r0
 @in		number v0
 @out	nil
 */
mrb_value MOAIParticleScript::_cos ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( COS, "RV" )
}

//----------------------------------------------------------------//
/**	@lua	cycle
	@text	Cycle v0 between v1 and v2.
	
	@in		MOAIParticleScript self
	@in		number r0
	@in		number v0
	@in		number v1
	@in		number v2
	@out	nil
*/
mrb_value MOAIParticleScript::_cycle ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( CYCLE, "RVVV" )
}

//----------------------------------------------------------------//
/**	@lua	div
	@text	r0 = v0 / v1
	
	@in		MOAIParticleScript self
	@in		number r0
	@in		number v0
	@in		number v1
	@out	nil
*/
mrb_value MOAIParticleScript::_div ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( DIV, "RVV" )
}

//----------------------------------------------------------------//
/**	@lua	ease
	@text	Load a register with a value interpolated between two numbers
			using an ease curve.
	
	@in		MOAIParticleScript self
	@in		number r0			Register to store result.
	@in		number v0			Starting value of the ease.
	@in		number v1			Ending value of the ease.
	@in		number easeType		See MOAIEaseType for a list of ease types.
	@out	nil
*/
mrb_value MOAIParticleScript::_ease ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( EASE, "RVVI" )
}

//----------------------------------------------------------------//
/**	@lua	easeDelta
	@text	Load a register with a value interpolated between two numbers
			using an ease curve. Apply as a delta.
	
	@in		MOAIParticleScript self
	@in		number r0			Register to store result.
	@in		number v0			Starting value of the ease.
	@in		number v1			Ending value of the ease.
	@in		number easeType		See MOAIEaseType for a list of ease types.
	@out	nil
*/
mrb_value MOAIParticleScript::_easeDelta ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( EASE_DELTA, "RVVI" )
}

//----------------------------------------------------------------//
/**	@lua	mul
	@text	r0 = v0 * v1
	
	@in		MOAIParticleScript self
	@in		number r0
	@in		number v0
	@in		number v1
	@out	nil
*/
mrb_value MOAIParticleScript::_mul ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( MUL, "RVV" )
}

//----------------------------------------------------------------//
/**	@lua	norm
	@text	<p>r0 = v0 / |v|</p>
			<p>r1 = v1 / |v|</p>
			<p>Where |v| == sqrt(v0^2 + v1^2)</p>
	
	@in		MOAIParticleScript self
	@in		number r0
	@in		number r1
	@in		number v0
	@in		number v1
	@out	nil
*/
mrb_value MOAIParticleScript::_norm ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( NORM, "RRVV" )
}

//----------------------------------------------------------------//
/**	@lua	oscillate
	@text	r0 = v0 + ( sin ( v1 + ( age * v2 )) * v3 )

	@in		MOAIParticleScript self
	@in		number r0
	@in		number v0
	@out	nil
*/
mrb_value MOAIParticleScript::_oscillate ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( OSCILLATE, "RVVVV" )
}

//----------------------------------------------------------------//
/**	@lua	packConst
	@text	Pack a const value into a particle script param.
	
	@in		number const		Const value to pack.
	@out	number packed		The packed value.
*/
mrb_value MOAIParticleScript::_packConst ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	float val = state.GetParamValue < float >( 1, 0.0f );
	u32 bits;
	memcpy ( &bits, &val, sizeof ( u32 ));

	return state.ToRValue ( Pack64 ( bits, PARAM_TYPE_CONST ) );
}

//----------------------------------------------------------------//
/**	@lua	packLiveReg
	@text	Pack a live register index into a particle script param.
	
	@in		number regIdx		Register index to pack.
	@out	number packed		The packed value.
*/
mrb_value MOAIParticleScript::_packLiveReg ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	u8 val = (state.GetParamValue < u8 >( 1, 0 ) - 1) % LIVE_REG_COUNT;

	return state.ToRValue ( Pack64 ( val, PARAM_TYPE_LIVE_REG ) );
}

//----------------------------------------------------------------//
/**	@lua	packReg
	@text	Pack a register index into a particle script param.
	
	@in		number regIdx		Register index to pack.
	@out	number packed		The packed value.
*/
mrb_value MOAIParticleScript::_packReg ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	u8 val = state.GetParamValue < u8 >( 1, 0 ) + MOAIParticle::TOTAL_PARTICLE_REG - 1;

	return state.ToRValue ( Pack64 ( val, PARAM_TYPE_PARTICLE_REG ) );
}

//----------------------------------------------------------------//
/**	@lua	rand
	@text	Load a register with a random number from a range.
	
	@in		MOAIParticleScript self
	@in		number r0			Register to store result.
	@in		number v0			Range minimum.
	@in		number v1			Range maximum.
	@out	nil
*/
mrb_value MOAIParticleScript::_rand ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( RAND, "RVV" )
}

//----------------------------------------------------------------//
/**	@lua	randInt
	@text	Load a register with a random integer from a range.
	
	@in		MOAIParticleScript self
	@in		number r0			Register to store result.
	@in		number v0			Range minimum.
	@in		number v1			Range maximum.
	@out	nil
 */
mrb_value MOAIParticleScript::_randInt ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( RAND_INT, "RVV" )
}

//----------------------------------------------------------------//
/**	@lua	randVec
	@text	Load two registers with the X and Y components of a vector
			with randomly chosen direction and length.
	
	@in		MOAIParticleScript self
	@in		number r0			Register to store result X.
	@in		number r1			Register to store result Y.
	@in		number v0			Minimum length of vector.
	@in		number v1			Maximum length of vector.
	@out	nil
*/
mrb_value MOAIParticleScript::_randVec ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( RAND_VEC, "RRVV" )
}

//----------------------------------------------------------------//
/**	@lua	set
	@text	Load a value into a register.
	
	@in		MOAIParticleScript self
	@in		number r0			Register to store result.
	@in		number v0			Value to load.
	@out	nil
*/
mrb_value MOAIParticleScript::_set ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( SET, "RV" )
}

//----------------------------------------------------------------//
/**	@lua	setLiveReg
	@text	Load a value into a live register. Live registers can be updated
			by additional calls to setReg, which does not alter the compiled particle
			script. Live registers are a distinct register set from the normal register
			set; use load () to load live register data into registers in an initialize
			or render script.
	
	@in		MOAIParticleScript self
	@in		number r0					Register to store result.
	@in		number v0					Value to load.
	@out	nil
*/
mrb_value MOAIParticleScript::_setLiveReg ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleScript, "UNN" )
	/* assumption:
	 * the packing system will always put the bits we care about in the
	 * low-order parts of a packed value
	 */
	int reg			= state.GetParamValue < u64 >( 1, 0 ) & 0xFF;
	float value		= state.GetParamValue < float >( 2, 0.0f );
	if ( reg < 0 || reg >= LIVE_REG_COUNT ) {
		return mrb_nil_value ();
	}
	self->mLiveRegisters[reg] = value;
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	sin
	@text	r0 = sin(v0)

	@in		MOAIParticleScript self
	@in		number r0
	@in		number v0
	@out	nil
*/
mrb_value MOAIParticleScript::_sin ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( SIN, "RV" )
}

//----------------------------------------------------------------//
/**	@lua	sprite
	@text	Push a new sprite for rendering. To render a particle, first
			call 'sprite' to create a new sprite at the particle's location.
			Then modify the sprite's registers to create animated effects
			based on the age of the particle (normalized to its term).
	
	@in		MOAIParticleScript self
	@out	nil
*/
mrb_value MOAIParticleScript::_sprite ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( SPRITE, "" )
}

//----------------------------------------------------------------//
/**	@lua	step
    @text	r0 = 0 if v0 < v1; 1 if v0 >= v1

    @in		MOAIParticleScript self
    @in		number r0
    @in		number v0
    @in		number v1
    @out	nil
*/
mrb_value MOAIParticleScript::_step ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( STEP, "RVV" )
}

//----------------------------------------------------------------//
/**	@lua	sub
	@text	r0 = v0 - v1
	
	@in		MOAIParticleScript self
	@in		number r0
	@in		number v0
	@in		number v1
	@out	nil
*/
mrb_value MOAIParticleScript::_sub ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( SUB, "RVV" )
}

//----------------------------------------------------------------//
/**	@lua	tan
 @text	r0 = tan(v0)
 
 @in		MOAIParticleScript self
 @in		number r0
 @in		number v0
 @out	nil
 */
mrb_value MOAIParticleScript::_tan ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( TAN, "RV" )
}

//----------------------------------------------------------------//
/**	@lua	time
	@text	Load the normalized age of the particle into a register.
	
	@in		MOAIParticleScript self
	@in		number r0
	@out	nil
*/
mrb_value MOAIParticleScript::_time ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( TIME, "R" )
}

//----------------------------------------------------------------//
/**	@lua	wrap
	@text	Wrap v0 between v1 and v2.
	
	@in		MOAIParticleScript self
	@in		number r0
	@in		number v0
	@in		number v1
	@in		number v2
	@out	nil
*/
mrb_value MOAIParticleScript::_wrap ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( WRAP, "RVVV" )
}

//----------------------------------------------------------------//
/**	@lua	vecAngle
	@text	Compute angle (in degrees) between v0 and v1.
	
	@in		MOAIParticleScript self
	@in		number r0
	@in		number v0
	@in		number v1
	@out	nil
*/
mrb_value MOAIParticleScript::_vecAngle ( mrb_state* M, mrb_value context ) {
	IMPL_RUBY_PARTICLE_OP ( VEC_ANGLE, "RVV" )
}

//================================================================//
// MOAIParticleScript
//================================================================//

//----------------------------------------------------------------//
u8* MOAIParticleScript::Compile () {

	if ( this->mCompiled ) return this->mBytecode;
	
	Instruction end;
	end.Init ( END, "" );

	u32 size = 0;
	FOREACH ( InstructionIt, instructionIt, this->mInstructions ) {
		Instruction& instruction = *instructionIt;
		size += instruction.GetSize ();
	}
	size += end.GetSize ();
	
	this->mBytecode.Init ( size );
	
	u8* cursor = this->mBytecode;
	
	u8* top = ( u8* )(( size_t )cursor + size );
	UNUSED ( top );
	
	FOREACH ( InstructionIt, instructionIt, this->mInstructions ) {
		Instruction& instruction = *instructionIt;
		cursor = instruction.Write ( cursor );
	}
	cursor = end.Write ( cursor );
	
	assert ( cursor == top );
	
	this->mInstructions.clear ();
	this->mCompiled = true;
	return this->mBytecode;
}

//----------------------------------------------------------------//
MOAIParticleScript::MOAIParticleScript () :
	mCompiled ( false ) {

	int i;
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END

	for (i = 0; i < LIVE_REG_COUNT; ++i) {
		this->mLiveRegisters[i] = 0.0;
	}
}

//----------------------------------------------------------------//
MOAIParticleScript::~MOAIParticleScript () {
}

//----------------------------------------------------------------//
MOAIParticleScript::Instruction& MOAIParticleScript::PushInstruction ( u32 op, cc8* format ) {

	Instruction instruction;
	instruction.Init ( op, format );
	
	this->mInstructions.push_back ( instruction );
	return this->mInstructions.back ();
}

//----------------------------------------------------------------//
u64 MOAIParticleScript::Pack64 ( u32 low, u32 hi ) {

	u64 val64 = 0; 
	u32 lohi [ 2 ]; 
      
	lohi [ 0 ] = low; 
	lohi [ 1 ] = hi; 
 
	memcpy ( &val64, lohi, sizeof ( val64 )); 
 
	return val64;
}

//----------------------------------------------------------------//
void MOAIParticleScript::PushSprite ( MOAIParticleSystem& system, float* registers ) {

	AKUParticleSprite sprite;

	sprite.mXLoc		= registers [ SPRITE_X_LOC ];
	sprite.mYLoc		= registers [ SPRITE_Y_LOC ];
	
	sprite.mZRot		= registers [ SPRITE_ROT ];
	
	sprite.mXScl		= registers [ SPRITE_X_SCL ];
	sprite.mYScl		= registers [ SPRITE_Y_SCL ];
	
	float opacity		= registers [ SPRITE_OPACITY ];
	float glow			= 1.0f - registers [ SPRITE_GLOW ];
	
	sprite.mRed			= registers [ SPRITE_RED ] * opacity;
	sprite.mGreen		= registers [ SPRITE_GREEN ] * opacity;
	sprite.mBlue		= registers [ SPRITE_BLUE ] * opacity;
	sprite.mAlpha		= opacity * glow;
	
	sprite.mGfxID		= ZLFloat::ToInt ( registers [ SPRITE_IDX ]);
	
	system.PushSprite ( sprite );
}

//----------------------------------------------------------------//
void MOAIParticleScript::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "PARTICLE_X",			Pack64 ( MOAIParticle::PARTICLE_X,		PARAM_TYPE_PARTICLE_REG ));
	state.DefineClassConst ( klass, "PARTICLE_Y",			Pack64 ( MOAIParticle::PARTICLE_Y,		PARAM_TYPE_PARTICLE_REG ));
	state.DefineClassConst ( klass, "PARTICLE_DX",			Pack64 ( MOAIParticle::PARTICLE_DX,		PARAM_TYPE_PARTICLE_REG ));
	state.DefineClassConst ( klass, "PARTICLE_DY",			Pack64 ( MOAIParticle::PARTICLE_DY,		PARAM_TYPE_PARTICLE_REG ));

	state.DefineClassConst ( klass, "SPRITE_X_LOC",		Pack64 ( SPRITE_X_LOC,		PARAM_TYPE_SPRITE_REG ));
	state.DefineClassConst ( klass, "SPRITE_Y_LOC",		Pack64 ( SPRITE_Y_LOC,		PARAM_TYPE_SPRITE_REG ));
	state.DefineClassConst ( klass, "SPRITE_ROT",			Pack64 ( SPRITE_ROT,		PARAM_TYPE_SPRITE_REG ));
	state.DefineClassConst ( klass, "SPRITE_X_SCL",		Pack64 ( SPRITE_X_SCL,		PARAM_TYPE_SPRITE_REG ));
	state.DefineClassConst ( klass, "SPRITE_Y_SCL",		Pack64 ( SPRITE_Y_SCL,		PARAM_TYPE_SPRITE_REG ));
	state.DefineClassConst ( klass, "SPRITE_RED",			Pack64 ( SPRITE_RED,		PARAM_TYPE_SPRITE_REG ));
	state.DefineClassConst ( klass, "SPRITE_GREEN",		Pack64 ( SPRITE_GREEN,		PARAM_TYPE_SPRITE_REG ));
	state.DefineClassConst ( klass, "SPRITE_BLUE",			Pack64 ( SPRITE_BLUE,		PARAM_TYPE_SPRITE_REG ));
	state.DefineClassConst ( klass, "SPRITE_OPACITY",		Pack64 ( SPRITE_OPACITY,	PARAM_TYPE_SPRITE_REG ));
	state.DefineClassConst ( klass, "SPRITE_GLOW",			Pack64 ( SPRITE_GLOW,		PARAM_TYPE_SPRITE_REG ));
	state.DefineClassConst ( klass, "SPRITE_IDX",			Pack64 ( SPRITE_IDX,		PARAM_TYPE_SPRITE_REG ));
	
	
	state.DefineClassConst ( klass, "PARTICLE_AGE",		Pack64 ( PARTICLE_AGE,		PARAM_TYPE_SPRITE_REG ));
	state.DefineClassConst ( klass, "PARTICLE_TIME",		Pack64 ( PARTICLE_TIME,		PARAM_TYPE_SPRITE_REG ));
	
	state.DefineStaticMethod ( klass, "packConst",			_packConst, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "packLiveReg",		_packLiveReg, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "packReg",			_packReg, MRB_ARGS_REQ ( 1 ) );
}

//----------------------------------------------------------------//
void MOAIParticleScript::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	state.DefineInstanceMethod ( klass, "abs",				_abs, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "add",				_add, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "angleVec",			_angleVec, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "color",			_color, MRB_ARGS_ARG ( 3, 1 ) );
	state.DefineInstanceMethod ( klass, "cos",				_cos, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "cycle",			_cycle, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "div",				_div, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "ease",				_ease, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "easeDelta",		_easeDelta, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "mul",				_mul, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "norm",				_norm, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "oscillate",		_oscillate, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "rand",				_rand, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "randInt",			_randInt, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "randVec",			_randVec, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "set",				_set, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setLiveReg",		_setLiveReg, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setReg",			_setLiveReg, MRB_ARGS_REQ ( 2 ) ); // TODO: mark as deprecated
	state.DefineInstanceMethod ( klass, "sin",				_sin, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "sprite",			_sprite, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "step",				_step, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "sub",				_sub, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "tan",				_tan, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "time",				_time, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "vecAngle",			_vecAngle, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "wrap",				_wrap, MRB_ARGS_REQ ( 4 ) );

}

//----------------------------------------------------------------//
void MOAIParticleScript::ResetRegisters ( float* spriteRegisters, float* particleRegisters, const MOAIParticleSystem &system ) {

	spriteRegisters [ SPRITE_X_LOC ]		= particleRegisters [ MOAIParticle::PARTICLE_X ];
	spriteRegisters [ SPRITE_Y_LOC ]		= particleRegisters [ MOAIParticle::PARTICLE_Y ];
	spriteRegisters [ SPRITE_ROT ]			= 0.0f;
	spriteRegisters [ SPRITE_X_SCL ]		= 1.0f;
	spriteRegisters [ SPRITE_Y_SCL ]		= 1.0f;
	spriteRegisters [ SPRITE_RED ]			= system.mR;
	spriteRegisters [ SPRITE_GREEN ]		= system.mG;
	spriteRegisters [ SPRITE_BLUE ]			= system.mB;
	spriteRegisters [ SPRITE_OPACITY ]		= system.mA;
	spriteRegisters [ SPRITE_GLOW ]			= 0.0f;
	spriteRegisters [ SPRITE_IDX ]			= 1.0f;
}

//----------------------------------------------------------------//
void MOAIParticleScript::Run ( MOAIParticleSystem& system, MOAIParticle& particle, float t0, float t1 ) {

	u8* dst;
	u8* bytecode = this->mBytecode;
	if ( !bytecode ) return;
	
	float particleRegisters [ MAX_PARTICLE_REGISTERS ];
	memcpy ( particleRegisters, particle.mData, sizeof ( float ) * system.mParticleSize );
	
	float spriteRegisters [ TOTAL_SPRITE_REG ];
	
	spriteRegisters [ PARTICLE_AGE ] = particle.mAge;
	spriteRegisters [ PARTICLE_TIME ] = t1;
	
	float* r0;
	float* r1;
	float* r2;
	float* r3;
	float v0, v1, v2, v3;
	u32 i0;
	
	u8 type;
	u8 regIdx;

	bool push = false;
	
	for ( u8 opcode = *( bytecode++ ); opcode != END; opcode = *( bytecode++ )) {
		
		switch ( opcode ) {

			case ABS: // RV
				READ_ADDR	( r0, bytecode );
				READ_VALUE	( v0, bytecode );

				if ( r0 ) {
					*r0 = fabsf ( v0 );
				}
				break;
			
			case ADD: // RVV
				
				READ_ADDR	( r0, bytecode );
				READ_VALUE	( v0, bytecode );
				READ_VALUE	( v1, bytecode );
				
				if ( r0 ) {
					*r0 = v0 + v1;
				}
				break;
			
			case ANGLE_VEC: // RRV
				
				// TODO: should pass in a length as well
				READ_ADDR ( r0, bytecode );
				READ_ADDR ( r1, bytecode );
				READ_VALUE ( v0, bytecode );

				if( r0 && r1){
					*r0 = ( float )( Cos ( v0 * ( float )D2R ));
					*r1 = ( float )( Sin ( v0 * ( float )D2R ));
				}
				break;
			
			case COLOR: // RRRR
				READ_ADDR   ( r0, bytecode );
				READ_ADDR   ( r1, bytecode );
				READ_ADDR   ( r2, bytecode );
				READ_ADDR   ( r3, bytecode );

				if (r0 && r1 && r2) {
					*r0 = system.mR;
					*r1 = system.mG;
					*r2 = system.mB;
					// allow Alpha to be omitted.
					if (r3) {
						*r3 = system.mA;
					}
				}
				break;
			case COS: // RVV
				READ_ADDR   ( r0, bytecode );
				READ_VALUE  ( v0, bytecode );
				
				if ( r0 ) {
					*r0 = ( float )( cos ( v0 ) );
				}
				break;

			case CYCLE: // RVVV
				
				READ_ADDR	( r0, bytecode );
				READ_VALUE	( v0, bytecode );
				READ_VALUE	( v1, bytecode );
				READ_VALUE	( v2, bytecode );
				
				if ( r0 ) {
					v3 = v2 - v1;
					v3 = ( v3 < 0.0f ) ? -v3 : v3;
					
					float cycle = ( v0 - v1 ) / v3;
					int cycleIdx = ZLFloat::ToInt ( ZLFloat::Floor ( cycle ));
					float cycleDec = cycle - ( float )cycleIdx;
					
					if ( cycleIdx & 0x01 ) {
						cycleDec = 1.0f - cycleDec;
					}
					
					v0 = v1 + ( cycleDec * v3 );
					
					*r0 = v0;
				}
				break;
			
			case DIV: // RVV
				
				READ_ADDR	( r0, bytecode );
				READ_VALUE	( v0, bytecode );
				READ_VALUE	( v1, bytecode );
				
				if ( r0 ) {
					*r0 = v0 / v1;
				}
				break;
			
			case EASE: // RVVI
				
				READ_ADDR	( r0, bytecode );
				READ_VALUE	( v0, bytecode );
				READ_VALUE	( v1, bytecode );
				READ_INT	( i0, bytecode );
				
				if ( r0 ) {
					*r0 = ZLInterpolate::Interpolate ( i0, v0, v1, t1 );
				}
				break;
			
			case EASE_DELTA: // RVVI
				
				READ_ADDR	( r0, bytecode );
				READ_VALUE	( v0, bytecode );
				READ_VALUE	( v1, bytecode );
				READ_INT	( i0, bytecode );
				
				if ( r0 ) {
					
					v2 = ZLInterpolate::Interpolate ( i0, v0, v1, t0 );
					v3 = ZLInterpolate::Interpolate ( i0, v0, v1, t1 );
					
					*r0 += ( v3 - v2 );
				}
				break;
			
			case MUL: // RVV
				
				READ_ADDR	( r0, bytecode );
				READ_VALUE	( v0, bytecode );
				READ_VALUE	( v1, bytecode );
				
				if ( r0 ) {
					*r0 = v0 * v1;
				}
				break;
			
			case NORM:
				
				READ_ADDR ( r0, bytecode );
				READ_ADDR ( r1, bytecode );
				READ_VALUE ( v0, bytecode );
				READ_VALUE ( v1, bytecode );

				if ( r0 && r1 ) {
					v3 = Sqrt (( v0 * v0 ) + ( v1 * v1 ));
					if ( v3 ) {
						*r0 = ( float )( v0 / v3 );
						*r1 = ( float )( v1 / v3 );
					}
					else {
						*r0 = 0;
						*r1 = 0;
					}
				}
				break;
			
			case OSCILLATE: // RV

				READ_ADDR   ( r0, bytecode );
				READ_VALUE  ( v0, bytecode );
				READ_VALUE  ( v1, bytecode );
				READ_VALUE  ( v2, bytecode );
				READ_VALUE  ( v3, bytecode );
				
				if ( r0 ) {
					*r0 = v0 + ( float )( sin ( v1 + ( particle.mAge * v2 )) * v3 );
				}
				break;
			
			case RAND: // RVV
				
				READ_ADDR	( r0, bytecode );
				READ_VALUE	( v0, bytecode );
				READ_VALUE	( v1, bytecode );
				
				if ( r0 ) {
					*r0 = ZLFloat::Rand ( v0, v1 );
				}
				break;
				
			case RAND_INT:
				
				READ_ADDR	( r0, bytecode );
				READ_VALUE	( v0, bytecode );
				READ_VALUE	( v1, bytecode );
				
				if ( r0 ) {
					*r0 = ( v0 != v1 ) ? v0 + ( rand () % ( int )( v1 - v0 + 1)) : v0;
				}
				break;

			case RAND_VEC: // RRVV
				
				READ_ADDR	( r0, bytecode );
				READ_ADDR	( r1, bytecode );
				READ_VALUE	( v0, bytecode );
				READ_VALUE	( v1, bytecode );
				
				v2 = ZLFloat::Rand ( 360.0f ) * ( float )D2R;
				v3 = ZLFloat::Rand ( v0,  v1 );
				
				if ( r0 ) {
					*r0 = Cos ( v2 ) * v3;
				}
				
				if ( r1 ) {
					*r1 = -Sin ( v2 ) * v3;
				}
				break;

			case SET: // RV
				
				READ_ADDR	( r0, bytecode )
				READ_VALUE	( v0, bytecode )
				
				if ( r0 ) {
					*r0 = v0;
				}
				break;
			
			case SIN: // RV

				READ_ADDR   ( r0, bytecode );
				READ_VALUE  ( v0, bytecode );
				
				if ( r0 ) {
					*r0 = ( float )( sin ( v0 ) );
				}
				break;

			case SPRITE: //
				
				if ( push ) {
					this->PushSprite ( system, spriteRegisters );
				}
				this->ResetRegisters ( spriteRegisters, particleRegisters, system );
				push = true;
				break;

			case STEP: // RVV

				READ_ADDR	( r0, bytecode );
				READ_VALUE	( v0, bytecode );
				READ_VALUE	( v1, bytecode );
				
				if ( r0 ) {
					*r0 = v0 < v1 ? 0.0f : 1.0f;
				}
				break;
			
			case SUB: // RVV
				
				READ_ADDR	( r0, bytecode );
				READ_VALUE	( v0, bytecode );
				READ_VALUE	( v1, bytecode );
				
				if ( r0 ) {
					*r0 = v0 - v1;
				}
				break;

			case TAN: // RV

				READ_ADDR   ( r0, bytecode );
				READ_VALUE  ( v0, bytecode );
				
				if ( r0 ) {
					*r0 = ( float )( tan ( v0 ) );
				}
				break;

			case TIME: // RVV
				
				READ_ADDR	( r0, bytecode );
				
				if ( r0 ) {
					*r0 = t1;
				}
				break;
			
			case VEC_ANGLE: // RVV
				
				READ_ADDR	( r0, bytecode );
				READ_VALUE	( v0, bytecode );
				READ_VALUE	( v1, bytecode );
				
				if ( r0 ) {
					*r0 = ( float )( atan2 ( v0, v1 ) * R2D );
				}
				break;
			
			case WRAP: // RVVV
				
				READ_ADDR	( r0, bytecode );
				READ_VALUE	( v0, bytecode );
				READ_VALUE	( v1, bytecode );
				READ_VALUE	( v2, bytecode );
				
				if ( r0 ) {
					v3 = v2 - v1;
					
					while ( v0 < v1 ) {
						v0 += v3;
					}
					
					while ( v0 >= v2 ) {
						v0 -= v3;
					}
					
					*r0 = v0;
				}
				break;
		}
	}
	
	memcpy ( particle.mData, particleRegisters, sizeof ( float ) * system.mParticleSize );
	
	if ( push ) {
		this->PushSprite ( system, spriteRegisters );
	}
}
