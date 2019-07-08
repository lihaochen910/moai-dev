// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGfxResourceClerk.h>
#include <moai-sim/MOAIIndexBuffer.h>
#include <moai-sim/MOAIVertexFormat.h>
#include <moai-sim/MOAIVertexFormatMgr.h>

//================================================================//
// MOAIGfxBufferBase
//================================================================//

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	copyFromStream
	@text	Copy the buffer contents from a stream. Optionally convert
			index size in bytes between streams.
			
			Warning: going from 4 bytes to 2 bytes is supported, but use
			at your own risk; truncations will not be reported.

	@overload

		@in		MOAIIndexBuffer self
		@in		MOAIStream stream
		@opt	number sourceSizeInBytes		Default value is 4.
		@out	nil

	@overload

		@in		MOAIIndexBuffer self
		@in		MOAIIndexBuffer indexBuffer		Source size in bytes taken from source buffer.
		@out	nil
*/
mrb_value MOAIIndexBuffer::_copyFromStream ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIIndexBuffer, "U" )
	
	// TODO: report trunctations!
	
	MOAIIndexBuffer* idxBuffer = state.GetRubyObject < MOAIIndexBuffer >( 1, false );
	if ( idxBuffer ) {
	
		size_t size = state.GetParamValue < u32 >( 2, ( u32 )( idxBuffer->GetLength () - idxBuffer->GetCursor () ));
		self->CopyFromStream ( *idxBuffer, size, idxBuffer->mIndexSize );
	}
	else {
	
		MOAIStream* stream = state.GetRubyObject < MOAIStream >( 1, true );
		if ( stream ) {
			size_t size = state.GetParamValue < u32 >( 2, ( u32 )( stream->GetLength () - stream->GetCursor () ));
			u32 srcInputSizeInBytes = state.GetParamValue ( 3, 4 );
			self->CopyFromStream ( *stream, size, srcInputSizeInBytes );
		}
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	countElements
	@text	Calculates the number of elements given a prim type.
 
	@in		MOAIIndexBuffer self
	@opt	number primType				Default value is GL_TRIANGLES.
	@out	number totalElements
*/
mrb_value MOAIIndexBuffer::_countElements ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIIndexBuffer, "U" )

	u32 totalElements = 0;
	
	// prim type, index size in bytes
	u32  primType = state.GetParamValue < u32 >( 1, ZGL_PRIM_TRIANGLES );
	
	totalElements = ( u32 )( self->GetSize () / self->mIndexSize );
	
	if ( primType == ZGL_PRIM_LINES ) {
		totalElements /= 2;
	}
	else if ( primType == ZGL_PRIM_TRIANGLES ) {
		totalElements /= 3;
	}
	
	return state.ToRValue ( totalElements );
}

//----------------------------------------------------------------//
/**	@lua	printIndices
	@text	Print the indices (for debugging purposes).
 
	@in		MOAIIndexBuffer self
	@out	nil
*/
mrb_value MOAIIndexBuffer::_printIndices ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIIndexBuffer, "U" )
	
	self->PrintIndices ();
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setIndexSize
	@text	Sets the index size in bytes.
	
			NOTE: The current implementation does not convert between sizes;
			going from 4 to 2 will result in twice as many indices.
 
	@in		MOAIIndexBuffer self
	@out	nil
*/
mrb_value MOAIIndexBuffer::_setIndexSize ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIIndexBuffer, "U" )
	
	u32 indexSize = state.GetParamValue < u32 >( 1, 4 );
	self->SetIndexSize ( indexSize ); // TODO: convert index size
	return context;
}

//================================================================//
// MOAIIndexBuffer
//================================================================//

//----------------------------------------------------------------//
u32 MOAIIndexBuffer::CountIndices () {

	return this->GetLength () / this->mIndexSize;
}

//----------------------------------------------------------------//
void MOAIIndexBuffer::CopyFromStream ( ZLStream& stream, size_t size, u32 srcInputSizeInBytes ) {

	u32 idxSizeInBytes = this->mIndexSize;	
	u32 totalIndices = ( u32 )( size / srcInputSizeInBytes );
	
	this->Reserve ( totalIndices * idxSizeInBytes );
	
	for ( u32 i = 0; i < totalIndices; ++i ) {
	
		u32 idx = 0;
		
		if ( srcInputSizeInBytes == 4 ) {
			idx = stream.Read < u32 >( 0 );
		}
		else {
			idx = stream.Read < u16 >( 0 );
		}
	
		if ( idxSizeInBytes == 4 ) {
			this->Write < u32 >( idx );
		}
		else {
			this->Write < u16 >(( u16 )idx );
		}
	}
	this->ScheduleForGPUUpdate ();
}

//----------------------------------------------------------------//
u32 MOAIIndexBuffer::GetIndex ( u32 element ) {

	const void* data = this->ZLCopyOnWrite::GetBuffer ();
	return this->mIndexSize == 4 ? (( const u32* )data )[ element ] : (( const u16* )data )[ element ];
}

//----------------------------------------------------------------//
MOAIIndexBuffer::MOAIIndexBuffer () :
	mIndexSize ( 2 ) {
	
	this->mTarget = ZGL_BUFFER_TARGET_ELEMENT_ARRAY;
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIGfxBuffer )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIIndexBuffer::~MOAIIndexBuffer () {
}

//----------------------------------------------------------------//
void MOAIIndexBuffer::PrintIndices () {

	size_t cursor = this->GetCursor ();
	u32 length = ( u32 )( cursor / this->mIndexSize );
	
	if ( length ) {
		this->Seek ( 0, SEEK_SET );
		for ( u32 i = 0; i < length; ++i ) {
		
			u32 v = this->mIndexSize == 4 ? this->Read < u32 >( 0 ) : ( u32 )this->Read < u16 >( 0 );
			printf ( "%d: %d\n", i, v );
		}
		this->SetCursor ( cursor );
	}
}

//----------------------------------------------------------------//
void MOAIIndexBuffer::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAIGfxBuffer::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIIndexBuffer::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIGfxBuffer::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "copyFromStream", _copyFromStream, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "countElements", _countElements, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "printIndices", _printIndices, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setIndexSize", _setIndexSize, MRB_ARGS_ARG ( 0, 1 ) );
	
}

//----------------------------------------------------------------//
void MOAIIndexBuffer::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	MOAIGfxBuffer::SerializeIn ( state, serializer );
	
	// this->mIndexSize = state.GetFieldValue < u32 >( -1, "mIndexSize", 0 );
}

//----------------------------------------------------------------//
void MOAIIndexBuffer::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	MOAIGfxBuffer::SerializeOut ( state, serializer );
	
	// state.SetField < u32 >( -1, "mIndexSize", this->mIndexSize );
}

//----------------------------------------------------------------//
void MOAIIndexBuffer::SetIndexSize ( u32 idxSize ) {

	assert (( idxSize == 2 ) || ( idxSize == 4 ));
	this->mIndexSize = idxSize;
}

//----------------------------------------------------------------//
void MOAIIndexBuffer::WriteIndex ( u32 index ) {

	if ( this->mIndexSize == 4 ) {
		this->Write < u32 >( index );
	}
	else {
		this->Write < u16 >(( u16 )index );
	}
}
