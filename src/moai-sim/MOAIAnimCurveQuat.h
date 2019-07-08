// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIANIMCURVEQUAT_H
#define	MOAIANIMCURVEQUAT_H

#include <moai-sim/MOAIAnimCurveBase.h>
#include <moai-sim/MOAINode.h>

//================================================================//
// MOAIAnimCurveQuat
//================================================================//
/**	@lua	MOAIAnimCurveQuat
	@text	Implementation of animation curve for rotation (via quaternion) values.
*/
class MOAIAnimCurveQuat :
	public virtual MOAIAnimCurveBase {
private:

	ZLLeanArray < ZLQuaternion > mSamples;
	ZLQuaternion mValue;

	//----------------------------------------------------------------//
	static mrb_value		_getValueAtTime		( mrb_state* M, mrb_value context );
	static mrb_value		_setKey				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	ZLQuaternion	GetCurveDelta		() const;
	ZLQuaternion	GetValue			( const MOAIAnimKeySpan& span ) const;

	//----------------------------------------------------------------//
	void			MOAINode_Update		();

public:
	
	DECL_RUBY_FACTORY ( MOAIAnimCurveQuat, MOAINode )
	
	//----------------------------------------------------------------//
	void			ApplyValueAttrOp		( MOAIAttribute& attr, u32 op );
	void			GetDelta				( MOAIAttribute& attr, const MOAIAnimKeySpan& span0, const MOAIAnimKeySpan& span1 ) const;
	ZLQuaternion	GetValue				( float time ) const;
	void			GetValue				( MOAIAttribute& attr, const MOAIAnimKeySpan& span ) const;
	void			GetZero					( MOAIAttribute& attr ) const;
					MOAIAnimCurveQuat		();
					~MOAIAnimCurveQuat		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			ReserveSamples			( u32 total );
	void			SetSample				( u32 id, float x, float y, float z );
};

#endif
