// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-util/MOAIMemStream.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	close
	@text	Close the memory stream and release its buffers.
	
	@in		MOAIMemStream self
	@out	nil
*/
mrb_value MOAIMemStream::_close ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMemStream, "U" );
	
	self->Clear ();
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMemStream::_discardAll ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMemStream, "U" );
	
	self->DiscardAll ();
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMemStream::_discardBack ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMemStream, "U" );
	
	self->DiscardBack ( state.GetParamValue ( 1, self->GetCursor ()));
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMemStream::_discardFront ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMemStream, "U" );
	
	self->DiscardFront ( state.GetParamValue < u32 >( 1, ( u32 )self->GetCursor ()));
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMemStream::_getString ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMemStream, "U" );

	size_t size = self->GetLength ();
	
	if ( size ) {
		
		/*size_t cursor = self->GetCursor ();

		self->Seek ( 0, SEEK_SET );
		void* str = 0;

		if ( size > ALLOCA_MAX ) {
			str = malloc ( size );
		}
		else {
			str = alloca ( size );
		}

		assert ( str );
		self->ReadBytes ( str, size );
		lua_pushlstring ( state, ( cc8* )str, size );

		if ( size > ALLOCA_MAX ) {
			free ( str );
		}

		self->Seek (( long )cursor, SEEK_SET );
		return 1;*/
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	open
	@text	Create a memory stream and optionally reserve some memory and set
			the chunk size by which the stream will grow if additional memory
			is needed.
	
	@in		MOAIMemStream self
	@opt	number reserve			Default value is 0.
	@opt	number chunkSize		Default value is MOAIMemStream.DEFAULT_CHUNK_SIZE (2048 bytes).
	@out	boolean success
*/
mrb_value MOAIMemStream::_open ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMemStream, "U" );
	
	self->Clear ();
	
	u32 reserve			= state.GetParamValue < u32 >( 1, 0 );
	u32 chunkSize		= state.GetParamValue < u32 >( 2, ZLMemStream::DEFAULT_CHUNK_SIZE );
	
	bool result = false;

	if ( chunkSize ) {
		self->SetChunkSize ( chunkSize );
		self->Reserve ( reserve );
		result = true;
	}
	
	return state.ToRValue ( result );
}

//================================================================//
// MOAIMemStream
//================================================================//

//----------------------------------------------------------------//
MOAIMemStream::MOAIMemStream () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIStream )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIMemStream::~MOAIMemStream () {
}

//----------------------------------------------------------------//
void MOAIMemStream::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIStream::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "DEFAULT_CHUNK_SIZE", ( u32 )ZLMemStream::DEFAULT_CHUNK_SIZE );
}

//----------------------------------------------------------------//
void MOAIMemStream::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIStream::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "close", _close, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "discardAll", _discardAll, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "discardBack", _discardBack, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "discardFront", _discardFront, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "getString", _getString, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "open", _open, MRB_ARGS_REQ ( 1 ) );

}

