// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAIMaterialBatch.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	clearTileFlags
	@text	Clears bits specified in mask.

	@in		MOAIGrid self
	@in		number xTile
	@in		number yTile
	@in		number mask
	@out	nil
*/
mrb_value MOAIGrid::_clearTileFlags ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGrid, "UNNN" )

	int xTile	= state.GetParamValue < int >( 1, 1 ) - 1;
	int yTile	= state.GetParamValue < int >( 2, 1 ) - 1;
	u32 mask	= state.GetParamValue < u32 >( 3, 0 );
	
	u32 tile = self->GetTile ( xTile, yTile );
	
	tile = tile & ~mask;
	
	self->SetTile ( xTile, yTile, tile );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	fill
	@text	Set all tiles to a single value

	@in		MOAIGrid self
	@in		number value
	@out	nil
*/
mrb_value MOAIGrid::_fill ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGrid, "UN" )

	u32 value	= state.GetParamValue < u32 >( 1, 1 );
	
	self->Fill ( value );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	getTile
	@text	Returns the value of a given tile.

	@in		MOAIGrid self
	@in		number xTile
	@in		number yTile
	@out	number tile
*/
mrb_value MOAIGrid::_getTile ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGrid, "UNN" )

	int xTile	= state.GetParamValue < int >( 1, 1 ) - 1;
	int yTile	= state.GetParamValue < int >( 2, 1 ) - 1;
	
	u32 tile = self->GetTile ( xTile, yTile );
	return state.ToRValue ( tile );
}

//----------------------------------------------------------------//
/**	@lua	getTileFlags
	@text	Returns the masked value of a given tile.

	@in		MOAIGrid self
	@in		number xTile
	@in		number yTile
	@in		number mask
	@out	number tile
*/
mrb_value MOAIGrid::_getTileFlags ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGrid, "UNNN" )

	int xTile	= state.GetParamValue < int >( 1, 1 ) - 1;
	int yTile	= state.GetParamValue < int >( 2, 1 ) - 1;
	u32 mask	= state.GetParamValue < u32 >( 3, 0 );
	
	u32 tile = self->GetTile ( xTile, yTile );
	
	tile = tile & mask;
	
	return state.ToRValue ( tile );
}

//----------------------------------------------------------------//
/**	@lua	setRow
	@text	Initializes a grid row given a variable argument list of values.

	@in		MOAIGrid self
	@in		number row
	@in		... values
	@out	nil
*/
mrb_value MOAIGrid::_setRow ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGrid, "UN" )

	u32 row = state.GetParamValue < u32 >( 1, 1 ) - 1;
	u32 total = state.GetParamsCount () - 1;
	
	for ( u32 i = 0; i < total; ++i ) {
	
		u32 tile = state.GetParamValue < u32 >( 2 + i, 0 );
		self->SetTile ( i, row, tile );
	}
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setTile
	@text	Sets the value of a given tile

	@in		MOAIGrid self
	@in		number xTile
	@in		number yTile
	@in		number value
	@out	nil
*/
mrb_value MOAIGrid::_setTile ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGrid, "UNNN" )

	int xTile	= state.GetParamValue < int >( 1, 1 ) - 1;
	int yTile	= state.GetParamValue < int >( 2, 1 ) - 1;
	u32 tile	= state.GetParamValue < u32 >( 3, 0 );
	
	self->SetTile ( xTile, yTile, tile );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setTileFlags
	@text	Sets a tile's flags given a mask.

	@in		MOAIGrid self
	@in		number xTile
	@in		number yTile
	@in		number mask
	@out	nil
*/
mrb_value MOAIGrid::_setTileFlags ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGrid, "UNNN" )

	int xTile	= state.GetParamValue < int >( 1, 1 ) - 1;
	int yTile	= state.GetParamValue < int >( 2, 1 ) - 1;
	u32 mask	= state.GetParamValue < u32 >( 3, 0 );
	
	u32 tile = self->GetTile ( xTile, yTile );
	
	tile = tile | mask;
	
	self->SetTile ( xTile, yTile, tile );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	streamTilesIn
	@text	Reads tiles directly from a stream. Call this only after
			initializing the grid. Only the content of the tiles
			buffer is read.

	@in		MOAIGrid self
	@in		MOAIStream stream
	@out	number bytesRead
*/
mrb_value MOAIGrid::_streamTilesIn ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGrid, "UU" )
	
	MOAIStream* stream = state.GetRubyObject < MOAIStream >( 1, true );
	if ( stream ) {
		return state.ToRValue ( ( u32 )self->StreamTilesIn ( stream ) );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	streamTilesOut
	@text	Writes tiles directly to a stream. Only the content of
			the tiles buffer is written.

	@in		MOAIGrid self
	@in		MOAIStream stream
	@out	number bytesWritten
*/
mrb_value MOAIGrid::_streamTilesOut ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGrid, "UU" )
	
	MOAIStream* stream = state.GetRubyObject < MOAIStream >( 1, true );
	if ( stream ) {
		return state.ToRValue ( ( u32 )self->StreamTilesOut ( stream ) );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	toggleTileFlags
	@text	Toggles a tile's flags given a mask.

	@in		MOAIGrid self
	@in		number xTile
	@in		number yTile
	@in		number mask
	@out	nil
*/
mrb_value MOAIGrid::_toggleTileFlags ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGrid, "UNNN" )

	int xTile	= state.GetParamValue < int >( 1, 1 ) - 1;
	int yTile	= state.GetParamValue < int >( 2, 1 ) - 1;
	u32 mask	= state.GetParamValue < u32 >( 3, 0 );
	
	u32 tile = self->GetTile ( xTile, yTile );
	
	tile = tile ^ mask;
	
	self->SetTile ( xTile, yTile, tile );
	
	return context;
}

//================================================================//
// MOAIGrid
//================================================================//

//----------------------------------------------------------------//
void MOAIGrid::Fill ( u32 value ) {

	this->mTiles.Fill ( value );
	return;
}

//----------------------------------------------------------------//
u32 MOAIGrid::GetTile ( int addr ) const {

	u32 size = ( u32 )this->mTiles.Size (); // TODO: cast

	if ( size ) {
		addr = addr % size;
		return this->mTiles [ addr ];
	}
	return 0;
}

//----------------------------------------------------------------//
u32 MOAIGrid::GetTile ( int xTile, int yTile ) const {

	MOAICellCoord coord ( xTile, yTile );
	if ( this->IsValidCoord ( coord )) {
		u32 addr = this->GetCellAddr ( coord );
		if ( addr < this->mTiles.Size ()) {
			return this->mTiles [ addr ];
		}
	}
	return 0;
}

//----------------------------------------------------------------//
MOAIGrid::MOAIGrid () {
	
	RTTI_SINGLE ( MOAIGridSpace )
}

//----------------------------------------------------------------//
MOAIGrid::~MOAIGrid () {
}

//----------------------------------------------------------------//
void MOAIGrid::OnResize () {

	// TODO: this should be smarter

	this->mTiles.Init ( this->GetTotalCells ());
	this->mTiles.Fill ( 0 );
}

//----------------------------------------------------------------//
void MOAIGrid::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIGridSpace::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIGrid::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIGridSpace::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "clearTileFlags", _clearTileFlags, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "fill", _fill, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getTile", _getTile, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "getTileFlags", _getTileFlags, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "setRow", _setRow, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setTile", _setTile, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "setTileFlags", _setTileFlags, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "streamTilesIn", _streamTilesIn, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "streamTilesOut", _streamTilesOut, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "toggleTileFlags", _toggleTileFlags, MRB_ARGS_REQ ( 3 ) );

}

//----------------------------------------------------------------//
void MOAIGrid::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	UNUSED ( serializer );

	// this->MOAIGridSpace::SerializeIn ( state, serializer );
	// this->mTiles.Init ( this->MOAIGridSpace::GetTotalCells ());

	// state.PushField ( -1, "mData" );

	// if ( state.IsType ( -1, LUA_TSTRING )) {
		
	// 	// TODO: all this is unsafe; don't assume sizes will be reasonable of that deflated data is guaranteed to be smaller; rewrite this with checking and recover in place
		
	// 	void* tiles = this->mTiles;
	// 	size_t tilesSize = this->mTiles.Size () * sizeof ( u32 );
		
	// 	STLString base64 = lua_tostring ( state, -1 ); 
	// 	base64.base_64_decode ( tiles, tilesSize );
		
	// 	ZLLeanArray < u8 > unzip;
	// 	ZLZip::Inflate ( this->mTiles, this->mTiles.Size () * sizeof ( u32 ), unzip );
		
	// 	tiles = unzip.Data ();
	// 	if ( unzip.Size () < tilesSize ) {
	// 		tilesSize = unzip.Size ();
	// 	}
	// 	memcpy ( this->mTiles, tiles, tilesSize );
	// }
	
	// lua_pop ( state, 1 );
}

//----------------------------------------------------------------//
void MOAIGrid::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	UNUSED ( serializer );

	// this->MOAIGridSpace::SerializeOut ( state, serializer );

	// ZLLeanArray < u8 > zip;
	// ZLZip::Deflate ( this->mTiles, this->mTiles.Size () * sizeof ( u32 ), zip );

	// STLString base64;
	// base64.base_64_encode ( zip.Data (), zip.Size ());
	
	// lua_pushstring ( state, base64.str ());
	// lua_setfield ( state, -2, "mData" );
}

//----------------------------------------------------------------//
void MOAIGrid::SetTile ( int addr, u32 tile ) {

	u32 size = ( u32 )this->mTiles.Size (); // TODO: cast

	if ( size ) {
		addr = addr % size;
		this->mTiles [ addr ] = tile;
	}
}

//----------------------------------------------------------------//
void MOAIGrid::SetTile ( int xTile, int yTile, u32 tile ) {

	MOAICellCoord coord ( xTile, yTile );
	if ( this->IsValidCoord ( coord )) {
	
		u32 addr = this->GetCellAddr ( coord );
		if ( addr < this->mTiles.Size ()) {
			this->mTiles [ addr ] = tile;
		}
	}
}

//----------------------------------------------------------------//
size_t MOAIGrid::StreamTilesIn ( ZLStream* stream ) {

	if ( !stream ) return 0;
	
	size_t size = this->mTiles.Size () * sizeof ( u32 );
	return stream->ReadBytes ( this->mTiles, size );
}

//----------------------------------------------------------------//
size_t MOAIGrid::StreamTilesOut ( ZLStream* stream ) {

	if ( !stream ) return 0;

	size_t size = this->mTiles.Size () * sizeof ( u32 );
	return stream->WriteBytes ( this->mTiles, size );
}
