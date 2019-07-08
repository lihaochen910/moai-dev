// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIANIMCURVEVEC_H
#define	MOAIANIMCURVEVEC_H

#include <moai-sim/MOAIAnimCurveBase.h>
#include <moai-sim/MOAINode.h>

//================================================================//
// MOAIAnimCurveVec
//================================================================//
/**	@lua	MOAIAnimCurveVec
	@text	Implementation of animation curve for 3D vector values.
*/
class MOAIAnimCurveVec :
	public virtual MOAIAnimCurveBase {
private:

	ZLLeanArray < ZLVec3D > mSamples;
	ZLVec3D mValue;

	//----------------------------------------------------------------//
	static mrb_value		_getValueAtTime		( mrb_state* M, mrb_value context );
	static mrb_value		_setKey				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	ZLVec3D			GetCurveDelta		() const;
	ZLVec3D			GetValue			( const MOAIAnimKeySpan& span ) const;

	//----------------------------------------------------------------//
	void			MOAINode_Update			();

public:
	
	DECL_RUBY_FACTORY ( MOAIAnimCurveVec, MOAINode )
	
	//----------------------------------------------------------------//
	void			ApplyValueAttrOp		( MOAIAttribute& attr, u32 op );
	void			GetDelta				( MOAIAttribute& attr, const MOAIAnimKeySpan& span0, const MOAIAnimKeySpan& span1 ) const;
	ZLVec3D			GetValue				( float time ) const;
	void			GetValue				( MOAIAttribute& attr, const MOAIAnimKeySpan& span ) const;
	void			GetZero					( MOAIAttribute& attr ) const;
					MOAIAnimCurveVec		();
					~MOAIAnimCurveVec		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			ReserveSamples			( u32 total );
	void			SetSample				( u32 id, const ZLVec3D& value );
};

#endif
