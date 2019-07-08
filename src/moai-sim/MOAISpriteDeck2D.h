// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAISPRITEDECK2D_H
#define	MOAISPRITEDECK2D_H

#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIMaterialBatchHolder.h>
#include <moai-sim/MOAIQuadBrush.h>

//================================================================//
// MOAISprite
//================================================================//
class MOAISprite {
private:

	friend class MOAISpriteDeck2D;

	u32		mUVQuadID;
	u32		mQuadID;
	u32		mMaterialID;
};

//================================================================//
// MOAISpriteList
//================================================================//
class MOAISpriteList {
private:

	friend class MOAISpriteDeck2D;

	u32		mBaseSprite;
	u32		mTotalSprites;
};

//================================================================//
// MOAISpriteDeck2D
//================================================================//
/**	@lua	MOAISpriteDeck2D
	@text	Deck of lists of textured quads. UV and model space quads are
			specified independently and associated via pairs. Pairs are referenced
			by lists sequentially. There may be multiple pairs with the same
			UV/model quad indices if geometry is used in multiple lists.
*/
class MOAISpriteDeck2D :
	public MOAIDeck,
	public MOAIMaterialBatchHolder {
private:
	
	ZLLeanArray < ZLQuad >				mUVQuads;
	ZLLeanArray < ZLQuad >				mQuads;
	ZLLeanArray < MOAISprite >			mSprites;
	ZLLeanArray < MOAISpriteList >		mSpriteLists;
	
	//----------------------------------------------------------------//
	static mrb_value	_getQuad				( mrb_state* M, mrb_value context );
	static mrb_value	_getRect				( mrb_state* M, mrb_value context );
	static mrb_value	_getUVQuad				( mrb_state* M, mrb_value context );
	static mrb_value	_getUVRect				( mrb_state* M, mrb_value context );
	static mrb_value	_reserveQuads			( mrb_state* M, mrb_value context );
	static mrb_value	_reserveSprites			( mrb_state* M, mrb_value context );
	static mrb_value	_reserveSpriteLists		( mrb_state* M, mrb_value context );
	static mrb_value	_reserveUVQuads			( mrb_state* M, mrb_value context );
	static mrb_value	_setQuad				( mrb_state* M, mrb_value context );
	static mrb_value	_setRect				( mrb_state* M, mrb_value context );
	static mrb_value	_setSprite				( mrb_state* M, mrb_value context );
	static mrb_value	_setSpriteList			( mrb_state* M, mrb_value context );
	static mrb_value	_setUVQuad				( mrb_state* M, mrb_value context );
	static mrb_value	_setUVRect				( mrb_state* M, mrb_value context );
	static mrb_value	_transform				( mrb_state* M, mrb_value context );
	static mrb_value	_transformUV			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	ZLBounds				MOAIDeck_ComputeMaxBounds		();
	void					MOAIDeck_Draw					( u32 idx );
	ZLBounds				MOAIDeck_GetBounds				( u32 idx );
	MOAICollisionShape*		MOAIDeck_GetCollisionShape		( u32 idx );
	bool					MOAIDeck_Overlap				( u32 idx, const ZLVec2D& vec, u32 granularity, ZLBounds* result );
	bool					MOAIDeck_Overlap				( u32 idx, const ZLVec3D& vec, u32 granularity, ZLBounds* result );

public:
	
	DECL_RUBY_FACTORY ( MOAISpriteDeck2D, MOAIDeck )
	
	//----------------------------------------------------------------//
	static MOAIDeck*	AffirmDeck					( MOAIRubyState& state, int idx );
	bool				Contains					( u32 idx, const ZLVec2D& vec );
	void				DrawIndex					( u32 idx, MOAIMaterialBatch* materials, ZLVec3D offset, ZLVec3D scale );
	bool				Inside						( u32 idx, MOAIMaterialBatch* materials, u32 granularity, ZLVec3D vec, float pad );
						MOAISpriteDeck2D			();
						~MOAISpriteDeck2D			();
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void				ReserveLists				( u32 total );
	void				ReservePairs				( u32 total );
	void				ReserveQuads				( u32 total );
	void				ReserveUVQuads				( u32 total );
	void				SerializeIn					( MOAIRubyState& state, MOAIDeserializer& serializer );
	void				SerializeOut				( MOAIRubyState& state, MOAISerializer& serializer );
	void				SetList						( u32 idx, u32 basePairID, u32 totalPairs );
	void				SetPair						( u32 idx, u32 uvRectID, u32 screenRectID, u32 materialID );
	void				SetQuad						( u32 idx, ZLQuad& quad );
	void				SetRect						( u32 idx, ZLRect& rect );
	void				SetUVQuad					( u32 idx, ZLQuad& quad );
	void				SetUVRect					( u32 idx, ZLRect& rect );
	void				Transform					( const ZLAffine3D& mtx );
	void				TransformUV					( const ZLAffine3D& mtx );
};

#endif
