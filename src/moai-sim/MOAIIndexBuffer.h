// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIINDEXBUFFER_H
#define	MOAIINDEXBUFFER_H

#include <moai-sim/MOAIGfxBuffer.h>

//================================================================//
// MOAIIndexBuffer
//================================================================//
/**	@lua	MOAIIndexBuffer
	@text	Buffer for vertex indices.
*/
class MOAIIndexBuffer :
	public MOAIGfxBuffer {
private:

	friend class MOAIGfxPipelineClerk;

	u32				mIndexSize;

	//----------------------------------------------------------------//
	static mrb_value		_copyFromStream			( mrb_state* M, mrb_value context );
	static mrb_value		_countElements			( mrb_state* M, mrb_value context );
	static mrb_value		_printIndices			( mrb_state* M, mrb_value context );
	static mrb_value		_setIndexSize			( mrb_state* M, mrb_value context );
	
public:
	
	GET ( u32, IndexSize, mIndexSize )
	
	DECL_RUBY_FACTORY ( MOAIIndexBuffer, MOAIGfxBuffer )
	
	//----------------------------------------------------------------//
	u32				CountIndices			();
	void			CopyFromStream			( ZLStream& stream, size_t size, u32 srcInputSizeInBytes );
	u32				GetIndex				( u32 element );
					MOAIIndexBuffer			();
					~MOAIIndexBuffer		();
	void			PrintIndices			();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			SerializeIn				( MOAIRubyState& state, MOAIDeserializer& serializer );
	void			SerializeOut			( MOAIRubyState& state, MOAISerializer& serializer );
	void			SetIndexSize			( u32 idxSize );
	void			WriteIndex				( u32 index );
};

#endif
