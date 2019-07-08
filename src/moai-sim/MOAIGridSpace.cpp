// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIGridSpace.h>

double sqrt3 = 1.73205080756887729352;

//================================================================//
// MOAICellCoord
//================================================================//

//----------------------------------------------------------------//
u32 MOAICellCoord::GetID () {

	return (( u32 )this->mX & 0xff ) | ((( u32 )this->mY & 0xff ) << 16 );
}

//----------------------------------------------------------------//
bool MOAICellCoord::IsEqual ( MOAICellCoord cellCoord ) {

	if ( this->mX != cellCoord.mX ) return false;
	if ( this->mY != cellCoord.mY ) return false;
	
	return true;
}

//----------------------------------------------------------------//
MOAICellCoord::MOAICellCoord () {
	mX = -1;
	mY = -1;
}

//----------------------------------------------------------------//
MOAICellCoord::~MOAICellCoord () {
}

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	cellAddrToCoord
	@text	Returns the coordinate of a cell given an address.

	@in		MOAIGridSpace self
	@in		number cellAddr
	@out	number xTile
	@out	number yTile
*/
mrb_value MOAIGridSpace::_cellAddrToCoord	( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "UN" )

	u32 addr = state.GetParamValue < u32 >( 1, 1 ) - 1;
	
	MOAICellCoord coord = self->GetCellCoord ( addr );
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( coord.mX + 1 );
	ret [ 1 ] = state.ToRValue ( coord.mY + 1 );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getCellAddr
	@text	Returns the address of a cell given a coordinate (in tiles).

	@in		MOAIGridSpace self
	@in		number xTile
	@in		number yTile
	@out	number cellAddr
*/
mrb_value MOAIGridSpace::_getCellAddr ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "UNN" )

	int xTile		= state.GetParamValue < int >( 1, 1 ) - 1;
	int yTile		= state.GetParamValue < int >( 2, 1 ) - 1;
	
	return state.ToRValue ( self->GetCellAddr ( xTile, yTile ) + 1 );
}

//----------------------------------------------------------------//
/**	@lua	getCellSize
	@text	Returns the dimensions of a single grid cell.

	@in		MOAIGridSpace self
	@out	number width
	@out	number height
*/
mrb_value MOAIGridSpace::_getCellSize ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "U" )

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( self->mCellWidth );
	ret [ 1 ] = state.ToRValue ( self->mCellHeight );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getOffset
	@text	Returns the offset of tiles from cells.

	@in		MOAIGridSpace self
	@out	number xOff
	@out	number yOff
*/
mrb_value MOAIGridSpace::_getOffset ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "U" )

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( self->mXOff );
	ret [ 1 ] = state.ToRValue ( self->mYOff );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getSize
	@text	Returns the dimensions of the grid (in tiles).

	@in		MOAIGridSpace self
	@out	number width
	@out	number height
*/
mrb_value MOAIGridSpace::_getSize ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "U" )

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( self->mWidth );
	ret [ 1 ] = state.ToRValue ( self->mHeight );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getTileLoc
	@text	Returns the grid space coordinate of the tile. The optional 'position'
			flag determines the location of the coordinate within the tile.

	@in		MOAIGridSpace self
	@in		number xTile
	@in		number yTile
	@opt	number position		See MOAIGridSpace for list of positions. Default it MOAIGridSpace.TILE_CENTER.
	@out	number x
	@out	number y
*/
mrb_value MOAIGridSpace::_getTileLoc ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "UNN" )
	
	MOAICellCoord coord;
	
	coord.mX		= state.GetParamValue < int >( 1, 1 ) - 1;
	coord.mY		= state.GetParamValue < int >( 2, 1 ) - 1;
	u32 position	= state.GetParamValue < u32 >( 3, MOAIGridSpace::TILE_CENTER );
	
	ZLVec2D loc = self->GetTilePoint ( coord, position );

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( loc.mX );
	ret [ 1 ] = state.ToRValue ( loc.mY );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getTileSize
	@text	Returns the dimensions of a single grid tile.

	@in		MOAIGridSpace self
	@out	number width
	@out	number height
*/
mrb_value MOAIGridSpace::_getTileSize ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "U" )

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( self->mTileWidth );
	ret [ 1 ] = state.ToRValue ( self->mTileHeight );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	initAxialHexGrid
	@text	Initialize a grid with hex tiles, using an axial coordinate system. The axial grid assumes that the flat sides of hexes are on the sides, and the points are up/down.

	@in		MOAIGridSpace self
	@in		number width
	@in		number height
	@opt	number tileWidth		Default value is 1.
	@opt	number tileHeight		Default value is 1.
	@opt	number xGutter			Default value is 0.
	@opt	number yGutter			Default value is 0.
	@out	nil
*/
mrb_value MOAIGridSpace::_initAxialHexGrid ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "UNN" )

	u32 width			= state.GetParamValue < u32 >( 1, 0 );
	u32 height			= state.GetParamValue < u32 >( 2, 0 );
	
	float tileWidth		= state.GetParamValue < float >( 3, 1.0f );
	float tileHeight	= state.GetParamValue < float >( 4, 1.0f );

	float xGutter		= state.GetParamValue < float >( 5, 0.0f );
	float yGutter		= state.GetParamValue < float >( 6, 0.0f );

	self->mShape = AXIAL_HEX_SHAPE;

	self->mWidth = width;
	self->mHeight = height;
	
	self->mCellWidth = tileWidth;
	self->mCellHeight = tileHeight;
	
	self->mXOff = xGutter * 0.5f;
	self->mYOff = yGutter * 0.5f;
	
	self->mTileWidth = tileWidth - xGutter;
	self->mTileHeight = tileHeight - yGutter;
	
	self->OnResize ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	initDiamondGrid
	@text	Initialize a grid with diamond tiles.

	@in		MOAIGridSpace self
	@in		number width
	@in		number height
	@opt	number tileWidth		Default value is 1.
	@opt	number tileHeight		Default value is 1.
	@opt	number xGutter			Default value is 0.
	@opt	number yGutter			Default value is 0.
	@out	nil
*/
mrb_value MOAIGridSpace::_initDiamondGrid ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "UNN" )

	u32 width			= state.GetParamValue < u32 >( 1, 0 );
	u32 height			= state.GetParamValue < u32 >( 2, 0 );
	
	float tileWidth		= state.GetParamValue < float >( 3, 1.0f );
	float tileHeight	= state.GetParamValue < float >( 4, 1.0f );

	float xGutter		= state.GetParamValue < float >( 5, 0.0f );
	float yGutter		= state.GetParamValue < float >( 6, 0.0f );

	self->mShape = DIAMOND_SHAPE;

	self->mWidth = width;
	self->mHeight = height;
	
	self->mCellWidth = tileWidth;
	self->mCellHeight = tileHeight * 0.5f;
	
	self->mXOff = xGutter * 0.5f;
	self->mYOff = ( yGutter * 0.5f ) - ( tileHeight * 0.25f );
	
	self->mTileWidth = tileWidth - xGutter;
	self->mTileHeight = tileHeight - yGutter;
	
	self->OnResize ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	initHexGrid
	@text	Initialize a grid with hexagonal tiles.

	@in		MOAIGridSpace self
	@in		number width
	@in		number height
	@opt	number radius			Default value is 1.
	@opt	number xGutter			Default value is 0.
	@opt	number yGutter			Default value is 0.
	@out	nil
*/
mrb_value MOAIGridSpace::_initHexGrid ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "UNN" )

	u32 width			= state.GetParamValue < u32 >( 1, 0 );
	u32 height			= state.GetParamValue < u32 >( 2, 0 );
	
	float hRad			= state.GetParamValue < float >( 3, 1.0f ) * 0.5f;

	float xGutter		= state.GetParamValue < float >( 4, 0.0f );
	float yGutter		= state.GetParamValue < float >( 5, 0.0f );

	float tileWidth = hRad * 6.0f;
	float tileHeight = hRad * ( float )( M_SQRT3 * 2.0 );

	self->mShape = HEX_SHAPE;

	self->mWidth = width;
	self->mHeight = height;
	
	self->mCellWidth = tileWidth;
	self->mCellHeight = tileHeight * 0.5f;
	
	self->mXOff = hRad + ( xGutter * 0.5f );
	self->mYOff = ( yGutter * 0.5f ) - ( tileHeight * 0.25f );
	
	self->mTileWidth = ( hRad * 4.0f ) - xGutter;
	self->mTileHeight = tileHeight - yGutter;
	
	self->OnResize ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	initObliqueGrid
	@text	Initialize a grid with oblique tiles.

	@in		MOAIGridSpace self
	@in		number width
	@in		number height
	@opt	number tileWidth		Default value is 1.
	@opt	number tileHeight		Default value is 1.
	@opt	number xGutter			Default value is 0.
	@opt	number yGutter			Default value is 0.
	@out	nil
*/
mrb_value MOAIGridSpace::_initObliqueGrid ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "UNN" )

	u32 width			= state.GetParamValue < u32 >( 1, 0 );
	u32 height			= state.GetParamValue < u32 >( 2, 0 );
	
	float tileWidth		= state.GetParamValue < float >( 3, 1.0f );
	float tileHeight	= state.GetParamValue < float >( 4, 1.0f );

	float xGutter		= state.GetParamValue < float >( 5, 0.0f );
	float yGutter		= state.GetParamValue < float >( 6, 0.0f );
	
	self->mShape = OBLIQUE_SHAPE;

	self->mWidth = width;
	self->mHeight = height;
	
	self->mCellWidth = tileWidth;
	self->mCellHeight = tileHeight;
	
	self->mXOff = xGutter * 0.5f;
	self->mYOff = yGutter * 0.5f;
	
	self->mTileWidth = ( tileWidth * 2.0f ) - xGutter;
	self->mTileHeight = tileHeight - yGutter;
	
	self->OnResize ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	initRectGrid
	@text	Initialize a grid with rectangular tiles.

	@in		MOAIGridSpace self
	@in		number width
	@in		number height
	@opt	number tileWidth		Default value is 1.
	@opt	number tileHeight		Default value is 1.
	@opt	number xGutter			Default value is 0.
	@opt	number yGutter			Default value is 0.
	@out	nil
*/
mrb_value MOAIGridSpace::_initRectGrid ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "UNN" )

	u32 width			= state.GetParamValue < u32 >( 1, 0 );
	u32 height			= state.GetParamValue < u32 >( 2, 0 );
	
	float tileWidth		= state.GetParamValue < float >( 3, 1.0f );
	float tileHeight	= state.GetParamValue < float >( 4, 1.0f );

	float xGutter		= state.GetParamValue < float >( 5, 0.0f );
	float yGutter		= state.GetParamValue < float >( 6, 0.0f );
	
	self->mShape = RECT_SHAPE;

	self->mWidth = width;
	self->mHeight = height;
	
	self->mCellWidth = tileWidth;
	self->mCellHeight = tileHeight;
	
	self->mXOff = xGutter * 0.5f;
	self->mYOff = yGutter * 0.5f;
	
	self->mTileWidth = tileWidth - xGutter;
	self->mTileHeight = tileHeight - yGutter;
	
	self->OnResize ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	locToCellAddr
	@text	Returns the address of a cell given a a coordinate in grid space.

	@in		MOAIGridSpace self
	@in		number x
	@in		number y
	@out	number cellAddr
*/
mrb_value MOAIGridSpace::_locToCellAddr ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "UNN" )

	ZLVec2D loc;
	loc.mX = state.GetParamValue < float >( 1, 0 );
	loc.mY = state.GetParamValue < float >( 2, 0 );
	
	MOAICellCoord coord;
	coord = self->GetCellCoord ( loc );

	return state.ToRValue ( self->GetCellAddr ( coord ) + 1 );
}

//----------------------------------------------------------------//
/**	@lua	locToCoord
	@text	Transforms a coordinate in grid space into a tile index.

	@in		MOAIGridSpace self
	@in		number x
	@in		number y
	@out	number xTile
	@out	number yTile
*/
mrb_value MOAIGridSpace::_locToCoord ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "UNN" )

	ZLVec2D loc;
	loc.mX = state.GetParamValue < float >( 1, 0 );
	loc.mY = state.GetParamValue < float >( 2, 0 );
	
	MOAICellCoord coord;
	coord = self->GetCellCoord ( loc );

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( coord.mX + 1 );
	ret [ 1 ] = state.ToRValue ( coord.mY + 1 );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	setRepeat
	@text	Repeats a grid indexer along X or Y. Only used when a grid
			is attached.
	
	@in		MOAIGridSpace self
	@opt	boolean repeatX		Default value is true.
	@opt	boolean repeatY		Default value is repeatX.
	@out	nil
*/
mrb_value MOAIGridSpace::_setRepeat ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "U" )

	bool repeatX = state.GetParamValue < bool >( 1, true );
	bool repeatY = state.GetParamValue < bool >( 2, repeatX );

	self->mRepeat = 0;
	self->mRepeat |= repeatX ? REPEAT_X : 0;
	self->mRepeat |= repeatY ? REPEAT_Y : 0;

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setShape
	@text	Set the shape of the grid tiles.

	@in		MOAIGridSpace self
	@opt	number shape		One of MOAIGridSpace.RECT_SHAPE, MOAIGridSpace.DIAMOND_SHAPE,
								MOAIGridSpace.OBLIQUE_SHAPE, MOAIGridSpace.HEX_SHAPE, MOAIGridSpace.AXIAL_HEX_SHAPE.
								Default value is MOAIGridSpace.RECT_SHAPE.
	@out	nil
*/
mrb_value MOAIGridSpace::_setShape ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "U" )

	self->mShape = state.GetParamValue < u32 >( 1, RECT_SHAPE );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setSize
	@text	Initializes dimensions of grid and reserves storage for tiles.

	@in		MOAIGridSpace self
	@in		number width
	@in		number height
	@opt	number cellWidth	Default value is 1.
	@opt	number cellHeight	Default value is 1.
	@opt	number xOff			X offset of the tile from the cell.
	@opt	number yOff			Y offset of the tile from the cell.
	@opt	number tileWidth	Default value is cellWidth.
	@opt	number tileHeight	Default value is cellHeight.
	@out	nil
*/
mrb_value MOAIGridSpace::_setSize ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "UNN" )

	u32 width			= state.GetParamValue < u32 >( 1, 0 );
	u32 height			= state.GetParamValue < u32 >( 2, 0 );
	
	float cellWidth		= state.GetParamValue < float >( 3, 1.0f );
	float cellHeight	= state.GetParamValue < float >( 4, 1.0f );

	float xOff			= state.GetParamValue < float >( 5, 0.0f );
	float yOff			= state.GetParamValue < float >( 6, 0.0f );
	
	float tileWidth		= state.GetParamValue < float >( 7, cellWidth );
	float tileHeight	= state.GetParamValue < float >( 8, cellHeight );
	
	self->SetWidth ( width );
	self->SetHeight ( height );
	
	self->SetCellWidth ( cellWidth );
	self->SetCellHeight ( cellHeight );
	
	self->SetXOff ( xOff );
	self->SetYOff ( yOff );
	
	self->SetTileWidth ( tileWidth );
	self->SetTileHeight ( tileHeight );
	
	self->OnResize ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	wrapCoord
	@text	Wraps a tile index to the range of the grid.

	@in		MOAIGridSpace self
	@in		number xTile
	@in		number yTile
	@out	number xTile
	@out	number yTile
*/
mrb_value MOAIGridSpace::_wrapCoord ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGridSpace, "UNN" )
	
	MOAICellCoord coord;
	coord.mX = state.GetParamValue < int >( 1, 1 ) - 1;
	coord.mY = state.GetParamValue < int >( 2, 1 ) - 1;

	coord = self->WrapCellCoord ( coord.mX, coord.mY );
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( coord.mX + 1 );
	ret [ 1 ] = state.ToRValue ( coord.mY + 1 );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//================================================================//
// MOAIGridSpace
//================================================================//

//----------------------------------------------------------------//
ZLVec2D MOAIGridSpace::CellToWorld ( MOAICellCoord cellCoord, ZLVec2D loc ) const {

	ZLVec2D result;
	result.mX = ( loc.mX + ( float )cellCoord.mX ) * this->mCellWidth;
	result.mY = ( loc.mY + ( float )cellCoord.mY ) * this->mCellHeight;
	
	return result;
}

//----------------------------------------------------------------//
MOAICellCoord MOAIGridSpace::Clamp ( MOAICellCoord cellCoord ) const {

	int xMax = this->mWidth - 1;
	int yMax = this->mHeight - 1;

	MOAICellCoord result;
	
	result.mX = ( cellCoord.mX < 0 ) ? 0 : cellCoord.mX;
	result.mX = ( cellCoord.mX > xMax ) ? xMax : result.mX;

	result.mY = ( cellCoord.mY < 0 ) ? 0 : cellCoord.mY;
	result.mY = ( cellCoord.mY > yMax ) ? yMax : result.mY;

	return result;
}

//----------------------------------------------------------------//
MOAICellCoord MOAIGridSpace::ClampX ( MOAICellCoord cellCoord ) const {

	int xMax = this->mWidth - 1;

	MOAICellCoord result;
	
	result.mX = ( cellCoord.mX < 0 ) ? 0 : cellCoord.mX;
	result.mX = ( cellCoord.mX > xMax ) ? xMax : result.mX;
	
	result.mY = cellCoord.mY;

	return result;
}

//----------------------------------------------------------------//
MOAICellCoord MOAIGridSpace::ClampY ( MOAICellCoord cellCoord ) const {
	
	int yMax = this->mHeight - 1;

	MOAICellCoord result;
	
	result.mY = ( cellCoord.mY < 0 ) ? 0 : cellCoord.mY;
	result.mY = ( cellCoord.mY > yMax ) ? yMax : result.mY;

	result.mX = cellCoord.mX;

	return result;
}

//----------------------------------------------------------------//
MOAICellCoord MOAIGridSpace::GetAxialHexCellCoord ( float x, float y ) const {
	float t1, t2, q, r;

	/* normalize to units of hex dimensions */
	x = ( x / this->mCellWidth ) - 0.5f;
	y = ( y / this->mCellHeight );

	/*
	 * from a comment thread on Amit Patel's hexagon page:

	 	x = (x - halfHexWidth) / hexWidth;

	 	double t1 = z / hexRadius, t2 = Math.Floor(x + t1);
	 	double r = Math.Floor((Math.Floor(t1 - x) + t2) / 3);
	 	double q = Math.Floor((Math.Floor(2 * x + 1) + t2) / 3) - r;

	 	return new Coord((int) q, (int) r); 
	 * note that "hexRadius" is pretty close to "mHeight / 2" for
	 * our purposes. The example assumed that z increases as you go
	 * down the screen, but it also assumed that the hex grid was
	 * counting the same way.
	 */
	t1 = ( y * 2 );
	t2 = floorf ( x + t1 );
	r = floorf (( floorf ( t1 - x ) + t2 ) / 3.0f );
	q = floorf (( floorf ( 2.0f * x + 1.0f ) + t2) / 3.0f );

	return MOAICellCoord (( int )q, ( int )r );
}

//----------------------------------------------------------------//
ZLRect MOAIGridSpace::GetBounds () const {

	ZLRect rect;

	rect.mXMin = 0.0f;
	rect.mYMin = 0.0f;
	
	rect.mXMax = this->mWidth * this->mCellWidth;
	rect.mYMax = this->mHeight * this->mCellHeight;
	
	if ( this->mShape & STAGGER_FLAG ) {
		rect.mXMax += this->mCellWidth * 0.5f;
		rect.mYMin -= this->mCellHeight * 0.5f;
		rect.mYMax += this->mCellHeight * 0.5f;
	}
	if ( this->mShape == AXIAL_HEX_SHAPE ) {
		rect.mXMin -= this->mCellWidth * 0.5f * (this->mHeight - 1);
		rect.mYMax -= this->mCellHeight * 0.25f * (this->mHeight - 1);
	}
	
	return rect;
}

//----------------------------------------------------------------//
ZLRect MOAIGridSpace::GetBounds ( MOAICellCoord c0, MOAICellCoord c1 ) const {

	ZLRect rect0 = this->GetCellRect ( c0 );
	ZLRect rect1 = this->GetCellRect ( c1 );
	
	rect0.Grow ( rect1 );
	
	if ( this->mShape & STAGGER_FLAG ) {
		rect0.mXMax += this->mCellWidth * 0.5f;
		rect0.mYMin -= this->mCellHeight * 0.5f;
		rect0.mYMax += this->mCellHeight * 0.5f;
	}
	if ( this->mShape == AXIAL_HEX_SHAPE ) {
		float rectHeight = rect0.mYMax - rect0.mYMin;
		rect0.mXMin -= this->mCellWidth * 0.5f * rectHeight;
		rect0.mYMax -= this->mCellHeight * 0.25f * rectHeight;
	}
	
	return rect0;
}

//----------------------------------------------------------------//
void MOAIGridSpace::GetBoundsInRect ( ZLRect rect, MOAICellCoord& c0, MOAICellCoord& c1, ZLRect maxSize ) const {

	rect.Bless ();
	maxSize.Bless ();

	if ( this->mShape == AXIAL_HEX_SHAPE ) {

		c0 = this->GetAxialHexCellCoord ( rect.mXMin, rect.mYMin );
		c1 = this->GetAxialHexCellCoord ( rect.mXMax, rect.mYMax );

		int rectHeight = c1.mY - c0.mY;

		// you need an extra column per two rows
		c1.mX = c1.mX +  ( rectHeight >> 1 );
	} else {
		c0.mX = ( int )floorf ( ( rect.mXMin / this->mCellWidth )  - ( maxSize.mXMax / 0.5f - 1.0f ) );
		c0.mY = ( int )floorf ( ( rect.mYMin / this->mCellHeight ) - ( maxSize.mYMax / 0.5f - 1.0f ) );

		c1.mX = ( int )floorf ( ( rect.mXMax / this->mCellWidth )  + ( maxSize.mXMin / -0.5f - 1.0f ) );
		c1.mY = ( int )floorf ( ( rect.mYMax / this->mCellHeight ) + ( maxSize.mYMin / -0.5f - 1.0f ) );
	}
	
	if ( this->mShape & STAGGER_FLAG ) {
		c0.mX--;
		c0.mY--;
		c1.mY++;
	}

	
	if ( this->mShape == OBLIQUE_SHAPE ) {
		c0.mX--;
	}
	
	if ( !( this->mRepeat & REPEAT_X )) {
		c0 = this->ClampX ( c0 );
		c1 = this->ClampX ( c1 );
	}
	
	if ( !( this->mRepeat & REPEAT_Y )) {
		c0 = this->ClampY ( c0 );
		c1 = this->ClampY ( c1 );
	}
}

//----------------------------------------------------------------//
int MOAIGridSpace::GetCellAddr ( MOAICellCoord cellCoord ) const {

	return this->GetCellAddr ( cellCoord.mX, cellCoord.mY );
}

//----------------------------------------------------------------//
int MOAIGridSpace::GetCellAddr ( int xCell, int yCell ) const {

	if ( !( this->mWidth && this->mHeight )) return 0;

	xCell = xCell % this->mWidth;
	if ( xCell < 0 ) xCell += this->mWidth;

	yCell = yCell % this->mHeight;
	if ( yCell < 0 ) yCell += this->mHeight;

	return ( yCell* this->mWidth ) + xCell;
}

//----------------------------------------------------------------//
MOAICellCoord MOAIGridSpace::GetCellCoord ( int cellAddr ) const {
	
	MOAICellCoord cellCoord;
	
	cellCoord.mX = ( cellAddr % this->mWidth );
	cellCoord.mY = (( int )( cellAddr / this->mWidth ) % this->mHeight );
	
	return cellCoord;
}

//----------------------------------------------------------------//
MOAICellCoord MOAIGridSpace::GetCellCoord ( ZLVec2D loc ) const {
	
	return this->GetCellCoord ( loc.mX, loc.mY );
}

//----------------------------------------------------------------//
MOAICellCoord MOAIGridSpace::GetCellCoord ( float x, float y ) const {
	
	MOAICellCoord cellCoord ( 0, 0 );
	
	switch ( this->mShape ) {
		
		case AXIAL_HEX_SHAPE:
			return this->GetAxialHexCellCoord ( x, y );

		case DIAMOND_SHAPE:
			return this->GetHexCellCoord ( x, y, 0.0f, 4.0f );
		
		case HEX_SHAPE:
			return this->GetHexCellCoord ( x, y, 2.0f, 10.0f );
			
		case OBLIQUE_SHAPE:
			return this->GetObliqueCellCoord ( x, y );
		
		case RECT_SHAPE:
			cellCoord.mX = ( int )floorf ( x / this->mCellWidth );
			cellCoord.mY = ( int )floorf ( y / this->mCellHeight );
			break;
	}
	
	return cellCoord;
}

//----------------------------------------------------------------//
MOAICellCoord MOAIGridSpace::GetCellCoord ( int xCell, int yCell ) const {

	MOAICellCoord cellCoord;
	cellCoord.mX = xCell;
	cellCoord.mY = yCell;
	return cellCoord;
}

//----------------------------------------------------------------//
ZLVec2D MOAIGridSpace::GetCellPoint ( MOAICellCoord cellCoord, u32 position ) const {
			
	float xOff = 0.0f;
	
	if (( this->mShape & STAGGER_FLAG ) && ( cellCoord.mY & 0x01 )) {
		xOff = this->mCellWidth * 0.5f;
	}
	
	if ( this->mShape == AXIAL_HEX_SHAPE ) {
		xOff = this->mCellWidth * -0.5f * cellCoord.mY;
		return this->GetRectPoint (
			( cellCoord.mX * this->mCellWidth ) + xOff,
			( cellCoord.mY * this->mCellHeight * 3 / 4 ) - ( this->mCellHeight * 0.5f ),
			this->mCellWidth,
			this->mCellHeight * 2.0f,
			position
		);
	}
	
	return this->GetRectPoint (
		( cellCoord.mX * this->mCellWidth ) + xOff,
		( cellCoord.mY * this->mCellHeight ) - ( this->mCellHeight* 0.5f ),
		this->mCellWidth,
		this->mCellHeight * 2.0f,
		position
	);
}

//----------------------------------------------------------------//
ZLRect MOAIGridSpace::GetCellRect ( MOAICellCoord cellCoord ) const {
			
	float xOff = 0.0f;
	float yOff = 0.0f;

	if (( this->mShape & STAGGER_FLAG ) && ( cellCoord.mY & 0x01 )) {
		xOff = this->mCellWidth * 0.5f;
	}
	if ( this->mShape == AXIAL_HEX_SHAPE ) {
		xOff = this->mCellWidth * -0.5f * cellCoord.mY;
		yOff = (cellCoord.mY - 1) * this->mCellHeight * 0.25f;
	}
	
	ZLRect rect;
	
	rect.mXMin = ( cellCoord.mX * this->mCellWidth ) + xOff;
	rect.mYMin = cellCoord.mY * this->mCellHeight + yOff;
	
	rect.mXMax = rect.mXMin + this->mCellWidth;
	rect.mYMax = rect.mYMin + this->mCellHeight;
	
	return rect;
}

//----------------------------------------------------------------//
MOAICellCoord MOAIGridSpace::GetHexCellCoord ( float x, float y, float a, float b ) const {

	// get the coord in tiles
	float xUnit = ( x / this->mCellWidth );
	float yUnit = ( y / this->mCellHeight );
	
	// get the y tile index
	int yTile = ( int )floorf ( yUnit );
	
	int stepRight = 0;
	int stepLeft = -1;
	
	// need to offset x into the previous tile if y is odd
	if ( yTile & 0x01 ) {
		xUnit -= 0.5f;
		
		stepRight = 1;
		stepLeft = 0;
	}
	
	// get the x tile index
	int xTile = ( int )floorf ( xUnit );
	
	// now get the local coord
	float xLocal = ( xUnit - ( float )xTile ) * ( a + b );
	float yLocal = (( yUnit - ( float )yTile ) * 2.0f ) - 1.0f;

	// check the position of the coord depending on tile quadrant
	// offset the tile index if out of bounds on any corner
	if ( xLocal < ( a + 1.0f )) {
	
		if ( yLocal < 0.0f ) {
			if ( yLocal < ( a - xLocal )) {
				xTile = xTile + stepLeft;
				yTile = yTile - 1;
			}
		}
		else if ( yLocal > ( xLocal - a )) {
			xTile = xTile + stepLeft;
			yTile = yTile + 1;
		}
	}
	else if ( xLocal > ( b - 1.0f )) {
		
		if ( yLocal < 0.0f ) {
			if ( yLocal < ( xLocal - b )) {
				xTile = xTile + stepRight;
				yTile = yTile - 1;
			}
		}
		else if ( yLocal > ( b - xLocal )) {
			xTile = xTile + stepRight;
			yTile = yTile + 1;
		}
	}
	
	MOAICellCoord cellCoord ( xTile, yTile );
	return cellCoord;
}

//----------------------------------------------------------------//
MOAICellCoord MOAIGridSpace::GetObliqueCellCoord ( float x, float y ) const {

	// get the coord in tiles
	float xUnit = ( x / this->mCellWidth );
	float yUnit = ( y / this->mCellHeight );
	
	// get the tile index
	int xTile = ( int )floorf ( xUnit );
	int yTile = ( int )floorf ( yUnit );
	
	// now get the local coord
	float xLocal = ( xUnit - ( float )xTile );
	float yLocal = ( yUnit - ( float )yTile );
	
	if ( yLocal > xLocal ) {
		xTile = xTile - 1;
	}
	
	MOAICellCoord cellCoord ( xTile, yTile );
	return cellCoord;
}

//----------------------------------------------------------------//
ZLVec2D MOAIGridSpace::GetRectPoint ( float x, float y, float width, float height, u32 position ) const {

	ZLVec2D origin;

	origin.mX = x;
	origin.mY = y;
	
	switch ( position ) {
		case TILE_LEFT_TOP: {
			break;
		}
		case TILE_RIGHT_TOP: {
			origin.mX = origin.mX + width;
			break;
		}
		case TILE_LEFT_BOTTOM: {
			origin.mY = origin.mY + height;
			break;
		}
		case TILE_RIGHT_BOTTOM: {
			origin.mX = origin.mX + width;
			origin.mY = origin.mY + height;
			break;
		}
		case TILE_LEFT_CENTER: {
			origin.mY = origin.mY + ( height * 0.5f );
			break;
		}
		case TILE_RIGHT_CENTER: {
			origin.mX = origin.mX + width;
			origin.mY = origin.mY + ( height * 0.5f );
			break;
		}
		case TILE_TOP_CENTER: {
			origin.mX = origin.mX + ( width * 0.5f );
			break;
		}
		case TILE_BOTTOM_CENTER: {
			origin.mX = origin.mX + ( width * 0.5f );
			origin.mY = origin.mY + height;
			break;
		}
		case TILE_CENTER:
		default: {
			origin.mX = origin.mX + ( width * 0.5f );
			origin.mY = origin.mY + ( height * 0.5f );
		}
	};
	
	return origin;
}

//----------------------------------------------------------------//
ZLVec2D MOAIGridSpace::GetTilePoint ( MOAICellCoord cellCoord, u32 position ) const {
	
	float xOff = 0.0f;

	if (( this->mShape & STAGGER_FLAG ) && ( cellCoord.mY & 0x01 )) {
		xOff = this->mCellWidth * 0.5f;
	}
	
	if ( this->mShape == AXIAL_HEX_SHAPE ) {
		xOff = this->mCellWidth * -0.5f * cellCoord.mY;
		return this->GetRectPoint (
			( cellCoord.mX * this->mCellWidth ) + this->mXOff + xOff,
			( cellCoord.mY * this->mCellHeight * 3 / 4 ) + this->mYOff,
			this->mTileWidth,
			this->mTileHeight,
			position
		);
	}
	
	return this->GetRectPoint (
		( cellCoord.mX * this->mCellWidth ) + this->mXOff + xOff,
		( cellCoord.mY * this->mCellHeight ) + this->mYOff,
		this->mTileWidth,
		this->mTileHeight,
		position
	);
}

//----------------------------------------------------------------//
ZLVec3D MOAIGridSpace::GetTilePoint ( MOAICellCoord cellCoord, u32 position, float z ) const {

	ZLVec2D point = this->GetTilePoint ( cellCoord, position );
	return ZLVec3D ( point.mX, point.mY, z );
}

//----------------------------------------------------------------//
ZLRect MOAIGridSpace::GetTileRect ( MOAICellCoord cellCoord ) const {

	float xOff = 0.0f;

	if (( this->mShape & STAGGER_FLAG ) && ( cellCoord.mY & 0x01 )) {
		xOff = this->mCellWidth * 0.5f;
	}

	ZLRect rect;
	
	rect.mXMin = ( cellCoord.mX * this->mCellWidth ) + this->mXOff + xOff;
	rect.mYMin = ( cellCoord.mY * this->mCellHeight ) + this->mYOff;
	
	rect.mXMax = rect.mXMin + this->mTileWidth;
	rect.mYMax = rect.mYMin + this->mTileHeight;
	
	return rect;
}

//----------------------------------------------------------------//
int MOAIGridSpace::GetTotalCells () const {

	return this->mWidth * this->mHeight;
}

//----------------------------------------------------------------//
void MOAIGridSpace::Init ( int width, int height, float tileWidth, float tileHeight ) {

	this->mXOff = 0.0f;
	this->mYOff = 0.0f;
	
	this->mWidth = width;
	this->mHeight = height;
	
	this->mTileWidth = tileWidth;
	this->mTileHeight = tileHeight;
	
	this->mCellWidth = tileWidth;
	this->mCellHeight = tileHeight;
}

//----------------------------------------------------------------//
bool MOAIGridSpace::IsValidCoord ( MOAICellCoord cellCoord ) const {

	if ( cellCoord.mX < 0 ) return false;
	if ( cellCoord.mY < 0 ) return false;

	if ( cellCoord.mX >= this->mWidth ) return false;
	if ( cellCoord.mY >= this->mHeight ) return false;
	
	return true;
}

//----------------------------------------------------------------//
MOAIGridSpace::MOAIGridSpace () :
	mXOff ( 0.0f ),
	mYOff ( 0.0f ),
	mCellWidth ( 1.0f ),
	mCellHeight ( 1.0f ),
	mTileWidth ( 0.0f ),
	mTileHeight ( 0.0f ),
	mWidth ( 0 ),
	mHeight ( 0 ),
	mShape ( RECT_SHAPE ),
	mRepeat ( 0 ) {
	
	RTTI_SINGLE ( MOAIRubyObject )
}

//----------------------------------------------------------------//
MOAIGridSpace::~MOAIGridSpace () {
}

//----------------------------------------------------------------//
void MOAIGridSpace::OnResize () {
}

//----------------------------------------------------------------//
void MOAIGridSpace::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	UNUSED ( serializer );
	
	// this->mXOff			= state.GetFieldValue ( -1, "mXOff", this->mXOff );
	// this->mYOff			= state.GetFieldValue ( -1, "mYOff", this->mYOff );
	
	// this->mCellWidth	= state.GetFieldValue ( -1, "mCellWidth", this->mCellWidth );
	// this->mCellHeight	= state.GetFieldValue ( -1, "mCellHeight", this->mCellHeight );
	
	// this->mTileWidth	= state.GetFieldValue ( -1, "mTileWidth", this->mTileWidth );
	// this->mTileHeight	= state.GetFieldValue ( -1, "mTileHeight", this->mTileHeight );
	
	// this->mWidth		= state.GetFieldValue ( -1, "mWidth", this->mWidth );
	// this->mHeight		= state.GetFieldValue ( -1, "mHeight", this->mHeight );
	
	// this->mShape		= state.GetFieldValue ( -1, "mShape", this->mShape );
	// this->mRepeat		= state.GetFieldValue ( -1, "mRepeat", this->mRepeat );
}

//----------------------------------------------------------------//
void MOAIGridSpace::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	UNUSED ( serializer );
	
	// state.DefineClassConst ( klass, "mXOff", this->mXOff );
	// state.DefineClassConst ( klass, "mYOff", this->mYOff );
	
	// state.DefineClassConst ( klass, "mCellWidth", this->mCellWidth );
	// state.DefineClassConst ( klass, "mCellHeight", this->mCellHeight );
	
	// state.DefineClassConst ( klass, "mTileWidth", this->mTileWidth );
	// state.DefineClassConst ( klass, "mTileHeight", this->mTileHeight );
	
	// state.DefineClassConst ( klass, "mWidth", this->mWidth );
	// state.DefineClassConst ( klass, "mHeight", this->mHeight );
	
	// state.DefineClassConst ( klass, "mShape", this->mShape );
	// state.DefineClassConst ( klass, "mRepeat", this->mRepeat );
}

//----------------------------------------------------------------//
void MOAIGridSpace::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "TILE_X_FLIP", ( u32 )MOAITileFlags::XFLIP );
	state.DefineClassConst ( klass, "TILE_Y_FLIP", ( u32 )MOAITileFlags::YFLIP );
	state.DefineClassConst ( klass, "TILE_XY_FLIP", ( u32 )MOAITileFlags::FLIP_MASK );
	state.DefineClassConst ( klass, "TILE_HIDE", ( u32 )MOAITileFlags::HIDDEN );
	
	state.DefineClassConst ( klass, "TILE_LEFT_TOP", ( u32 )MOAIGridSpace::TILE_LEFT_TOP );
	state.DefineClassConst ( klass, "TILE_RIGHT_TOP", ( u32 )MOAIGridSpace::TILE_RIGHT_TOP );
	state.DefineClassConst ( klass, "TILE_LEFT_BOTTOM", ( u32 )MOAIGridSpace::TILE_LEFT_BOTTOM );
	state.DefineClassConst ( klass, "TILE_RIGHT_BOTTOM", ( u32 )MOAIGridSpace::TILE_RIGHT_BOTTOM );
	
	state.DefineClassConst ( klass, "TILE_LEFT_CENTER", ( u32 )MOAIGridSpace::TILE_LEFT_CENTER );
	state.DefineClassConst ( klass, "TILE_RIGHT_CENTER", ( u32 )MOAIGridSpace::TILE_RIGHT_CENTER );
	state.DefineClassConst ( klass, "TILE_TOP_CENTER", ( u32 )MOAIGridSpace::TILE_TOP_CENTER );
	state.DefineClassConst ( klass, "TILE_BOTTOM_CENTER", ( u32 )MOAIGridSpace::TILE_BOTTOM_CENTER );
	
	state.DefineClassConst ( klass, "TILE_CENTER", ( u32 )MOAIGridSpace::TILE_CENTER );
	
	state.DefineClassConst ( klass, "RECT_SHAPE", RECT_SHAPE );
	state.DefineClassConst ( klass, "AXIAL_HEX_SHAPE", AXIAL_HEX_SHAPE );
	state.DefineClassConst ( klass, "DIAMOND_SHAPE", DIAMOND_SHAPE );
	state.DefineClassConst ( klass, "OBLIQUE_SHAPE", OBLIQUE_SHAPE );
	state.DefineClassConst ( klass, "HEX_SHAPE", HEX_SHAPE );
}

//----------------------------------------------------------------//
void MOAIGridSpace::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "cellAddrToCoord", _cellAddrToCoord, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getCellAddr", _getCellAddr, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "getCellSize", _getCellSize, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getOffset", _getOffset, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getSize", _getSize, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getTileLoc", _getTileLoc, MRB_ARGS_ARG ( 2, 1 ) );
	state.DefineInstanceMethod ( klass, "getTileSize", _getTileSize, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "initDiamondGrid", _initDiamondGrid, MRB_ARGS_ARG ( 2, 4 ) );
	state.DefineInstanceMethod ( klass, "initAxialHexGrid", _initAxialHexGrid, MRB_ARGS_ARG ( 2, 4 ) );
	state.DefineInstanceMethod ( klass, "initHexGrid", _initHexGrid, MRB_ARGS_ARG ( 2, 3 ) );
	state.DefineInstanceMethod ( klass, "initObliqueGrid", _initObliqueGrid, MRB_ARGS_ARG ( 2, 4 ) );
	state.DefineInstanceMethod ( klass, "initRectGrid", _initRectGrid, MRB_ARGS_ARG ( 2, 4 ) );
	state.DefineInstanceMethod ( klass, "locToCellAddr", _locToCellAddr, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "locToCoord", _locToCoord, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setRepeat", _setRepeat, MRB_ARGS_ARG ( 0, 2 ) );
	state.DefineInstanceMethod ( klass, "setShape", _setShape, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setSize", _setSize, MRB_ARGS_ARG ( 2, 6 ) );
	state.DefineInstanceMethod ( klass, "wrapCoord", _wrapCoord, MRB_ARGS_REQ ( 2 ) );

}

//----------------------------------------------------------------//
MOAICellCoord MOAIGridSpace::WrapCellCoord ( int xCell, int yCell ) const {

	MOAICellCoord wrap;
	
	wrap.mX = xCell % this->mWidth;
	if ( wrap.mX < 0 ) wrap.mX += this->mWidth;
	
	wrap.mY = yCell % this->mHeight;
	if ( wrap.mY < 0 ) wrap.mY += this->mHeight;
	
	return wrap;
}

//----------------------------------------------------------------//
ZLVec2D MOAIGridSpace::WorldToCell ( MOAICellCoord cellCoord, ZLVec2D loc ) const {
	
	ZLVec2D result = loc;
	result.mX = ( loc.mX / this->mCellWidth ) - ( float )cellCoord.mX;
	result.mY = ( loc.mY / this->mCellHeight ) - ( float )cellCoord.mY;
	
	return result;
}

