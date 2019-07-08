// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPARTICLEEMITTER_H
#define	MOAIPARTICLEEMITTER_H

#include <moai-sim/MOAIAction.h>
#include <moai-sim/MOAITransform.h>

class MOAIParticleSystem;
class MOAIPartitionHull;

//================================================================//
// MOAIParticleEmitter
//================================================================//
/**	@lua	MOAIParticleEmitter
	@text	Particle emitter.
*/
class MOAIParticleEmitter :
	public MOAITransform,
	public MOAIAction {
protected:

	u32		mShapeID;
	ZLRect	mRect;
	
	float	mInnerRadius;
	float	mOuterRadius;
	
	u32		mMinEmission;
	u32		mMaxEmission;

	float	mMinAngle;
	float	mMaxAngle;
	
	float	mMinMagnitude;
	float	mMaxMagnitude;

	u32		mEmission;
	u32		mParticleState;

	MOAIRubySharedPtr < MOAIParticleSystem > mSystem;
	MOAIRubySharedPtr < MOAIPartitionHull > mMaskProp;

	//----------------------------------------------------------------//
	static mrb_value		_setAngle				( mrb_state* M, mrb_value context );
	static mrb_value		_setEmission			( mrb_state* M, mrb_value context );
	static mrb_value		_setMagnitude			( mrb_state* M, mrb_value context );
	static mrb_value		_setMask				( mrb_state* M, mrb_value context );
	static mrb_value		_setRadius				( mrb_state* M, mrb_value context );
	static mrb_value		_setRect				( mrb_state* M, mrb_value context );
	static mrb_value		_setState				( mrb_state* M, mrb_value context );
	static mrb_value		_setSystem				( mrb_state* M, mrb_value context );
	static mrb_value		_surge					( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	u32				GetRandomEmission		();
	void			GetRandomParticle		( ZLVec3D& loc, ZLVec3D& vec ); // in local space
	ZLVec3D			GetRandomVec			( float minAngle, float maxAngle, float min, float max );
	bool			MaskParticle			( const ZLVec3D& loc );

	//----------------------------------------------------------------//
	bool			MOAIAction_IsDone		();
	void			MOAINode_Update			();

public:
	
	enum {
		POINT,
		CIRCLE,
		RECT,
	};
	
	DECL_RUBY_FACTORY ( MOAIParticleEmitter, MOAIAction )

	SET ( ZLRect, Rect, mRect )
	SET ( u32, ShapeID, mShapeID )

	//----------------------------------------------------------------//
					MOAIParticleEmitter		();
					~MOAIParticleEmitter	();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			SerializeIn				( MOAIRubyState& state, MOAIDeserializer& serializer );
	void			SerializeOut			( MOAIRubyState& state, MOAISerializer& serializer );
	void			SetAngleRange			( float min, float max );
	void			SetEmissionRange		( u32 min, u32 max );
	void			SetMagnitudeRange		( float min, float max );
	void			Surge					( u32 total );
};

#endif
