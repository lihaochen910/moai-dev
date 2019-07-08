// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIFOO_H
#define MOAIFOO_H

#include <moai-core/MOAIRuby.h>

#include <moai-core/MOAIRubyState-impl.h>
#include <moai-core/MOAIRubyClass-impl.h>

//================================================================//
// MOAIFoo
//================================================================//
/**	@lua	MOAIFoo
	@text	Example class for extending Moai using MOAIRubyObject.
			Copy this object, rename it and add your own stuff.
			Just don't forget to register it with the runtime
			using the REGISTER_LUA_CLASS macro (see moaicore.cpp).
*/
class MOAIFoo :
	public virtual MOAIRubyObject {
private:
	
	//----------------------------------------------------------------//
	static mrb_value		_classHello			( mrb_state* M, mrb_value context );
	static mrb_value		_instanceHello		( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_FACTORY ( MOAIFoo, MOAIRubyObject )

	//----------------------------------------------------------------//
							MOAIFoo				();
							~MOAIFoo			();
	void					RegisterRubyClass	( MOAIRubyState& state, RClass* klass );
	void					RegisterRubyFuncs	( MOAIRubyState& state, RClass* klass );
};

#endif
