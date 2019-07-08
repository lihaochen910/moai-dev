// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGfxResourceClerk.h>
#include <moai-sim/MOAIImage.h>
#include <moai-sim/MOAITextureBase.h>
#include <moai-sim/strings.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getSize
	@text	Returns the width and height of the texture's source image.
			Avoid using the texture width and height to compute UV
			coordinates from pixels, as this will prevent texture
			resolution swapping.
	
	@in		MOAITextureBase self
	@out	number width
	@out	number height
*/
mrb_value MOAITextureBase::_getSize ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextureBase, "U" )
	
	self->DoCPUCreate ();
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( self->mWidth );
	ret [ 1 ] = state.ToRValue ( self->mHeight );

	return mrb_ary_new_from_values ( M, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	release
	@text	Releases any memory associated with the texture.
	
	@in		MOAITextureBase self
	@out	nil
*/
mrb_value MOAITextureBase::_release ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextureBase, "U" )
	
	self->Destroy ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setDebugName
	@text	Set a name for the texture to use during memory logging.
	
	@in		MOAITextureBase self
	@in		string debugName
	@out	nil
*/
mrb_value MOAITextureBase::_setDebugName ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextureBase, "U" )

	self->mDebugName = state.GetParamValue < cc8* >( 1, "" );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setFilter
	@text	Set default filtering mode for texture.
	
	@in		MOAITextureBase self
	@in		number min			One of MOAITextureBase.GL_LINEAR, MOAITextureBase.GL_LINEAR_MIPMAP_LINEAR, MOAITextureBase.GL_LINEAR_MIPMAP_NEAREST,
								MOAITextureBase.GL_NEAREST, MOAITextureBase.GL_NEAREST_MIPMAP_LINEAR, MOAITextureBase.GL_NEAREST_MIPMAP_NEAREST
	@opt	number mag			Defaults to value passed to 'min'.
	@out	nil
*/
mrb_value MOAITextureBase::_setFilter ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextureBase, "UN" )

	int min = state.GetParamValue < int >( 1, ZGL_SAMPLE_LINEAR );
	int mag = state.GetParamValue < int >( 2, min );

	MOAITextureBase::CheckFilterModes ( min, mag );

	self->SetFilter ( min, mag );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setWrap
	@text	Set wrapping mode for texture.
	
	@in		MOAITextureBase self
	@in		boolean wrap		Texture will wrap if true, clamp if not.
	@out	nil
*/
mrb_value MOAITextureBase::_setWrap ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextureBase, "UB" )
	
	bool wrap = state.GetParamValue < bool >( 1, false );
	
	self->mWrap = wrap ? ZGL_WRAP_MODE_REPEAT : ZGL_WRAP_MODE_CLAMP;

	return context;
}

//================================================================//
// MOAITextureBase
//================================================================//

//----------------------------------------------------------------//
void MOAITextureBase::CheckFilterModes ( int min, int mag ) {
	UNUSED ( min );
	UNUSED ( mag );

	#ifdef _DEBUG
		switch ( min ) {
			case ZGL_SAMPLE_LINEAR_MIPMAP_LINEAR:
			case ZGL_SAMPLE_LINEAR_MIPMAP_NEAREST:
			case ZGL_SAMPLE_NEAREST_MIPMAP_LINEAR:
			case ZGL_SAMPLE_NEAREST_MIPMAP_NEAREST:
		
				if ( !(( mag == ZGL_SAMPLE_LINEAR ) || ( mag == ZGL_SAMPLE_NEAREST ))) {

					ZLLog_Warning ( "WARNING: possibly incompatible filter modes; MIPMAP not supported for mag filter\n" );
				}
				break;
				
			default:
				break;
		}
	#endif
}

//----------------------------------------------------------------//
void MOAITextureBase::CleanupOnError () {

	this->mTextureSize = 0;
	MOAIGfxMgr::GetDrawingAPI ().DeleteResource ( this->mGLTexture );
	this->mWidth = 0;
	this->mHeight = 0;
	this->mTextureSize = 0;
}

//----------------------------------------------------------------//
bool MOAITextureBase::CreateTextureFromImage ( MOAIImage& srcImage ) {

	if ( !MOAIGfxMgr::Get ().GetHasContext ()) return false;

	ZLGfx& gfx = MOAIGfxMgr::GetDrawingAPI ();

	MOAIImage altImage;

	ZLColor::ColorFormat colorFormat = srcImage.GetColorFormat ();
	if ( colorFormat == ZLColor::CLR_FMT_UNKNOWN ) return false;

	if (( colorFormat == ZLColor::A_1 ) || ( colorFormat == ZLColor::A_4 )) {
		colorFormat = ZLColor::A_8;
	}
	
	if (( colorFormat != srcImage.GetColorFormat ()) || ( srcImage.GetPixelFormat () != MOAIImage::TRUECOLOR )) {
		//if ( !altImage.Convert ( srcImage, colorFormat, MOAIImage::TRUECOLOR )); // TODO: what was this? add error handling?
		altImage.Convert ( srcImage, colorFormat, MOAIImage::TRUECOLOR );
	}
	
	MOAIImage& image = altImage.IsOK () ? altImage : srcImage;
	if ( !image.IsOK ()) return false;

	//MOAIGfxMgr::Get ().ClearErrors ();
	this->mGLTexture = gfx.CreateTexture ();
	//if ( !this->mGLTexture ) return false;

	// get the dimensions before trying to get the OpenGL texture ID
	this->mWidth = image.GetWidth ();
	this->mHeight = image.GetHeight ();

	// warn if not a power of two (if we're supposed to generate mipmaps)
	if ( this->ShouldGenerateMipmaps () && !image.IsPow2 ()) {
		MOAILogF ( 0, ZLLog::LOG_WARNING, MOAISTRING_MOAITexture_NonPowerOfTwo_SDD, ( cc8* )this->mDebugName, this->mWidth, this->mHeight );
	}

	// GL_ALPHA
	// GL_RGB
	// GL_RGBA
	// GL_LUMINANCE
	// GL_LUMINANCE_ALPHA

	// GL_UNSIGNED_BYTE
	// GL_UNSIGNED_SHORT_5_6_5
	// GL_UNSIGNED_SHORT_4_4_4_4
	// GL_UNSIGNED_SHORT_5_5_5_1

	switch ( colorFormat ) {
		
		case ZLColor::A_8:
			this->mGLInternalFormat = ZGL_PIXEL_FORMAT_ALPHA;
			this->mGLPixelType = ZGL_PIXEL_TYPE_UNSIGNED_BYTE;
			break;
		
		case ZLColor::LA_8:
			this->mGLInternalFormat = ZGL_PIXEL_FORMAT_LUMINANCE_ALPHA;
			this->mGLPixelType = ZGL_PIXEL_TYPE_UNSIGNED_BYTE;
			break;
		
		case ZLColor::RGB_888:
			this->mGLInternalFormat = ZGL_PIXEL_FORMAT_RGB;
			this->mGLPixelType = ZGL_PIXEL_TYPE_UNSIGNED_BYTE;
			break;
		
		case ZLColor::RGB_565:
			this->mGLInternalFormat = ZGL_PIXEL_FORMAT_RGB;
			this->mGLPixelType = ZGL_PIXEL_TYPE_UNSIGNED_SHORT_5_6_5;
			break;
		
		case ZLColor::RGBA_5551:
			this->mGLInternalFormat = ZGL_PIXEL_FORMAT_RGBA;
			this->mGLPixelType = ZGL_PIXEL_TYPE_UNSIGNED_SHORT_5_5_5_1;
			break;
		
		case ZLColor::RGBA_4444:
			this->mGLInternalFormat = ZGL_PIXEL_FORMAT_RGBA;
			this->mGLPixelType = ZGL_PIXEL_TYPE_UNSIGNED_SHORT_4_4_4_4;
			break;
		
		case ZLColor::RGBA_8888:
			this->mGLInternalFormat = ZGL_PIXEL_FORMAT_RGBA;
			this->mGLPixelType = ZGL_PIXEL_TYPE_UNSIGNED_BYTE;
			break;
			
		default:
			this->CleanupOnError ();
			return false;
	}

	gfx.BindTexture ( this->mGLTexture );

	gfx.TexImage2D (
		0,
		this->mGLInternalFormat,
		this->mWidth,  
		this->mHeight,  
		this->mGLInternalFormat,
		this->mGLPixelType,
		image.GetBitmapBuffer ()
	);
	
	this->mTextureSize = image.GetBitmapSize ();

	// TODO: error handling
//	if ( MOAIGfxMgr::Get ().LogErrors ()) {
//		this->CleanupOnError ();
//		return false;
//	}
//	else if ( this->ShouldGenerateMipmaps ()) {
	
	if ( this->ShouldGenerateMipmaps ()) {
	
		u32 mipLevel = 1;
		
		MOAIImage mipmap;
		mipmap.Copy ( image );
		
		while ( mipmap.MipReduce ()) {
			
			gfx.TexImage2D (
				mipLevel++,  
				this->mGLInternalFormat,
				mipmap.GetWidth (),  
				mipmap.GetHeight (),  
				this->mGLInternalFormat,
				this->mGLPixelType,
				mipmap.GetBitmapBuffer ()
			);
			
			if ( MOAIGfxMgr::Get ().LogErrors ()) {
				this->CleanupOnError ();
				return false;
			}
			this->mTextureSize += mipmap.GetBitmapSize ();
		}
	}
	
	//MOAIGfxMgr::Get ().ReportTextureAlloc ( this->mDebugName, this->mTextureSize );
	
	return true;
}

//----------------------------------------------------------------//
u32 MOAITextureBase::GetHeight () {
	return this->mHeight;
}

//----------------------------------------------------------------//
u32 MOAITextureBase::GetWidth () {
	return this->mWidth;
}

//----------------------------------------------------------------//
MOAITextureBase::MOAITextureBase () :
	mWidth ( 0 ),
	mHeight ( 0 ),
	mMinFilter ( ZGL_SAMPLE_LINEAR ),
	mMagFilter ( ZGL_SAMPLE_NEAREST ),
	mWrap ( ZGL_WRAP_MODE_CLAMP ),
	mTextureSize ( 0 ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
		RTTI_EXTEND ( MOAIGfxResource )
	RTTI_END
}

//----------------------------------------------------------------//
MOAITextureBase::~MOAITextureBase () {

	this->OnGPUDeleteOrDiscard ( true );
}

//----------------------------------------------------------------//
bool MOAITextureBase::OnCPUCreate () {

	return true;
}

//----------------------------------------------------------------//
void MOAITextureBase::OnCPUDestroy () {
}

//----------------------------------------------------------------//
void MOAITextureBase::OnGfxEvent ( u32 event, void* userdata ) {

	MOAIGfxMgr::Get ().ReportTextureAlloc ( this->mDebugName, this->mTextureSize );
	MOAIGfxResource::OnGfxEvent ( event, userdata );
}

//----------------------------------------------------------------//
void MOAITextureBase::OnGPUBind () {

	MOAIGfxMgr::GetDrawingAPI ().BindTexture ( this->mGLTexture );
}

//----------------------------------------------------------------//
void MOAITextureBase::OnGPUDeleteOrDiscard ( bool shouldDelete ) {

	if ( this->mGLTexture.CanBind () && MOAIGfxMgr::IsValid ()) {
		MOAIGfxMgr::Get ().ReportTextureFree ( this->mDebugName, this->mTextureSize );
	}
	MOAIGfxResourceClerk::DeleteOrDiscard ( this->mGLTexture, shouldDelete );
	this->mGLTexture = ZLGfxHandle (); // clear out the handle
}

//----------------------------------------------------------------//
void MOAITextureBase::OnGPUUnbind () {

	MOAIGfxMgr::GetDrawingAPI ().BindTexture ( ZLGfxResource::UNBIND );
}

//----------------------------------------------------------------//
bool MOAITextureBase::OnGPUUpdate () {

	ZLGfx& gfx = MOAIGfxMgr::GetDrawingAPI ();

	gfx.TexParameteri ( ZGL_TEXTURE_WRAP_S, this->mWrap );
	gfx.TexParameteri ( ZGL_TEXTURE_WRAP_T, this->mWrap );
		
	gfx.TexParameteri ( ZGL_TEXTURE_MIN_FILTER, this->mMinFilter );
	gfx.TexParameteri ( ZGL_TEXTURE_MAG_FILTER, this->mMagFilter );
	
	return true;
}

//----------------------------------------------------------------//
void MOAITextureBase::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIGfxResource::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "GL_LINEAR",					( u32 )ZGL_SAMPLE_LINEAR );
	state.DefineClassConst ( klass, "GL_LINEAR_MIPMAP_LINEAR",		( u32 )ZGL_SAMPLE_LINEAR_MIPMAP_LINEAR );
	state.DefineClassConst ( klass, "GL_LINEAR_MIPMAP_NEAREST",	( u32 )ZGL_SAMPLE_LINEAR_MIPMAP_NEAREST );
	
	state.DefineClassConst ( klass, "GL_NEAREST",					( u32 )ZGL_SAMPLE_NEAREST );
	state.DefineClassConst ( klass, "GL_NEAREST_MIPMAP_LINEAR",	( u32 )ZGL_SAMPLE_NEAREST_MIPMAP_LINEAR );
	state.DefineClassConst ( klass, "GL_NEAREST_MIPMAP_NEAREST",	( u32 )ZGL_SAMPLE_NEAREST_MIPMAP_NEAREST );
	
	state.DefineClassConst ( klass, "GL_RGBA4",					( u32 )ZGL_PIXEL_FORMAT_RGBA4 );
	state.DefineClassConst ( klass, "GL_RGB5_A1",					( u32 )ZGL_PIXEL_FORMAT_RGB5_A1 );
	state.DefineClassConst ( klass, "GL_DEPTH_COMPONENT16",		( u32 )ZGL_PIXEL_FORMAT_DEPTH_COMPONENT16 );
	//***state.DefineClassConst ( klass, "GL_DEPTH_COMPONENT24",	( u32 )GL_DEPTH_COMPONENT24 );
	//***state.DefineClassConst ( klass, "GL_STENCIL_INDEX1",		( u32 )GL_STENCIL_INDEX1 );
	//***state.DefineClassConst ( klass, "GL_STENCIL_INDEX4",		( u32 )GL_STENCIL_INDEX4 );
	state.DefineClassConst ( klass, "GL_STENCIL_INDEX8",			( u32 )ZGL_PIXEL_FORMAT_STENCIL_INDEX8 );
	//***state.DefineClassConst ( klass, "GL_STENCIL_INDEX16",		( u32 )GL_STENCIL_INDEX16 );
	
	// TODO:
	#ifdef MOAI_OS_ANDROID
		state.DefineClassConst ( klass, "GL_RGB565",				( u32 )ZGL_PIXEL_FORMAT_RGB565 );
	#else
		state.DefineClassConst ( klass, "GL_RGBA8",				( u32 )ZGL_PIXEL_FORMAT_RGBA8 );
	#endif
}

//----------------------------------------------------------------//
void MOAITextureBase::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIGfxResource::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getSize", _getSize, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "release", _release, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setDebugName", _setDebugName, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setFilter", _setFilter, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "setWrap", _setWrap, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAITextureBase::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAITextureBase::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAITextureBase::SetFilter ( int filter ) {

	this->SetFilter ( filter, filter );
}

//----------------------------------------------------------------//
void MOAITextureBase::SetFilter ( int min, int mag ) {

	this->mMinFilter = min;
	this->mMagFilter = mag;
	
	this->ScheduleForGPUUpdate ();
}

//----------------------------------------------------------------//
void MOAITextureBase::SetGLTexture ( const ZLGfxHandle& glTexture, int internalFormat, int pixelType, size_t textureSize ) {

	this->mGLTexture = glTexture;
	this->mGLInternalFormat = internalFormat;
	this->mGLPixelType = pixelType;
	this->mTextureSize = textureSize;

	this->ScheduleForGPUUpdate ();
}

//----------------------------------------------------------------//
void MOAITextureBase::SetWrap ( int wrap ) {

	this->mWrap = wrap;
	
	this->ScheduleForGPUUpdate ();
}

//----------------------------------------------------------------//
bool MOAITextureBase::ShouldGenerateMipmaps () {

	return (
		( this->mMinFilter == ZGL_SAMPLE_LINEAR_MIPMAP_LINEAR ) ||
		( this->mMinFilter == ZGL_SAMPLE_LINEAR_MIPMAP_NEAREST ) ||
		( this->mMinFilter == ZGL_SAMPLE_NEAREST_MIPMAP_LINEAR ) ||
		( this->mMinFilter == ZGL_SAMPLE_NEAREST_MIPMAP_NEAREST )
	);
}

//----------------------------------------------------------------//
bool MOAITextureBase::UpdateTextureFromImage ( MOAIImage& image, ZLIntRect rect ) {

	// TODO: what happens when image is an unsupported format?

	// if we need to generate mipmaps or the dimensions have changed, clear out the old texture
	if ( this->ShouldGenerateMipmaps () || ( this->mWidth != image.GetWidth ()) || ( this->mHeight != image.GetHeight ())) {
	
		MOAIGfxMgr::Get ().ReportTextureFree ( this->mDebugName, this->mTextureSize );
		MOAIGfxResourceClerk::DeleteOrDiscard ( this->mGLTexture, false );
		
		if ( this->CreateTextureFromImage ( image )) {
			MOAIGfxMgr::Get ().ReportTextureAlloc ( this->mDebugName, this->mTextureSize );
			return true;
		}
		return false;
	}
	
	ZLGfx& gfx = MOAIGfxMgr::GetDrawingAPI ();
	
	// if the texture exists just update the sub-region
	// otherwise create a new texture from the image
	if ( this->mGLTexture.CanBind ()) {

		gfx.BindTexture ( this->mGLTexture );

		rect.Bless ();
		ZLIntRect imageRect = image.GetRect ();
		imageRect.Clip ( rect );
		
		ZLSharedConstBuffer* bitmapBuffer = image.GetBitmapBuffer ();
		
		MOAIImage subImage;
		if (( this->mWidth != ( u32 )rect.Width ()) || ( this->mHeight != ( u32 )rect.Height ())) {
			subImage.GetSubImage ( image, rect ); // TODO: need to convert to correct format for texture
			bitmapBuffer = subImage.GetBitmapBuffer ();
		}

		gfx.TexSubImage2D (
			0,
			rect.mXMin,
			rect.mYMin,
			rect.Width (),
			rect.Height (),
			this->mGLInternalFormat,
			this->mGLPixelType,  
			bitmapBuffer
		);
		
		MOAIGfxMgr::Get ().LogErrors ();
		
		return true;
	}
	
	return false;
}
