// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-util/MOAIDataBuffer.h>
#include <moai-util/MOAIDataBufferStream.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	close
	@text	Disassociates and unlocks the stream's MOAIDataBuffer.
	
	@in		MOAIDataBufferStream self
	@out	nil
*/
mrb_value MOAIDataBufferStream::_close ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDataBufferStream, "U" );
	
	self->Close ();
	return context;
}

//----------------------------------------------------------------//
/**	@lua	open
	@text	Associate the stream with a MOAIDataBuffer. Note that the
			MOAIDataBuffer will be locked with a mutex while it is open
			thus blocking any asynchronous operations.
	
	@in		MOAIDataBufferStream self
	@in		MOAIDataBuffer buffer
	@out	boolean success
*/
mrb_value MOAIDataBufferStream::_open ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDataBufferStream, "UU" );
	
	self->Close ();
	
	MOAIDataBuffer* buffer = state.GetRubyObject < MOAIDataBuffer >( 1, true );
	if ( !buffer ) return mrb_nil_value ();
	
	bool result = self->Open ( buffer );
	
	return state.ToRValue ( result );
}

//================================================================//
// MOAIDataBufferStream
//================================================================//

//----------------------------------------------------------------//
void MOAIDataBufferStream::Close () {

	this->Clear ();

	if ( this->mDataBuffer ) {
		this->mDataBuffer->Unlock ();
		this->mDataBuffer.Set ( *this, 0 );
	}
}

//----------------------------------------------------------------//
MOAIDataBufferStream::MOAIDataBufferStream () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIStream )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIDataBufferStream::~MOAIDataBufferStream () {
}

//----------------------------------------------------------------//
bool MOAIDataBufferStream::Open ( MOAIDataBuffer* buffer ) {

	this->Close ();

	if ( !buffer ) return false;
	
	this->mDataBuffer.Set ( *this, buffer );
	
	void* bytes = 0;
	size_t size = 0;
	
	buffer->Lock ( &bytes, &size );
	
	this->SetBuffer ( buffer, size );
	this->SetLength ( size );
	
	return true;
}

//----------------------------------------------------------------//
void MOAIDataBufferStream::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIStream::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIDataBufferStream::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIStream::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "close", _close, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "open", _open, MRB_ARGS_REQ ( 1 ) );

}

