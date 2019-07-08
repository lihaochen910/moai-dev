// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAITEXTUREBASE_H
#define	MOAITEXTUREBASE_H

#include <moai-sim/MOAIGfxResource.h>

class MOAIImage;

//================================================================//
// MOAITextureBase
//================================================================//
/**	@lua	MOAITextureBase
	@text	Base class for texture resources.

	@const	GL_LINEAR
	@const	GL_LINEAR_MIPMAP_LINEAR
	@const	GL_LINEAR_MIPMAP_NEAREST
	@const	GL_NEAREST
	@const	GL_NEAREST_MIPMAP_LINEAR
	@const	GL_NEAREST_MIPMAP_NEAREST
*/
class MOAITextureBase :
	public MOAIGfxResource {
protected:

	friend class MOAIGfxMgr;
	friend class MOAIGfxStateGPUCache;
	friend class MOAIImageFormat;

	// debug name for memory use reporting
	STLString			mDebugName;

	// GL texture
	ZLGfxHandle			mGLTexture;
	
	// size of the original texture
	u32					mWidth;
	u32					mHeight;

	// GL_LINEAR
	// GL_LINEAR_MIPMAP_LINEAR
	// GL_LINEAR_MIPMAP_NEAREST
	// GL_NEAREST
	// GL_NEAREST_MIPMAP_LINEAR
	// GL_NEAREST_MIPMAP_NEAREST
	int					mMinFilter;
	int					mMagFilter;
	
	// GL_CLAMP_TO_EDGE
	// GL_REPEAT
	int					mWrap;
	
	int					mGLInternalFormat;
	int					mGLPixelType;
	
	size_t				mTextureSize;

	//----------------------------------------------------------------//
	static mrb_value			_getSize				( mrb_state* M, mrb_value context );
	static mrb_value			_release				( mrb_state* M, mrb_value context );
	static mrb_value			_setDebugName			( mrb_state* M, mrb_value context );
	static mrb_value			_setFilter				( mrb_state* M, mrb_value context );
	static mrb_value			_setWrap				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void				CleanupOnError				();
	bool				CreateTextureFromImage		( MOAIImage& srcImage );
	bool				OnCPUCreate					();
	void				OnCPUDestroy				();
	void				OnGfxEvent					( u32 event, void* userdata );
	void				OnGPUBind					();
	void				OnGPUDeleteOrDiscard		( bool shouldDelete );
	void				OnGPUUnbind					();
	bool				OnGPUUpdate					();
	void				SetGLTexture				( const ZLGfxHandle& glTexture, int internalFormat, int pixelType, size_t textureSize );
	bool				ShouldGenerateMipmaps		();
	bool				UpdateTextureFromImage		( MOAIImage& image, ZLIntRect rect );
	
public:
	
	GET_SET ( cc8*, DebugName, mDebugName );
	GET_CONST ( ZLGfxHandle&, GLTexture, mGLTexture );

	//----------------------------------------------------------------//
	static void			CheckFilterModes			( int min, int mag );
	void				DeleteTexture				();
	u32					GetHeight					();
	u32					GetWidth					();
	bool				IsValid						();
						MOAITextureBase				();
						~MOAITextureBase			();
	void				SerializeIn					( MOAIRubyState& state, MOAIDeserializer& serializer );
	void				SerializeOut				( MOAIRubyState& state, MOAISerializer& serializer );
	void				SetFilter					( int filter );
	void				SetFilter					( int min, int mag );
	void				SetWrap						( int wrap );
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
};

#endif
