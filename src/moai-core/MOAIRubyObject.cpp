// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-core/MOAIDeserializer.h>
#include <moai-core/MOAIRuby.h>
#include <moai-core/MOAISerializer.h>
#include <moai-core/MOAIRubyState-impl.h>
#include <moai-core/strings.h>

#define FINALIZE_FUNC_NAME "finalize"
#define MOAI_TAG "moai"

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
mrb_value MOAIRubyObject::_alertNewIsUnsupported ( mrb_state* M, mrb_value context ) {

	//MOAILogF ( M, ZLLog::LOG_ERROR, MOAISTRING_NewIsUnsupported );
	return context;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyObject::_gc ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIRubyObject, "U" )
	
	//edgecase: ignore _gc() called by previous Ruby userdata
	self->mActiveUserdataCount--;
	if ( self->mActiveUserdataCount > 0 ) return context;
	
	if ( MOAIRubyRuntime::IsValid () ) {
		
		/*if ( self->mFinalizer ) {
			self->mFinalizer.PushRef ( state );
			if ( state.IsType ( -1, LUA_TFUNCTION )) {
				state.DebugCall ( 0, 0 );
			}
			else if ( state.IsType ( -1, LUA_TSTRING )) {
				printf ( "%s\n", state.GetValue < cc8* >( -1, "" ));
			}
			else {
				state.Pop ( 1 );
			}
			self->mFinalizer.Clear ();
		}*/
		
		if ( MOAIRubyRuntime::Get ().mReportGC ) {
			printf ( "GC %s <%p>\n", self->TypeName (), self );
		}
		
		self->mUserdata.Clear ();
	}
	
	if ( self->GetRefCount () == 0 ) {
		delete ( self );
	}
	return context;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyObject::_getClassName ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIRubyObject, "U" )

	if ( self ) {
		return state.ToRValue ( self->TypeName () );
	}
	return context;
}

//----------------------------------------------------------------//
//mrb_value MOAIRubyObject::_serializeIn ( mrb_state* M, mrb_value context ) {
//	MOAI_RUBY_SETUP ( MOAIRubyObject, "UT" )
//
//	MOAIDeserializer dummy;
//	self->SerializeIn ( state, dummy );
//	return context;
//}

//----------------------------------------------------------------//
//mrb_value MOAIRubyObject::_serializeOut ( mrb_state* M, mrb_value context ) {
//	MOAI_RUBY_SETUP ( MOAIRubyObject, "UT" )
//	
//	MOAISerializer dummy;
//	self->SerializeOut ( state, dummy );
//	return context;
//}

//----------------------------------------------------------------//
mrb_value MOAIRubyObject::_setFinalizer ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIRubyObject, "U" )

	self->mFinalizer.SetRef ( state.GetParamValue ( 1 ) );
	return context;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyObject::_pin ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIRubyObject, "UU" )

	MOAIRubyObject* object = state.GetRubyObject < MOAIRubyObject >( 1, true );
	object->RubyRetain ( object );
	return context;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyObject::_tostring ( mrb_state* M, mrb_value context ) {

	MOAIRubyState state ( M );

	/*MOAIRubyObject* self = MOAIRubyObject::GetRubyObject ( M, context );
	if ( self ) {

		STLString str;

		lua_getfield ( state, 1, "getClassName" );
		if ( state.IsType ( -1, LUA_TFUNCTION )) {

			lua_pushvalue ( state, 1 );
			state.DebugCall ( 1, 1 );
			cc8* classname = state.GetValue < cc8* >( -1, "" );
			str.write ( "%p <%s>", self, classname );
			state.Push ( str );
			return 1;
		}

		str.write ( "%p <%s>", self, self->TypeName ());
		state.Push ( str );
		return 1;
	}*/
	return context;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyObject::_unpin ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIRubyObject, "UU" )

	MOAIRubyObject* object = state.GetRubyObject < MOAIRubyObject >( 1, true );
	self->RubyRelease ( object );
	return context;
}

//================================================================//
// MOAIRubyObject
//================================================================//

//----------------------------------------------------------------//
// userdata -> memberTable -> refTable -> interfaceTable
// userdata is the object
// memberTable is for ad hoc members added by the scripter
// refTable is for ad hoc mambers added by the engine
// interfaceTable is the 'class' table - methods and constants
// __index looks in membertTable then *skips* refTable to fall back on interfaceTable
void MOAIRubyObject::BindToRuby ( MOAIRubyState& state ) {

	assert ( !this->mUserdata );
	
	if ( MOAIRubyRuntime::IsValid () ) {
		MOAIRubyRuntime::Get ().RegisterObject ( state, *this );
	}
	
	MOAIRubyClass* type = this->GetRubyClass ();
	assert ( type );
	
	// both singletons and instances may be bound to a userdata
	//state.PushPtrUserData ( this ); // userdata
	
	// for now, singletons are just userdata with no add'l metatables
	// so only build the metatable stack if we're *not* a singleton
	if ( !type->IsSingleton () ) {
	
		// hang on to the userdata in case this object was created on the stack
		// shouldn't need to do this for singletons
		MOAIRubyRuntime::Get ().CacheUserdata ( state, -1 );
	
		// instances get the 'full stack' of metatables
		//lua_newtable ( state ); // ref table
		//lua_newtable ( state ); // member table
		//type->PushInterfaceTable ( state ); // interface table
		this->MakeRubyBinding ( state );
	}
	
	// and take a weak ref back to the userdata	
	this->mUserdata.SetRef ( this->GetMRBObject () );
	assert ( this->mUserdata );
	this->mActiveUserdataCount++;
	
	// NOTE: we have to do this *after* mUserdata has been initialized as RubyRetain calls PushRubyUserdata
	// which in turn calls BindToRuby if there is no mUserdata...
	if ( type->IsSingleton () ) {
		this->RubyRetain ( this ); // create a circular reference to 'pin' the userdata
		
		// in the case of a singleton, the ref table is kept in the class record, which is a strong
		// ref (as opposed to the metatable version attached to the weak userdata for regular instance
		// classes).
	}
}

//----------------------------------------------------------------//
mrb_value MOAIRubyObject::GetMRBObject () {
	
	//ZLLog_DebugF ( ZLLog::CONSOLE, "moai-lib-core::%s::GetMRBObject() %p!\n", this->TypeName (), this->mPtr );
	return mrb_obj_value ( this->mPtr );
}

//----------------------------------------------------------------//
MOAIRubyClass* MOAIRubyObject::GetRubyClass () {

	// no implementation
	//assert ( false );
	return &MOAIRubyFactoryClass < MOAIRubyObject >::Get ();
}

//----------------------------------------------------------------//
MOAIRubyClass* MOAIRubyObject::GetSuperRubyClass () {

	return &MOAIRubyBasicClass::Get ();
}

//----------------------------------------------------------------//
void MOAIRubyObject::GetRef ( MOAIRubyRef& ref, u32 refType ) {

	//MOAIScopedRubyState state = MOAIRubyRuntime::Get ().State ();
	MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
	//this->PushRubyUserdata ( state );
	ref.SetRef ( this->GetMRBObject (), refType );
}

//----------------------------------------------------------------//
//int MOAIRubyObject::InjectAndCall ( lua_CFunction func, MOAIRubyObject* self, lua_State* L ) {
//
//	MOAIRubyState state ( L );
//	self->PushRubyUserdata ( state );
//	lua_insert ( L, 1 );
//	return func ( L );
//}

//----------------------------------------------------------------//
bool MOAIRubyObject::IsBound () {

	return ( bool )this->mUserdata;
}

//----------------------------------------------------------------//
bool MOAIRubyObject::IsMoaiUserdata ( MOAIRubyState& state, int idx ) {

	bool result = false;
	/*if ( state.IsType ( idx, LUA_TUSERDATA )) {
		if ( lua_getmetatable ( state, idx )) {
			result = state.HasField ( -1, MOAI_TAG );
			state.Pop ( 1 );
		}
	}*/
	return result;
}

//----------------------------------------------------------------//
bool MOAIRubyObject::IsSingleton () {

	MOAIRubyClass* rubyClass = this->GetRubyClass ();
	return rubyClass->IsSingleton ();
}

//----------------------------------------------------------------//
void MOAIRubyObject::RubyRelease () {

	this->RubyRelease( this );
}

//----------------------------------------------------------------//
void MOAIRubyObject::RubyRelease ( MOAIRubyObject* object ) {

	if ( !object ) return;
	
	if ( this->IsBound () && MOAIRubyRuntime::IsValid ()) {
		MOAIRubyState state = MOAIRubyRuntime::Get ().State ();

		if ( this->mRefTable ) {

			// look up the old count using the object as a key
			mrb_value v = mrb_hash_fetch ( state, this->mRefTable, object->GetMRBObject (), state.ToRValue < u32 >( 0 ));

			// get the count (or nil)
			u32 count = state.ToCValue < u32 >( v );

			if ( count == 0 ) {
				return; // do nothing
			}

			if ( count > 1 ) {
				v = state.ToRValue < u32 >( count - 1 ); // push the new count
			}
			else {
				v = mrb_nil_value ();
			}

			// this should make the object eligible for garbage collection
			mrb_hash_set ( state, this->mRefTable, mrb_obj_value ( object ), v );
		}
	}
	
	// this will take the ref count to zero, but if the object hasn't been collected it *won't* get deleted
	// thanks to the override of ZLRefCountedObject::ZLRefCountedObjectBase_OnRelease ()
	object->Release ();
}

//----------------------------------------------------------------//
void MOAIRubyObject::RubyRetain () {

	this->RubyRetain ( this );
}

//----------------------------------------------------------------//
void MOAIRubyObject::RubyRetain ( MOAIRubyObject* object ) {

	if ( !object ) return;
	object->Retain (); // strong ref

	MOAIRubyState state = MOAIRubyRuntime::Get ().State ();

	if ( this->mRefTable ) {

		// look up the old count using the object as a key
		mrb_value v = mrb_hash_fetch ( state, this->mRefTable, object->GetMRBObject (), state.ToRValue < u32 >( 0 ));

		u32 count = state.ToCValue < u32 >( v );

		v = state.ToRValue < u32 > ( count + 1 );

		// store the new count in the ref table
		mrb_hash_set ( state, this->mRefTable, object->GetMRBObject (), v );
	}
}

//----------------------------------------------------------------//
MOAIRubyObject::MOAIRubyObject () :
	mActiveUserdataCount ( 0 ) {
	RTTI_SINGLE ( RTTIBase )
	
	if ( MOAIRubyRuntime::IsValid () ) {
		MOAIRubyRuntime::Get ().RegisterObject ( *this );
	}
}

//----------------------------------------------------------------//
MOAIRubyObject::~MOAIRubyObject () {

	if ( MOAIRubyRuntime::IsValid () ) {
		
		MOAIRubyRuntime::Get ().DeregisterObject ( *this );

		//printf ( "~MOAIRubyObject() %s %p %p!\n", this->TypeName (), this, this->mPtr );
		
		// TODO: change from both patrick's fork and the community branch; double check
		//if ( this->IsBound () && this->mUserdata ) {
		//	MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
		//	
		//	// clear out the gc
		//	this->mUserdata.PushRef ( state );
		//	
		//	MOAIRubyRuntime::Get ().PurgeUserdata ( state, -1 );
		//	
		//	if ( lua_getmetatable ( state, -1 )) {
		//		lua_pushnil ( state );
		//		lua_setfield ( state, -2, "__gc" );
		//		state.Pop ( 1 );
		//	}
		//				
		//	// and the ref table
		//	lua_pushnil ( state );
		//	lua_setmetatable ( state, -2 );
		//}
	}
}

//----------------------------------------------------------------//
void MOAIRubyObject::MakeRubyBinding ( MOAIRubyState& state ) {

	// stack:
	// -1: interface table
	// -2: member table
	// -3: ref table
	// -4: userdata

	//int top = state.GetTop ();
	//int memberTable = top - 1;
	//int refTable = top - 2;

	//// ref table gets gc and tostring
	//lua_pushcfunction ( state, MOAIRubyObject::_gc );
	//lua_setfield ( state, refTable, "__gc" );
	//
	//lua_pushcfunction ( state, MOAIRubyObject::_tostring );
	//lua_setfield ( state, refTable, "__tostring" );
	//
	//// ref table gets 'moai' tag set to true
	//lua_pushboolean ( state, 1 );
	//lua_setfield ( state, refTable, MOAI_TAG );
	//
	//
	//// member table is __index and __newindex for ref table
	//lua_pushvalue ( state, memberTable );
	//lua_setfield ( state, refTable, "__index" );
	//
	//lua_pushvalue ( state, memberTable );
	//lua_setfield ( state, refTable, "__newindex" );
	//
	//// build the metatable stack
	//lua_setmetatable ( state, -2 ); // interface is meta of member
	//lua_setmetatable ( state, -2 ); // member is meta of ref
	//lua_setmetatable ( state, -2 ); // ref is meta of userdata
}

//----------------------------------------------------------------//
void MOAIRubyObject::PrintTracking () {

	MOAIRubyRuntime::Get ().PrintTracking ( *this );
}

//----------------------------------------------------------------//
mrb_value MOAIRubyObject::PushRubyUserdata ( MOAIRubyState& state ) {
	UNUSED ( state );

	return this->GetMRBObject ();
}

//----------------------------------------------------------------//
void MOAIRubyObject::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIRubyObject::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "gc",			_gc, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getClassName",	_getClassName, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "pin",			_pin, MRB_ARGS_NONE () );
	//state.DefineInstanceMethod ( klass, "serializeIn",	_serializeIn, MRB_ARGS_NONE () );
	//state.DefineInstanceMethod ( klass, "serializeOut",	_serializeOut, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setFinalizer",	_setFinalizer, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "unpin",		_unpin, MRB_ARGS_NONE () );
}

//----------------------------------------------------------------//
void MOAIRubyObject::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAIRubyObject::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAIRubyObject::SetRObjectPtr ( RObject* ptr ) {
	//assert ( this->mPtr == 0 );

	this->mPtr = ptr;

	if ( ptr ) {

		MOAIRubyState& state = MOAIRubyRuntime::Get ().State ();

		state.SetRObjectField ( ptr, "@__ref_table", mrb_hash_new ( state ), true );

		this->mRefTable.SetRef ( state.GetRObjectField ( ptr, "@__ref_table" ));
	}
}

//----------------------------------------------------------------//
cc8* MOAIRubyObject::TypeName () const {
	return "MOAIRubyObject";
}

//================================================================//
// ZLRefCountedObjectBase
//================================================================//

//----------------------------------------------------------------//
void MOAIRubyObject::ZLRefCountedObjectBase_OnRelease ( u32 refCount ) {

	// The engine is done with this object, so it's OK to delete
	// it if there is no connection to the Ruby runtime. If there
	// is, then refcount can remain 0 and the object will be
	// collected by the Ruby GC.

	if (( !this->IsBound ()) && ( refCount == 0 )) {
		//printf ( "MOAIRubyObject::ZLRefCountedObjectBase_OnRelease() %s %p!\n", this->TypeName (), this->mPtr );
		
		// no Ruby binding and no references, so
		// go ahead and kill this turkey
		delete this;
	}
}
