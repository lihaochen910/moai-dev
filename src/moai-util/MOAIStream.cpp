// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-util/MOAIStream.h>

#define MAX_STACK_BUFFER 1024

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	collapse
	@text	Removes a series of spans from the stream and "collapses" the
			remainder. Used to remove a series of regularly repeating bytes.
			For example, if the stream contains vertices and user wishes to
			remove the vertex normals.

	@in		MOAIStream self
	@in		number clipBase		Offset from the cursot to the first clip to remove.
	@in		number clipSize		Size of the clip to remove.
	@in		number chunkSize	The stride: the next clip will begin at clipBase + chunkSize.
	@opt	number size			The amount of the stream to process. Default is stream.getLength () - stream.getCursor ()
	@opt	boolean invert		Inverts the clip. Default value is false.
	@out	number result		The new size in bytes of the collapsed section of the stream.
*/
mrb_value MOAIStream::_collapse ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	
	MOAIStream* stream = state.GetRubyObject < MOAIStream >( 1, false );
	
	int idx = 2;
	if ( !stream ) {
		idx = 1;
		stream = self;
	}
	
	u32 clipBase		= state.GetParamValue < u32 >( idx++, 0 );
	u32 clipSize		= state.GetParamValue < u32 >( idx++, 0 );
	u32 chunkSize		= state.GetParamValue < u32 >( idx++, 0 );
	u32 size			= state.GetParamValue < u32 >( idx++, ( u32 )( stream->GetLength () - stream->GetCursor ()));
	bool invert			= state.GetParamValue < bool >( idx++, false );
	
	size_t result = self->Collapse ( *stream, clipBase, clipSize, chunkSize, size, invert );
	
	return state.ToRValue ( ( u32 ) result );
}

//----------------------------------------------------------------//
/**	@lua	compact
	@text	If the stream is backed by an internal buffer, and the buffer may
			be reallocated by the stream, compact () causes the buffer to be
			reallocated so that it more closely matches the current length of the
			stream.
			
			For streams that are not buffer backer or that may not be reallocated,
			compact () has no effect.

	@in		MOAIStream self
	@out	number
*/
mrb_value MOAIStream::_compact ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	self->Compact ();
	return context;
}

//----------------------------------------------------------------//
/**	@lua	flush
	@text	Forces any remaining buffered data into the stream.
	
	@in		MOAIStream self
	@out	nil
*/
mrb_value MOAIStream::_flush ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	self->Flush ();
	return context;
}

//----------------------------------------------------------------//
/**	@lua	getCursor
	@text	Returns the current cursor position in the stream.
	
	@in		MOAIStream self
	@out	number cursor
*/
mrb_value MOAIStream::_getCursor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	size_t cursor = self->GetCursor ();
	return state.ToRValue ( ( u32 )cursor );
}

//----------------------------------------------------------------//
/**	@lua	getLength
	@text	Returns the length of the stream.
	
	@in		MOAIStream self
	@out	number length
*/
mrb_value MOAIStream::_getLength ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	size_t length = self->GetLength ();
	return state.ToRValue ( ( u32 )length );
}

//----------------------------------------------------------------//
/**	@lua	read
	@text	Reads bytes from the stream.
	
	@in		MOAIStream self
	@opt	number byteCount		Number of bytes to read. Default value is the length of the stream.
	@out	string bytes			Data read from the stream.
	@out	number actualByteCount	Size of data successfully read.
*/
mrb_value MOAIStream::_read ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );

	size_t len = state.GetParamValue < u32 >( 1, ( u32 )self->GetLength ());
	
	if ( !len ) {
		mrb_value ret [ 2 ];
		ret [ 0 ] = state.ToRValue < cc8* >( "" );
		ret [ 1 ] = state.ToRValue ( 0 );
		return mrb_ary_new_from_values ( state, 2, ret );
	}
	
	char* buffer = 0;
	
	if ( len > MAX_STACK_BUFFER ) {
		buffer = ( char* )malloc ( len );
	}
	else {
		buffer = ( char* )alloca ( len );
	}
	
	len = self->ReadBytes ( buffer, len );

	mrb_value ret [ 2 ];
	
	if ( len ) {
		//lua_pushlstring ( state, buffer, len );
		ret [ 0 ] = state.ToRValue < cc8* >( buffer );
	}
	else {
		ret [ 0 ] = state.ToRValue < cc8* >( "" );
	}
	
	if ( len > MAX_STACK_BUFFER ) {
		free ( buffer );
	}
	
	ret [ 0 ] = state.ToRValue ( ( u32 )len );
	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	read8
	@text	Reads a signed 8-bit value from the stream.
	
	@in		MOAIStream self
	@out	number value		Value from the stream.
	@out	number size			Number of bytes successfully read.
*/
mrb_value MOAIStream::_read8 ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return self->ReadValues < s8 > ( state, 1 );
}

//----------------------------------------------------------------//
/**	@lua	read16
	@text	Reads a signed 16-bit value from the stream.
	
	@in		MOAIStream self
	@out	number value		Value from the stream.
	@out	number size			Number of bytes successfully read.
*/
mrb_value MOAIStream::_read16 ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return self->ReadValues < s16 >( state, 1 );
}

//----------------------------------------------------------------//
/**	@lua	read32
	@text	Reads a signed 32-bit value from the stream.
	
	@in		MOAIStream self
	@out	number value		Value from the stream.
	@out	number size			Number of bytes successfully read.
*/
mrb_value MOAIStream::_read32 ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return self->ReadValues < s32 >( state, 1 );
}

//----------------------------------------------------------------//
/**	@lua	readBoolean
	@text	Reads an 8-bit boolean value from the stream.
	
	@in		MOAIStream self
	@out	number value		Value from the stream.
	@out	number size			Number of bytes successfully read.
*/
mrb_value MOAIStream::_readBoolean ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return self->ReadValues < bool >( state, 1 );
}

//----------------------------------------------------------------//
/**	@lua	readDouble
	@text	Reads a 64-bit floating point value from the stream.
	
	@in		MOAIStream self
	@out	number value		Value from the stream.
	@out	number size			Number of bytes successfully read.
*/
mrb_value MOAIStream::_readDouble ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return self->ReadValues < double >( state, 1 );
}

//----------------------------------------------------------------//
/**	@lua	readFloat
	@text	Reads a 32-bit floating point value from the stream.
	
	@in		MOAIStream self
	@out	number value		Value from the stream.
	@out	number size			Number of bytes successfully read.
*/
mrb_value MOAIStream::_readFloat ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return self->ReadValues < float >( state, 1 );
}

//----------------------------------------------------------------//
/**	@lua	readFormat
	@text	Reads a series of values from the stream given a format string.
			Valid tokens for the format string are: u8 u16 u32 f d s8 s16 s32.
			Tokens may be optionally separated by spaces or commas.
	
	@in		MOAIStream self
	@in		string format
	@out	...	values			Values read from the stream or 'nil'.
	@out	number size			Number of bytes successfully read.
*/
mrb_value MOAIStream::_readFormat ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "US" );
	//return self->ReadFormat ( state, 1 );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIStream::_readString ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );

	size_t len = self->Read < u32 >( 0 );
	assert ( len < 1024 ); // TODO: should be defined somewhere

	if ( len > 0 ) {
		char* buffer = ( char* )alloca ( len + 1 );

		self->ReadBytes ( buffer, len );
		buffer [ len ] = 0;

		return mrb_str_new ( M, buffer, len );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	readU8
	@text	Reads an unsigned 8-bit value from the stream.
	
	@in		MOAIStream self
	@out	number value		Value from the stream.
	@out	number size			Number of bytes successfully read.
*/
mrb_value MOAIStream::_readU8 ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return self->ReadValues < u8 >( state, 1 );
}

//----------------------------------------------------------------//
/**	@lua	readU16
	@text	Reads an unsigned 16-bit value from the stream.
	
	@in		MOAIStream self
	@out	number value		Value from the stream.
	@out	number size			Number of bytes successfully read.
*/
mrb_value MOAIStream::_readU16 ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return self->ReadValues < u16 >( state, 1 );
}

//----------------------------------------------------------------//
/**	@lua	readU32
	@text	Reads an unsigned 32-bit value from the stream.
	
	@in		MOAIStream self
	@out	number value		Value from the stream.
	@out	number size			Number of bytes successfully read.
*/
mrb_value MOAIStream::_readU32 ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return self->ReadValues < u32 >( state, 1 );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIStream::_sample ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );

	u32 sampleSize		= state.GetParamValue < u32 >( 1, 1 );
	u32 streamType		= state.GetParamValue < u32 >( 2, ZLSample::SAMPLE_FLOAT );
	
	return state.ToRValue ( self->Sample ( streamType, sampleSize ) );
}

//----------------------------------------------------------------//
/**	@lua	seek
	@text	Repositions the cursor in the stream.
	
	@in		MOAIStream self
	@opt	number offset		Value from the stream. Default value is 0.
	@opt	number mode			One of MOAIStream.SEEK_CUR, MOAIStream.SEEK_END, MOAIStream.SEEK_SET.
								Default value is MOAIStream.SEEK_SET.
	@out	nil
*/
mrb_value MOAIStream::_seek ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	
	s32 offset	= state.GetParamValue < s32 >( 1, 0 );
	u32 mode	= state.GetParamValue < u32 >( 2, SEEK_SET );
	
	self->Seek ( offset, mode );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	write
	@text	Write binary data to the stream.
	
	@in		MOAIStream self
	@in		string bytes		Binary data to write.
	@opt	number size			Number of bytes to write. Default value is the size of the string.
	@out	number size			Number of bytes successfully written.
*/
mrb_value MOAIStream::_write ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "US" );

	size_t len = RSTRING_LEN ( state.GetParamValue ( 1 ) );
	cc8* str = state.GetParamValue < cc8* >( 1, "" );
	
	// TODO: 64bit
	size_t writeLen = state.GetParamValue < u32 >( 2, ( u32 )len );
	if ( len < writeLen ) {
		writeLen = len;
	}
	
	writeLen = self->WriteBytes ( str, writeLen );
	
	return state.ToRValue ( ( u32 )writeLen );
}

//----------------------------------------------------------------//
/**	@lua	write8
	@text	Writes a signed 8-bit value to the stream.
	
	@in		MOAIStream self
	@in		number value		Value to write.
	@out	number size			Number of bytes successfully written.
*/
mrb_value MOAIStream::_write8 ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return state.ToRValue ( self->WriteValues < s8 >( state, 1 ) );
}

//----------------------------------------------------------------//
/**	@lua	write16
	@text	Writes a signed 16-bit value to the stream.
	
	@in		MOAIStream self
	@in		number value		Value to write.
	@out	number size			Number of bytes successfully written.
*/
mrb_value MOAIStream::_write16 ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return state.ToRValue ( self->WriteValues < s16 >( state, 1 ) );
}

//----------------------------------------------------------------//
/**	@lua	write32
	@text	Writes a signed 32-bit value to the stream.
	
	@in		MOAIStream self
	@in		number value		Value to write.
	@out	number size			Number of bytes successfully written.
*/
mrb_value MOAIStream::_write32 ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return state.ToRValue ( self->WriteValues < s32 >( state, 1 ) );
}

//----------------------------------------------------------------//
/**	@lua	writeBoolean
	@text	Writes an 8-bit boolean value to the stream.
	
	@in		MOAIStream self
	@in		boolean value		Value to write.
	@out	number size			Number of bytes successfully written.
*/
mrb_value MOAIStream::_writeBoolean ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return state.ToRValue ( self->WriteValues < bool >( state, 1 ) );
}

//----------------------------------------------------------------//
/**	@lua	writeColor32
	@text	Write a packed 32-bit color to the vertex buffer.
	
	@in		MOAIStream self
	@opt	number r				Default value is 1.
	@opt	number g				Default value is 1.
	@opt	number b				Default value is 1.
	@opt	number a				Default value is 1.
	@out	nil
*/
mrb_value MOAIStream::_writeColor32 ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" )
	
	float r = state.GetParamValue < float >( 1, 1.0f );
	float g = state.GetParamValue < float >( 2, 1.0f );
	float b = state.GetParamValue < float >( 3, 1.0f );
	float a = state.GetParamValue < float >( 4, 1.0f );
	
	u32 color = ZLColor::PackRGBA ( r, g, b, a );
	self->Write < u32 >( color );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	writeDouble
	@text	Writes a 64-bit floating point value to the stream.
	
	@in		MOAIStream self
	@in		number value		Value to write.
	@out	number size			Number of bytes successfully written.
*/
mrb_value MOAIStream::_writeDouble ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return state.ToRValue ( self->WriteValues < double >( state, 1 ) );
}

//----------------------------------------------------------------//
/**	@lua	writeFloat
	@text	Writes a 32-bit floating point value to the stream.
	
	@in		MOAIStream self
	@in		number value		Value to write.
	@out	number size			Number of bytes successfully written.
*/
mrb_value MOAIStream::_writeFloat ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return state.ToRValue ( self->WriteValues < float >( state, 1 ) );
}

//----------------------------------------------------------------//
/**	@lua	writeFormat
	@text	Writes a series of values to the stream given a format string.
			See 'readFormat' for a list of valid format tokens.
	
	@in		MOAIStream self
	@in		string format
	@in		... values			Values to be written to the stream.
	@out	number size			Number of bytes successfully written.
*/
mrb_value MOAIStream::_writeFormat ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "US" );
	return state.ToRValue ( self->WriteFormat ( state, 1 ) );
}

//----------------------------------------------------------------//
/**	@lua	writeStream
	@text	Reads bytes from the given stream into the calling stream.
	
	@in		MOAIStream self
	@in		MOAIStream stream	Value to write.
	@opt	number size			Number of bytes to read/write. Default value is the length of the input stream.
	@out	number size			Number of bytes successfully written.
*/
mrb_value MOAIStream::_writeStream ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "UU" );
	
	MOAIStream* stream = state.GetRubyObject < MOAIStream >( 1, true );
	size_t result = 0;
	
	if ( stream ) {
		
		ZLStream* inStream = stream;
		ZLStream* outStream = self;
		
		if ( inStream && outStream ) {
		
			if ( state.ParamIsType ( 2, MRB_TT_FIXNUM )) {
				u32 size = state.GetParamValue < u32 >( 2, 0 );
				if ( size ) {
					result = outStream->WriteStream ( *inStream, size );
				}
			}
			else {
				result = outStream->WriteStream ( *inStream );
			}
		}
	}
	
	return state.ToRValue ( ( u32 )result );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIStream::_writeString ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );

	size_t len = RSTRING_LEN ( state.GetParamValue ( 1 ) );
	cc8* str = state.GetParamValue < cc8* >( 1, "" );
	
	size_t result = 0;
	
	self->Write < u32 >( ( u32 )len ); // TODO: assert on overflow; report error
	result = self->WriteBytes ( str, len );

	return state.ToRValue ( ( u32 )result );
}

//----------------------------------------------------------------//
/**	@lua	writeU8
	@text	Writes an unsigned 8-bit value to the stream.
	
	@in		MOAIStream self
	@in		number value		Value to write.
	@out	number size			Number of bytes successfully written.
*/
mrb_value MOAIStream::_writeU8 ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return state.ToRValue ( self->WriteValues < u8 >( state, 1 ) );
}

//----------------------------------------------------------------//
/**	@lua	writeU16
	@text	Writes an unsigned 16-bit value to the stream.
	
	@in		MOAIStream self
	@in		number value		Value to write.
	@out	number size			Number of bytes successfully written.
*/
mrb_value MOAIStream::_writeU16 ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return state.ToRValue ( self->WriteValues < u16 >( state, 1 ) );
}

//----------------------------------------------------------------//
/**	@lua	writeU32
	@text	Writes an unsigned 32-bit value to the stream.
	
	@in		MOAIStream self
	@in		number value		Value to write.
	@out	number size			Number of bytes successfully written.
*/
mrb_value MOAIStream::_writeU32 ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStream, "U" );
	return state.ToRValue ( self->WriteValues < u32 >( state, 1 ) );
}

//================================================================//
// MOAIStream
//================================================================//

//----------------------------------------------------------------//
MOAIStream::MOAIStream () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIStream::~MOAIStream () {
}

//----------------------------------------------------------------//
cc8* MOAIStream::ParseTypeToken ( cc8* format, u32& type ) {

	bool more = true;
	u32 result = UNKNOWN;
	u32 size = 0;
	
	while ( more ) {
	
		char c = *( format++ );
		
		switch ( c ) {
			
			case 'd':
				result = DOUBLE;
				more = false;
				break;
			case 'f':
				result = FLOAT;
				more = false;
				break;
			case 's':
				result = SIGNED;
				break;
			case 'u':
				result = UNSIGNED;
				break;
			case '1':
				size = 16;
				format++;
				more = false;
				break;
			case '3':
				size = 32;
				format++;
				more = false;
				break;
			case '8':
				size = 8;
				more = false;
				break;
			case 0:
				format = 0;
				more = false;
				break;
		}
	}
	
	if ( format && format [ 0 ]) {
		for ( ; *format == ','; format++ );
	}
	else {
		format = 0;
	}
	
	if ( size ) {
	
		if ( result == UNKNOWN ) {
			result = SIGNED;
		}

		if ( result == SIGNED ) {
			switch ( size ) {
				case 8:
					result = INT_8;
					break;
				case 16:
					result = INT_16;
					break;
				case 32:
					result = INT_32;
					break;
			}
		}
		
		if ( result == UNSIGNED ) {
			switch ( size ) {
				case 8:
					result = UINT_8;
					break;
				case 16:
					result = UINT_16;
					break;
				case 32:
					result = UINT_32;
					break;
			}
		}
	}
	
	type = result;
	return format;
}

//----------------------------------------------------------------//
int MOAIStream::ReadFormat ( MOAIRubyState& state, int idx ) {

	idx = state.AbsIndex ( idx );
	cc8* format = state.GetParamValue < cc8* >( idx, "" );
	
	size_t bytes = 0;
	u32 type = UNKNOWN;
	
	while ( format ) {
		
		format = MOAIStream::ParseTypeToken ( format, type );	
		
		switch ( type ) {
			case INT_8:
				bytes += this->ReadValue < s8 >( state );
				break;
			case INT_16:
				bytes += this->ReadValue < s16 >( state );
				break;
			case INT_32:
				bytes += this->ReadValue < s32 >( state );
				break;
			case DOUBLE:
				bytes += this->ReadValue < double >( state );
				break;
			case FLOAT:
				bytes += this->ReadValue < float >( state );
				break;
			case UINT_8:
				bytes += this->ReadValue < u8 >( state );
				break;
			case UINT_16:
				bytes += this->ReadValue < u16 >( state );
				break;
			case UINT_32:
				bytes += this->ReadValue < u32 >( state );
				break;
			default:
				format = 0;
		}
	}
	
	//state.Push (( u64 )bytes );
	return -1;
}

//----------------------------------------------------------------//
void MOAIStream::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "SEEK_CUR", state.ToRValue ( ( u32 )SEEK_CUR ) );
	state.DefineClassConst ( klass, "SEEK_END", state.ToRValue ( ( u32 )SEEK_END ) );
	state.DefineClassConst ( klass, "SEEK_SET", state.ToRValue ( ( u32 )SEEK_SET ) );
	
	state.DefineClassConst ( klass, "SAMPLE_S8",		state.ToRValue ( ( u32 )ZLSample::SAMPLE_S8 ) );
	state.DefineClassConst ( klass, "SAMPLE_U8",		state.ToRValue ( ( u32 )ZLSample::SAMPLE_U8 ) );
	state.DefineClassConst ( klass, "SAMPLE_S16",		state.ToRValue ( ( u32 )ZLSample::SAMPLE_S16 ) );
	state.DefineClassConst ( klass, "SAMPLE_U16",		state.ToRValue ( ( u32 )ZLSample::SAMPLE_U16 ) );
	state.DefineClassConst ( klass, "SAMPLE_S32",		state.ToRValue ( ( u32 )ZLSample::SAMPLE_S32 ) );
	state.DefineClassConst ( klass, "SAMPLE_U32",		state.ToRValue ( ( u32 )ZLSample::SAMPLE_U32 ) );
	state.DefineClassConst ( klass, "SAMPLE_FLOAT",		state.ToRValue ( ( u32 )ZLSample::SAMPLE_FLOAT ) );
}

//----------------------------------------------------------------//
void MOAIStream::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "collapse", _collapse, MRB_ARGS_ARG ( 3, 2 ) );
	state.DefineInstanceMethod ( klass, "compact", _compact, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "flush", _flush, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getCursor", _getCursor, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getLength", _getLength, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "read", _read, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "read8", _read8, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "read16", _read16, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "read32", _read32, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "readBoolean", _readBoolean, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "readDouble", _readDouble, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "readFloat", _readFloat, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "readFormat", _readFormat, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "readString", _readString, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "readU8", _readU8, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "readU16", _readU16, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "readU32", _readU32, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "sample", _sample, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "seek", _seek, MRB_ARGS_ARG ( 0, 2 ) );
	state.DefineInstanceMethod ( klass, "write", _write, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "write8", _write8, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "write16", _write16, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "write32", _write32, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "writeBoolean", _writeBoolean, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "writeColor32", _writeColor32, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "writeDouble", _writeDouble, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "writeFloat", _writeFloat, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "writeFormat", _writeFormat, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "writeStream", _writeStream, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "writeString", _writeString, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "writeU8", _writeU8, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "writeU16", _writeU16, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "writeU32", _writeU32, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
int MOAIStream::WriteFormat ( MOAIRubyState& state, int idx ) {

	cc8* format = state.GetParamValue < cc8* >( idx++, "" );
	
	size_t size;
	size_t result = 0;
	size_t bytes = 0;
	u32 type = UNKNOWN;
	
	while ( format ) {
	
		size = 0;
		format = MOAIStream::ParseTypeToken ( format, type );	
		result = this->GetCursor ();
		
		switch ( type ) {
			case INT_8:
				size = sizeof ( s8 );
				this->Write < s8 >( state.GetParamValue < s8 >( idx++, 0 ));
				break;
			case INT_16:
				size = sizeof ( s16 );
				this->Write < s16 >( state.GetParamValue < s16 >( idx++, 0 ));
				break;
			case INT_32:
				size = sizeof ( s32 );
				this->Write < s32 >( state.GetParamValue < s32 >( idx++, 0 ));
				break;
			case DOUBLE:
				size = sizeof ( double );
				this->Write < double >( state.GetParamValue < double >( idx++, 0 ));
				break;
			case FLOAT:
				size = sizeof ( float );
				this->Write < float >( state.GetParamValue < float >( idx++, 0 ));
				break;
			case UINT_8:
				size = sizeof ( u8 );
				this->Write < u8 >( state.GetParamValue < u8 >( idx++, 0 ));
				break;
			case UINT_16:
				size = sizeof ( u16 );
				this->Write < u16 >( state.GetParamValue < u16 >( idx++, 0 ));
				break;
			case UINT_32:
				size = sizeof ( u32 );
				this->Write < u32 >( state.GetParamValue < u32 >( idx++, 0 ));
				break;
			default:
				format = 0;
		}
		
		result = this->GetCursor () - result;
		bytes += result;
		if ( result != size ) {
			break;
		}
	}
	
	return ( u32 )bytes;
}
