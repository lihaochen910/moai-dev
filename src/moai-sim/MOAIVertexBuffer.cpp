// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGfxResourceClerk.h>
#include <moai-sim/MOAIVertexBuffer.h>
#include <moai-sim/MOAIVertexFormat.h>
#include <moai-sim/MOAIVertexFormatMgr.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	computeBounds
	@text	Calculates the buffer's axis-aligned bounding box. Returns
			nil if buffer is empty.
 
	@in		MOAIVertexBuffer self
	@in		MOAIVertexFormat format
	@out	xMin
	@out	yMin
	@out	zMin
	@out	xMax
	@out	yMax
	@out	zMax
*/
mrb_value MOAIVertexBuffer::_computeBounds ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVertexBuffer, "U" )
	
	bool hasBounds = false;
	ZLBox bounds;
	bounds.Init ( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
	
	// vertex format
	if ( state.CheckParams ( 1, "U", false )) {
	
		MOAIVertexFormat* format = state.GetRubyObject < MOAIVertexFormat >( 1, 0 );
		if ( format ) {
			u32 length = state.GetParamValue < u32 >( 2, ( u32 )self->GetCursor ());
			self->Seek ( 0, SEEK_SET );
			hasBounds = format->ComputeBounds ( bounds, *self, length );
		}
	}
	
	if ( hasBounds ) {
		return state.Get ( bounds );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	countElements
	@text	Get the number of vertices given either a vertex size or
			a vertex format.
 
	@overload
 
		@in		MOAIVertexBuffer self
		@in		number vertexSize
		@out	number vertexCount
	
	@overload
 
		@in		MOAIVertexBuffer self
		@in		MOAIVertexFormat format
		@out	number vertexCount
*/
mrb_value MOAIVertexBuffer::_countElements ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVertexBuffer, "U" )

	u32 totalElements = 0;
	
	// size
	if ( state.CheckParams ( 1, "N", false )) {
		
		u32  elementSize = state.GetParamValue < u32 >( 1, 4 );
		totalElements = ( u32 )( self->GetSize () / elementSize ); // TODO: cast
	}
	
	// vertex format
	else if ( state.CheckParams ( 1, "U", false )) {
	
		MOAIVertexFormat* format = state.GetRubyObject < MOAIVertexFormat >( 1, 0 );
		if ( format ) {
			u32 length = state.GetParamValue < u32 >( 2, ( u32 )self->GetLength ());
			totalElements = ( u32 )( length / format->GetVertexSize ());
		}
	}
	
	return state.ToRValue ( totalElements );
}

//----------------------------------------------------------------//
/**	@lua	printVertices
	@text	Print the vertices (for debugging purposes).
 
	@in		MOAIVertexBuffer self
	@in		MOAIVertexFormat format
	@out	nil
*/
mrb_value MOAIVertexBuffer::_printVertices ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVertexBuffer, "U" )
	
	MOAIVertexFormat* format = state.GetRubyObject < MOAIVertexFormat >( 1, true );
	if ( format ) {
		self->PrintVertices ( *format );
	}
	return context;
}

//================================================================//
// MOAIVertexBuffer
//================================================================//

//----------------------------------------------------------------//
MOAIVertexBuffer::MOAIVertexBuffer () {
	
	this->mTarget = ZGL_BUFFER_TARGET_ARRAY;
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIGfxBuffer )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIVertexBuffer::~MOAIVertexBuffer () {
}

//----------------------------------------------------------------//
void MOAIVertexBuffer::PrintVertices ( MOAIVertexFormat& vertexFormat ) {

	size_t cursor = this->GetCursor ();
	if ( cursor ) {
		this->SetCursor ( 0 );
		vertexFormat.PrintVertices ( *this, cursor );
		this->SetCursor ( cursor );
	}
}

//----------------------------------------------------------------//
void MOAIVertexBuffer::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAIGfxBuffer::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIVertexBuffer::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIGfxBuffer::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "computeBounds", _computeBounds, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "countElements", _countElements, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "printVertices", _printVertices, MRB_ARGS_REQ ( 1 ) );
	
}

//----------------------------------------------------------------//
void MOAIVertexBuffer::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	MOAIGfxBuffer::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIVertexBuffer::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	MOAIGfxBuffer::SerializeOut ( state, serializer );
}
