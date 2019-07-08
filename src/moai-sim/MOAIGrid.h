// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIGRID_H
#define	MOAIGRID_H

#include <moai-sim/MOAIGridSpace.h>
#include <moai-sim/MOAIDeck.h>

class MOAIMaterialBatch;

//================================================================//
// MOAIGrid
//================================================================//
/**	@lua	MOAIGrid
	@text	Grid data object. Grid cells are indexed starting and (1,1).
			Grid indices will wrap if out of range.
*/
class MOAIGrid :
	public MOAIGridSpace {
private:

	ZLLeanArray < u32 >			mTiles; // TODO: fix size

	//----------------------------------------------------------------//
	static mrb_value		_clearTileFlags		( mrb_state* M, mrb_value context );
	static mrb_value		_fill				( mrb_state* M, mrb_value context );
	static mrb_value		_getTile			( mrb_state* M, mrb_value context );
	static mrb_value		_getTileFlags		( mrb_state* M, mrb_value context );
	static mrb_value		_setRow				( mrb_state* M, mrb_value context );
	static mrb_value		_setTile			( mrb_state* M, mrb_value context );
	static mrb_value		_setTileFlags		( mrb_state* M, mrb_value context );
	static mrb_value		_streamTilesIn		( mrb_state* M, mrb_value context );
	static mrb_value		_streamTilesOut		( mrb_state* M, mrb_value context );
	static mrb_value		_toggleTileFlags	( mrb_state* M, mrb_value context );

protected:

	//----------------------------------------------------------------//
	void			OnResize			();

public:
	
	DECL_RUBY_FACTORY ( MOAIGrid, MOAIGridSpace )
	
	//----------------------------------------------------------------//
	void			Fill				( u32 value );
	u32				GetTile				( int addr ) const;
	u32				GetTile				( int xTile, int yTile ) const;
					MOAIGrid			();
					~MOAIGrid			();
	void			RegisterRubyClass	( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs	( MOAIRubyState& state, RClass* klass );
	void			RowFromString		( u32 rowID, cc8* str );
	STLString		RowToString			( u32 rowID );
	void			SerializeIn			( MOAIRubyState& state, MOAIDeserializer& serializer );
	void			SerializeOut		( MOAIRubyState& state, MOAISerializer& serializer );
	void			SetTile				( int addr, u32 tile );
	void			SetTile				( int xTile, int yTile, u32 tile );
	size_t			StreamTilesIn		( ZLStream* stream );
	size_t			StreamTilesOut		( ZLStream* stream );
};

#endif
