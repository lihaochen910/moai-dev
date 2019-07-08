// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIGFXBUFFER_H
#define	MOAIGFXBUFFER_H

#include <moai-sim/MOAIGfxResource.h>

class MOAIVertexFormat;

//================================================================//
// MOAIGfxBufferLoader
//================================================================//
class MOAIGfxBufferLoader {
public:

	STLString			mFilename;
	size_t				mOffset;
	size_t				mElemSize;
	size_t				mStride;
	size_t				mTotalElements;
};

//================================================================//
// MOAIGfxBuffer
//================================================================//
/**	@lua	MOAIGfxBuffer
	@text	Base class for MOAIVertexBuffer and MOAIIndexBuffer.
*/
class MOAIGfxBuffer :
	public MOAIGfxResource,
	public virtual MOAIStream,
	public ZLCopyOnWrite {
protected:
	
	friend class MOAIGfxPipelineClerk;
	friend class MOAIGfxStateGPUCache;
	friend class MOAIVertexArrayItem;
	
	enum {
		UPDATE_MODE_MAPBUFFER,
		UPDATE_MODE_ORPHAN,
		UPDATE_MODE_SUBDATA,
	};
	
	ZLLeanArray < ZLGfxHandle >		mVBOs;
	u32								mCurrentVBO;
	u32								mTarget;

	MOAIGfxBufferLoader*			mLoader;

	bool							mUseVBOs;
	bool							mCopyOnUpdate;

	//----------------------------------------------------------------//
	static mrb_value				_copyFromStream			( mrb_state* M, mrb_value context );
	static mrb_value				_release				( mrb_state* M, mrb_value context );
	static mrb_value				_reserve				( mrb_state* M, mrb_value context );
	static mrb_value				_reserveVBOs			( mrb_state* M, mrb_value context );
	static mrb_value				_scheduleFlush			( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	ZLSharedConstBuffer*	GetBufferForBind		( ZLGfx& gfx );
	bool					OnCPUCreate				();
	void					OnCPUDestroy			();
	void					OnGPUBind				();
	bool					OnGPUCreate				();
	void					OnGPUDeleteOrDiscard	( bool shouldDelete );
	void					OnGPUUnbind				();

public:
	
	GET ( size_t, BufferCount, mVBOs.Size ())
	GET ( u32, Target, mTarget )
	GET_SET ( bool, CopyOnUpdate, mCopyOnUpdate )
	
	IS ( UsingVBOs, mUseVBOs, true )
	
	//----------------------------------------------------------------//
	void						Clear					();
	void						CopyFromStream			( ZLStream& stream, size_t size );
	ZLSharedConstBuffer*		GetBuffer				();
								MOAIGfxBuffer			();
								~MOAIGfxBuffer			();
	bool						OnGPUUpdate				();
	void						RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void						RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void						Reserve					( u32 size );
	void						ReserveVBOs				( u32 gpuBuffers );
	void						SerializeIn				( MOAIRubyState& state, MOAIDeserializer& serializer );
	void						SerializeOut			( MOAIRubyState& state, MOAISerializer& serializer );
};

#endif
