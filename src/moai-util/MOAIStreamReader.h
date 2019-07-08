// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAISTREAMREADER_H
#define MOAISTREAMREADER_H

#include <moai-util/MOAIStreamAdapter.h>

//================================================================//
// MOAIStreamReader
//================================================================//
/**	@lua	MOAIStreamReader
	@text	This class has been deprecated. Use MOAIStreamAdapter instead.
*/
class MOAIStreamReader :
	public virtual MOAIStreamAdapter {
public:
	
	DECL_RUBY_FACTORY ( MOAIStreamReader, MOAIStreamAdapter )

	//----------------------------------------------------------------//
					MOAIStreamReader		();
					~MOAIStreamReader		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
