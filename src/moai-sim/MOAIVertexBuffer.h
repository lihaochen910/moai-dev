// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIVERTEXBUFFER_H
#define	MOAIVERTEXBUFFER_H

#include <moai-sim/MOAIGfxBuffer.h>

class MOAIVertexFormat;

//================================================================//
// MOAIVertexBuffer
//================================================================//
/**	@lua	MOAIIndexBuffer
	@text	Buffer for vertices.
*/
class MOAIVertexBuffer :
	public MOAIGfxBuffer {
private:
	
	friend class MOAIGfxPipelineClerk;
	friend class MOAIVertexBufferWithFormat;

	//----------------------------------------------------------------//
	static mrb_value		_computeBounds			( mrb_state* M, mrb_value context );
	static mrb_value		_countElements			( mrb_state* M, mrb_value context );
	static mrb_value		_printVertices			( mrb_state* M, mrb_value context );
	
public:
	
	DECL_RUBY_FACTORY ( MOAIVertexBuffer, MOAIGfxBuffer )
	
	//----------------------------------------------------------------//
					MOAIVertexBuffer		();
					~MOAIVertexBuffer		();
	void			PrintVertices			( MOAIVertexFormat& vertexFormat );
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			SerializeIn				( MOAIRubyState& state, MOAIDeserializer& serializer );
	void			SerializeOut			( MOAIRubyState& state, MOAISerializer& serializer );
};

#endif
