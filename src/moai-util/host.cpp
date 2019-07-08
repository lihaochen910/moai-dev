// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-util/host.h>
#include <moai-util/headers.h>

//================================================================//
// aku-util
//================================================================//

//----------------------------------------------------------------//
void AKUUtilAppFinalize () {
}

//----------------------------------------------------------------//
void AKUUtilAppInitialize () {
}

//----------------------------------------------------------------//
void AKUUtilContextInitialize () {

	MOAIMainThreadTaskSubscriber::Affirm ();
	
	//MOAIMath::Affirm ();
	
	// MOAI
	REGISTER_RUBY_CLASS ( MOAIByteStream )
	REGISTER_RUBY_CLASS ( MOAIDataBuffer )
	REGISTER_RUBY_CLASS ( MOAIDataBufferStream )
	REGISTER_RUBY_CLASS ( MOAIFileStream )
	REGISTER_RUBY_CLASS ( MOAIFileSystem )
	//REGISTER_LUA_CLASS ( MOAIFourier )
	
	#ifndef MOAI_WITH_LUAJIT
		//REGISTER_LUA_CLASS ( MOAILuaUtil )
	#endif

	//REGISTER_RUBY_CLASS ( MOAIHashWriter )
	//REGISTER_LUA_CLASS ( MOAIMath )
	REGISTER_RUBY_CLASS ( MOAIMemStream )
	REGISTER_RUBY_CLASS ( MOAIStreamAdapter )
	REGISTER_RUBY_CLASS ( MOAITaskQueue )
	
	#if MOAI_WITH_JANSSON
		//REGISTER_LUA_CLASS ( MOAIJsonParser )
	#endif
	
	#if MOAI_WITH_GPB
		//REGISTER_LUA_CLASS ( MOAIParser )
	#endif
	
	#if MOAI_WITH_TINYXML && MOAI_WITH_EXPAT
  		//REGISTER_LUA_CLASS ( MOAIXmlParser )
	#endif
}
