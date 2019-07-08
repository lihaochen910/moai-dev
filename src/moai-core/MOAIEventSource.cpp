// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <moai-core/MOAIEventSource.h>
#include <moai-core/MOAIRuby.h>

//================================================================//
// MOAIEventSource
//================================================================//

//----------------------------------------------------------------//
MOAIEventSource::MOAIEventSource () {

	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIEventSource::~MOAIEventSource () {
}

//================================================================//
// MOAIInstanceEventSource ruby
//================================================================//

//----------------------------------------------------------------//
/**	@ruby	getListener
	@text	Gets the listener callback for a given event ID.

	@in		MOAIInstanceEventSource self
	@in		number eventID				The ID of the event.
	@out	function listener			The listener callback.
*/
mrb_value MOAIInstanceEventSource::_getListener ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIInstanceEventSource, "N" );

	return mrb_hash_get ( M, self->mListenerTable, state.GetParamValue ( 1 ) );
}

//----------------------------------------------------------------//
/**	@ruby	setListener
	@text	Sets a listener callback for a given event ID. It is up
			to individual classes to declare their event IDs.

	@in		MOAIInstanceEventSource self
	@in		number eventID				The ID of the event.
	@opt	function callback			The callback to be called when the object emits the event. Default value is nil.
	@out	nil
*/
mrb_value MOAIInstanceEventSource::_setListener ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIInstanceEventSource, "NP" );

	self->SetListener ( state, state.GetParamValue < u32 > ( 1, 0 ), state.GetParamValue ( 2 ) );

	return context;
}

//================================================================//
// MOAIInstanceEventSource
//================================================================//

//----------------------------------------------------------------//
void MOAIInstanceEventSource::AffirmListenerTable ( MOAIRubyState& state ) {

	if ( !this->mListenerTable ) {
		mrb_value hash = mrb_hash_new ( state );
		mrb_value self = MOAIRubyObject::GetMRBObject ();
		//ZLLog_DebugF ( ZLLog::CONSOLE, "moai-lib-core::MOAIRubyObject::GetMRBObject() ok %p\n", mrb_ptr ( self ) );
		state.SetInstanceField ( self, LISTENER_TABLE, hash, false );
		this->mListenerTable.SetRef ( hash );
	}

	//ZLLog_DebugF ( ZLLog::CONSOLE, "moai-lib-core::MOAIInstanceEventSource::AffirmListenerTable() ok!\n" );
}

//----------------------------------------------------------------//
mrb_value MOAIInstanceEventSource::GetListener ( mrb_state* M, u32 eventID ) {

	MOAIRubyState state ( M );

	this->AffirmListenerTable ( state );

	return mrb_hash_get ( M, this->mListenerTable, state.ToRValue ( eventID ) );
}

//----------------------------------------------------------------//
void MOAIInstanceEventSource::InvokeListener ( u32 eventID, mrb_int argc, const mrb_value* args ) {

	//ZLLog_DebugF ( ZLLog::CONSOLE, "moai-lib-core::%s::InvokeListener() start\n", this->TypeName () );

	if ( MOAIRubyRuntime::IsValid () ) {
		MOAIRubyState state = MOAIRubyRuntime::Get ().State ();

		if ( !state.IsNil ( this->GetListener ( state, eventID ) ) ) {
			state.FuncCall ( this->GetListener ( state, eventID ), "call", argc, args );
		}
	}
}

//----------------------------------------------------------------//
void MOAIInstanceEventSource::InvokeListenerWithSelf ( u32 eventID ) {

	/*if ( MOAIRubyRuntime::IsValid () ) {
		MOAIScopedRubyState state = MOAIRubyRuntime::Get ().State ();
		if ( this->PushListenerAndSelf ( eventID, state )) {
			state.DebugCall ( 1, 0 );
		}
	}*/
}

//----------------------------------------------------------------//
MOAIInstanceEventSource::MOAIInstanceEventSource () {

	RTTI_BEGIN
		RTTI_EXTEND ( MOAIEventSource )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIInstanceEventSource::~MOAIInstanceEventSource () {
}

//----------------------------------------------------------------//
void MOAIInstanceEventSource::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "getListener", _getListener, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setListener", _setListener, MRB_ARGS_REQ ( 2 ) );

}

//----------------------------------------------------------------//
void MOAIInstanceEventSource::SetListener ( mrb_state* M, u32 eventID, mrb_value handler ) {

	MOAIRubyState state ( M );

	this->AffirmListenerTable ( state );

	mrb_hash_set ( M, this->mListenerTable, state.ToRValue ( eventID ), handler );
}

//================================================================//
// MOAIGlobalEventSource
//================================================================//

//----------------------------------------------------------------//
void MOAIGlobalEventSource::AffirmListenerTable ( MOAIRubyState& state ) {

	if ( !this->mListenerTable ) {
		mrb_value hash = mrb_hash_new ( state );
		state.SetInstanceField ( MOAIRubyObject::GetMRBObject (), LISTENER_TABLE, hash );
		this->mListenerTable.SetRef ( hash );
	}

	//ZLLog_DebugF ( ZLLog::CONSOLE, "moai-lib-core::MOAIGlobalEventSource::AffirmListenerTable() ok!\n" );
}

mrb_value MOAIGlobalEventSource::GetListener ( mrb_state* M, u32 eventID ) {

	MOAIRubyState state ( M );

	this->AffirmListenerTable ( state );

	return mrb_hash_get ( M, this->mListenerTable, state.ToRValue ( eventID ) );
}

//----------------------------------------------------------------//
void MOAIGlobalEventSource::InvokeListener ( u32 eventID, mrb_int argc, const mrb_value* args ) {

	//ZLLog_DebugF ( ZLLog::CONSOLE, "moai-lib-core::MOAIGlobalEventSource::InvokeListener() start\n" );

	if ( MOAIRubyRuntime::IsValid () ) {
		MOAIRubyState state = MOAIRubyRuntime::Get ().State ();

		if ( !state.IsNil ( this->GetListener ( state, eventID ) ) ) {
			//ZLLog_DebugF ( ZLLog::CONSOLE, "moai-lib-core::MOAIGlobalEventSource::FuncCall() start\n" );
			state.FuncCall ( this->GetListener ( state, eventID ), "call", argc, args );
		}
	}
}

//----------------------------------------------------------------//
MOAIGlobalEventSource::MOAIGlobalEventSource () {

	RTTI_BEGIN
		RTTI_EXTEND ( MOAIEventSource )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIGlobalEventSource::~MOAIGlobalEventSource () {
}

//----------------------------------------------------------------//
void MOAIGlobalEventSource::SetListener ( mrb_state* M, u32 eventID, mrb_value handler ) {

	MOAIRubyState state ( M );

	this->AffirmListenerTable ( state );

	mrb_hash_set ( M, this->mListenerTable, state.ToRValue ( eventID ), handler );
}
