// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-core/MOAIRubyObject.h>
#include <moai-core/MOAIRubyClass.h>
#include <moai-core/MOAIRubyRef.h>
#include <moai-core/MOAIRubyRuntime.h>

//----------------------------------------------------------------//
typedef std::weak_ptr < RObject >	weakref;
typedef std::shared_ptr < RObject > strongref;

void strongref_free ( mrb_state* M, void* ptr ) {

	reinterpret_cast< strongref* >( ptr )->~strongref ();
	mrb_free ( M, ptr );
}

mrb_data_type const strongref_type = { "StrongRef", &strongref_free };

//================================================================//
// NullDeleter<T>
//================================================================//
template<class T>
struct NullDeleter {
	void operator()( T* p ) const {}
};

//================================================================//
// MOAIRubyRef
//================================================================//

//----------------------------------------------------------------//
void MOAIRubyRef::Clear () {

	if ( this->mOwnsRef ) {

		if ( mRefType == STRONG ) {

			if ( !this->IsNil () ) {
				
				if ( MOAIRubyRuntime::IsValid () ) {
					mrb_gc_unregister ( MOAIRubyRuntime::Get ().GetMainState (), mrb_obj_value ( this->mStrongPtr.get () ) );
					MOAIRubyRuntime::Get ().Unref ( mrb_obj_value ( this->mStrongPtr.get () ) );
				}
			}

			/*if ( this->mStrongPtr ) {
				this->mStrongPtr.reset ();
			}*/
		}
		else {
			
			/*if ( !this->mWeakPtr.expired () ) {
				this->mWeakPtr.reset ();
			}*/
		}
	}

	this->mOwnsRef = false;
	this->mRefType = MOAIRubyRef::NONE;
}

//----------------------------------------------------------------//
bool MOAIRubyRef::Empty() {

	if ( this->mRefType == MOAIRubyRef::NONE ) {
		return true;
	}
	else if ( this->mRefType == MOAIRubyRef::STRONG ) {
		//return mrb_nil_p ( mrb_obj_value ( this->mStrongPtr.get () ) );
		return this->mStrongPtr == 0;
	}
	else {
		return this->mWeakPtr.expired ();
	}
}

//----------------------------------------------------------------//
mrb_value MOAIRubyRef::GetRef () {

	if ( this->mRefType == MOAIRubyRef::NONE ) {
		ZLLog_DebugF ( ZLLog::CONSOLE, "moai-lib-core::MOAIRubyRef::GetRef() NONE!\n" );
		return mrb_nil_value ();
	}
	else if ( this->mRefType == MOAIRubyRef::STRONG ) {
		//ZLLog_DebugF ( ZLLog::CONSOLE, "moai-lib-core::MOAIRubyRef::GetRef() STRONG %p!\n", this->mStrongPtr );
		return this->mStrongPtr != 0 ? mrb_obj_value ( this->mStrongPtr.get () ) : mrb_nil_value ();
	}
	else {
		//ZLLog_DebugF ( ZLLog::CONSOLE, "moai-lib-core::MOAIRubyRef::GetRef() WEAK %p!\n", this->mWeakPtr );
		return !this->mWeakPtr.expired () ? mrb_obj_value ( this->mWeakPtr.lock ().get () ) : mrb_nil_value ();
	}
}

//----------------------------------------------------------------//
bool MOAIRubyRef::IsNil () {

	if ( this->mRefType == MOAIRubyRef::NONE ) {
		return true;
	}
	else if ( this->mRefType == MOAIRubyRef::STRONG ) {

		if ( this->mStrongPtr == 0 ) {
			return true;
		}

		return mrb_nil_p ( mrb_obj_value ( this->mStrongPtr.get () ) );
	}
	else {
		
		if ( this->mWeakPtr.expired () ) {
			return true;
		}

		if ( this->mWeakPtr.lock () == 0 ) {
			return true;
		}

		return mrb_nil_p ( mrb_obj_value ( this->mWeakPtr.lock ().get () ) );
	}
}

//----------------------------------------------------------------//
void MOAIRubyRef::MakeStrong () {

	//this->mRef = MOAIRubyRuntime::Get ().MakeStrong ( this->mRefID );
}

//----------------------------------------------------------------//
void MOAIRubyRef::MakeWeak () {

	//this->mRef = MOAIRubyRuntime::Get ().MakeWeak ( this->mRefID );
}

//----------------------------------------------------------------//
MOAIRubyRef::MOAIRubyRef () :
	mOwnsRef ( false ),
	mRefType ( NONE ) {
}

//----------------------------------------------------------------//
MOAIRubyRef::MOAIRubyRef ( const MOAIRubyRef& assign ) :
	mOwnsRef ( false ) {
	this->Take ( assign );
}

//----------------------------------------------------------------//
MOAIRubyRef::~MOAIRubyRef () {
	
	if ( MOAIRubyRuntime::IsValid ()) {
		this->Clear ();
	}
}

//----------------------------------------------------------------//
void MOAIRubyRef::SetRef ( MOAIRubyObject* object, u32 type ) {

	//MOAIScopedRubyState state = MOAIRubyRuntime::Get ().State ();
	//state.Push ( object );
	this->SetRef ( object->GetMRBObject (), MAKE_WEAK );
	//state.Pop ( 1 );
}

//----------------------------------------------------------------//
void MOAIRubyRef::SetRef ( mrb_value value ) {
	UNUSED ( value );
	assert ( false );
}

//----------------------------------------------------------------//
void MOAIRubyRef::SetRef ( mrb_value value, u32 type ) {

	MOAIRubyState& state = MOAIRubyRuntime::Get ().GetMainState ();

	if ( state.IsNil ( value ) ) {
		//ZLLog_ErrorF ( ZLLog::CONSOLE, "MOAIRubyRef::SetRef() Try to set nil ref. \n" );
		this->Clear ();
		return;
	}

	this->Clear ();

	if ( type == MAKE_STRONG ) {

		mrb_gc_register ( state, value );
		MOAIRubyRuntime::Get ().Ref ( value );
		this->mStrongPtr = std::shared_ptr < RObject >( mrb_obj_ptr ( value ), NullDeleter < RObject >() );

		//ZLLog_DebugF ( ZLLog::CONSOLE, "MOAIRubyRef::SetRef() MAKE_STRONG %p\n", this->mStrongPtr.get () );
	}
	else {

		strongref* ref = 0;

		mrb_sym const sym = mrb_intern_cstr ( state, "@__strongref" );

		if ( mrb_iv_defined ( state, value, sym ) ) {
			ref = reinterpret_cast< strongref* > ( mrb_data_check_get_ptr ( state, mrb_iv_get ( state, value, sym ), &strongref_type ) );
		}
		else {
			ref = new ( mrb_malloc ( state, sizeof ( strongref ) ) )
				strongref ( mrb_obj_ptr ( value ), NullDeleter < RObject >() );

			//ref = new strongref ( mrb_obj_ptr ( value ), NullDeleter < RObject >() );

			mrb_iv_set ( state, value, sym, mrb_obj_value (
				mrb_data_object_alloc ( state, state->object_class, ref, &strongref_type ) ) );
		}

		this->mWeakPtr = *ref;
	}
	this->mRefType = type == MAKE_STRONG ? STRONG : WEAK;
	this->mOwnsRef = true;

	//ZLLog_DebugF ( ZLLog::CONSOLE, "moai-lib-core::MOAIRubyRef::SetRef() %d ok!\n", this->Empty () );
}

//----------------------------------------------------------------//
void MOAIRubyRef::Take ( const MOAIRubyRef& assign ) {

	this->Clear ();

	this->mRefType = assign.mRefType;
	this->mStrongPtr = assign.mStrongPtr;
	this->mWeakPtr = assign.mWeakPtr;

	// cast the const away
	(( MOAIRubyRef& )assign ).mOwnsRef = false;
}

//================================================================//
// MOAIRubyStrongRef
//================================================================//

//----------------------------------------------------------------//
MOAIRubyStrongRef::MOAIRubyStrongRef () {
}

//----------------------------------------------------------------//
MOAIRubyStrongRef::MOAIRubyStrongRef ( MOAIRubyObject* object ) {
	this->SetRef ( object );
}

//----------------------------------------------------------------//
MOAIRubyStrongRef::MOAIRubyStrongRef ( mrb_value value ) {
	this->SetRef ( value );
}

//----------------------------------------------------------------//
void MOAIRubyStrongRef::SetRef ( MOAIRubyObject* object ) {
	MOAIRubyRef::SetRef ( object, MAKE_STRONG );
}

//----------------------------------------------------------------//
void MOAIRubyStrongRef::SetRef ( mrb_value value ) {
	MOAIRubyRef::SetRef ( value, MAKE_STRONG );
}

//================================================================//
// MOAIRubyWeakRef
//================================================================//

//----------------------------------------------------------------//
MOAIRubyWeakRef::MOAIRubyWeakRef () {
}

//----------------------------------------------------------------//
MOAIRubyWeakRef::MOAIRubyWeakRef ( MOAIRubyObject* object ) {
	this->SetRef ( object );
}

//----------------------------------------------------------------//
MOAIRubyWeakRef::MOAIRubyWeakRef ( mrb_value value ) {
	this->SetRef ( value );
}

//----------------------------------------------------------------//
void MOAIRubyWeakRef::SetRef ( MOAIRubyObject* object ) {
	MOAIRubyRef::SetRef ( object, MAKE_WEAK );
}

//----------------------------------------------------------------//
void MOAIRubyWeakRef::SetRef ( mrb_value value ) {
	MOAIRubyRef::SetRef ( value, MAKE_WEAK );
}

//================================================================//
// MOAIRubyMemberRef
//================================================================//

//----------------------------------------------------------------//
void MOAIRubyMemberRef::Clear () {

	/*if ( this->mRefID != 0 ) {

		if ( this->mOwner->IsBound () && MOAIRubyRuntime::IsValid ()) {
			MOAIScopedRubyState state = MOAIRubyRuntime::Get ().State ();
			if ( this->mOwner->PushRefTable ( state )) {
				luaL_unref ( state, -1, this->mRefID );
			}
		}
		this->mRefID = LUA_NOREF;
		this->mOwner = 0;
	}*/
}

//----------------------------------------------------------------//
MOAIRubyMemberRef::MOAIRubyMemberRef () :
	mRefID ( 0 ),
	mOwner ( 0 ) {
}

//----------------------------------------------------------------//
MOAIRubyMemberRef::~MOAIRubyMemberRef () {
	this->Clear ();
}

//----------------------------------------------------------------//
bool MOAIRubyMemberRef::PushRef ( MOAIRubyState& state ) {

	/*if ( this->mRefID != 0 ) {

		bool isNil = true;

		if ( this->mOwner->IsBound ()) {
			if ( this->mOwner->PushRefTable ( state )) {
				lua_rawgeti ( state, -1, this->mRefID );
				lua_replace ( state, -2 );
			}
			isNil = state.IsNilOrNone ( -1 );
		}

		if ( isNil ) {
			this->mRefID = LUA_NOREF;
			this->mOwner = 0;
			return false;
		}
		return true;
	}
	lua_pushnil ( state );*/
	return false;
}

//----------------------------------------------------------------//
void MOAIRubyMemberRef::SetRef ( MOAIRubyObject& owner, MOAIRubyState& state, int idx ) {

	/*this->Clear ();

	if ( state.IsNilOrNone ( idx ) == false ) {

		idx = state.AbsIndex ( idx );

		this->mOwner = &owner;
		this->mOwner->PushRefTable ( state );

		lua_pushvalue ( state, idx );
		this->mRefID = luaL_ref ( state, -2 );

		lua_pop ( state, 1 );
	}*/
}
