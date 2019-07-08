// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-core/MOAIFooMgr.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	singletonHello
	@text	Prints the string 'MOAIFooMgr singleton foo!' to the console.

	@out	nil
*/
mrb_value MOAIFooMgr::_singletonHello ( mrb_state* M, mrb_value context ) {
	UNUSED ( M );

	printf ( "MOAIFooMgr singleton foo!\n" );
	
	return context;
}

//================================================================//
// MOAIFooMgr
//================================================================//

//----------------------------------------------------------------//
MOAIFooMgr::MOAIFooMgr () {
	
	// register all classes MOAIFooMgr derives from
	// we need this for custom RTTI implementation
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
		
		// and any other objects from multiple inheritance...
		// RTTI_EXTEND ( MOAIFooMgrBase )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIFooMgr::~MOAIFooMgr () {
}

//----------------------------------------------------------------//
void MOAIFooMgr::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	// call any initializers for base classes here:
	// MOAIFooBase::RegisterRubyClass ( state );

	// also register constants:
	// state.SetField ( -1, "FOO_CONST", ( u32 )FOO_CONST );

	// here are the class methods:
	state.DefineStaticMethod ( klass, "singletonHello", _singletonHello, MRB_ARGS_NONE () );
}
