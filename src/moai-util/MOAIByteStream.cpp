// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-util/MOAIByteStream.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	close
	@text	Release the byte stream's internal buffer.

	@in		MOAIByteStream self
	@out	nil
*/
mrb_value MOAIByteStream::_close ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIByteStream, "U" );
	
	self->Close ();
	return context;
}

//----------------------------------------------------------------//
/**	@lua	open
	@text	Allocate and initialize the byte stream's internal buffer.

	@overload

		@in		MOAIByteStream self
		@in		string buffer			Initialize the stream's buffer as a copy of provided string.
		@out	nil
	
	@overload
	
		@in		MOAIByteStream self
		@in		number size				Initialize the stream with a buffer of the given size. Buffer will be filled with zero.
		@out	nil
*/
mrb_value MOAIByteStream::_open ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIByteStream, "U" );
	
	if ( state.ParamIsType ( 1, MRB_TT_STRING )) {
		void* data;
		size_t size = RSTRING_LEN ( state.GetParamValue ( 1 ) );
		data = ( void* )state.GetParamValue < cc8* > ( 1, "" );
		self->Open ( data, size );
	}
	
	if ( state.ParamIsType ( 1, MRB_TT_FIXNUM )) {
		u32 size = state.GetParamValue < u32 >( 2, 0 );
		self->Open ( size );
	}
	return context;
}

//================================================================//
// MOAIByteStream
//================================================================//

//----------------------------------------------------------------//
void MOAIByteStream::Close () {

	this->Clear ();

	if ( this->mData ) {
		free ( this->mData );
		this->mData = 0;
	}
}

//----------------------------------------------------------------//
MOAIByteStream::MOAIByteStream () :
	mData ( 0 ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIStream )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIByteStream::~MOAIByteStream () {
}

//----------------------------------------------------------------//
void MOAIByteStream::Open ( size_t size ) {

	this->Close ();
	
	if ( size ) {
		this->mData = calloc ( size, 1 );
		this->SetBuffer ( this->mData, size );
	}
}

//----------------------------------------------------------------//
void MOAIByteStream::Open ( void* data, size_t size ) {

	this->Open ( size );
	
	if ( data && size ) {
		memcpy ( this->mData, data, size );
	}
	this->SetLength ( size );
}

//----------------------------------------------------------------//
void MOAIByteStream::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIStream::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIByteStream::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIStream::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "close", _close, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "open", _open, MRB_ARGS_REQ ( 1 ) );

}

