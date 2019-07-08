// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAILIGHT_H
#define	MOAILIGHT_H

#include <moai-sim/MOAINode.h>
#include <moai-sim/MOAIShaderUniformSchema.h>

class MOAILightFormat;
class MOAIShader;
class MOAITexture;

//================================================================//
// MOAILight
//================================================================//
class MOAILight :
	public virtual MOAINode,
	public MOAIShaderUniformSchema {
private:

	MOAIRubySharedPtr < MOAILightFormat >	mFormat;
	ZLLeanArray < u8 >						mBuffer;
	ZLLeanArray < MOAITexture* >			mTextures;

	//----------------------------------------------------------------//
	static mrb_value			_getFormat					( mrb_state* M, mrb_value context );
	static mrb_value			_setFormat					( mrb_state* M, mrb_value context );
	static mrb_value			_setTexture					( mrb_state* M, mrb_value context );
	static mrb_value			_setTransform				( mrb_state* M, mrb_value context );
	static mrb_value			_setUniform					( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	bool								MOAINode_ApplyAttrOp						( u32 attrID, MOAIAttribute& attr, u32 op );
	MOAIShaderUniformHandle				MOAIShaderUniformSchema_GetUniformHandle	( void* buffer, u32 uniformID ) const;

public:

	DECL_RUBY_FACTORY ( MOAILight, MOAINode )

	//----------------------------------------------------------------//
	void				ApplyUniforms				( void* buffer, size_t bufferSize );
	void				BindTextures				( u32 textureOffset );
						MOAILight					();
						~MOAILight					();
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void				SetFormat					( MOAILightFormat* format );
};

#endif
