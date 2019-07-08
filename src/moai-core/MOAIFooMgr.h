// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIFOOMGR_H
#define MOAIFOOMGR_H

#include <moai-core/MOAIRuby.h>

#include <moai-core/MOAIRubyState-impl.h>
#include <moai-core/MOAIRubyClass-impl.h>

//================================================================//
// MOAIFooMgr
//================================================================//
/**	@lua	MOAIFooMgr
	@text	Example singleton for extending Moai using MOAIRubyObject.
			Copy this object, rename it and add your own stuff.
			Just don't forget to register it with the runtime
			using the REGISTER_LUA_CLASS macro (see moaicore.cpp).
*/
class MOAIFooMgr :
	public ZLContextClass < MOAIFooMgr, MOAIRubyObject > {
private:
	
	//----------------------------------------------------------------//
	static mrb_value		_singletonHello		( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_SINGLETON ( MOAIFooMgr )

	//----------------------------------------------------------------//
							MOAIFooMgr			();
							~MOAIFooMgr			();
	void					RegisterRubyClass	( MOAIRubyState& state, RClass* klass );
};

#endif
