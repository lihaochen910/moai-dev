// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-util/MOAIDataIOTask.h>
#include <moai-util/MOAIDataBuffer.h>
#include <moai-util/MOAITaskSubscriber.h>

//================================================================//
// MOAIDataBuffer
//================================================================//

//----------------------------------------------------------------//
/**	@lua	base64Decode
	@text	If a string is provided, decodes it as a base64 encoded string.  Otherwise, decodes the current data stored in this object as a base64 encoded sequence of characters.

	@opt	MOAIDataBuffer self
	@opt	string data				The string data to decode.  You must either provide either a MOAIDataBuffer (via a :base64Decode type call) or string data (via a .base64Decode type call), but not both.
	@out	string output			If passed a string, returns either a string or nil depending on whether it could be decoded.  Otherwise the decoding occurs inline on the existing data buffer in this object, and nil is returned.
*/
mrb_value MOAIDataBuffer::_base64Decode ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	if ( state.ParamIsType ( 1, MRB_TT_STRING )) {
		//return state.Base64Decode ( 1 ) ? 1 : 0;
	}
	
	MOAIDataBuffer* self = state.GetRubyObject < MOAIDataBuffer >( 1, true );
	if ( self ) {
		if ( state.ParamIsType ( 2, MRB_TT_STRING )) {
			size_t len = RSTRING_LEN ( state.GetParamValue ( 2 ) );
			cc8* str = state.GetParamValue < cc8* > ( 2, "" );
			self->Load (( void* )str, len );
		}
		self->Base64Decode ();
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	base64Encode
	@text	If a string is provided, encodes it in base64.  Otherwise, encodes the current data stored in this object as a base64 encoded sequence of characters.

	@opt	MOAIDataBuffer self
	@opt	string data				The string data to encode.  You must either provide either a MOAIDataBuffer (via a :base64Encode type call) or string data (via a .base64Encode type call), but not both.
	@out	string output			If passed a string, returns either a string or nil depending on whether it could be encoded.  Otherwise the encoding occurs inline on the existing data buffer in this object, and nil is returned.
*/
mrb_value MOAIDataBuffer::_base64Encode ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	if ( state.ParamIsType ( 1, MRB_TT_STRING )) {
		//return state.Base64Encode ( 1 ) ? 1 : 0;
	}
	
	MOAIDataBuffer* self = state.GetRubyObject < MOAIDataBuffer >( 1, true );
	if ( self ) {
		if ( state.ParamIsType ( 2, MRB_TT_STRING )) {
			size_t len = RSTRING_LEN ( state.GetParamValue ( 2 ) );
			cc8* str = state.GetParamValue < cc8* > ( 2, "" );
			self->Load (( void* )str, len );
		}
		self->Base64Encode ();
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	clear
	@text	Release the data buffer's memory.

	@in		MOAIDataBuffer self
	@out	nil
*/
mrb_value MOAIDataBuffer::_clear ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDataBuffer, "U" );
	self->Clear ();
	return context;
}

//----------------------------------------------------------------//
/**	@lua	deflate
	@text	Compresses the string or the current data stored in this object using the DEFLATE algorithm.

	@overload
	@in		string data				The string data to deflate.
	@opt	number level			The level used in the DEFLATE algorithm.
	@opt	number windowBits		The window bits used in the DEFLATE algorithm.
	@out	string output			If passed a string, returns either a string or nil depending on whether it could be compressed.  Otherwise the compression occurs inline on the existing data buffer in this object, and nil is returned.

	@overload
	@in		MOAIDataBuffer self
	@opt	number level			The level used in the DEFLATE algorithm.
	@opt	number windowBits		The window bits used in the DEFLATE algorithm.
	@out	string output			If passed a string, returns either a string or nil depending on whether it could be compressed.  Otherwise the compression occurs inline on the existing data buffer in this object, and nil is returned.
*/
mrb_value MOAIDataBuffer::_deflate ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	int level = state.GetParamValue < int >( 2, ZLDeflateWriter::DEFAULT_LEVEL );
	int windowBits = state.GetParamValue < int >( 3, ZLDeflateWriter::DEFAULT_WBITS );

	if ( state.ParamIsType ( 1, MRB_TT_STRING )) {
		//return state.Deflate ( 1, level, windowBits ) ? 1 : 0;
	}
	
	MOAIDataBuffer* self = state.GetRubyObject < MOAIDataBuffer >( 1, true );
	if ( self ) {
		self->Deflate ( level, windowBits );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	getSize
	@text	Returns the number of bytes in this data buffer object.

	@in		MOAIDataBuffer self
	@out	number size				The number of bytes in this data buffer object.
*/
mrb_value MOAIDataBuffer::_getSize ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDataBuffer, "U" );

	void* bytes;
	size_t size;
	self->Lock ( &bytes, &size );
	
	self->Unlock ();

	return state.ToRValue ( size );
}

//----------------------------------------------------------------//
/**	@lua	getString
	@text	Returns the contents of the data buffer object as a string value.

	@in		MOAIDataBuffer self
	@out	string data				The data buffer object as a string.
*/
mrb_value MOAIDataBuffer::_getString ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDataBuffer, "U" );

	return self->PushString ( state );
}

//----------------------------------------------------------------//
/**	@lua	hexDecode
	@text	If a string is provided, decodes it as a hex encoded string.  Otherwise, decodes the current data stored in this object as a hex encoded sequence of bytes.

	@opt	MOAIDataBuffer self
	@opt	string data				The string data to decode.  You must either provide either a MOAIDataBuffer (via a :hexDecode type call) or string data (via a .hexDecode type call), but not both.
	@out	string output			If passed a string, returns either a string or nil depending on whether it could be decoded.  Otherwise the decoding occurs inline on the existing data buffer in this object, and nil is returned.
*/
mrb_value MOAIDataBuffer::_hexDecode ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	if ( state.ParamIsType ( 1, MRB_TT_STRING )) {
		//return state.HexDecode ( 1 ) ? 1 : 0;
	}
	
	MOAIDataBuffer* self = state.GetRubyObject < MOAIDataBuffer >( 1, true );
	if ( self ) {
		if ( state.ParamIsType ( 2, MRB_TT_STRING )) {
			size_t len = RSTRING_LEN ( state.GetParamValue ( 2 ) );
			cc8* str = state.GetParamValue < cc8* > ( 2, "" );
			self->Load (( void* )str, len );
		}
		self->HexDecode ();
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	hexEncode
	@text	If a string is provided, encodes it in hex.  Otherwise, encodes the current data stored in this object as a hex encoded sequence of characters.

	@opt	MOAIDataBuffer self
	@opt	string data				The string data to encode.  You must either provide either a MOAIDataBuffer (via a :hexEncode type call) or string data (via a .hexEncode type call), but not both.
	@out	string output			If passed a string, returns either a string or nil depending on whether it could be encoded.  Otherwise the encoding occurs inline on the existing data buffer in this object, and nil is returned.
*/
mrb_value MOAIDataBuffer::_hexEncode ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	if ( state.ParamIsType ( 1, MRB_TT_STRING )) {
		//return state.HexEncode ( 1 ) ? 1 : 0;
	}
	
	MOAIDataBuffer* self = state.GetRubyObject < MOAIDataBuffer >( 1, true );
	if ( self ) {
		if ( state.ParamIsType ( 2, MRB_TT_STRING )) {
			size_t len = RSTRING_LEN ( state.GetParamValue ( 2 ) );
			cc8* str = state.GetParamValue < cc8* > ( 2, "" );
			self->Load (( void* )str, len );
		}
		self->HexEncode ();
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	inflate
	@text	Decompresses the string or the current data stored in this object using the DEFLATE algorithm.

	@overload
	@in		string data				The string data to inflate.
	@opt	number windowBits		The window bits used in the DEFLATE algorithm.
	@out	string output			If passed a string, returns either a string or nil depending on whether it could be decompressed.  Otherwise the decompression occurs inline on the existing data buffer in this object, and nil is returned.

	@overload
	@in		MOAIDataBuffer self
	@opt	number windowBits		The window bits used in the DEFLATE algorithm.
	@out	string output			If passed a string, returns either a string or nil depending on whether it could be decompressed.  Otherwise the decompression occurs inline on the existing data buffer in this object, and nil is returned.
*/
mrb_value MOAIDataBuffer::_inflate ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	int windowBits = state.GetParamValue < int >( 2, ZLDeflateWriter::DEFAULT_WBITS );

	if ( state.ParamIsType ( 1, MRB_TT_STRING )) {
		//return state.Inflate ( 1, windowBits ) ? 1 : 0;
	}
	
	MOAIDataBuffer* self = state.GetRubyObject < MOAIDataBuffer >( 1, true );
	if ( self ) {
		self->Inflate ( windowBits );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	load
	@text	Copies the data from the given file into this object.  This method is a synchronous operation and will block until the file is loaded.

	@in		MOAIDataBuffer self
	@in		string filename			The path to the file that the data should be loaded from.
	@opt	number detectZip		One of MOAIDataBuffer.NO_INFLATE, MOAIDataBuffer.FORCE_INFLATE, MOAIDataBuffer.INFLATE_ON_EXT
	@opt	number windowBits		The window bits used in the DEFLATE algorithm.  Pass nil to use the default value.
	@out	boolean success			Whether the file could be loaded into the object.
*/
mrb_value MOAIDataBuffer::_load ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDataBuffer, "US" );

	cc8* filename	= state.GetParamValue < cc8* >( 2, "" );
	u32 detectZip	= state.GetParamValue < u32 >( 3, NO_INFLATE );
	int windowBits	= state.GetParamValue < int >( 4, ZLDeflateWriter::DEFAULT_WBITS );

	bool success = self->Load ( filename );
	
	if ( success && ( detectZip != NO_INFLATE )) {
		if (( detectZip == FORCE_INFLATE ) || ( MOAIDataBuffer::IsZipFilename ( filename ))) {
			success = self->Inflate ( windowBits );
		}
	}
	
	return state.ToRValue ( success );
}

//----------------------------------------------------------------//
/**	@lua	loadAsync
	@text	Asynchronously copies the data from the given file into this object.  This method is an asynchronous
			operation and will return immediately.

	@in		MOAIDataBuffer self
	@in		string filename			The path to the file that the data should be loaded from.
	@in		MOAITaskQueue queue		The queue to perform the loading operation.
	@opt	function callback		The function to be called when the asynchronous operation is complete. The MOAIDataBuffer is passed as the first parameter.
	@opt	number detectZip		One of MOAIDataBuffer.NO_INFLATE, MOAIDataBuffer.FORCE_INFLATE, MOAIDataBuffer.INFLATE_ON_EXT
	@opt	boolean inflateAsync	'true' to inflate on task thread. 'false' to inflate on subscriber thread. Default value is 'true.'
	@opt	number windowBits		The window bits used in the DEFLATE algorithm.  Pass nil to use the default value.
	@out	nil
*/
mrb_value MOAIDataBuffer::_loadAsync ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDataBuffer, "USU" );

	//cc8* filename			= state.GetParamValue < cc8* >( 1, "" );
	//MOAITaskQueue* queue	= state.GetRubyObject < MOAITaskQueue >( 2, true );
	//u32 detectZip			= state.GetParamValue < u32 >( 4, NO_INFLATE );
	//bool inflateAsync		= state.GetParamValue < bool >( 5, false );
	//int windowBits			= state.GetParamValue < int >( 6, ZLDeflateWriter::DEFAULT_WBITS );

	//if ( !queue ) return context;

	//MOAIDataIOTask* task = new MOAIDataIOTask ();
	////task->PushRubyUserdata ( state );
	//task->Init ( filename, *self, MOAIDataIOTask::LOAD_ACTION );
	////task->SetCallback ( L, 4 );
	//
	//if (( detectZip != NO_INFLATE ) && (( detectZip == FORCE_INFLATE ) || ( MOAIDataBuffer::IsZipFilename ( filename )))) {
	//	task->SetInflateOnLoad ( true, inflateAsync, windowBits );
	//}
	//
	//task->Start ( *queue, MOAIMainThreadTaskSubscriber::Get ());

	return context;
}

//----------------------------------------------------------------//
/**	@lua	save
	@text	Saves the data in this object to the given file.  This method is a synchronous operation and will block until the data is saved.

	@in		MOAIDataBuffer self
	@in		string filename			The path to the file that the data should be saved to.
	@out	boolean success			Whether the data could be saved to the file.
*/
mrb_value MOAIDataBuffer::_save ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDataBuffer, "US" );

	cc8* filename = state.GetParamValue < cc8* >( 1, "" );

	return state.ToRValue ( self->Save ( filename ) );
}

//----------------------------------------------------------------//
/**	@lua	saveAsync
	@text	Asynchronously saves the data in this object to the given file.  This method is an asynchronous
			operation and will return immediately.

	@in		MOAIDataBuffer self
	@in		string filename			The path to the file that the data should be saved to.
	@in		MOAITaskQueue queue		The queue to perform the saving operation.
	@opt	function callback		The function to be called when the asynchronous operation is complete. The MOAIDataBuffer is passed as the first parameter.
	@out	nil
*/
mrb_value MOAIDataBuffer::_saveAsync ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDataBuffer, "USU" );

	/*cc8* filename			= state.GetParamValue < cc8* >( 2, "" );
	MOAITaskQueue* queue	= state.GetRubyObject < MOAITaskQueue >( 3, true );

	if ( !queue ) return context;

	MOAIDataIOTask* task = new MOAIDataIOTask ();
	task->Init ( filename, *self, MOAIDataIOTask::SAVE_ACTION );
	task->SetCallback ( M, 4 );

	task->Start ( *queue, MOAIMainThreadTaskSubscriber::Get ());*/

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setString
	@text	Replaces the contents of this object with the string specified.

	@in		MOAIDataBuffer self
	@in		string data				The string data to replace the contents of this object with.
	@out	nil
*/
mrb_value MOAIDataBuffer::_setString ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDataBuffer, "US" );
	
	self->Load ( state, 2 );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	toCppHeader
	@text	Convert data to CPP header file.

	@overload

		@in		string data				The string data to encode
		@in		string name
		@opt	number columns			Default value is 12
		@out	string output
	
	@overload

		@in		MOAIDataBuffer data		The data buffer to encode
		@in		string name
		@opt	number columns			Default value is 12
		@out	string output
*/
mrb_value MOAIDataBuffer::_toCppHeader ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	cc8* name		= state.GetParamValue < cc8* >( 2, "" );
	u32 columns		= state.GetParamValue < u32 >( 3, 12 );
	
	if ( !strlen ( name )) return context;
	
	ZLMemStream memStream;
	
	if ( state.ParamIsType ( 1, MRB_TT_STRING )) {
		
		size_t size = RSTRING_LEN ( state.GetParamValue ( 1 ) );
		const void* bytes = state.GetParamValue < cc8* >( 1, "" );
		ZLHexDump::DumpAsCPPHeader ( memStream, name, bytes, size, columns );
	}
	
	MOAIDataBuffer* dataBuffer = state.GetRubyObject < MOAIDataBuffer >( 1, true );
	if ( dataBuffer ) {
		
		size_t size;
		void* bytes;
		dataBuffer->Lock ( &bytes, &size );
		ZLHexDump::DumpAsCPPHeader ( memStream, name, bytes, size, columns );
	}
	
	if ( memStream.GetLength ()) {
		
		memStream.Seek ( 0, SEEK_SET );
		STLString result = memStream.ReadString ( memStream.GetLength ());
		
		return state.ToRValue ( result.c_str () );
	}
	return context;
}

//================================================================//
// MOAIDataBuffer
//================================================================//

//----------------------------------------------------------------//
bool MOAIDataBuffer::Base64Decode () {

	ZLBase64Reader base64;
	return this->Decode ( base64 );
}

//----------------------------------------------------------------//
bool MOAIDataBuffer::Base64Encode () {

	ZLBase64Writer base64;
	return this->Encode ( base64 );
}

//----------------------------------------------------------------//
void MOAIDataBuffer::Clear () {

	this->mMutex.Lock ();
	this->mBytes.Clear ();
	this->mMutex.Unlock ();
}

//----------------------------------------------------------------//
bool MOAIDataBuffer::Decode ( ZLStreamAdapter& reader ) {
	
	this->mMutex.Lock ();
	
	ZLByteStream cryptStream;
	cryptStream.SetBuffer ( this->mBytes, this->mBytes.Size ());
	cryptStream.SetLength ( this->mBytes.Size ());
	
	ZLMemStream plainStream;
	
	reader.Open ( &cryptStream );
	plainStream.WriteStream ( reader );
	reader.Close ();
	
	size_t len = plainStream.GetLength ();
	this->mBytes.Init ( len );
	
	plainStream.Seek ( 0, SEEK_SET );
	plainStream.ReadBytes ( this->mBytes, len );
	
	this->mMutex.Unlock ();
	return true;
}

//----------------------------------------------------------------//
bool MOAIDataBuffer::Deflate ( int level, int windowBits ) {

	ZLDeflateWriter deflater;
	deflater.SetCompressionLevel ( level );
	deflater.SetWindowBits ( windowBits );
	
	return this->Encode ( deflater );
}

//----------------------------------------------------------------//
bool MOAIDataBuffer::Encode ( ZLStreamAdapter& writer ) {
	
	this->mMutex.Lock ();
	
	ZLMemStream stream;
	
	writer.Open ( &stream );
	writer.WriteBytes ( this->mBytes, this->mBytes.Size ());
	writer.Close ();
	
	size_t len = stream.GetLength ();
	this->mBytes.Init ( len );
	
	stream.Seek ( 0, SEEK_SET );
	stream.ReadBytes ( this->mBytes, len );

	this->mMutex.Unlock ();
	return true;
}

//----------------------------------------------------------------//
void* MOAIDataBuffer::GetBuffer () {
	return this->mBytes.Data ();
}

//----------------------------------------------------------------//
bool MOAIDataBuffer::HexDecode () {

	ZLHexAdapter hex;
	return this->Decode ( hex );
}

//----------------------------------------------------------------//
bool MOAIDataBuffer::HexEncode () {

	ZLHexAdapter hex;
	return this->Encode ( hex );
}

//----------------------------------------------------------------//
bool MOAIDataBuffer::Inflate ( int windowBits ) {

	ZLDeflateReader inflater;
	inflater.SetWindowBits ( windowBits );
	
	return this->Decode ( inflater );
}

//----------------------------------------------------------------//
bool MOAIDataBuffer::IsZipFilename ( cc8* filename ) {

	size_t filenameLength = strlen ( filename );
	if ( filenameLength > 3 ) {
		
		char ext [ 5 ];
		for ( size_t i = filenameLength - 4, j = 0; i < filenameLength; i++, j++ ) {
			ext [ j ] = ( char )tolower ( filename [ i ]);
		}
		ext [ 4 ] = 0;
		
		if ( strcmp ( ext, ".zip" ) == 0 ) {
			return true;
		}
		
		if ( strcmp ( &ext [ 1 ], ".gz" ) == 0 ) {
			return true;
		}
	}
	
	return false;
}

//----------------------------------------------------------------//
bool MOAIDataBuffer::Load ( cc8* filename ) {

	ZLFileStream in;
	if ( !in.OpenRead ( filename )) return false;

	this->mMutex.Lock ();

	size_t size = in.GetLength ();
	this->mBytes.Init ( size );
	in.ReadBytes ( this->mBytes , size );

	this->mMutex.Unlock ();

	return true;
}

//----------------------------------------------------------------//
void MOAIDataBuffer::Load ( void* bytes, size_t size ) {

	this->mMutex.Lock ();
	
	this->mBytes.Init ( size );
	memcpy ( this->mBytes.Data (), bytes, size );
	
	this->mMutex.Unlock ();
}

//----------------------------------------------------------------//
void MOAIDataBuffer::Load ( MOAIRubyState& state, int idx ) {

	size_t len = RSTRING_LEN ( state.GetParamValue ( 1 ) );
	//cc8* str = lua_tolstring ( state, idx, &len );
	cc8* str = state.GetParamValue < cc8* > ( 1, "" );
	
	this->Load (( void* )str, len );
}

//----------------------------------------------------------------//
void MOAIDataBuffer::Lock ( void** bytes, size_t* size ) {

	this->mMutex.Lock ();
	( *bytes ) = this->mBytes;
	( *size ) = this->mBytes.Size ();
}

//----------------------------------------------------------------//
MOAIDataBuffer::MOAIDataBuffer () {
	
	RTTI_SINGLE ( MOAIRubyObject )
}

//----------------------------------------------------------------//
MOAIDataBuffer::~MOAIDataBuffer () {

	this->Clear ();
}

//----------------------------------------------------------------//
mrb_value MOAIDataBuffer::PushString ( MOAIRubyState& state ) {

	size_t size;
	void* buffer;
	
	this->Lock ( &buffer, &size );
	//lua_pushlstring ( state, ( cc8* )buffer, size );
	this->Unlock ();

	return mrb_str_new ( state, ( cc8* )buffer, size );
}

//----------------------------------------------------------------//
size_t MOAIDataBuffer::Read ( void* buffer, size_t size ) {

	this->mMutex.Lock ();
	
	ZLByteStream byteStream;
	byteStream.SetBuffer ( this->mBytes, this->mBytes.Size (), this->mBytes.Size ());
	size = byteStream.ReadBytes ( buffer, size );
	
	this->mMutex.Unlock ();
	
	return size;
}

//----------------------------------------------------------------//
void MOAIDataBuffer::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "NO_INFLATE",		( u32 )NO_INFLATE );
	state.DefineClassConst ( klass, "FORCE_INFLATE",	( u32 )FORCE_INFLATE );
	state.DefineClassConst ( klass, "INFLATE_ON_EXT",	( u32 )INFLATE_ON_EXT );

	state.DefineStaticMethod ( klass, "base64Decode", _base64Decode, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "base64Encode", _base64Encode, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "deflate", _deflate, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "hexDecode", _hexDecode, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "hexEncode", _hexEncode, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "inflate", _inflate, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "toCppHeader", _toCppHeader, MRB_ARGS_ANY () );

}

//----------------------------------------------------------------//
void MOAIDataBuffer::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "base64Decode", _base64Decode, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "base64Encode", _base64Encode, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "clear", _clear, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "deflate", _deflate, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "getSize", _getSize, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getString", _getString, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "hexDecode", _hexDecode, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "hexEncode", _hexEncode, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "inflate", _inflate, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "load", _load, MRB_ARGS_ARG ( 1, 2 ) );
	state.DefineInstanceMethod ( klass, "loadAsync", _loadAsync, MRB_ARGS_ARG ( 2, 4 ) );
	state.DefineInstanceMethod ( klass, "save", _save, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "saveAsync", _saveAsync, MRB_ARGS_ARG ( 2, 1 ) );
	state.DefineInstanceMethod ( klass, "setString", _setString, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
bool MOAIDataBuffer::Save ( cc8* filename ) {

	ZLFileStream out;

	if ( !out.OpenWrite ( filename )) return false;

	this->mMutex.Lock ();
	out.WriteBytes ( this->mBytes , this->mBytes.Size ());
	this->mMutex.Unlock ();

	return true;
}

//----------------------------------------------------------------//
size_t MOAIDataBuffer::Size () {
	return this->mBytes.Size ();
}

//----------------------------------------------------------------//
void MOAIDataBuffer::Unlock () {

	this->mMutex.Unlock ();
}
