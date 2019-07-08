// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAICANARY_H
#define MOAICANARY_H

#include <moai-core/MOAIRuby.h>

#include <moai-core/MOAIRubyState-impl.h>
#include <moai-core/MOAIRubyClass-impl.h>

//================================================================//
// MOAICanary
//================================================================//
class MOAICanary :
	public virtual MOAIRubyObject {
private:
	
	STLString			mMessage;
	
	//----------------------------------------------------------------//
	static mrb_value	_setMessage			( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_FACTORY ( MOAICanary, MOAIRubyObject )

	//----------------------------------------------------------------//
						MOAICanary			();
						~MOAICanary			();
	void				RegisterRubyClass	( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs	( MOAIRubyState& state, RClass* klass );
};

#endif
