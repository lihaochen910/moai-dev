// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIGFXMGR_H
#define	MOAIGFXMGR_H

#include <moai-sim/MOAIBlendMode.h>
#include <moai-sim/MOAIColor.h>
#include <moai-sim/MOAIFrameBuffer.h>
#include <moai-sim/MOAIGfxState.h>
#include <moai-sim/MOAIGfxPipelineClerk.h>
#include <moai-sim/MOAIGfxResourceClerk.h>
#include <moai-sim/MOAIGfxStateVertexCache.h>
#include <moai-sim/MOAIImage.h>

class MOAIFrameBuffer;
class MOAIGfxResource;
class MOAIShader;
class MOAIShaderProgram;
class MOAITexture;
class MOAITextureBase;
class MOAIVertexFormat;

//================================================================//
// MOAIGfxMgr
//================================================================//
/**	@lua	MOAIGfxMgr
	@text	Interface to the graphics singleton.
	
	@const	EVENT_RESIZE
*/
class MOAIGfxMgr :
	public ZLContextClass < MOAIGfxMgr, MOAIGlobalEventSource > {
public:
	
	enum {
		EVENT_RESIZE,
		TOTAL_EVENTS,
	};
	
private:

	bool				mHasContext;

	bool				mIsFramebufferSupported;
	bool				mIsOpenGLES;

	u32					mMajorVersion;
	u32					mMinorVersion;
	
	size_t				mTextureMemoryUsage;
	u32					mMaxTextureSize;

	//----------------------------------------------------------------//
	static mrb_value			_enablePipelineLogging		( mrb_state* M, mrb_value context );
	static mrb_value			_getFrameBuffer				( mrb_state* M, mrb_value context );
	static mrb_value			_getMaxTextureSize			( mrb_state* M, mrb_value context );
	static mrb_value			_getMaxTextureUnits			( mrb_state* M, mrb_value context );
	static mrb_value			_getViewSize				( mrb_state* M, mrb_value context );
	static mrb_value			_purgeResources				( mrb_state* M, mrb_value context );
	static mrb_value			_renewResources				( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	void				OnGlobalsFinalize			();
	void				OnGlobalsInitialize			();
	
public:
	
	friend class MOAIGfxResource;
	friend class MOAIShaderProgram;
	friend class MOAITextureBase;
	
	DECL_RUBY_SINGLETON ( MOAIGfxMgr )
	
	GET ( size_t, TextureMemoryUsage, mTextureMemoryUsage )
	GET ( u32, MaxTextureSize, mMaxTextureSize )
	GET ( bool, HasContext, mHasContext )
	
	GET_BOOL ( IsOpenGLES, mIsOpenGLES )
	GET_BOOL ( IsFramebufferSupported, mIsFramebufferSupported )
	
	MOAIGfxResourceClerk		mResourceMgr;
	MOAIGfxState				mGfxState;
	MOAIGfxPipelineClerk		mPipelineMgr;
	
	//----------------------------------------------------------------//
	
	void			ClearErrors				();
	void			DetectContext			();
	void			DetectFramebuffer		();
	
	u32				LogErrors				();
	
					MOAIGfxMgr				();
					~MOAIGfxMgr				();

	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	
	void			ReportTextureAlloc		( cc8* name, size_t size );
	void			ReportTextureFree		( cc8* name, size_t size );
	
	void			ResetDrawCount			();
	
	void			SetBufferScale			( float scale );
	void			SetBufferSize			( u32 width, u32 height );
	
	//----------------------------------------------------------------//
	static ZLGfx& GetDrawingAPI () {
		
		return MOAIGfxMgr::Get ().mPipelineMgr.GetDrawingAPI ();
	}
};

#endif
