// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-util/MOAIStreamWriter.h>

//================================================================//
// lua
//================================================================//

//================================================================//
// MOAIStreamWriter
//================================================================//

//----------------------------------------------------------------//
MOAIStreamWriter::MOAIStreamWriter () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIStreamAdapter )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIStreamWriter::~MOAIStreamWriter () {
}

//----------------------------------------------------------------//
void MOAIStreamWriter::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIStreamAdapter::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIStreamWriter::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIStreamAdapter::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "openBase64", MOAIStreamAdapter::_openBase64Writer, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "openDeflate", MOAIStreamAdapter::_openDeflateWriter, MRB_ARGS_ARG ( 1, 2 ) );

}

