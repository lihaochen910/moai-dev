// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIFancyGrid.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIMaterialBatch.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	fillColor
	@text	Set all tiles to a single color

	@in		MOAIFancyGrid self
	@in		number value
	@out	nil
*/
mrb_value MOAIFancyGrid::_fillColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFancyGrid, "U" )

	self->FillColor ( state.GetParamsCount () >= 4 ? state.GetColor32 ( 1, 0.0f, 0.0f, 0.0f, 0.0f ) : state.GetParamValue < u32 >( 1, 0 ));
	return context;
}

//----------------------------------------------------------------//
/**	@lua	getColor
	@text	Returns the color index of a given tile.

	@in		MOAIFancyGrid self
	@in		number xTile
	@in		number yTile
	@out	number color	index into palette
*/
mrb_value MOAIFancyGrid::_getColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFancyGrid, "UNN" )

	int xTile	= state.GetParamValue < int >( 1, 1 ) - 1;
	int yTile	= state.GetParamValue < int >( 2, 1 ) - 1;
	
	u32 tile = self->GetColor ( xTile, yTile );
	return state.ToRValue ( tile );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIFancyGrid::_getPaletteColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFancyGrid, "UN" )

	u32 idx	= state.GetParamValue < u32 >( 1, 1 );
	
	return state.ToRValue < MOAIRubyObject* >( self->GetPaletteColor ( idx ) );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIFancyGrid::_getTileColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFancyGrid, "UN" )

	int xTile	= state.GetParamValue < int >( 1, 1 ) - 1;
	int yTile	= state.GetParamValue < int >( 2, 1 ) - 1;

	ZLColorVec color = self->GetTileColor ( xTile, yTile );
	return state.Get ( color );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIFancyGrid::_reservePalette ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFancyGrid, "UN" )

	u32 size = state.GetParamValue < u32 >( 1, 0 );
	self->ReservePalette ( size );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setColor
	@text	Sets the color index of a given tile

	@in		MOAIFancyGrid self
	@in		number xTile
	@in		number yTile
	@in		number value
	@out	nil
*/
mrb_value MOAIFancyGrid::_setColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFancyGrid, "U" )

	int xTile	= state.GetParamValue < int >( 1, 1 ) - 1;
	int yTile	= state.GetParamValue < int >( 2, 1 ) - 1;
	u32 value	= state.GetParamsCount () >= 6 ? state.GetColor32 ( 3, 0.0f, 0.0f, 0.0f, 0.0f ) : state.GetParamValue < u32 >( 3, 0 );
	
	self->SetColor ( xTile, yTile, value );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIFancyGrid::_setColorRow ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFancyGrid, "UN" )

	u32 row = state.GetParamValue < u32 >( 1, 1 ) - 1;
	u32 total = state.GetParamsCount () - 1;
	
	for ( u32 i = 0; i < total; ++i ) {
	
		u32 color = state.GetParamValue < u32 >( 2 + i, 0 );
		self->SetColor ( i, row, color );
	}
	
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIFancyGrid::_setPaletteColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFancyGrid, "U" )

	u32 idx				= state.GetParamValue < u32 >( 1, 1 );
	MOAIColor* color	= state.GetRubyObject < MOAIColor >( 2, true );
	
	self->SetPaletteColor ( idx, color );
	return context;
}


//================================================================//
// MOAIFancyGrid
//================================================================//

//----------------------------------------------------------------//
void MOAIFancyGrid::AffirmColorSet () {

	size_t totalCells = this->GetTotalCells ();
	if ( totalCells ) {
		this->mColorSet = malloc ( totalCells * this->mColorSize );
	}
}

//----------------------------------------------------------------//
void MOAIFancyGrid::DiscardColorSet () {

	if ( this->mColorSet ) {
		free ( this->mColorSet );
		this->mColorSet = 0;
	}
}

//----------------------------------------------------------------//
void MOAIFancyGrid::DiscardPalette () {

	for ( size_t i = 0; i < this->mPalette.Size (); ++i ) {
		this->RubyRelease ( this->mPalette [ i ]);
	}
	this->mPalette.Clear ();
}

//----------------------------------------------------------------//
void MOAIFancyGrid::FillColor ( u32 value ) {

	if ( this->mColorSize ) {
	
		size_t totalCells = this->GetTotalCells ();
		
		switch ( this->mColorSize ) {
			
			case COLOR_SIZE_8:
				for ( size_t i = 0; i < totalCells; ++i ) {
					(( u8* )this->mColorSet )[ i ] = value & 0x000000ff;
				}
				break;
			
			case COLOR_SIZE_16:
				for ( size_t i = 0; i < totalCells; ++i ) {
					(( u16* )this->mColorSet )[ i ] = value & 0x0000ffff;
				}
				break;
			
			case COLOR_SIZE_32:
				for ( size_t i = 0; i < totalCells; ++i ) {
					(( u32* )this->mColorSet )[ i ] = value & 0xffffffff;
				}
				break;
		}
	}
}

//----------------------------------------------------------------//
u32 MOAIFancyGrid::GetColor ( int addr ) const {

	if ( this->mColorSet && ( addr < this->GetTotalCells ())) {

		switch ( this->mColorSize ) {
		
			case COLOR_SIZE_8:
				return (( u8* )this->mColorSet )[ addr ];
				break;
			
			case COLOR_SIZE_16:
				return (( u16* )this->mColorSet )[ addr ];
				break;
			
			case COLOR_SIZE_32:
				return (( u32* )this->mColorSet )[ addr ];
				break;
		}
	}
	return NO_COLOR;
}

//----------------------------------------------------------------//
u32 MOAIFancyGrid::GetColor ( int xTile, int yTile ) const {

	MOAICellCoord coord ( xTile, yTile );
	
	if ( this->IsValidCoord ( coord )) {
		return this->GetColor ( this->GetCellAddr ( coord ));
	}
	return 0;
}

//----------------------------------------------------------------//
MOAIColor* MOAIFancyGrid::GetPaletteColor ( u32 idx ) const {

	if ( idx < this->mPalette.Size ()) {
		return this->mPalette [ idx ];
	}
	return 0;
}

//----------------------------------------------------------------//
ZLColorVec MOAIFancyGrid::GetTileColor ( int addr ) const {

	size_t paletteSize = this->mPalette.Size ();
	ZLColorVec result = ZLColorVec::CLEAR;

	u32 color = this->GetColor ( addr );
	if ( paletteSize ) {
	
		MOAIColor* paletteColor = this->mPalette [ color ];
		if ( paletteColor ) {
			result = *paletteColor;
		}
	}
	else {
		result.SetRGBA ( color );
	}
	return result;
}

//----------------------------------------------------------------//
ZLColorVec MOAIFancyGrid::GetTileColor ( int xTile, int yTile ) const {

	MOAICellCoord coord ( xTile, yTile );
	
	if ( this->IsValidCoord ( coord )) {
		this->GetTileColor ( this->GetCellAddr ( coord ));
	}
	return ZLColorVec::WHITE;
}

//----------------------------------------------------------------//
MOAIFancyGrid::MOAIFancyGrid () :
	mColorSize ( COLOR_SIZE_32 ),
	mColorSet ( 0 ) {
	
	RTTI_SINGLE ( MOAIGrid )
}

//----------------------------------------------------------------//
MOAIFancyGrid::~MOAIFancyGrid () {

	this->DiscardColorSet ();
	this->DiscardPalette ();
}

//----------------------------------------------------------------//
void MOAIFancyGrid::OnResize () {

	MOAIGrid::OnResize ();
	this->DiscardColorSet ();
	this->AffirmColorSet ();
}

//----------------------------------------------------------------//
void MOAIFancyGrid::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIGrid::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIFancyGrid::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIGrid::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "fillColor", _fillColor, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getColor", _getColor, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "getPaletteColor", _getPaletteColor, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "reservePalette", _reservePalette, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setColor", _setColor, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "setColorRow", _setColorRow, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "setPaletteColor", _setPaletteColor, MRB_ARGS_REQ ( 2 ) );

}

//----------------------------------------------------------------//
void MOAIFancyGrid::ReservePalette ( u32 size ) {

	this->DiscardPalette ();

	if ( !size ) {
		this->mColorSize = COLOR_SIZE_32;
		this->DiscardColorSet ();
		return;
	}
	
	u32 colorSizeInBytes = COLOR_SIZE_8;
	
	if ( size > 0xff ) {
	
		colorSizeInBytes = COLOR_SIZE_16;
		
		if ( size > 0xffff ) {
		
			colorSizeInBytes = COLOR_SIZE_32;
		}
	}
	
	this->mPalette.Resize ( size );
	
	if ( this->mColorSize != colorSizeInBytes ) {
	
		this->DiscardColorSet ();
		this->AffirmColorSet ();
	}
}

//----------------------------------------------------------------//
void MOAIFancyGrid::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {

	MOAIGrid::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIFancyGrid::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {

	MOAIGrid::SerializeOut ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIFancyGrid::SetColor ( int addr, u32 value ) {

	if ( addr < this->GetTotalCells ()) {

		switch ( this->mColorSize ) {
		
			case COLOR_SIZE_8:
				(( u8* )this->mColorSet )[ addr ] = value & 0x000000ff;
				break;
			
			case COLOR_SIZE_16:
				(( u16* )this->mColorSet )[ addr ] = value & 0x0000ffff;
				break;
			
			case COLOR_SIZE_32:
				(( u32* )this->mColorSet )[ addr ] = value & 0xffffffff;
				break;
		}
	}
}

//----------------------------------------------------------------//
void MOAIFancyGrid::SetColor ( int xTile, int yTile, u32 value ) {

	MOAICellCoord coord ( xTile, yTile );
	
	if ( this->IsValidCoord ( coord )) {
		this->SetColor ( this->GetCellAddr ( coord ), value );
	}
}

//----------------------------------------------------------------//
void MOAIFancyGrid::SetPaletteColor ( u32 idx, MOAIColor* color ) {

	if ( idx < this->mPalette.Size ()) {
	
		MOAIColor* prevColor = this->mPalette [ idx ];
	
		if ( prevColor != color ) {
			this->RubyRelease ( prevColor );
		}
		this->RubyRetain ( color );
		this->mPalette [ idx ] = color;
	}
}

