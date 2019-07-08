// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIANIMCURVEBONE_H
#define	MOAIANIMCURVEBONE_H

#include <moai-sim/MOAIAnimCurveBase.h>
#include <moai-sim/MOAINode.h>

//================================================================//
// MOAIAnimCurveBone
//================================================================//
// TODO: doxygen
class MOAIAnimCurveBone :
	public virtual MOAIAnimCurveBase {
private:

	ZLLeanArray < ZLVec3D >			mPositionSamples;
	ZLLeanArray < ZLQuaternion >	mRotationSamples;
	ZLLeanArray < ZLVec3D >			mScaleSamples;
	
	ZLAffine3D mValue;

	//----------------------------------------------------------------//
	static mrb_value		_getValueAtTime		( mrb_state* M, mrb_value context );
	static mrb_value		_setKey				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	static ZLAffine3D		Compose				( const ZLVec3D& pos, const ZLQuaternion& rot, const ZLVec3D& scl );
	void					GetCurveDelta		( ZLVec3D& pos, ZLQuaternion& rot, ZLVec3D& scl ) const;
	ZLAffine3D				GetValue			( const MOAIAnimKeySpan& span ) const;
	void					GetValue			( const MOAIAnimKeySpan& span, ZLVec3D& pos, ZLQuaternion& rot, ZLVec3D& scl ) const;
	void					ReserveSamples		( u32 total );

	//----------------------------------------------------------------//
	void					MOAINode_Update		();

public:
	
	DECL_RUBY_FACTORY ( MOAIAnimCurveBone, MOAINode )
	
	//----------------------------------------------------------------//
	void			ApplyValueAttrOp		( MOAIAttribute& attr, u32 op );
	void			GetDelta				( MOAIAttribute& attr, const MOAIAnimKeySpan& span0, const MOAIAnimKeySpan& span1 ) const;
	ZLAffine3D		GetValue				( float time ) const;
	void			GetValue				( MOAIAttribute& attr, const MOAIAnimKeySpan& span ) const;
	void			GetZero					( MOAIAttribute& attr ) const;
					MOAIAnimCurveBone		();
					~MOAIAnimCurveBone		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			SetSamplePosition		( u32 idx, float x, float y, float z );
	void			SetSampleRotation		( u32 idx, float x, float y, float z, float w );
	void			SetSampleScale			( u32 idx, float x, float y, float z );
};

#endif
