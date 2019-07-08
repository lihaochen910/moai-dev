// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIFRAMEBUFFERTEXTURE_H
#define	MOAIFRAMEBUFFERTEXTURE_H

#include <moai-sim/MOAIFrameBuffer.h>
#include <moai-sim/MOAITextureBase.h>

//================================================================//
// MOAIFrameBufferTexture
//================================================================//
/**	@lua	MOAIFrameBufferTexture
	@text	This is an implementation of a frame buffer that may be
			attached to a MOAIPartitionViewLayer for offscreen rendering. It is
			also a texture that may be bound and used like any other.
*/
class MOAIFrameBufferTexture :
	public MOAIFrameBuffer,
	public MOAITextureBase {
private:
	
	ZLGfxHandle			mGLColorBuffer;
	ZLGfxHandle			mGLDepthBuffer;
	ZLGfxHandle			mGLStencilBuffer;
	
	u32					mColorFormat;
	u32					mDepthFormat;
	u32					mStencilFormat;
	
	//----------------------------------------------------------------//
	static mrb_value	_init						( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	void				OnGPUBind					();
	bool				OnGPUCreate					();
	void				OnGPUDeleteOrDiscard		( bool shouldDelete );

	//----------------------------------------------------------------//
	void				MOAIFrameBuffer_AffirmBuffers		();

public:
	
	friend class MOAIGfxMgr;
	friend class MOAITextureBase;
	
	DECL_RUBY_FACTORY ( MOAIFrameBufferTexture, MOAIInstanceEventSource )
	
	//----------------------------------------------------------------//
	void				Init						( u32 width, u32 height, u32 colorFormat, u32 depthFormat, u32 stencilFormat );
						MOAIFrameBufferTexture		();
						~MOAIFrameBufferTexture		();
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void				SerializeIn					( MOAIRubyState& state, MOAIDeserializer& serializer );
	void				SerializeOut				( MOAIRubyState& state, MOAISerializer& serializer );
};

#endif
