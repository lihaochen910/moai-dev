// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIANIMCURVE_H
#define	MOAIANIMCURVE_H

#include <moai-sim/MOAIAnimCurveBase.h>
#include <moai-sim/MOAINode.h>

//================================================================//
// MOAIAnimCurve
//================================================================//
/**	@lua	MOAIAnimCurve
	@text	Implementation of animation curve for floating point values.
*/
class MOAIAnimCurve :
	public virtual MOAIAnimCurveBase {
private:

	ZLLeanArray < float > mSamples;
	float mValue;

	//----------------------------------------------------------------//
	static mrb_value		_getValueAtTime		( mrb_state* M, mrb_value context );
	static mrb_value		_getValueRange		( mrb_state* M, mrb_value context );
	static mrb_value		_setKey				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	float			GetCurveDelta		() const;
	float			GetValue			( const MOAIAnimKeySpan& span ) const;
	
	//----------------------------------------------------------------//
	void			MOAINode_Update		();

public:
	
	DECL_RUBY_FACTORY ( MOAIAnimCurve, MOAINode )
	
	//----------------------------------------------------------------//
	void			ApplyValueAttrOp	( MOAIAttribute& attr, u32 op );
	void			Draw				( u32 resolution ) const;
	void			GetDelta			( MOAIAttribute& attr, const MOAIAnimKeySpan& span0, const MOAIAnimKeySpan& span1 ) const;
	float			GetSample			( u32 id );
	float			GetValue			( float time ) const;
	void			GetValue			( MOAIAttribute& attr, const MOAIAnimKeySpan& span ) const;
	void			GetValueRange		( float t0, float t1, float &min, float &max );
	void			GetZero				( MOAIAttribute& attr ) const;
					MOAIAnimCurve		();
					~MOAIAnimCurve		();
	void			RegisterRubyClass	( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs	( MOAIRubyState& state, RClass* klass );
	void			ReserveSamples		( u32 total );
	void			SetSample			( u32 id, float value );
};

#endif
