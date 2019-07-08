// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-util/MOAIFileStream.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	close
	@text	Close and release the associated file handle.
	
	@in		MOAIFileStream self
	@out	nil
*/
mrb_value MOAIFileStream::_close ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFileStream, "U" );
	
	self->Close ();
	return context;
}

//----------------------------------------------------------------//
/**	@lua	open
	@text	Open or create a file stream given a valid path.
	
	@in		MOAIFileStream self
	@in		string fileName
	@opt	number mode			One of MOAIFileStream.APPEND, MOAIFileStream.READ, MOAIFileStream.READ_WRITE, MOAIFileStream.READ_WRITE_AFFIRM,
								MOAIFileStream.READ_WRITE_NEW, MOAIFileStream.WRITE. Default value is MOAIFileStream.READ.
	@out	boolean success
*/
mrb_value MOAIFileStream::_open ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFileStream, "US" );
	
	cc8* filename	= state.GetParamValue < cc8* >( 1, "" );
	u32 mode		= state.GetParamValue < u32 >( 2, ZLFileStream::READ );
	
	bool result = self->Open ( filename, mode );
	
	return state.ToRValue ( result );
}

//================================================================//
// MOAIFileStream
//================================================================//

//----------------------------------------------------------------//
MOAIFileStream::MOAIFileStream () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIStream )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIFileStream::~MOAIFileStream () {
	this->Close ();
}

//----------------------------------------------------------------//
void MOAIFileStream::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIStream::RegisterRubyClass ( state, klass );

	state.DefineClassConst ( klass, "APPEND",				( u32 )ZLFileStream::APPEND );
	state.DefineClassConst ( klass, "READ",					( u32 )ZLFileStream::READ );
	state.DefineClassConst ( klass, "READ_WRITE",			( u32 )ZLFileStream::READ_WRITE );
	state.DefineClassConst ( klass, "READ_WRITE_AFFIRM",	( u32 )ZLFileStream::READ_WRITE_AFFIRM );
	state.DefineClassConst ( klass, "READ_WRITE_NEW",		( u32 )ZLFileStream::READ_WRITE_NEW );
	state.DefineClassConst ( klass, "WRITE",				( u32 )ZLFileStream::WRITE );
}

//----------------------------------------------------------------//
void MOAIFileStream::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIStream::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "close", _close, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "open", _open, MRB_ARGS_REQ ( 1 ) );

}

