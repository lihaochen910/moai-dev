// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIIMAGETEXTURE_H
#define	MOAIIMAGETEXTURE_H

#include <moai-sim/MOAIImage.h>
#include <moai-sim/MOAITextureBase.h>

//================================================================//
// MOAIImageTexture
//================================================================//
/**	@lua	MOAIImageTexture
	@text	Binds an image (CPU memory) to a texture (GPU memory).
			Regions of the texture (or the entire texture) may be
			invalidated. Invalidated regions will be reloaded
			into GPU memory the next time the texture is bound.
*/
class MOAIImageTexture :
	public MOAITextureBase,
	public MOAIImage {
private:

	ZLIntRect	mRegion;

	//----------------------------------------------------------------//
	static mrb_value		_updateRegion			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void			OnClearDirty			();
	bool			OnGPUCreate				();
	bool			OnGPUUpdate				();
	void			OnImageStatusChanged	( bool isOK );

public:
	
	DECL_RUBY_FACTORY ( MOAIImageTexture, MOAIImage )
	
	static const u32 DEFAULT_TRANSFORM = MOAIImageTransform::TRUECOLOR | MOAIImageTransform::PREMULTIPLY_ALPHA;
	
	//----------------------------------------------------------------//
					MOAIImageTexture		();
					~MOAIImageTexture		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			SerializeIn				( MOAIRubyState& state, MOAIDeserializer& serializer );
	void			SerializeOut			( MOAIRubyState& state, MOAISerializer& serializer );
	void			UpdateRegion			();
	void			UpdateRegion			( ZLIntRect rect );
};

#endif
