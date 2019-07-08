// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIANIM_H
#define	MOAIANIM_H

#include <moai-sim/MOAITimer.h>

class MOAIAnimCurveBase;

//================================================================//
// MOAIAnimLink
//================================================================//
class MOAIAnimLink {
public:

	MOAIRubySharedPtr < MOAIAnimCurveBase > mCurve;
	MOAIRubySharedPtr < MOAINode > mTarget;
	u32 mAttrID;
	bool mRelative;
};

//================================================================//
// MOAIAnim
//================================================================//
/**	@lua MOAIAnim
	@text Bind animation curves to nodes and provides timer controls for animation playback.
*/
class MOAIAnim :
	public virtual MOAITimer {
private:

	float mLength;

	ZLLeanArray < MOAIAnimLink > mLinks;

	//----------------------------------------------------------------//
	static mrb_value		_apply				( mrb_state* M, mrb_value context );
	static mrb_value		_getLength			( mrb_state* M, mrb_value context );
	static mrb_value		_reserveLinks		( mrb_state* M, mrb_value context );
	static mrb_value		_setLink			( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	void			MOAIAction_Update	( double step );
	
public:
	
	DECL_RUBY_FACTORY ( MOAIAnim, MOAITimer )
	
	GET ( float, Length, mLength )
	
	//----------------------------------------------------------------//
	void			Apply				( float t );
	void			Apply				( float t0, float t1 );
	void			Clear				();
	void			ClearLinks			();
					MOAIAnim			();
					~MOAIAnim			();
	void			RegisterRubyClass	( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs	( MOAIRubyState& state, RClass* klass );
	void			ReserveLinks		( u32 totalLinks );
	void			SetLink				( u32 linkID, MOAIAnimCurveBase* curve, MOAINode* target, u32 attrID, bool relative );
};

#endif
