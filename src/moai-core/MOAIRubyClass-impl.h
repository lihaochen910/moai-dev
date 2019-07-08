// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIRUBYCLASS_IMPL_H
#define	MOAIRUBYCLASS_IMPL_H

#include <moai-core/MOAIRubyClass.h>
#include <moai-core/MOAIRubyRuntime.h>

//================================================================//
// ruby
//================================================================//
	
//----------------------------------------------------------------//
template < typename TYPE >
mrb_value MOAIRubyFactoryClass < TYPE >::_getClassName ( mrb_state* M, mrb_value self ) {
	
	TYPE object;
	return mrb_str_new_cstr ( M, object.TypeName () );
}

//----------------------------------------------------------------//
template < typename TYPE >
mrb_value MOAIRubyFactoryClass < TYPE >::_getTypeID ( mrb_state* M, mrb_value self ) {
	UNUSED ( M );
	
	return mrb_fixnum_value ( ZLTypeID < TYPE >::GetID () );
}

//----------------------------------------------------------------//
template < typename TYPE >
mrb_value MOAIRubyFactoryClass < TYPE >::_new ( mrb_state* M, mrb_value self ) {
	
	MOAIRubyState state ( M );
	TYPE* data = new TYPE ();
	//data->PushRubyUserdata ( state );
	RUBY_FACTORY_NEW ( TYPE )

	//ZLLog_DebugF ( ZLLog::CONSOLE, "moai-lib-core::MOAIRubyFactoryClass::_new() %s\n", data->TypeName () );

	// since we're creating this just to hand to Ruby, we can safely
	// remove it from the runtime's global cache
	//MOAIRubyRuntime::Get ().PurgeUserdata ( state, -1 );
	
	return self;
}

//================================================================//
// MOAIRubyFactoryClass
//================================================================//

//----------------------------------------------------------------//
template < typename TYPE >
MOAIRubyFactoryClass < TYPE >& MOAIRubyFactoryClass < TYPE >::Get () {
	MOAIRubyFactoryClass < TYPE >* typeClass = ZLContextMgr::Get ()->GetGlobal < MOAIRubyFactoryClass >();
	if ( !typeClass ) {
		typeClass = ZLContextMgr::Get ()->AffirmGlobal < MOAIRubyFactoryClass >();
		MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
		TYPE type;
		typeClass->InitRubyFactoryClass ( type, state );
	}
	assert ( typeClass );
	return *typeClass;
}

//----------------------------------------------------------------//
template < typename TYPE >
MOAIRubyFactoryClass < TYPE >::MOAIRubyFactoryClass () {
	this->mIsSingleton = false;
}

//----------------------------------------------------------------//
template < typename TYPE >
void MOAIRubyFactoryClass < TYPE >::Register () {
}

//----------------------------------------------------------------//
template < typename TYPE >
void MOAIRubyFactoryClass < TYPE >::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineStaticMethod ( klass, "getClassName", _getClassName, MRB_ARGS_NONE() );
	state.DefineStaticMethod ( klass, "getTypeID", _getTypeID, MRB_ARGS_NONE() );
	state.DefineInstanceMethod ( klass, "initialize", _new, MRB_ARGS_NONE () );
}

//================================================================//
// ruby
//================================================================//
	
//----------------------------------------------------------------//
template < typename TYPE >
mrb_value MOAIRubySingletonClass < TYPE >::_getClassName ( mrb_state* M, mrb_value self ) {
	
	MOAIRubyObject* singleton = ZLContextMgr::Get ()->GetGlobal < TYPE >();
	return mrb_str_new_cstr ( M, singleton->TypeName () );
}

//----------------------------------------------------------------//
template < typename TYPE >
mrb_value MOAIRubySingletonClass < TYPE >::_getTypeID ( mrb_state* M, mrb_value self ) {
	
	return mrb_fixnum_value ( ZLTypeID < TYPE >::GetID () );
}

//----------------------------------------------------------------//
template < typename TYPE >
mrb_value MOAIRubySingletonClass < TYPE >::_new ( mrb_state* M, mrb_value self ) {

	MOAIRubyState state ( M );
	MOAIRubyObject* data = MOAIRubySingletonClass < TYPE >::Get ().GetSingleton ();
	//ZLLog_DebugF ( ZLLog::CONSOLE, "moai-lib-core::MOAIRubySingletonClass::_new() %s %p\n", data->TypeName (), data );

	RUBY_SINGLETON_CTOR ( TYPE )
}

//================================================================//
// MOAIRubySingletonClass
//================================================================//

//----------------------------------------------------------------//
template < typename TYPE >
MOAIRubySingletonClass < TYPE >& MOAIRubySingletonClass < TYPE >::Get () {
	return *ZLContextMgr::Get ()->AffirmGlobal < MOAIRubySingletonClass >();
}

//----------------------------------------------------------------//
template < typename TYPE >
MOAIRubyObject* MOAIRubySingletonClass < TYPE >::GetSingleton () {
	return ZLContextMgr::Get ()->AffirmGlobal < TYPE >();
}

//----------------------------------------------------------------//
template < typename TYPE >
MOAIRubySingletonClass < TYPE >::MOAIRubySingletonClass () {
	this->mIsSingleton = true;
}

//----------------------------------------------------------------//
template < typename TYPE >
void MOAIRubySingletonClass < TYPE >::Register () {

	//MOAIScopedRubyState state = MOAIRubyRuntime::Get ().State ();
	MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
	MOAIRubyObject* type = this->GetSingleton ();
	this->InitRubySingletonClass ( *type, state );
}

//----------------------------------------------------------------//
template < typename TYPE >
void MOAIRubySingletonClass < TYPE >::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineStaticMethod ( klass, "getClassName", _getClassName, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getTypeID", _getTypeID, MRB_ARGS_NONE () );
	//state.DefineInstanceMethod ( klass, "initialize", _new, MRB_ARGS_NONE () );
}

#endif
