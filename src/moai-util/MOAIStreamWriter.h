// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAISTREAMWRITER_H
#define MOAISTREAMWRITER_H

#include <moai-util/MOAIStreamAdapter.h>

//================================================================//
// MOAIStreamWriter
//================================================================//
/**	@lua	MOAIStreamWriter
	@text	This class has been deprecated. Use MOAIStreamAdapter instead.
*/
class MOAIStreamWriter :
	public virtual MOAIStreamAdapter {
public:
	
	DECL_RUBY_FACTORY ( MOAIStreamWriter, MOAIStreamAdapter )

	//----------------------------------------------------------------//
					MOAIStreamWriter		();
					~MOAIStreamWriter		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
