// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAICOROUTINE_H
#define	MOAICOROUTINE_H

#include <moai-sim/MOAIAction.h>

class MOAICoroutine;

//================================================================//
// MOAICoroutine
//================================================================//
/**	@lua MOAICoroutine
	@text Binds a Ruby fiber to a MOAIAction.
*/
class MOAICoroutine :
	public MOAIAction {
private:

	MOAIRubyStrongRef			mRef; // reference to the Fiber
	u32							mNarg;
	STLString					mFuncName;
	STLString					mTrackingGroup;
	
	bool						mIsUpdating;
	bool						mIsActive;
	bool						mIsDefaultParent;
	
	//----------------------------------------------------------------//
	static mrb_value			_blockOnAction			( mrb_state* M, mrb_value context );
	static mrb_value			_currentThread			( mrb_state* M, mrb_value context );
	static mrb_value			_getHistogram			( mrb_state* M, mrb_value context );
	static mrb_value			_getTrackingGroup		( mrb_state* M, mrb_value context );
	static mrb_value			_reportHistogram		( mrb_state* M, mrb_value context );
	static mrb_value			_reportLeaks			( mrb_state* M, mrb_value context );
	static mrb_value			_run					( mrb_state* M, mrb_value context );
	static mrb_value			_setDefaultParent		( mrb_state* M, mrb_value context );
	static mrb_value			_setTrackingGroup		( mrb_state* M, mrb_value context );
	static mrb_value			_step					( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	mrb_value					Resume					( float step );
	
	//----------------------------------------------------------------//
	STLString					MOAIAction_GetDebugInfo		() const;
	bool						MOAIAction_IsDone			();
	void						MOAIAction_Start			();
	void						MOAIAction_Stop				();
	void						MOAIAction_Update			( double step );

public:
	
	DECL_RUBY_FACTORY ( MOAICoroutine, MOAIAction )
	
	//----------------------------------------------------------------//
	MOAIAction*					GetDefaultParent		();
								MOAICoroutine			();
								~MOAICoroutine			();
	void						RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void						RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
