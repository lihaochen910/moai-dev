// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAIQuadBrush.h>
#include <moai-sim/MOAIMetaTileDeck2D.h>
#include <moai-sim/MOAITextureBase.h>
#include <moai-sim/MOAITransformBase.h>

//================================================================//
// MOAIMetaTile
//================================================================//
	
//----------------------------------------------------------------//
MOAIMetaTile::MOAIMetaTile () :
	mMin ( 0, 0 ),
	mMax ( 0, 0 ),
	mOffset ( 0.0f, 0.0f ) {
}

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	reserveMetaTiles
	@text	Set capacity of grid deck.
	
	@in		MOAIMetaTileDeck2D self
	@in		number nBrushes
	@out	nil
*/
mrb_value MOAIMetaTileDeck2D::_reserveMetaTiles ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMetaTileDeck2D, "UN" )
	
	u32 total = state.GetParamValue < u32 >( 1, 0 );
	self->mBrushes.Init ( total );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setDeck
	@text	Sets or clears the deck to be indexed by the grid.
	
	@in		MOAIMetaTileDeck2D self
	@opt	MOAIDeck deck		Default value is nil.
	@out	nil
*/
mrb_value MOAIMetaTileDeck2D::_setDeck ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMetaTileDeck2D, "U" )
	
	MOAIDeck* deck = state.GetRubyObject < MOAIDeck >( 1, true );
	self->mDeck.Set ( *self, deck );
	self->SetBoundsDirty ();
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setGrid
	@text	Sets or clears the grid to be sampled by the brushes.
	
	@in		MOAIMetaTileDeck2D self
	@opt	MOAIGrid grid		Default value is nil.
	@out	nil
*/
mrb_value MOAIMetaTileDeck2D::_setGrid ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMetaTileDeck2D, "U" )
	
	MOAIGrid* grid = state.GetRubyObject < MOAIGrid >( 1, true );
	self->mGrid.Set ( *self, grid );
	self->SetBoundsDirty ();
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setMetaTile
	@text	Initializes a metaTile.
	
	@in		MOAIMetaTileDeck2D self
	@in		number idx		Index of the brush.
	@in		number xTile
	@in		number yTile
	@in		number width
	@in		number height
	@opt	number xOff		Default value is 0.
	@opt	number yOff		Default value is 0.
	@out	nil
*/
mrb_value MOAIMetaTileDeck2D::_setMetaTile ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMetaTileDeck2D, "UNNNNN" )
	
	u32 idx = state.GetParamValue < int >( 1, 1 ) - 1;
	if ( idx < self->mBrushes.Size ()) {
	
		MOAIMetaTile& brush = self->mBrushes [ idx ];
		
		brush.mMin.mX		= state.GetParamValue < int >( 2, 1 ) - 1;
		brush.mMin.mY		= state.GetParamValue < int >( 3, 1 ) - 1;
		brush.mMax.mX		= state.GetParamValue < u32 >( 4, 0 ) + brush.mMin.mX - 1;
		brush.mMax.mY		= state.GetParamValue < u32 >( 5, 0 ) + brush.mMin.mY - 1;
		brush.mOffset.mX	= state.GetParamValue < float >( 6, 0.0f );
		brush.mOffset.mY	= state.GetParamValue < float >( 7, 0.0f );
		
		self->SetBoundsDirty ();
	}
	return mrb_nil_value ();
}

//================================================================//
// MOAIMetaTileDeck2D
//================================================================//

//----------------------------------------------------------------//
MOAIMetaTileDeck2D::MOAIMetaTileDeck2D () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIDeck )
	RTTI_END
	
	this->mMaxBounds.Init ( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
}

//----------------------------------------------------------------//
MOAIMetaTileDeck2D::~MOAIMetaTileDeck2D () {

	this->mGrid.Set ( *this, 0 );
	this->mDeck.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAIMetaTileDeck2D::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIDeck::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIMetaTileDeck2D::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIDeck::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "reserveMetaTiles",	_reserveMetaTiles, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setDeck",			_setDeck, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setGrid",			_setGrid, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setMetaTile",		_setMetaTile, MRB_ARGS_ARG ( 5, 2 ) );

}

//----------------------------------------------------------------//
void MOAIMetaTileDeck2D::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {

	MOAIDeck::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIMetaTileDeck2D::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {

	MOAIDeck::SerializeOut ( state, serializer );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
ZLBounds MOAIMetaTileDeck2D::MOAIDeck_ComputeMaxBounds () {

	u32 size = ( u32 )this->mBrushes.Size ();
	if ( size == 0 ) {
		return ZLBounds::EMPTY;
	}

	ZLBounds bounds = this->GetBounds ( 1 );
	for ( u32 i = 1; i < this->mBrushes.Size (); ++i ) {
		bounds.Grow ( this->GetBounds ( i ));
	}
	bounds.UpdateStatus ();
	return bounds;
}

//----------------------------------------------------------------//
void MOAIMetaTileDeck2D::MOAIDeck_Draw ( u32 idx ) {
	UNUSED ( idx );

	u32 size = ( u32 )this->mBrushes.Size ();
	if ( !size ) return;
	
	if ( !this->mGrid ) return;
	if ( !this->mDeck ) return;
	
	idx = ( idx - 1 ) % size;
	MOAIMetaTile& brush = this->mBrushes [ idx ];
	
	MOAICellCoord c0 = brush.mMin;
	MOAICellCoord c1 = brush.mMax;
	
	MOAIGrid& grid = *this->mGrid;
	
	float tileWidth = grid.GetTileWidth ();
	float tileHeight = grid.GetTileHeight ();
	
	//scale.Scale ( tileWidth, tileHeight, 1.0f );
	
	//offset.mX = offset.mX - ( c0.mX * tileWidth ) + brush.mOffset.mX;
	//offset.mY = offset.mY - ( c0.mY * tileHeight ) + brush.mOffset.mY;
	
	float xOff = brush.mOffset.mX - ( c0.mX * tileWidth );
	float yOff = brush.mOffset.mY - ( c0.mY * tileHeight );
	
	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	const ZLAffine3D& modelToWorldMtx = gfxState.GetMtx ( MOAIGfxState::MODEL_TO_WORLD_MTX );
	
	for ( int y = c0.mY; y <= c1.mY; ++y ) {
		for ( int x = c0.mX; x <= c1.mX; ++x ) {
			
			MOAICellCoord wrap = grid.WrapCellCoord ( x, y );
			idx = grid.GetTile ( wrap.mX, wrap.mY );
			
			MOAICellCoord coord ( x, y );
			ZLVec3D loc = grid.GetTilePoint ( coord, MOAIGridSpace::TILE_CENTER, 0.0f );
			
			ZLAffine3D mtx = modelToWorldMtx;
			mtx.PrependSclTr2D ( tileWidth, tileHeight, xOff + loc.mX, yOff + loc.mY );
			gfxState.SetMtx ( MOAIGfxState::MODEL_TO_WORLD_MTX, mtx );

			//this->mDeck->Draw ( MOAIDeckRemapper::Remap ( this->mRemapper, idx ), materials, loc, scale );
			this->mDeck->Draw ( idx );
		}
	}
}

//----------------------------------------------------------------//
ZLBounds MOAIMetaTileDeck2D::MOAIDeck_GetBounds ( u32 idx ) {
	
	u32 size = ( u32 )this->mBrushes.Size ();
	if ( this->mGrid && size ) {
		
		// TODO: handle oversized decks (don't assume unit sized deck items)
		idx = idx % size;
		
		MOAIMetaTile& brush = this->mBrushes [ idx ];
		ZLRect rect = this->mGrid->GetBounds ( brush.mMin, brush.mMax );
		rect.Offset ( brush.mOffset.mX - rect.mXMin, brush.mOffset.mY - rect.mYMin );
		return ZLBounds ( rect );
	}
	return ZLBounds::EMPTY;
}

//----------------------------------------------------------------//
MOAICollisionShape* MOAIMetaTileDeck2D::MOAIDeck_GetCollisionShape ( u32 idx ) {
	UNUSED ( idx );

	return 0;
}

//----------------------------------------------------------------//
bool MOAIMetaTileDeck2D::MOAIDeck_Overlap ( u32 idx, const ZLVec2D& vec, u32 granularity, ZLBounds* result ) {
	UNUSED ( idx );
	UNUSED ( vec );
	UNUSED ( granularity );
	UNUSED ( result );

	return false;
}

//----------------------------------------------------------------//
bool MOAIMetaTileDeck2D::MOAIDeck_Overlap ( u32 idx, const ZLVec3D& vec, u32 granularity, ZLBounds* result ) {
	UNUSED ( idx );
	UNUSED ( vec );
	UNUSED ( granularity );
	UNUSED ( result );

	return false;
}
