// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGfxResourceClerk.h>
#include <moai-sim/MOAIImageFormatMgr.h>
#include <moai-sim/MOAITexture.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	load
	@text	Loads a texture from a data buffer or a file. Optionally pass
			in an image transform (not applicable to PVR textures).
	
	@overload
		@in		MOAITexture self
		@in		string filename
		@opt	number transform		Any bitwise combination of MOAIImage.QUANTIZE, MOAIImage.TRUECOLOR, MOAIImage.PREMULTIPLY_ALPHA
		@opt	string debugname		Name used when reporting texture debug information
		@out	nil
	
	@overload
		@in		MOAITexture self
		@in		MOAIImage image
		@opt	string debugname		Name used when reporting texture debug information
		@opt	boolean autoClear		Default value is 'false.' Only used if there is a reloader in play.
		@out	nil
	
	@overload
		@in		MOAITexture self
		@in		MOAIDataBuffer buffer
		@opt	number transform		Any bitwise combination of MOAIImage.QUANTIZE, MOAIImage.TRUECOLOR, MOAIImage.PREMULTIPLY_ALPHA
		@opt	string debugname		Name used when reporting texture debug information
		@out	nil
	
	@overload
		@in		MOAITexture self
		@in		MOAIStream buffer
		@opt	number transform		Any bitwise combination of MOAIImage.QUANTIZE, MOAIImage.TRUECOLOR, MOAIImage.PREMULTIPLY_ALPHA
		@opt	string debugname		Name used when reporting texture debug information
		@out	nil
*/
mrb_value MOAITexture::_load ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITexture, "U" )

	self->Init ( state, 1 );
	return context;
}

//================================================================//
// MOAITexture
//================================================================//

//----------------------------------------------------------------//
MOAITextureBase* MOAITexture::AffirmTexture ( MOAIRubyState& state, int idx ) {

	MOAITextureBase* textureBase = 0;
	
	textureBase = state.GetRubyObject < MOAITextureBase >( idx, false );
	if ( textureBase ) return textureBase;
	
	MOAITexture* texture = state.CreateClassInstance < MOAITexture >();
	if ( !texture->Init ( state, idx )) {
		// TODO: report error
		delete texture;
		texture = 0;
	}
	return texture;
}

//----------------------------------------------------------------//
void MOAITexture::Clear () {

	this->Destroy ();

	this->mFilename.clear ();
	this->mDebugName.clear ();
	
	if ( this->mImage && this->mAutoClearImage ) {
		this->mImage->Clear ();
	}
	
	this->mImage.Set ( *this, 0 );
	this->mAutoClearImage = false;
	
	if ( this->mTextureData ) {
		free ( this->mTextureData );
		this->mTextureData = 0;
	}
	this->mTextureDataSize = 0;
	this->mTextureDataFormat = 0;
}

//----------------------------------------------------------------//
bool MOAITexture::Init ( MOAIRubyState& state, int idx ) {

	u32 transform = MOAITexture::DEFAULT_TRANSFORM;
	cc8* debugName = 0;
	int debugNameIdx = 1;
	
	if ( state.ParamIsType ( idx + 1, MRB_TT_FIXNUM )) {
		transform = state.GetParamValue < u32 >( idx + 1, MOAITexture::DEFAULT_TRANSFORM );
		debugNameIdx++;
	}
	debugName = state.GetParamValue < cc8* >( debugNameIdx, 0 );
	
	bool done = false;

	if ( state.ParamIsType ( idx, MRB_TT_STRING )) {
		cc8* filename = state.GetParamValue < cc8* > ( idx, "" );
		transform = state.GetParamValue < u32 >( idx + 1, MOAITexture::DEFAULT_TRANSFORM );
		this->Init ( filename, transform, debugName ? debugName : filename );
		done = true;
	}
	
	if ( state.ParamIsType ( idx, MRB_TT_DATA )) {
	
		if ( !done ) {
			MOAIImage* image = state.GetRubyObject < MOAIImage >( idx, false );
			if ( image ) {
				bool autoClear	= state.GetParamValue < bool >( debugNameIdx + 1, false );
				this->Init ( *image, debugName ? debugName : "(texture from MOAIImage)", autoClear );
				done = true;
			}
		}
		
		if ( !done ) {
			MOAIDataBuffer* data = state.GetRubyObject < MOAIDataBuffer >( idx, false );
			if ( data ) {
				this->Init ( *data, transform, debugName ? debugName : "(texture from MOAIDataBuffer)" );
				done = true;
			}
		}
		
		if ( !done ) {
			MOAIStream* stream = state.GetRubyObject < MOAIStream >( idx, false );
			if ( stream ) {
				this->Init ( *stream, transform, debugName ? debugName : "(texture from MOAIStream)" );
				done = true;
			}
		}
	}
	return done;
}

//----------------------------------------------------------------//
void MOAITexture::Init ( MOAIImage& image, cc8* debugname, bool autoClear ) {

	this->Clear ();
	
	if ( image.IsOK ()) {
		this->mImage.Set ( *this, &image );
		this->mAutoClearImage = autoClear;
		this->mDebugName = debugname;
		this->FinishInit ();
		this->DoCPUCreate (); // If you do not calculate here, it is impossible to get the texture size.
	}
}

//----------------------------------------------------------------//
void MOAITexture::Init ( MOAIImage& image, int srcX, int srcY, int width, int height, cc8* debugname ) {

	this->Clear ();
	
	if ( image.IsOK ()) {
	
		this->mImage.Set ( *this, MOAIRubyRuntime::Get ().GetMainState ().CreateClassInstance < MOAIImage >());
		this->mAutoClearImage = true;
		
		this->mImage->Init ( width, height, image.GetColorFormat (), image.GetPixelFormat ());
		this->mImage->Blit ( image, srcX, srcY, 0, 0, width, height );
		this->mDebugName = debugname;
		this->FinishInit ();
		this->DoCPUCreate (); // If you do not calculate here, it is impossible to get the texture size.
	}
}

//----------------------------------------------------------------//
void MOAITexture::Init ( cc8* filename, u32 transform, cc8* debugname ) {

	this->Clear ();
	
	if ( MOAILogMgr::CheckFileExists ( filename )) {
		
		this->mFilename = ZLFileSys::GetAbsoluteFilePath ( filename );
		if ( debugname ) {
			this->mDebugName = debugname;
		}
		else {
			this->mDebugName = this->mFilename;
		}		
		this->mTransform = transform;
		this->FinishInit ();
		this->DoCPUCreate (); // If you do not calculate here, it is impossible to get the texture size.
	}
}

//----------------------------------------------------------------//
void MOAITexture::Init ( ZLStream& stream, u32 transform, cc8* debugname ) {

	this->Clear ();
	this->LoadFromStream ( stream, transform );
	
	// if we're OK, store the debugname and load
	if ( this->mTextureData || ( this->mImage && this->mImage->IsOK ())) {
		this->mDebugName = debugname;
		this->FinishInit ();
		this->DoCPUCreate (); // If you do not calculate here, it is impossible to get the texture size.
	}
}

//----------------------------------------------------------------//
void MOAITexture::Init ( MOAIDataBuffer& data, u32 transform, cc8* debugname ) {

	void* bytes;
	size_t size;
	data.Lock ( &bytes, &size );
	this->Init ( bytes, size, transform, debugname );
	data.Unlock ();
}

//----------------------------------------------------------------//
void MOAITexture::Init ( const void* data, size_t size, u32 transform, cc8* debugname ) {

	ZLByteStream stream;
	stream.SetBuffer ( data, size, size );
	this->Init ( stream, transform, debugname );
}

//----------------------------------------------------------------//
bool MOAITexture::LoadFromStream ( ZLStream& stream, u32 transform ) {
	UNUSED ( transform ); // TODO: why is transform unused?

	MOAIImageFormat* format = 0;
	bool result = false;

	format = MOAIImageFormatMgr::Get ().FindFormat ( stream );
	if ( format ) {
	
		MOAITextureInfo textureInfo;
	
		if ( format->GetTextureInfo ( stream, textureInfo )) {
			
			void* data = malloc ( textureInfo.mSize );
			size_t size = stream.ReadBytes ( data, textureInfo.mSize );
			
			if ( size == textureInfo.mSize ) {
			
				this->mTextureData = data;
				this->mTextureDataSize = size;
				this->mTextureDataFormat = format;
				
				this->mWidth = textureInfo.mWidth;
				this->mHeight = textureInfo.mHeight;
				
				result = true;
			}
			else {
				free ( data );
			}
		}
		else {
		
			MOAIImage* image = MOAIRubyRuntime::Get ().State ().CreateClassInstance < MOAIImage >();
			format->ReadImage ( *image, stream, this->mTransform );
			
			if ( image->IsOK ()) {
				this->mImage.Set ( *this, image );
				this->mAutoClearImage = true;
				this->mWidth = image->GetWidth ();
				this->mHeight = image->GetHeight ();
				result = true;
			}
			else {
				delete image;
			}
		}
	}
	return result;
}

//----------------------------------------------------------------//
MOAITexture::MOAITexture () :
	mTransform ( DEFAULT_TRANSFORM ),
	mTextureData ( 0 ),
	mTextureDataSize ( 0 ),
	mTextureDataFormat ( 0 ),
	mAutoClearImage ( false ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAITextureBase )
	RTTI_END
}

//----------------------------------------------------------------//
MOAITexture::~MOAITexture () {

	this->Clear ();
}

//----------------------------------------------------------------//
bool MOAITexture::OnCPUCreate () {

	if ( this->mFilename.size ()) {
		ZLFileStream stream;
		stream.OpenRead ( this->mFilename );
		this->LoadFromStream ( stream, this->mTransform );
		stream.Close ();
	}
	return (( this->mImage && this->mImage->IsOK ()) || this->mTextureData );
}

//----------------------------------------------------------------//
void MOAITexture::OnCPUDestroy () {

	// if we know the filename it is safe to clear out
	// the image and/or buffer
	if ( this->HasReloader () || this->mFilename.size ()) {
		
		// force cleanup right away - the image is now in OpenGL, why keep it around until the next GC?
		if ( this->mImage && this->mAutoClearImage ) {
			this->mImage->Clear ();
		}
		
		this->mImage.Set ( *this, 0 );
		
		if ( this->mTextureData ) {
			free ( this->mTextureData );
			this->mTextureData = 0;
		}
		this->mTextureDataSize = 0;
		this->mTextureDataFormat = 0;
	}
	
	if ( this->HasReloader ()) {
		this->mFilename.clear ();
	}
}

//----------------------------------------------------------------//
bool MOAITexture::OnGPUCreate () {
	
	bool success = false;
	
	if ( this->mImage && this->mImage->IsOK ()) {
		success =  this->CreateTextureFromImage ( *this->mImage );
	}
	else if ( this->mTextureDataFormat && this->mTextureData ) {
		success = this->mTextureDataFormat->CreateTexture ( *this, this->mTextureData, this->mTextureDataSize );
	}
	
	if ( !success ) {
		this->Clear ();
		return false;
	}
	
	return this->OnGPUUpdate ();
}

//----------------------------------------------------------------//
void MOAITexture::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	MOAITextureBase::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAITexture::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAITextureBase::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "load", _load, MRB_ARGS_ANY () );
	
}

//----------------------------------------------------------------//
void MOAITexture::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	MOAITextureBase::SerializeIn ( state, serializer );
	
	//STLString path = state.GetFieldValue ( -1, "mPath", "" );
	//
	//if ( path.size ()) {
	//	this->Init ( path, DEFAULT_TRANSFORM ); // TODO: serialization
	//}
}

//----------------------------------------------------------------//
void MOAITexture::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	MOAITextureBase::SerializeOut ( state, serializer );
	
	STLString path = ZLFileSys::GetRelativePath ( this->mFilename );
	state.SetField ( -1, "mPath", path.str ());
}
