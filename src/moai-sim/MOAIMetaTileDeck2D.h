// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIMETATILE2D_H
#define	MOAIMETATILE2D_H

//================================================================//
// MOAIMetaTile
//================================================================//
class MOAIMetaTile {
private:

	friend class MOAIMetaTileDeck2D;
	
	MOAICellCoord	mMin;
	MOAICellCoord	mMax;
	ZLVec2D			mOffset;

public:

	//----------------------------------------------------------------//
			MOAIMetaTile		();
};

//================================================================//
// MOAIMetaTileDeck2D
//================================================================//
/**	@lua	MOAIMetaTileDeck2D
	@text	This deck renders 'metatiles' which are sampled from a tile map.
			The tile map is specified by the attached grid and deck.
			Each 'brush' defines a rectangle of tiles to draw and an offset.
*/
class MOAIMetaTileDeck2D :
	public MOAIDeck {
private:
	
	ZLLeanArray < MOAIMetaTile > mBrushes;
	
	MOAIRubySharedPtr < MOAIGrid > mGrid;
	MOAIRubySharedPtr < MOAIDeck > mDeck;
	
	//----------------------------------------------------------------//
	static mrb_value		_reserveMetaTiles		( mrb_state* M, mrb_value context );
	static mrb_value		_setDeck				( mrb_state* M, mrb_value context );
	static mrb_value		_setGrid				( mrb_state* M, mrb_value context );
	static mrb_value		_setMetaTile			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	ZLBounds				MOAIDeck_ComputeMaxBounds		();
	void					MOAIDeck_Draw					( u32 idx );
	ZLBounds				MOAIDeck_GetBounds				( u32 idx );
	MOAICollisionShape*		MOAIDeck_GetCollisionShape		( u32 idx );
	bool					MOAIDeck_Overlap				( u32 idx, const ZLVec2D& vec, u32 granularity, ZLBounds* result );
	bool					MOAIDeck_Overlap				( u32 idx, const ZLVec3D& vec, u32 granularity, ZLBounds* result );

public:
	
	DECL_RUBY_FACTORY ( MOAIMetaTileDeck2D, MOAIDeck )
	
	//using MOAIDeck::DrawIndex;
	
	//----------------------------------------------------------------//
							MOAIMetaTileDeck2D		();
							~MOAIMetaTileDeck2D		();
	void					RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void					RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void					SerializeIn				( MOAIRubyState& state, MOAIDeserializer& serializer );
	void					SerializeOut			( MOAIRubyState& state, MOAISerializer& serializer );
};

#endif
