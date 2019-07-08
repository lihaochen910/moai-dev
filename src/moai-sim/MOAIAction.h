// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIACTION_H
#define	MOAIACTION_H

#include <moai-sim/MOAIBlocker.h>
#include <moai-sim/MOAINode.h>

class MOAIAction;
class MOAIActionTree;

//================================================================//
// MOAIActionStackMgr
//================================================================//
class MOAIActionStackMgr :
	public ZLContextClass < MOAIActionStackMgr > {
private:
	
	ZLLeanStack < MOAIAction* > mStack;
	
	//----------------------------------------------------------------//
	MOAIAction*			GetCurrent				();
	MOAIAction*			GetDefaultParent		();
	void				Pop						();
	void				Push					( MOAIAction& action );
	
public:
	
	friend class MOAIAction;
	friend class MOAIActionTree;
	friend class MOAICoroutine;
	
	//----------------------------------------------------------------//
						MOAIActionStackMgr		();
						~MOAIActionStackMgr		();
};

//================================================================//
// MOAIAction
//================================================================//
/**	@lua MOAIAction
	@text Base class for actions.
	
	@const	EVENT_START
	@const	EVENT_STOP		ID of event stop callback. Signature is: nil onStop ()
*/
class MOAIAction :
	public MOAIBlocker,
	public virtual MOAIInstanceEventSource {
private:
	
	u32 mPass; // total number of times the node has been updated
	
	MOAIAction*	mParent;
	
	typedef ZLLeanList < MOAIAction* >::Iterator ChildIt;
	ZLLeanList < MOAIAction* > mChildren;
	
	ZLLeanLink < MOAIAction* > mLink;
	
	ChildIt mNextChildIt; // this iterator is used when updating the action tree
	
	float	mThrottle;
	
	enum {
		FLAGS_AUTO_STOP			= 1 << 0,
		FLAGS_IS_PAUSED			= 1 << 1,
		FLAGS_IS_UPDATING		= 1 << 2,
	};
	
	u32 mActionFlags;
	
	//----------------------------------------------------------------//
	static mrb_value		_addChild				( mrb_state* M, mrb_value context );
	static mrb_value		_attach					( mrb_state* M, mrb_value context );
	static mrb_value		_clear					( mrb_state* M, mrb_value context );
	static mrb_value		_defer					( mrb_state* M, mrb_value context );
	static mrb_value		_detach					( mrb_state* M, mrb_value context );
	static mrb_value 		_getChildren			( mrb_state* M, mrb_value context );
	static mrb_value 		_hasChildren			( mrb_state* M, mrb_value context );
	static mrb_value		_isActive				( mrb_state* M, mrb_value context );
	static mrb_value		_isBusy					( mrb_state* M, mrb_value context );
	static mrb_value		_isDone					( mrb_state* M, mrb_value context );
	static mrb_value		_isPaused				( mrb_state* M, mrb_value context );
	static mrb_value		_pause					( mrb_state* M, mrb_value context );
	static mrb_value		_setAutoStop			( mrb_state* M, mrb_value context );
	static mrb_value		_start					( mrb_state* M, mrb_value context );
	static mrb_value		_stop					( mrb_state* M, mrb_value context );
	static mrb_value		_throttle				( mrb_state* M, mrb_value context );
	static mrb_value		_update					( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void					ResetPass				( u32 pass = 0 );
	void					Update					( MOAIActionTree& tree, double step );

protected:

	//----------------------------------------------------------------//
	virtual void			MOAIAction_DidLoseChild			( MOAIAction* child );
	virtual STLString		MOAIAction_GetDebugInfo			() const;
	virtual MOAIAction*		MOAIAction_GetDefaultParent		();
	virtual bool			MOAIAction_IsDone				();
	virtual void			MOAIAction_Start				();
	virtual void			MOAIAction_Stop					();
	virtual void			MOAIAction_Update				( double step );
	void					MOAIBlocker_Unblock				();
	
public:
	
	friend class MOAIActionTree;
	friend class MOAIActionStackMgr;
	
	DECL_RUBY_FACTORY ( MOAIAction, MOAIInstanceEventSource )
	
	enum {
		EVENT_ACTION_PRE_UPDATE,
		EVENT_ACTION_POST_UPDATE,
		EVENT_START,
		EVENT_STOP,
		TOTAL_EVENTS,
	};
	
	//----------------------------------------------------------------//
	void					Attach					( MOAIAction* parent, bool defer );
	void					ClearChildren			();
	void					Defer					( bool defer );
	void					Detach					();
	virtual MOAIAction*		GetDefaultParent		();
	bool					IsActive				();
	bool					IsBusy					();
	bool					IsDone					();
	bool					IsPaused				();
							MOAIAction				();
							~MOAIAction				();
	void					RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void					RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void					Start					( MOAIAction* parent, bool defer );
	void					Stop					();
};

#endif
