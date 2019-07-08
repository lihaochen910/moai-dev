// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIFILESTREAM_H
#define MOAIFILESTREAM_H

#include <moai-util/MOAIStream.h>

//================================================================//
// MOAIFileStream
//================================================================//
/**	@lua	MOAIFileStream
	@text	MOAIFileStream opens a system file handle for reading or writing.
	
	@const	READ
	@const	READ_WRITE
	@const	READ_WRITE_AFFIRM
	@const	READ_WRITE_NEW
	@const	WRITE
*/
class MOAIFileStream :
	public virtual ZLFileStream,
	public virtual MOAIStream {
private:
	
	//----------------------------------------------------------------//
	static mrb_value		_close					( mrb_state* M, mrb_value context );
	static mrb_value		_open					( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_FACTORY ( MOAIFileStream, MOAIRubyObject )

	//----------------------------------------------------------------//
					MOAIFileStream			();
					~MOAIFileStream			();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
