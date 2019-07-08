// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIEASEDRIVER_H
#define MOAIEASEDRIVER_H

#include <moai-sim/MOAITimer.h>

class MOAINode;

//================================================================//
// MOAIEaseDriverLink
//================================================================//
class MOAIEaseDriverLink {
public:
	
	MOAIRubySharedPtr < MOAINode >	mSource;
	u32								mSourceAttrID;
	
	MOAIRubySharedPtr < MOAINode >	mDest;
	u32								mDestAttrID;
	
	float							mV0;
	float							mV1;
	u32								mMode;
};

//================================================================//
// MOAIEaseDriver
//================================================================//
/**	@lua MOAIEaseDriver
	@text Action that applies simple ease curves to node attributes.
*/
class MOAIEaseDriver :
	public MOAITimer {
private:

	ZLLeanArray < MOAIEaseDriverLink > mLinks;

	//----------------------------------------------------------------//
	static mrb_value		_reserveLinks	( mrb_state* M, mrb_value context );
	static mrb_value		_setLink		( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void			MOAIAction_Update		( double step );

public:

	DECL_RUBY_FACTORY ( MOAIEaseDriver, MOAITimer )

	//----------------------------------------------------------------//
					MOAIEaseDriver			();
					~MOAIEaseDriver			();
	u32				ParseForMove			( MOAIRubyState& state, int idx, MOAINode* dest, u32 total, int mode, ... );
	u32				ParseForSeek			( MOAIRubyState& state, int idx, MOAINode* dest, u32 total, int mode, ... );
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			ReserveLinks			( u32 total );
	void			SetLink					( u32 idx, MOAINode* dest, u32 destAttrID, float v1, u32 mode );
	void			SetLink					( u32 idx, MOAINode* dest, u32 destAttrID, MOAINode* source, u32 sourceAttrID, u32 mode );
};

#endif
