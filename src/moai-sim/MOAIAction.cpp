// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIAction.h>
#include <moai-sim/MOAIActionTree.h>
#include <moai-sim/MOAISim.h>
#include <moai-sim/strings.h>

//================================================================//
// MOAIActionStackMgr
//================================================================//
	
//----------------------------------------------------------------//
MOAIAction* MOAIActionStackMgr::GetCurrent () {

	size_t top = this->mStack.GetTop ();
	return top > 0 ? this->mStack [ top - 1 ] : 0;
}

//----------------------------------------------------------------//
MOAIAction* MOAIActionStackMgr::GetDefaultParent () {

	MOAIAction* defaultParent = 0;

	MOAIAction* cursor = this->GetCurrent ();
	for ( ; cursor; cursor = cursor->mParent ) {
		defaultParent = cursor->GetDefaultParent ();
		if ( defaultParent ) {
			return defaultParent;
		}
	}
	return MOAISim::Get ().GetActionMgr ().GetDefaultParent ();
}

//----------------------------------------------------------------//
MOAIActionStackMgr::MOAIActionStackMgr () {
}

//----------------------------------------------------------------//
MOAIActionStackMgr::~MOAIActionStackMgr () {
}

//----------------------------------------------------------------//
void MOAIActionStackMgr::Pop () {

	this->mStack.Pop ();
}

//----------------------------------------------------------------//
void MOAIActionStackMgr::Push ( MOAIAction& action ) {

	this->mStack.Push ( &action );
}

//================================================================//
// ruby
//================================================================//

//----------------------------------------------------------------//
/**	@ruby	addChild
	@text	Attaches a child action for updating.

	@in		MOAIAction self
	@in		MOAIAction child
	@opt	boolean defer		Default value is 'false.'
	@out	MOAIAction self
*/
mrb_value MOAIAction::_addChild ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAction, "U" )
	
	MOAIAction* action		= state.GetRubyObject < MOAIAction >( 1, true );
	bool defer				= state.GetParamValue < bool >( 2, false );
	
	if ( action ) {
		action->Attach ( self, defer );
	}

	return context;
}

//----------------------------------------------------------------//
/**	@ruby	attach
	@text	Attaches a child to a parent action. The child will receive
			updates from the parent only if the parent is in the action tree.

	@in		MOAIAction self
	@opt	MOAIAction parent		Default value is nil; same effect as calling detach ().
	@opt	boolean defer			Default value is 'false.'
	@out	MOAIAction self
*/
mrb_value MOAIAction::_attach ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAction, "U" )
	
	MOAIAction* parent		= state.GetRubyObject < MOAIAction >( 1, true );
	bool defer				= state.GetParamValue < bool >( 2, false );
	
	self->Attach ( parent, defer );
	
	return context;
}

//----------------------------------------------------------------//
/**	@ruby	clear
	@text	Removes all child actions.

	@in		MOAIAction self
	@out	MOAIAction self
*/
mrb_value MOAIAction::_clear ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAction, "U" )

	self->ClearChildren ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@ruby	defer
	@text	Defers action's update until the next time the action tree is processed.

	@in		MOAIAction self
	@out	nil
*/
mrb_value MOAIAction::_defer ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAction, "U" )
	
	bool defer = state.GetParamValue < bool >( 1, false );
	self->Defer ( defer );
	return context;
}

//----------------------------------------------------------------//
/**	@ruby	detach
	@text	Detaches an action from its parent (if any) thereby removing
			it from the action tree. Same effect as calling stop ().

	@in		MOAIAction self
	@out	MOAIAction self
*/
mrb_value MOAIAction::_detach ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAction, "U" )
	
	self->Detach ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@ruby	getChildren
	@text	Get action's children (if any).

	@in		MOAIAction self
	@out	...					Child actions (returned as multiple values).
*/
mrb_value MOAIAction::_getChildren ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAction, "U" )
	
	ChildIt childIt = self->mChildren.Head ();

	mrb_value array = mrb_ary_new ( state );

	for ( ; childIt; childIt = childIt->Next () ) {
		mrb_ary_push ( state, array, childIt->Data ()->GetMRBObject () );
	}
	return array;
}

//----------------------------------------------------------------//
/**	@ruby	hasChildren
	@text	Returns 'true; if action has children and the number of
			children.

	@in		MOAIAction self
	@out	boolean hasChildren
	@out	number nChildren
*/
mrb_value MOAIAction::_hasChildren ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAction, "U" )

	size_t count = self->mChildren.Count ();

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( count > 0 );
	ret [ 1 ] = state.ToRValue ( ( u32 )count );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@ruby	isActive
	@text	Checks to see if an action is currently in the action tree.

	@in		MOAIAction self
	@out	boolean isActive
*/
mrb_value MOAIAction::_isActive ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAction, "U" );

	return state.ToRValue ( self->IsActive () );
}

//----------------------------------------------------------------//
/**	@ruby	isBusy
	@text	Checks to see if an action is currently busy. An action is
			'busy' only if it is 'active' and not 'done.'

	@in		MOAIAction self
	@out	boolean isBusy
*/
mrb_value MOAIAction::_isBusy ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAction, "U" );

	return state.ToRValue ( self->IsBusy () );
}

//----------------------------------------------------------------//
/**	@ruby	isDone
	@text	Checks to see if an action is 'done.' Definition of 'done'
			is up to individual action implementations.

	@in		MOAIAction self
	@out	boolean isDone
*/
mrb_value MOAIAction::_isDone ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAction, "U" );

	return state.ToRValue ( self->IsDone () );
}

//----------------------------------------------------------------//
/**	@ruby	isPaused
	@text	Checks to see if an action is 'paused.'
 
	@in		MOAIAction self
	@out	bool isPaused
*/
mrb_value MOAIAction::_isPaused ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAction, "U" );
	
	return state.ToRValue ( self->IsPaused () );
}

//----------------------------------------------------------------//
/**	@ruby	pause
	@text	Leaves the action in the action tree but prevents it from
			receiving updates. Call pause ( false ) or start () to unpause.

	@in		MOAIAction self
	@opt	boolean pause			Default value is 'true.'
	@out	nil
*/
mrb_value MOAIAction::_pause ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAction, "U" );

	self->mActionFlags = state.GetParamValue < bool >( 1, true ) ? self->mActionFlags | FLAGS_IS_PAUSED : self->mActionFlags & ~FLAGS_IS_PAUSED;
	return context;
}

//----------------------------------------------------------------//
/**	@ruby	setAutoStop
	@text	Flag action to automatically stop (and be removed from action tree)
			when no longer busy.

	@in		MOAIAction self
	@out	nil
*/
mrb_value MOAIAction::_setAutoStop ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAction, "U" );
	self->mActionFlags = state.GetParamValue < bool >( 0, false ) ? self->mActionFlags | FLAGS_AUTO_STOP : self->mActionFlags & ~FLAGS_AUTO_STOP;
	return context;
}

//----------------------------------------------------------------//
/**	@ruby	start
	@text	Adds the action to a parent action or the root of the action tree.

	@in		MOAIAction self
	@opt	MOAIAction parent		Default value is MOAIActionMgr.getRoot ()
	@opt	boolean defer			Action will first run during the next sim step, even if it visited during the current sim step. Default value is 'false.'
	@out	MOAIAction self
*/
mrb_value MOAIAction::_start ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAction, "U" )

	MOAIAction* parent		= state.GetRubyObject < MOAIAction >( 1, true );
	bool defer				= state.GetParamValue < bool >( 2, false );
	
	self->Start ( parent, defer );

	return context;
}

//----------------------------------------------------------------//
/**	@ruby	stop
	@text	Removed the action from its parent action; action will
			stop being updated.

	@in		MOAIAction self
	@out	MOAIAction self
*/
mrb_value MOAIAction::_stop ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAction, "U" )

	self->Detach ();
	self->mActionFlags &= ~FLAGS_IS_PAUSED;

	return context;
}

//----------------------------------------------------------------//
/**	@ruby	throttle
	@text	Sets the actions throttle. Throttle is a scalar on time.
			Is is passed to the action's children.
	
	@in		MOAIAction self
	@opt	number throttle	Default value is 1.
	@out	MOAIAction self
*/
mrb_value MOAIAction::_throttle ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAction, "U" )

	self->mThrottle = state.GetParamValue < float >( 1, 1.0f );
	
	return context;
}

//----------------------------------------------------------------//
/**	@ruby	update
	@text	Update action manually. This call will not update child actions.
	
	@in		MOAIAction  self
	@opt	number      step     Default value is sim step
	@out	MOAIAction  self
*/
mrb_value MOAIAction::_update ( mrb_state* M, mrb_value context ) {
    MOAI_RUBY_SETUP ( MOAIAction, "U" )
    
    double step = state.GetParamValue < double >( 1, MOAISim::Get ().GetStep () );
    self->MOAIAction_Update ( step );
    
    return context;
}

//================================================================//
// MOAIAction
//================================================================//

//----------------------------------------------------------------//
void MOAIAction::Attach ( MOAIAction* parent, bool defer ) {

	MOAIAction* oldParent = this->mParent;
	if ( oldParent == parent ) return;

	this->Retain ();

	if ( parent ) {
		parent->RubyRetain ( this );
	}
	
	if ( oldParent ) {
	
		// if we're detaching the action while the parent action is updating
		// and the action we are detaching is *next in line* for an update, then we
		// need to *skip* the action, update the cursor, retain it (if it exists) and
		// release the action we are detaching
		if (( oldParent->mActionFlags & FLAGS_IS_UPDATING ) && ( oldParent->mNextChildIt == &this->mLink )) {
			oldParent->mNextChildIt = oldParent->mNextChildIt->Next (); // skip this link
			if ( oldParent->mNextChildIt ) {
				oldParent->mNextChildIt->Data ()->Retain ();
			}
			this->Release ();
		}
		
		oldParent->mChildren.Remove ( this->mLink );
		oldParent->MOAIAction_DidLoseChild ( this );
		
		// TODO: hmmm...
		this->UnblockSelf ();
		this->UnblockAll ();
		this->mParent = 0;
		
		oldParent->RubyRelease ( this );
	}
	
	if ( oldParent && ( !parent )) {
		this->MOAIAction_Stop ();
	}
	
	if ( parent ) {
	
		this->mParent = parent;
		parent->mChildren.PushBack ( this->mLink );
		
		// if we're attaching the action while while the previous action is updating, then
		// the previous action will *always* be the tail and the *next* action it will therefore
		// be *nil*; in this case we can should update the iterator and do an extra retain
		if (( parent->mActionFlags & FLAGS_IS_UPDATING ) && ( !parent->mNextChildIt )) {
			this->Retain (); // extra retain
			parent->mNextChildIt = &this->mLink; // in case more actions are added
		}
		
		this->ResetPass ( defer ? parent->mPass + 1 : parent->mPass );
	}
	
	if (( !oldParent ) && parent ) {
		if ( !( this->mActionFlags & FLAGS_IS_PAUSED )) {
			this->MOAIAction_Start ();
		}
	}

	this->Release ();
}

//----------------------------------------------------------------//
void MOAIAction::ClearChildren () {

	while ( ChildIt actionIt = this->mChildren.Head ()) {
		actionIt->Data ()->Detach ();
	}
}

//----------------------------------------------------------------//
void MOAIAction::Defer ( bool defer ) {

	if ( this->mParent ) {
		this->mPass = defer ? this->mParent->mPass + 1 : this->mParent->mPass;
	}
}

//----------------------------------------------------------------//
void MOAIAction::Detach () {

	this->Attach ( 0, false );
}

//----------------------------------------------------------------//
MOAIAction* MOAIAction::GetDefaultParent () {

	return this->MOAIAction_GetDefaultParent ();
}

//----------------------------------------------------------------//
bool MOAIAction::IsActive () {

	return ( this->mParent != 0 );
}

//----------------------------------------------------------------//
bool MOAIAction::IsBusy () {

	return ( this->IsActive () && ( !this->IsDone ()));
}

//----------------------------------------------------------------//
bool MOAIAction::IsDone () {

	return this->MOAIAction_IsDone ();
}

//----------------------------------------------------------------//
bool MOAIAction::IsPaused () {

	return ( this->mActionFlags & FLAGS_IS_PAUSED ) || (this->mParent && this->mParent->IsPaused ());
}

//----------------------------------------------------------------//
MOAIAction::MOAIAction () :
	mPass ( 0 ),
	mParent ( 0 ),
	mNextChildIt ( 0 ),
	mThrottle ( 1.0f ),
	mActionFlags ( FLAGS_AUTO_STOP ) {

	this->mLink.Data ( this );

	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
		RTTI_EXTEND ( MOAIInstanceEventSource )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIAction::~MOAIAction () {

	this->ClearChildren ();
}

//----------------------------------------------------------------//
void MOAIAction::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "EVENT_ACTION_PRE_UPDATE", state.ToRValue ( ( u32 )EVENT_ACTION_PRE_UPDATE ) );
	state.DefineClassConst ( klass, "EVENT_ACTION_POST_UPDATE", state.ToRValue ( ( u32 )EVENT_ACTION_POST_UPDATE ) );
	state.DefineClassConst ( klass, "EVENT_START", state.ToRValue ( ( u32 )EVENT_START ) );
	state.DefineClassConst ( klass, "EVENT_STOP", state.ToRValue ( ( u32 )EVENT_STOP ) );

}

//----------------------------------------------------------------//
void MOAIAction::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "addChild", _addChild, MRB_ARGS_REQ ( 1 ) | MRB_ARGS_OPT ( 1 ) );
	state.DefineInstanceMethod ( klass, "attach", _attach, MRB_ARGS_OPT ( 2 ) );
	state.DefineInstanceMethod ( klass, "clear", _clear, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "defer", _defer, MRB_ARGS_OPT ( 1 ) );
	state.DefineInstanceMethod ( klass, "detach", _detach, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getChildren", _getChildren, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "hasChildren", _hasChildren, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isActive", _isActive, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isBusy", _isBusy, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isDone", _isDone, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isPaused", _isPaused, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "pause", _pause, MRB_ARGS_OPT ( 1 ) );
	state.DefineInstanceMethod ( klass, "setAutoStop", _setAutoStop, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "start", _start, MRB_ARGS_OPT ( 2 ) );
	state.DefineInstanceMethod ( klass, "stop", _stop, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "throttle", _throttle, MRB_ARGS_OPT ( 1 ) );
	state.DefineInstanceMethod ( klass, "update", _update, MRB_ARGS_OPT ( 1 ) );

}

//----------------------------------------------------------------//
void MOAIAction::ResetPass ( u32 pass ) {

	if ( this->mPass > pass ) {
		ChildIt childIt = this->mChildren.Head ();
		for ( ; childIt; childIt = this->mNextChildIt->Next ()) {
			this->mNextChildIt->Data ()->ResetPass ( pass );
		}
	}
	this->mPass = pass;
}

//----------------------------------------------------------------//
void MOAIAction::Update ( MOAIActionTree& tree, double step ) {

	if ( this->IsPaused () || this->IsBlocked () ) return;

	MOAIActionStackMgr::Get ().Push ( *this );

	double t0 = 0.0;
	bool profilingEnabled = false;

	profilingEnabled = tree.GetProfilingEnabled ();
	if ( profilingEnabled ) {
		t0 = ZLDeviceTime::GetTimeInSeconds ();
	}
	
	this->mPass = this->mParent ? this->mParent->mPass : this->mPass + 1;
	
	// handles the case when Moai has been running continuously for approx. 136 years at 60 fps
	if ( this->mPass == 0xffffffff ) {
		this->ResetPass ();
	}
	
	step *= this->mThrottle;
	
	this->mActionFlags |= FLAGS_IS_UPDATING;
	
	this->InvokeListener ( EVENT_ACTION_PRE_UPDATE );
	this->MOAIAction_Update ( step );
	this->InvokeListener ( EVENT_ACTION_POST_UPDATE );

	if ( profilingEnabled ) {
		double elapsed = ZLDeviceTime::GetTimeInSeconds () - t0;
		if ( elapsed >= 0.005 ) {
			STLString debugInfo = this->MOAIAction_GetDebugInfo ();
			MOAILogF ( 0, ZLLog::LOG_STATUS, MOAISTRING_MOAIAction_Profile_PSFF, this, this->TypeName (), debugInfo.c_str(), step * 1000, elapsed * 1000 );
		}
	}
	
	// the trick below is to alway retain the current child plus the
	// *next* child in the list. each child is processed once and 
	// released after processing, so all the children should be 
	// retain/release'd exactly once.
	
	// we retain the head child in the list (if any)
	// here because the first child retained inside the loop (below)
	// is the *second* child in the list
	this->mNextChildIt = this->mChildren.Head ();
	if ( this->mNextChildIt ) {
		this->mNextChildIt->Data ()->Retain ();
	}
	
	MOAIAction* child = 0;
	while ( this->mNextChildIt ) {
		
		child = this->mNextChildIt->Data ();
		
		// retain the *next* child in the list (if any)
		this->mNextChildIt = this->mNextChildIt->Next ();
		if ( this->mNextChildIt ) {
			this->mNextChildIt->Data ()->Retain ();
		}
		
		if ( child->mParent && ( child->mPass <= this->mPass )) {
			child->Update ( tree, step );
		}
		
		// release the *current* child
		child->Release ();
	}
	
	this->mActionFlags &= ~FLAGS_IS_UPDATING;
	this->mNextChildIt = 0;
	
	if ( this->IsDone ()) {
		this->Detach ();
	}
	
	MOAIActionStackMgr::Get ().Pop ();
}

//----------------------------------------------------------------//
void MOAIAction::Start ( MOAIAction* parent, bool defer ) {

	parent = parent ? parent : MOAIActionStackMgr::Get ().GetDefaultParent ();
	this->Attach ( parent, defer );
	this->mActionFlags &= ~FLAGS_IS_PAUSED;
}

//----------------------------------------------------------------//
void MOAIAction::Stop () {

	this->Detach ();
	this->mActionFlags &= ~FLAGS_IS_PAUSED;
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAIAction::MOAIAction_DidLoseChild ( MOAIAction* child ) {
	UNUSED ( child );
}

//----------------------------------------------------------------//
STLString MOAIAction::MOAIAction_GetDebugInfo () const {
	return TypeName();
}

//----------------------------------------------------------------//
MOAIAction* MOAIAction::MOAIAction_GetDefaultParent () {

	return 0;
}

//----------------------------------------------------------------//
bool MOAIAction::MOAIAction_IsDone () {

	return (( this->mActionFlags & FLAGS_AUTO_STOP ) && ( this->mChildren.Count () == 0 ));
}

//----------------------------------------------------------------//
void MOAIAction::MOAIAction_Start () {

	this->InvokeListener ( EVENT_START );
}

//----------------------------------------------------------------//
void MOAIAction::MOAIAction_Stop () {

	this->InvokeListener ( EVENT_STOP );
}

//----------------------------------------------------------------//
void MOAIAction::MOAIAction_Update ( double step ) {
	UNUSED ( step );
}

//----------------------------------------------------------------//
void MOAIAction::MOAIBlocker_Unblock () {

	// TODO: does this make sense?
	this->mPass = 0;
}
