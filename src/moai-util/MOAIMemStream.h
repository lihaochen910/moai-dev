// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIMEMSTREAM_H
#define MOAIMEMSTREAM_H

#include <moai-util/MOAIStream.h>

//================================================================//
// MOAIMemStream
//================================================================//
/**	@lua	MOAIMemStream
	@text	MOAIMemStream implements an in-memory stream and grows as
			needed. The memory stream expands on demands by allocating
			additional 'chunks' or memory. The chunk size may be configured
			by the user. Note that the chunks are not guaranteed to be
			contiguous in memory.
*/
class MOAIMemStream :
	public virtual ZLMemStream,
	public virtual MOAIStream {
private:
	
	//----------------------------------------------------------------//
	static mrb_value		_close				( mrb_state* M, mrb_value context );
	static mrb_value		_discardAll			( mrb_state* M, mrb_value context );
	static mrb_value		_discardBack		( mrb_state* M, mrb_value context );
	static mrb_value		_discardFront		( mrb_state* M, mrb_value context );
	static mrb_value		_getString			( mrb_state* M, mrb_value context );
	static mrb_value		_open				( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_FACTORY ( MOAIMemStream, MOAIRubyObject )

	//----------------------------------------------------------------//
					MOAIMemStream		();
					~MOAIMemStream		();
	bool			Open				( u32 reserve, u32 chunkSize );
	void			RegisterRubyClass	( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs	( MOAIRubyState& state, RClass* klass );
};

#endif
