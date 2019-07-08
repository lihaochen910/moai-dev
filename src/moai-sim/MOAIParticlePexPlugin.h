// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPARTICLEPEXPLUGIN_H
#define	MOAIPARTICLEPEXPLUGIN_H

#if MOAI_WITH_TINYXML

#include <moai-sim/host_particles.h>
#include <moai-sim/MOAIParticlePlugin.h>
#include <moai-sim/MOAIParticleScript.h>
#include <tinyxml/tinyxml.h>

//================================================================//
// MOAIParticlePexPlugin
//================================================================//
/**	@lua	MOAIParticlePexPlugin
	@text	Allows custom particle processing derived from .pex file via C language callback.
*/
class MOAIParticlePexPlugin :
	public virtual MOAIParticlePlugin {
private:

	enum EmitterType{
		EMITTER_GRAVITY,
		EMITTER_RADIAL,
	};

	u32				mNumParticles;
	EmitterType		mEmitterType;
	
	float	mLifespan;
	float	mLifespanVariance;
	float	mLifespanTerm [ 2 ];

	s32		mLifespanRegister;

	float	mAngle;
	float	mAngleVariance;
	s32		mAngleRegister;

	float	mStartColor [ 4 ];
	float	mStartColorVariance [ 4 ];
	int		mStartColorRegister [ 4 ];
	
	float	mFinishColor [ 4 ];
	float	mFinishColorVariance [ 4 ];
	s32		mFinishColorRegister [ 4 ];

	float	mStartSize;
	float	mStartSizeVariance;
	s32		mStartSizeRegister;
	
	float	mFinishSize;
	float	mFinishSizeVariance;
	s32		mFinishSizeRegister;
	
	float	mGravity [ 2 ];
	
	float	mMaxRadius;
	float	mMaxRadiusVariance;
	s32		mMaxRadiusRegister;
	
	float	mMinRadius;
	
	float	mRadialAcceleration;
	float	mRadialAccelVariance;
	s32		mRadialAccelRegister;
	
	float	mTanAccel;
	float	mTanAccelVariance;
	s32		mTanAccelRegister;
	
	float	mRotStart;
	float	mRotStartVariance;
	s32		mRotStartRegister;
	
	float	mRotEnd;
	float	mRotEndVariance;
	s32		mRotEndRegister;
	
	float	mSpeed;
	float	mSpeedVariance;
	s32		mSpeedRegister;
	
	float	mRotPerSecond;
	float	mRotPerSecondVariance;
	
	float	mSourcePos [ 2 ];
	float	mSourcePosVariance [ 2 ];
	
	float	mDuration;

	u32		mBlendFuncSrc;
	u32		mBlendFuncDst;

	STLString	mTextureName;
	STLString	mParticlePath;

	// Gravity Registers
	s32		mStartXRegister;
	s32		mStartYRegister;
	s32		mDirectionXRegister;
	s32		mDirectionYRegister;

	// Radial Registers
	s32		mRotPerSecondRegister;
	s32		mRadialRegister;

	// Emission information.
	u32		mEmissionCount;
	float	mEmissionRate;
	
	//MOAIRubySharedPtr < MOAIImage > mImage;
	
	//----------------------------------------------------------------//
	static mrb_value		_getBlendMode		( mrb_state* M, mrb_value context );
	static mrb_value		_getDuration		( mrb_state* M, mrb_value context );
	static mrb_value		_getEmission		( mrb_state* M, mrb_value context );
	static mrb_value		_getFrequency		( mrb_state* M, mrb_value context );
	static mrb_value		_getLifespan		( mrb_state* M, mrb_value context );
	static mrb_value		_getMaxParticles	( mrb_state* M, mrb_value context );
	static mrb_value		_getRect			( mrb_state* M, mrb_value context );
	static mrb_value		_getTextureName		( mrb_state* M, mrb_value context );
	static mrb_value		_load				( mrb_state* M, mrb_value context );

	
	static void		Parse				( cc8* filename, MOAIParticlePexPlugin& plugin, TiXmlNode* node );
	
public:
	
	friend class MOAIParticleState;
	
	DECL_RUBY_FACTORY ( MOAIParticlePexPlugin, MOAIRubyObject )
	
	//----------------------------------------------------------------//
					MOAIParticlePexPlugin		();
					~MOAIParticlePexPlugin		();
	void			OnInit						( float* particle, float* registers );
	void			OnRender					( float* particle, float* registers, AKUParticleSprite* sprite, float t0, float t1, float term );
	void			RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
};

#endif
#endif
