// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIMATERIALBATCH_H
#define	MOAIMATERIALBATCH_H

#include <moai-sim/MOAIMaterial.h>

class MOAIImage;
class MOAIShader;
class MOAITextureBase;

//================================================================//
// MOAIMaterialBatch
//================================================================//
/**	@lua	MOAIMaterialBatch
	@text	This is an indexed set of materials. A material is a shader, a texture
			and an optional hit mask. A material batch may be attached to a prop
			or a deck to change the materials used when rendering deck indices.
			Some decks also allow multiple material batch indicies to be specified
			per deck item. This is useful when rendering compound objects that
			need elements form multiple shaders and textures.
*/
class MOAIMaterialBatch :
	public virtual MOAIRubyObject {
private:
		
	u32								mIndexBatchSize;
	ZLLeanArray < MOAIMaterial >	mMaterials;
	
	//----------------------------------------------------------------//
	static mrb_value			_getBlendMode				( mrb_state* M, mrb_value context );
	static mrb_value			_getCullMode				( mrb_state* M, mrb_value context );
	static mrb_value			_getDepthMask				( mrb_state* M, mrb_value context );
	static mrb_value			_getDepthTest				( mrb_state* M, mrb_value context );
	static mrb_value			_getIndexBatchSize			( mrb_state* M, mrb_value context );
	static mrb_value			_getLight					( mrb_state* M, mrb_value context );
	static mrb_value			_getShader					( mrb_state* M, mrb_value context );
	static mrb_value			_getTexture					( mrb_state* M, mrb_value context );
	static mrb_value			_reserveMaterials			( mrb_state* M, mrb_value context );
	static mrb_value			_setBlendMode				( mrb_state* M, mrb_value context );
	static mrb_value			_setCullMode				( mrb_state* M, mrb_value context );
	static mrb_value			_setDepthMask				( mrb_state* M, mrb_value context );
	static mrb_value			_setDepthTest				( mrb_state* M, mrb_value context );
	static mrb_value			_setIndexBatchSize			( mrb_state* M, mrb_value context );
	static mrb_value			_setLight					( mrb_state* M, mrb_value context );
	static mrb_value			_setShader					( mrb_state* M, mrb_value context );
	static mrb_value			_setTexture					( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	static u32			GetMaterialID				( MOAIRubyState& state, int& idx );
	static u32			GetMaterialID				( MOAIRubyState& state, int& idx, bool& set );
	static u32			GetNamedGlobalID			( MOAIRubyState& state, int& idx, u32& globalID );

	//----------------------------------------------------------------//
	inline u32 GetRawIndex ( u32 idx ) {
	
		size_t totalMaterials = this->mMaterials.Size ();
		return ( totalMaterials && ( this->mIndexBatchSize > 0 )) ? ((( u32 )( idx / this->mIndexBatchSize )) % totalMaterials ) : 0;
	}

public:

	static const u32 UNKNOWN = 0xffffffff;

	GET_SET ( u32, IndexBatchSize, mIndexBatchSize )

	DECL_RUBY_FACTORY ( MOAIMaterialBatch, MOAIRubyObject )

	//----------------------------------------------------------------//
	MOAIMaterial&		AffirmMaterial				( u32 idx );
	void				Clear						();
	int					GetBlendEquation			( MOAIRubyState& state, int idx );
	mrb_value			GetBlendMode				( MOAIRubyState& state, int idx );
	mrb_value			GetCullMode					( MOAIRubyState& state, int idx );
	mrb_value			GetDepthMask				( MOAIRubyState& state, int idx );
	mrb_value			GetDepthTest				( MOAIRubyState& state, int idx );
	mrb_value			GetLight					( MOAIRubyState& state, int idx );
	MOAIMaterial*		GetMaterial					( u32 idx );
	mrb_value			GetShader					( MOAIRubyState& state, int idx );
	mrb_value			GetTexture					( MOAIRubyState& state, int idx );
						MOAIMaterialBatch			();
	virtual				~MOAIMaterialBatch			();
	MOAIMaterial*		RawGetMaterial				( u32 idx );
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void				Reserve						( u32 n );
	void				SerializeIn					( MOAIRubyState& state, MOAIDeserializer& serializer );
	void				SerializeOut				( MOAIRubyState& state, MOAISerializer& serializer );
	
	void				SetBlendMode				( u32 idx );
	void				SetBlendMode				( u32 idx, const MOAIBlendMode& blendMode );
	void				SetBlendMode				( MOAIRubyState& state, int idx );
	
	void				SetCullMode					( u32 idx );
	void				SetCullMode					( u32 idx, int cullMode );
	void				SetCullMode					( MOAIRubyState& state, int idx );
	
	void				SetDepthMask				( u32 idx );
	void				SetDepthMask				( u32 idx, bool depthMask );
	void				SetDepthMask				( MOAIRubyState& state, int idx );
	
	void				SetDepthTest				( u32 idx );
	void				SetDepthTest				( u32 idx, int depthTest );
	void				SetDepthTest				( MOAIRubyState& state, int idx );
	
	void				SetLight					( u32 idx, u32 name );
	void				SetLight					( u32 idx, u32 name, MOAILight* light );
	MOAILight*			SetLight					( MOAIRubyState& state, int idx );

	void				SetShader					( u32 idx );
	void				SetShader					( u32 idx, u32 shaderID );
	void				SetShader					( u32 idx, MOAIShader* shader );
	MOAIShader*			SetShader					( MOAIRubyState& state, int idx );

	void				SetTexture					( u32 idx );
	void				SetTexture					( u32 idx, MOAITextureBase* texture );
	void				SetTexture					( u32 idx, u32 name );
	void				SetTexture					( u32 idx, u32 name, MOAITextureBase* texture );
	MOAITextureBase*	SetTexture					( MOAIRubyState& state, int idx );

	size_t				Size						();

	
	//bool				TestHit						( MOAIMaterialBatch* override, u32 idx, float x, float y );
	//bool				TestHit						( MOAIMaterialBatch* override, u32 materialID, u32 deckIndex, float x, float y );
	//bool				TestHit						( MOAIMaterialBatch* override, u32 idx, u32 granularity, const ZLQuad& modelQuad, const ZLQuad& uvQuad, float x, float y ); // in local (model) space of the quad
	//bool				TestHit						( MOAIMaterialBatch* override, u32 materialID, u32 deckIndex, u32 granularity, const ZLQuad& modelQuad, const ZLQuad& uvQuad, float x, float y ); // in local (model) space of the quad
};

#endif
