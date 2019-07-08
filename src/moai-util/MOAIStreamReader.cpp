// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-util/MOAIStreamReader.h>

//================================================================//
// lua
//================================================================//

//================================================================//
// MOAIStreamReader
//================================================================//

//----------------------------------------------------------------//
MOAIStreamReader::MOAIStreamReader () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIStreamAdapter )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIStreamReader::~MOAIStreamReader () {
}

//----------------------------------------------------------------//
void MOAIStreamReader::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIStreamAdapter::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIStreamReader::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIStreamAdapter::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "openBase64Reader", MOAIStreamAdapter::_openBase64Reader, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "openBase64Writer", MOAIStreamAdapter::_openDeflateReader, MRB_ARGS_REQ ( 1 ) );

}

