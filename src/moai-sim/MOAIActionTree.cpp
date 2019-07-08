// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIAction.h>
#include <moai-sim/MOAIActionTree.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getRoot
	@text	Returns the current root action.

	@out	MOAIAction root
*/
mrb_value MOAIActionTree::_getRoot ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIActionTree, "U" )
	
	MOAIAction* root = self->AffirmRoot ();
	return root->GetMRBObject ();
}

//----------------------------------------------------------------//
/**	@lua	setProfilingEnabled
	@text	Enables action profiling.

	@opt	boolean enable		Default value is false.
	@out	nil
*/
mrb_value MOAIActionTree::_setProfilingEnabled ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIActionTree, "U" )
	
	bool enable = state.GetParamValue < bool >( 1, false );
	self->SetProfilingEnabled ( enable );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setRoot
	@text	Replaces or clears the root action.

	@opt	MOAIAction root		Default value is nil.
	@out	nil
*/
mrb_value MOAIActionTree::_setRoot ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIActionTree, "U" )
	
	MOAIAction* root = state.GetRubyObject < MOAIAction >( 1, false );
	self->SetRoot ( root );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setThreadInfoEnabled
	@text	Enables function name and line number info for MOAICoroutine.

	@opt	boolean enable		Default value is false.
	@out	nil
*/
mrb_value MOAIActionTree::_setThreadInfoEnabled ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIActionTree, "U" )

	bool enable = state.GetParamValue < bool >( 1, false );
	self->SetThreadInfoEnabled ( enable );

	return context;
}

//================================================================//
// MOAIActionTree
//================================================================//

//----------------------------------------------------------------//
MOAIAction* MOAIActionTree::AffirmRoot () {

	if ( !this->mRoot ) {
		//this->SetRoot ( new MOAIAction ());

		MOAIRubyState& state = MOAIRubyRuntime::Get ().State();
		this->SetRoot ( state.CreateClassInstance < MOAIAction >() );
	}
	return this->mRoot;
}

//----------------------------------------------------------------//
MOAIAction* MOAIActionTree::GetDefaultParent () {

	return this->AffirmRoot ();
}

//----------------------------------------------------------------//
MOAIActionTree::MOAIActionTree () :
	mRoot ( 0 ),
	mProfilingEnabled ( false ),
	mThreadInfoEnabled ( false ) {
	
	RTTI_SINGLE ( MOAIAction )
}

//----------------------------------------------------------------//
MOAIActionTree::~MOAIActionTree () {
}

//----------------------------------------------------------------//
void MOAIActionTree::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
}

//----------------------------------------------------------------//
void MOAIActionTree::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "getRoot", _getRoot, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setProfilingEnabled", _setProfilingEnabled, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setRoot", _setRoot, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setThreadInfoEnabled", _setThreadInfoEnabled, MRB_ARGS_ARG ( 0, 1 ) );
}

//----------------------------------------------------------------//
void MOAIActionTree::SetRoot ( MOAIAction* root ) {

	if ( this->mRoot ) {
		this->mRoot->Detach ();
	}

	this->mRoot = root;

	if ( this->mRoot ) {
		this->mRoot->Attach ( this, false );
	}
}

//----------------------------------------------------------------//
void MOAIActionTree::Update ( double step ) {

	this->MOAIAction::Update ( *this, step );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAIActionTree::MOAIAction_DidLoseChild ( MOAIAction* child ) {
	if ( this->mRoot == child ) {
		this->mRoot = 0;
	}
}

//----------------------------------------------------------------//
bool MOAIActionTree::MOAIAction_IsDone () {
	return false;
}

//----------------------------------------------------------------//
void MOAIActionTree::MOAIAction_Update ( double step ) {
	UNUSED ( step );
}
