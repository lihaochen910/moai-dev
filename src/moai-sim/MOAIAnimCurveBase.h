// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIANIMCURVEBASE_H
#define	MOAIANIMCURVEBASE_H

#include <moai-sim/MOAINode.h>

//================================================================//
// MOAIAnimKey
//================================================================//
class MOAIAnimKey {
public:

	float		mTime;
	u32			mMode;
	float		mWeight;
	
	bool operator < ( const MOAIAnimKey &rhs ) const {
		return mTime < rhs.mTime;
	}
	
	bool operator > ( const MOAIAnimKey &rhs ) const {
		return mTime > rhs.mTime;
	}
};

//================================================================//
// MOAIAnimKeySpan
//================================================================//
class MOAIAnimKeySpan {
public:

	u32		mKeyID;		// ID of first key in span
	float	mTime;		// Local time (within span)
	float	mCycle;		// Total number of times to add curve delta (repeat cycle)
};

//================================================================//
// MOAIAnimCurveBase
//================================================================//
/**	@lua	MOAIAnimCurveBase
	@text	Piecewise animation function with one input (time) and one output (value). This
			is the base class for typed animation curves (float, quaternion, etc.).
	
	@attr	ATTR_TIME
	@attr	ATTR_VALUE
	
	@attr  CLAMP
	@attr  WRAP
	@attr  MIRROR
	@attr  APPEND
*/
class MOAIAnimCurveBase :
	public virtual MOAINode {
protected:

	static const u32 NULL_KEY_ID = ( u32 )-1;

	ZLLeanArray < MOAIAnimKey > mKeys;

	float	mTime;
	u32		mWrapMode;

	//----------------------------------------------------------------//
	static mrb_value	_getLength				( mrb_state* M, mrb_value context );
	static mrb_value	_reserveKeys			( mrb_state* M, mrb_value context );
	static mrb_value	_setTime				( mrb_state* M, mrb_value context );
	static mrb_value	_setWrapMode			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	virtual void		ApplyValueAttrOp		( MOAIAttribute& attr, u32 op ) = 0;
	virtual void		GetDelta				( MOAIAttribute& attr, const MOAIAnimKeySpan& span0, const MOAIAnimKeySpan& span1 ) const = 0;
	MOAIAnimKeySpan		GetSpan					( float time ) const;
	virtual void		GetValue				( MOAIAttribute& attr, const MOAIAnimKeySpan& span ) const = 0;
	virtual void		GetZero					( MOAIAttribute& attr ) const = 0;
	virtual void		ReserveSamples			( u32 total ) = 0;

	//----------------------------------------------------------------//
	bool				MOAINode_ApplyAttrOp	( u32 attrID, MOAIAttribute& attr, u32 op );

public:
	
	DECL_ATTR_HELPER ( MOAIAnimCurveBase )
	
	enum {
		ATTR_TIME,
		ATTR_VALUE,
		TOTAL_ATTR,
	};

	enum {
		CLAMP,
		WRAP,
		MIRROR,
		APPEND,
	};
	
	//----------------------------------------------------------------//
	void				Clear					();
	virtual void		Draw					( u32 resolution ) const;
	u32					FindKeyID				( float time ) const;
	void				GetDelta				( MOAIAttribute& attr, float t0, float t1 );
	const MOAIAnimKey&	GetKey					( u32 id ) const;
	float				GetLength				() const;
	void				GetValue				( MOAIAttribute& attr, float time );
						MOAIAnimCurveBase		();
						~MOAIAnimCurveBase		();
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void				ReserveKeys				( u32 total );
	void				SetKey					( u32 id, float time, u32 mode, float weight = 1.0f );
	u32					Size					() const;
	float				WrapTime				( float t, float &repeat ) const;
};

#endif
