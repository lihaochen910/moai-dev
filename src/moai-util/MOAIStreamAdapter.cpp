// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-util/MOAIStreamAdapter.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	close
	@text	Flush any remaining buffered data (if a write) and detach the target stream.
			(This only detaches the target from the formatter; it does
			not also close the target stream).
	
	@in		MOAIStreamWriter self
	@out	nil
*/
mrb_value MOAIStreamAdapter::_close ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStreamAdapter, "U" );
	
	self->Close ();
	return context;
}

//----------------------------------------------------------------//
/**	@lua	openBase64
	@text	Open a base 64 formatted stream for reading (i.e. decode
			bytes from base64).
	
	@in		MOAIStreamAdapter self
	@in		MOAIStream target
	@out	boolean success
*/
mrb_value MOAIStreamAdapter::_openBase64Reader ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStreamAdapter, "U" );
	
	return self->Open ( state, 2, new ZLBase64Reader ());
}

//----------------------------------------------------------------//
/**	@lua	openBase64Writer
	@text	Open a base 64 formatted stream for writing (i.e. encode
			bytes to base64).
	
	@in		MOAIStreamAdapter self
	@in		MOAIStream target
	@out	boolean success
*/
mrb_value MOAIStreamAdapter::_openBase64Writer ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStreamAdapter, "U" );
	
	return self->Open ( state, 2, new ZLBase64Writer ());
}

//----------------------------------------------------------------//
/**	@lua	openDeflate
	@text	Open a 'deflate' formatted stream for reading (i.e. decompress
			bytes using the 'deflate' algorithm).
	
	@in		MOAIStreamAdapter self
	@in		MOAIStream target
	@opt	number windowBits		The window bits used in the DEFLATE algorithm.
	@out	boolean success
*/
mrb_value MOAIStreamAdapter::_openDeflateReader ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStreamAdapter, "U" );
	
	self->Close ();
	
	MOAIStream* stream = state.GetRubyObject < MOAIStream >( 2, true );
	if ( !stream ) return mrb_nil_value ();
	
	int windowBits	= state.GetParamValue < int >( 3, ZLDeflateWriter::DEFAULT_WBITS );
	
	ZLDeflateReader* reader = new ZLDeflateReader ();
	
	reader->SetWindowBits ( windowBits );
	
	ZLResultCode result = self->Open ( reader, stream );
	
	return state.ToRValue ( result == ZL_OK );
}

//----------------------------------------------------------------//
/**	@lua	openDeflateWriter
	@text	Open a 'deflate' formatted stream for writing (i.e. compress
			bytes using the 'deflate' algorithm).
	
	@in		MOAIStreamAdapter self
	@in		MOAIStream target
	@opt	number level			The level used in the DEFLATE algorithm.
	@opt	number windowBits		The window bits used in the DEFLATE algorithm.
	@out	boolean success
*/
mrb_value MOAIStreamAdapter::_openDeflateWriter ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStreamAdapter, "U" );
	
	self->Close ();
	
	MOAIStream* stream = state.GetRubyObject < MOAIStream >( 2, true );
	if ( !stream ) return mrb_false_value ();
	
	int level		= state.GetParamValue < int >( 3, ZLDeflateWriter::DEFAULT_LEVEL );
	int windowBits	= state.GetParamValue < int >( 4, ZLDeflateWriter::DEFAULT_WBITS );
	
	ZLDeflateWriter* writer = new ZLDeflateWriter ();
	
	writer->SetCompressionLevel ( level );
	writer->SetWindowBits ( windowBits );
	
	ZLResultCode result = self->Open ( writer, stream );
	
	return state.ToRValue ( result == ZL_OK );
}

//----------------------------------------------------------------//
/**	@lua	openHex
	@text	Open a hex formatted stream for reading and writing (i.e. decode
			and encode bytes to and from hex).
	
	@in		MOAIStreamReader self
	@in		MOAIStream target
	@out	boolean success
*/
mrb_value MOAIStreamAdapter::_openHex ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStreamAdapter, "U" );
	
	return self->Open ( state, 2, new ZLHexAdapter ());
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIStreamAdapter::_openRing ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStreamAdapter, "U" );
	
	MOAIStream* stream = state.GetRubyObject < MOAIStream >( 1, true );
	u32 size = state.GetParamValue < u32 >( 2, ( u32 )stream->GetLength ());
	
	if ( size > 0 ) {
	
		ZLRingAdapter* adapter = new ZLRingAdapter ();
		ZLResultCode result = self->Open ( adapter, stream );
		
		if ( result == ZL_OK ) {
			adapter->SetLength ( size );
			return state.ToRValue ( result );
		}
	}
	return context;
}

//================================================================//
// MOAIStreamAdapter
//================================================================//

//----------------------------------------------------------------//
void MOAIStreamAdapter::Clear () {
	
	this->Close ();
	
	if ( this->mAdapter ) {
		delete this->mAdapter;
		this->mAdapter = 0;
	}
}

//----------------------------------------------------------------//
void MOAIStreamAdapter::Close () {
	
	if ( this->mAdapter ) {
		this->mAdapter->Close (); // would be called anyway by the destructor; doing it explicitly here
	}
	this->mAdaptedStream.Set ( *this, 0 );
	this->SetProxiedStream ( 0 );
}

//----------------------------------------------------------------//
MOAIStreamAdapter::MOAIStreamAdapter () :
	mAdapter ( 0 ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIStream )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIStreamAdapter::~MOAIStreamAdapter () {

	this->Clear ();
}

//----------------------------------------------------------------//
ZLResultCode MOAIStreamAdapter::Open ( ZLStreamAdapter* adapter, MOAIStream* stream ) {

	this->Clear ();
	
	this->mAdapter = adapter;
	this->SetProxiedStream ( this->mAdapter );
	this->mAdaptedStream.Set ( *this, stream );
	
	ZLResultCode result = this->mAdapter->Open ( this->mAdaptedStream );
	
	if ( result != ZL_OK ) {
		this->Close ();
	}
	return result;
}

//----------------------------------------------------------------//
mrb_value MOAIStreamAdapter::Open ( MOAIRubyState& state, int idx, ZLStreamAdapter* adapter) {

	MOAIStream* stream = state.GetRubyObject < MOAIStream >( idx, true );

	ZLResultCode result = this->Open ( adapter, stream );
	return state.ToRValue ( result == ZL_OK );
}

//----------------------------------------------------------------//
void MOAIStreamAdapter::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIStream::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "DEFAULT_LEVEL", ZLDeflateWriter::DEFAULT_LEVEL );
	state.DefineClassConst ( klass, "DEFAULT_WBITS", ZLDeflateWriter::DEFAULT_WBITS );
}

//----------------------------------------------------------------//
void MOAIStreamAdapter::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIStream::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "close", _close, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "openBase64Reader", _openBase64Reader, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "openBase64Writer", _openBase64Writer, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "openDeflateReader", _openDeflateReader, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "openDeflateWriter", _openDeflateWriter, MRB_ARGS_ARG ( 1, 2 ) );
	state.DefineInstanceMethod ( klass, "openHex", _openHex, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "openRing", _openRing, MRB_ARGS_REQ ( 2 ) );

}

