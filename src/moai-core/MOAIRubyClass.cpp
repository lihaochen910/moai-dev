// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-core/MOAIRuby.h>
#include <moai-core/MOAISerializer.h>

//================================================================//
// MOAIRubyClass
//================================================================//

//----------------------------------------------------------------//
mrb_value MOAIRubyClass::_new ( mrb_state* M, mrb_value self ) {

	UNUSED ( M );
	return self;

	// upvalues:
	// 1: interface table
	// 2: original 'new'

	//MOAIRubyState state ( L );
	//
	//lua_pushvalue ( L, lua_upvalueindex ( 2 ));
	//if ( state.IsType ( -1, LUA_TFUNCTION )) {
	//
	//	// call the original new
	//	state.DebugCall ( 0, 1 );
	//	
	//	if ( state.IsType ( -1, LUA_TUSERDATA )) {
	//	
	//		// get the ref table
	//		if ( lua_getmetatable ( state, -1 )) {
	//			
	//			// get the member table
	//			if ( lua_getmetatable ( state, -1 )) {
	//			
	//				// get the interface table
	//				lua_pushvalue ( L, lua_upvalueindex ( 1 ));
	//				
	//				// set the interface table as the metatable
	//				lua_setmetatable ( L, -2 );
	//				
	//				// done with the member table
	//				lua_pop ( L, 1 );
	//			}
	//			// done with the ref table
	//			lua_pop ( L, 1 );
	//		}
	//	}
	//	return 1;
	//}
	//return 0;
}

//----------------------------------------------------------------//
RClass* MOAIRubyClass::GetBasicRClass () {
	return this->mSuperClass;
}

//----------------------------------------------------------------//
RClass* MOAIRubyClass::GetRClass () {
	return this->mClass;
}

//----------------------------------------------------------------//
MOAIRubyObject* MOAIRubyClass::GetSingleton () {
	return 0;
}

//----------------------------------------------------------------//
RObject* MOAIRubyClass::GetSingletonRObject () {
	return mrb_obj_ptr ( this->mSingletonRef.GetRef () );
}

//----------------------------------------------------------------//
void MOAIRubyClass::InitRubyFactoryClass ( MOAIRubyObject& data, MOAIRubyState& state ) {

	//ZLLog_DebugF ( ZLLog::CONSOLE, "MOAIRubyClass::InitRubyFactoryClass() %s \n", data.TypeName () );

	this->mSuperClass = data.GetSuperRubyClass ()->GetRClass ();
	this->mClass = mrb_define_class ( state, data.TypeName (), this->mSuperClass );
	MRB_SET_INSTANCE_TT ( this->mClass, MRB_TT_DATA );
	
	data.MOAIRubyObject::RegisterRubyFuncs ( state, this->mClass );
	data.RegisterRubyFuncs ( state, this->mClass );

	//this->mInterfaceTable.SetRef ( state, -1 );
	
	// reset top
	//lua_settop ( state, top );

	// push class table
	//lua_newtable ( state );
	this->RegisterRubyClass ( state, this->mClass );
	data.MOAIRubyObject::RegisterRubyClass ( state, this->mClass );
	data.RegisterRubyClass ( state, this->mClass );

	// init the extend method
	//lua_pushvalue ( state, -1 ); // class table
	//this->mInterfaceTable.PushRef ( state ); // interface table
	//lua_pushcclosure ( state, _extendFactory, 2 );
	//lua_setfield ( state, -2, "extend" );

	// init the getInterfaceTable method
	//this->mInterfaceTable.PushRef ( state ); // interface table
	//lua_pushcclosure ( state, _getInterfaceTable, 1 );
	//lua_setfield ( state, -2, "getInterfaceTable" );

	// ref class table and expose as global
	//this->mClassTable.SetRef ( state, -1 );
	//lua_setglobal ( state, data.TypeName ());

	// reset top
	//lua_settop ( state, top );
}

//----------------------------------------------------------------//
void MOAIRubyClass::InitRubySingletonClass ( MOAIRubyObject& data, MOAIRubyState& state ) {

	//ZLLog_DebugF ( ZLLog::CONSOLE, "MOAIRubyClass::InitRubySingletonClass() %s \n", data.TypeName () );

	this->mSuperClass = mrb_class_get ( state, "Module" );
	this->mClass = mrb_define_module ( state, data.TypeName () );
	MRB_SET_INSTANCE_TT ( this->mClass, MRB_TT_DATA );

	data.mPtr = ( RObject* )mrb_ptr ( mrb_obj_value ( this->mClass ) );

	// push class table
	//lua_newtable ( state );
	this->RegisterRubyClass ( state, this->mClass );

	// create the singleton class instance
	//mrb_value singleton_instance = mrb_class_new_instance ( state, 0, 0, this->mClass );
	//this->mSingletonRef.SetRef ( singleton_instance );

	data.MOAIRubyObject::RegisterRubyClass ( state, this->mClass );
	data.RegisterRubyClass ( state, this->mClass );

	// attach the ref table (for leak reporting only; no metamethods)
	//this->PushRefTable ( state );
	//lua_setmetatable ( state, -2 ); // for leak reporting only (no metamethods)
	
	// init the extend method
	//state.PushPtrUserData ( &data ); // copy of userdata
	//lua_pushvalue ( state, -2 ); // copy of class table
	//lua_pushcclosure ( state, _extendSingleton, 2 );
	//lua_setfield ( state, -2, "extend" );
	
	// ref class table
	//this->mClassTable.SetRef ( state, -1 );
	
	//lua_pushvalue ( state, -1 );
	//lua_setfield ( state, -2, "__index" );

	//lua_pushvalue ( state, -1 );
	//lua_setfield ( state, -2, "__newindex" );

	//lua_setglobal ( state, data.TypeName ());
}

//----------------------------------------------------------------//
bool MOAIRubyClass::IsSingleton () {

	return ( this->mIsSingleton );
}

//----------------------------------------------------------------//
MOAIRubyClass::MOAIRubyClass () :
	mIsSingleton ( false ) {
}

//----------------------------------------------------------------//
MOAIRubyClass::~MOAIRubyClass () {
}

//================================================================//
// MOAIRubyBasicClass
//================================================================//

//----------------------------------------------------------------//
MOAIRubyBasicClass& MOAIRubyBasicClass::Get () {
	return *ZLContextMgr::Get ()->AffirmGlobal < MOAIRubyBasicClass > ();
}

//----------------------------------------------------------------//
MOAIRubyBasicClass::MOAIRubyBasicClass () {

	MOAIRubyState& state = MOAIRubyRuntime::Get ().GetMainState ();

	this->mClass = state->object_class;
	this->mSuperClass = mrb_class_get ( state, "BasicObject" );
}

//----------------------------------------------------------------//
void MOAIRubyBasicClass::Register () {
}

//----------------------------------------------------------------//
void MOAIRubyBasicClass::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
}
