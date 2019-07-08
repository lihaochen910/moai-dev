// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIACTIONTREE_H
#define	MOAIACTIONTREE_H

#include <moai-sim/MOAIAction.h>

// TODO: finish out for general purpose use

//================================================================//
// MOAIActionTree
//================================================================//
/**	@lua MOAIActionTree
	@text	Tree of MOAIAction objects. Formerly a singleton; not yet
			ready for general purpose use.
*/
class MOAIActionTree :
	public virtual MOAIAction {
private:

	static const u32 RESET_PASS	= 0xffffffff;

	bool mProfilingEnabled;
	bool mThreadInfoEnabled;

	MOAIAction* mRoot;

	//----------------------------------------------------------------//
	MOAIAction*			AffirmRoot					();
	void				SetRoot						( MOAIAction* root );

	//----------------------------------------------------------------//
	void				MOAIAction_DidLoseChild		( MOAIAction* child );
	bool				MOAIAction_IsDone			();
	void				MOAIAction_Update			( double step );

protected:

	//----------------------------------------------------------------//
	static mrb_value	_getRoot				( mrb_state* M, mrb_value context );
	static mrb_value	_setProfilingEnabled	( mrb_state* M, mrb_value context );
	static mrb_value	_setRoot				( mrb_state* M, mrb_value context );
	static mrb_value	_setThreadInfoEnabled	( mrb_state* M, mrb_value context );

public:

	friend class MOAIAction;

	DECL_RUBY_FACTORY ( MOAIActionTree, MOAIAction )
	
	GET_SET ( bool, ProfilingEnabled, mProfilingEnabled )
	GET_SET ( bool, ThreadInfoEnabled, mThreadInfoEnabled )

	//----------------------------------------------------------------//
	MOAIAction*			GetDefaultParent		();
						MOAIActionTree			();
						~MOAIActionTree			();
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void				SetDefaultParent		();
	void				SetDefaultParent		( MOAIAction* defaultParent );
	void				Update					( double step );
};

#endif
