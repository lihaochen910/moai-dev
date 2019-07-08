// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-core/MOAIFoo.h>

//================================================================//
// ruby
//================================================================//

//----------------------------------------------------------------//
/**	@ruby	classHello
	@text	Class (a.k.a. static) method. Prints the string 'MOAIFoo class foo!' to the console.

	@out	nil
*/
mrb_value MOAIFoo::_classHello ( mrb_state* M, mrb_value context ) {
	UNUSED ( M );

	printf ( "MOAIFoo class foo!\n" );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	instanceHello
	@text	Prints the string 'MOAIFoo instance foo!' to the console.
	
	@in		MOAIFoo self
	@out	nil
*/
mrb_value MOAIFoo::_instanceHello ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFoo, "*" ) // this macro initializes the 'self' variable and type checks arguments
	
	u32 param1 = state.GetParamValue < u32 >( 1, 0 );
	float param2 = state.GetParamValue < float >( 2, 0 );
	cc8* param3 = state.GetParamValue < cc8* >( 3, "default" );
	double param4 = state.GetParamValue < double >( 4, 0.0 );
	
	printf ( "MOAIFoo instance foo! %d %f %s %f\n", param1, param2, param3, param4 );
	
	return context;
}

//================================================================//
// MOAIFoo
//================================================================//

//----------------------------------------------------------------//
MOAIFoo::MOAIFoo () {
	
	// register all classes MOAIFoo derives from
	// we need this for custom RTTI implementation
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
		
		// and any other objects from multiple inheritance...
		// RTTI_EXTEND ( MOAIFooBase )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIFoo::~MOAIFoo () {
}

//----------------------------------------------------------------//
void MOAIFoo::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	// call any initializers for base classes here:
	// MOAIFooBase::RegisterLuaClass ( state, klass );

	// also register constants:
	// state.DefineClassConst ( klass, "FOO_CONST", ( u32 )FOO_CONST );

	// here are the class methods:
	state.DefineStaticMethod ( klass, "classHello", _classHello, MRB_ARGS_NONE () );
}

//----------------------------------------------------------------//
void MOAIFoo::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	// call any initializers for base classes here:
	// MOAIFooBase::RegisterLuaFuncs ( state, klass );

	// here are the instance methods:
	state.DefineInstanceMethod ( klass, "instanceHello", _instanceHello, MRB_ARGS_ANY () );
}
