// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAICoroutine.h>
#include <moai-sim/MOAISim.h>

//================================================================//
// ruby
//================================================================//

//----------------------------------------------------------------//
/**	@lua	blockOnAction
	@text	Skip updating current thread until the specified action is
			no longer busy. A little more efficient than spinlocking from
			Ruby.

	@in		MOAIAction blocker
	@out	nil
*/
mrb_value MOAICoroutine::_blockOnAction ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	if ( !state.CheckParams ( 1, "U" )) return context;

	MOAIAction* current = MOAIActionStackMgr::Get ().GetCurrent ();
	if ( !current ) return context;
	
	MOAIAction* blocker = state.GetRubyObject < MOAIAction >( 1, true );
	if ( !blocker || !blocker->IsBusy () ) return context;
	
	current->SetBlocker ( blocker );
	
	//return lua_yield ( state, 0 );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	currentThread
	@text	Returns the currently running thread (if any).
	
	@out	MOAICoroutine currentThread	Current thread or nil.
*/
mrb_value MOAICoroutine::_currentThread ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	MOAIAction* current = MOAIActionStackMgr::Get ().GetCurrent ();
	if ( !current ) return context;
	
	return current->GetMRBObject ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAICoroutine::_getHistogram ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICoroutine, "U" )
	
	MOAIRubyRuntime::Get ().PushHistogram ( state, self->mTrackingGroup );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAICoroutine::_getTrackingGroup ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICoroutine, "U" )
	
	return state.ToRValue ( self->mTrackingGroup.c_str () );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAICoroutine::_reportHistogram ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICoroutine, "U" )

	cc8* filename = state.GetParamValue < cc8* >( 1, 0 );
	MOAIRubyRuntime::Get ().ReportHistogram ( filename, self->mTrackingGroup );
	
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAICoroutine::_reportLeaks ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICoroutine, "U" )
	
	cc8* filename = state.GetParamValue < cc8* >( 1, 0 );
	MOAIRubyRuntime::Get ().ReportLeaksFormatted ( filename, self->mTrackingGroup );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	run
	@text	Starts a thread with a function and passes parameters to it.
	
	@in		MOAICoroutine self
	@in		function threadFunc
	@in		... parameters
	@out	nil
*/
mrb_value MOAICoroutine::_run ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICoroutine, "U" )

	if ( !MOAISim::IsValid () ) return context;

	bool defer = false;
	int baseParam = 1;
	if ( state.ParamIsType ( baseParam, MRB_TT_TRUE ) || state.ParamIsType ( baseParam, MRB_TT_FALSE ) ) {
		defer = state.GetParamValue < bool >( baseParam++, false );
	}

	if ( MOAISim::Get ().GetActionMgr ().GetThreadInfoEnabled () ) {

		// Get a copy of the function's debug info and store it so we can
		// refer to it in any debugging info regarding this thread.
		/*lua_Debug ar;
		lua_pushvalue ( state, baseParam );
		lua_getinfo ( state, ">Snl", &ar );

		bool isC = strcmp ( ar.what, "C" ) == 0;

		if ( !ar.what ) {
			ar.what = "??";
		}

		if ( !ar.source ) {
			if ( isC ) {
				ar.source = "@?";
			}
			else {
				ar.source = "@<string>";
			}
		}

		self->mFuncName.clear ();
		if ( ar.name ) {
			self->mFuncName.write ( "%s:%s%s:%d", ar.what, ar.name, ar.source, ar.linedefined );
		}
		else {
			self->mFuncName.write ( "%s:%s:%d", ar.what, ar.source, ar.linedefined );
		}*/
	}

	mrb_value fiber = state.GetParamValue ( 1 );

	//self->mNarg = lua_gettop ( state ) - baseParam;
	self->mRef.SetRef ( fiber );

	//lua_xmove ( state, self->mState, self->mNarg + 1 );
	
	self->Start ( 0, defer );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setDefaultParent
	@text	This coroutine will be used as the default parent for
			any actions launched from within this coroutine.

	@in		MOAICoroutine coroutine
	@out	nil
*/
mrb_value MOAICoroutine::_setDefaultParent ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICoroutine, "U" );
	
	self->mIsDefaultParent = state.GetParamValue < bool >( 1, true );
	
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAICoroutine::_setTrackingGroup ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICoroutine, "U" );
	
	self->mTrackingGroup = state.GetParamValue < cc8* >( 1, "" );
	
	if ( MOAIActionStackMgr::Get ().GetCurrent () == self ) {
		MOAIRubyRuntime::Get ().SetTrackingGroup ( self->mTrackingGroup );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	step
	@text	Resume the coroutine (run until the next call to coroutine.yield ()
			or until the coroutine's main method returns).

	@in		MOAICoroutine coroutine
	@out	nil
*/
mrb_value MOAICoroutine::_step ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICoroutine, "U" )

	/*self->mNarg = lua_gettop ( state ) - 1;
	if ( self->mNarg > 0 ) {
		lua_xmove ( state, self->mState, self->mNarg );
	}*/

	return self->Resume ( 0.0f );
}

//================================================================//
// MOAICoroutine
//================================================================//

//----------------------------------------------------------------//
MOAIAction* MOAICoroutine::GetDefaultParent () {

	return this->mIsDefaultParent ? this : 0;
}

//----------------------------------------------------------------//
MOAICoroutine::MOAICoroutine () :
	mNarg ( 0 ),
	mIsUpdating ( false ),
	mIsActive ( false ),
	mIsDefaultParent ( false ) {

	RTTI_SINGLE ( MOAIAction )
	
	this->mTrackingGroup.write ( "MOAICoroutine: 0x%p", this );
}

//----------------------------------------------------------------//
MOAICoroutine::~MOAICoroutine () {
}

//----------------------------------------------------------------//
void MOAICoroutine::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineStaticMethod ( klass, "blockOnAction", _blockOnAction, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "currentThread", _currentThread, MRB_ARGS_NONE () );

}

//----------------------------------------------------------------//
void MOAICoroutine::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "getHistogram", _getHistogram, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getTrackingGroup", _getTrackingGroup, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "reportHistogram", _reportHistogram, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "reportLeaks", _reportLeaks, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "run", _run, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setDefaultParent", _setDefaultParent, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setTrackingGroup", _setTrackingGroup, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "step", _step, MRB_ARGS_NONE () );

	// ?
	
	/*lua_getglobal ( state, "coroutine" );

	lua_getfield ( state, -1, "create" );
	lua_setfield ( state, -3, "create" );

	lua_getfield ( state, -1, "resume" );
	lua_setfield ( state, -3, "resume" );

	lua_pop ( state, 1 );*/
}

//----------------------------------------------------------------//
mrb_value MOAICoroutine::Resume ( float step ) {
	UNUSED ( step );

	MOAIRubyState& state = MOAIRubyRuntime::Get ().GetMainState ();

	mrb_value returnVal = mrb_nil_value ();
	
	MOAIRubyRuntime::Get ().SetTrackingGroup ( this->mTrackingGroup );
	
	//if ( state.ToCValue < bool >( mrb_fiber_alive_p ( state, this->mRef ) ) ) {
	if ( this->mRef ) {
		
		/*int result;

		int narg = this->mNarg;
		this->mNarg = 0;

		if ( narg == 0 ) {
			lua_pushnumber ( this->mState, step );
			narg = 1;
		}*/
		
		MOAIActionStackMgr& coroutineMgr = MOAIActionStackMgr::Get ();
		coroutineMgr.Push ( *this );
		
		/*#if LUA_VERSION_NUM < 502
			result = lua_resume ( this->mState, narg );
		#else
			result = lua_resume ( this->mState, NULL, narg );
		#endif*/

		//returnVal = state.FiberResume ( this->mRef );
		returnVal = state.FuncCall ( this->mRef, "call" );

		coroutineMgr.Pop ();
		
		//if (( result != LUA_YIELD )) {
		//
		//	if ( result != 0 ) {

		//		cc8* msg = lua_tostring ( this->mState, -1 );
		//		MOAIRubyState state ( this->mState );

		//		#if ( MOAI_WITH_LUAJIT )
		//		
		//			// luajit has assertions on lua_call if the thread has crashed due to runtime error
		//			// this means we can't run our custom stacktrace using this state. we will just print
		//			// the debug stack trace and bail
		//			if ( msg ) {
		//				ZLLog_ErrorF ( ZLLog::CONSOLE, "%s\n", msg );
		//			}
		//			state.LogStackTrace ( ZLLog::LOG_ERROR, ZLLog::CONSOLE, NULL, 0 );
		//		
		//		#else
		//		
		//			// run the custom stack trace
		//			MOAIRubyRuntime::Get ().PushTraceback ( state );
		//			state.Push ( msg );
		//			lua_call ( this->mState, 1, 0 );
		//			lua_pop ( this->mState, 1 );
		//		
		//		#endif
		//	}
		//	this->Stop ();
		//	this->mRef.Clear ();
		//}
		//else {
		//	returnCount = lua_gettop ( this->mState );
		//}
	}
	
	MOAIRubyRuntime::Get ().SetTrackingGroup ();
	
	return returnVal;
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
STLString MOAICoroutine::MOAIAction_GetDebugInfo () const {
	return mFuncName;
}

//----------------------------------------------------------------//
bool MOAICoroutine::MOAIAction_IsDone () {

	return ( this->mRef == false );
}

//----------------------------------------------------------------//
void MOAICoroutine::MOAIAction_Start () {
	MOAIAction::MOAIAction_Start ();
}

//----------------------------------------------------------------//
void MOAICoroutine::MOAIAction_Stop () {
	MOAIAction::MOAIAction_Stop ();
	
	// if we're stopping the thread from outside of its coroutine, clear out the ref
	if ( MOAIActionStackMgr::IsValid () ) {
		if ( MOAIActionStackMgr::Get ().GetCurrent () != this ) {
			this->mRef.Clear ();
		}
	}
}

//----------------------------------------------------------------//
void MOAICoroutine::MOAIAction_Update ( double step ) {
	this->Resume (( float )step );
}
