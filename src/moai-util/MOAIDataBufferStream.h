// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIDATABUFFERSTREAM_H
#define MOAIDATABUFFERSTREAM_H

#include <moai-util/MOAIStream.h>

class MOAIDataBuffer;

//================================================================//
// MOAIDataBufferStream
//================================================================//
/**	@lua	MOAIDataBufferStream
	@text	MOAIDataBufferStream locks an associated MOAIDataBuffer for
			reading and writing.
*/
class MOAIDataBufferStream :
	public virtual ZLByteStream,
	public virtual MOAIStream {
private:
	
	MOAIRubySharedPtr < MOAIDataBuffer > mDataBuffer;
	
	//----------------------------------------------------------------//
	static mrb_value	_close		( mrb_state* M, mrb_value context );
	static mrb_value	_open		( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_FACTORY ( MOAIDataBufferStream, MOAIRubyObject )

	//----------------------------------------------------------------//
	void				Close						();
						MOAIDataBufferStream		();
						~MOAIDataBufferStream		();
	bool				Open						( MOAIDataBuffer* buffer );
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
};

#endif
