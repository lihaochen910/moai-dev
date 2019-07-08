// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIBYTESTREAM_H
#define MOAIBYTESTREAM_H

#include <moai-util/MOAIStream.h>

//================================================================//
// MOAIByteStream
//================================================================//
/**	@lua	MOAIByteStream
	@text	MOAIByteStream implements a stream with a fixed-size internal buffer.
*/
class MOAIByteStream :
	public virtual ZLByteStream,
	public virtual MOAIStream {
private:
	
	void*			mData;
	
	//----------------------------------------------------------------//
	static mrb_value		_close				( mrb_state* M, mrb_value context );
	static mrb_value		_open				( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_FACTORY ( MOAIByteStream, MOAIRubyObject )

	//----------------------------------------------------------------//
	void			Close				();
					MOAIByteStream		();
					~MOAIByteStream		();
	void			Open				( size_t size );
	void			Open				( void* data, size_t size );
	void			RegisterRubyClass	( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs	( MOAIRubyState& state, RClass* klass );
};

#endif
