// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIMATERIALBATCHHOLDER_H
#define	MOAIMATERIALBATCHHOLDER_H

#include <moai-sim/MOAIMaterialBatch.h>

class MOAIImage;
class MOAIMaterial;
class MOAIShader;
class MOAITextureBase;

//================================================================//
// MOAIMaterialBatchHolder
//================================================================//
// TODO: doxygen
class MOAIMaterialBatchHolder :
	public virtual MOAIRubyObject {
protected:

	MOAIRubySharedPtr < MOAIMaterialBatch > mMaterialBatch;

	//----------------------------------------------------------------//
	static mrb_value				_getBlendMode			( mrb_state* M, mrb_value context );
	static mrb_value				_getCullMode			( mrb_state* M, mrb_value context );
	static mrb_value				_getDepthMask			( mrb_state* M, mrb_value context );
	static mrb_value				_getDepthTest			( mrb_state* M, mrb_value context );
	static mrb_value				_getLight				( mrb_state* M, mrb_value context );
	static mrb_value				_getMaterialBatch		( mrb_state* M, mrb_value context );
	static mrb_value				_getShader				( mrb_state* M, mrb_value context );
	static mrb_value				_getTexture				( mrb_state* M, mrb_value context );
	static mrb_value				_reserveMaterials		( mrb_state* M, mrb_value context );
	static mrb_value				_setBlendMode			( mrb_state* M, mrb_value context );
	static mrb_value				_setCullMode			( mrb_state* M, mrb_value context );
	static mrb_value				_setDepthMask			( mrb_state* M, mrb_value context );
	static mrb_value				_setDepthTest			( mrb_state* M, mrb_value context );
	static mrb_value				_setIndexBatchSize		( mrb_state* M, mrb_value context );
	static mrb_value				_setLight				( mrb_state* M, mrb_value context );
	static mrb_value				_setMaterialBatch		( mrb_state* M, mrb_value context );
	static mrb_value				_setShader				( mrb_state* M, mrb_value context );
	static mrb_value				_setTexture				( mrb_state* M, mrb_value context );

public:

	DECL_RUBY_FACTORY ( MOAIMaterialBatchHolder, MOAIRubyObject )

	//----------------------------------------------------------------//
	MOAIMaterialBatch*		AffirmMaterialBatch			();
	MOAIMaterial*			GetMaterial					( u32 idx = 0 );
							MOAIMaterialBatchHolder		();
							~MOAIMaterialBatchHolder	();
	void					RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void					RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void					SerializeIn					( MOAIRubyState& state, MOAIDeserializer& serializer );
	void					SerializeOut				( MOAIRubyState& state, MOAISerializer& serializer );
};

#endif
