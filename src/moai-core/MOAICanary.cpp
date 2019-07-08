// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-core/MOAICanary.h>

//================================================================//
// ruby
//================================================================//

//----------------------------------------------------------------//
/**	@lua	setMessage
	@text	Sets the message to print on destruction.
	
	@in		MOAICanary self
	@out	nil
*/
mrb_value MOAICanary::_setMessage ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICanary, "S" ) // this macro initializes the 'self' variable and type checks arguments
	
	self->mMessage = state.GetParamValue < cc8* >( 1, "" );
	
	return context;
}

//================================================================//
// MOAICanary
//================================================================//

//----------------------------------------------------------------//
MOAICanary::MOAICanary () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAICanary::~MOAICanary () {

	if ( this->mMessage.size ()) {
		printf ( "DELETED CANARY: %s\n", this->mMessage.c_str ());
	}
}

//----------------------------------------------------------------//
void MOAICanary::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAICanary::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	mrb_define_method ( state, klass, "setMessage", _setMessage, MRB_ARGS_REQ ( 1 ) );
}

