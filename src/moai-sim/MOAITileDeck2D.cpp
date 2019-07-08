// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAIMaterialMgr.h>
#include <moai-sim/MOAIQuadBrush.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAITileDeck2D.h>
#include <moai-sim/MOAITextureBase.h>
#include <moai-sim/MOAITransformBase.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	setQuad
	@text	Set model space quad. Vertex order is clockwise from
			upper left (xMin, yMax)
	
	@in		MOAITileDeck2D self
	@in		number x0
	@in		number y0
	@in		number x1
	@in		number y1
	@in		number x2
	@in		number y2
	@in		number x3
	@in		number y3
	@out	nil
*/
mrb_value MOAITileDeck2D::_setQuad ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITileDeck2D, "UNNNNNNNN" )

	ZLQuad quad;
	
	quad.mV [ 0 ].mX = state.GetParamValue < float >( 1, 0.0f );
	quad.mV [ 0 ].mY = state.GetParamValue < float >( 2, 0.0f );
	quad.mV [ 1 ].mX = state.GetParamValue < float >( 3, 0.0f );
	quad.mV [ 1 ].mY = state.GetParamValue < float >( 4, 0.0f );
	quad.mV [ 2 ].mX = state.GetParamValue < float >( 5, 0.0f );
	quad.mV [ 2 ].mY = state.GetParamValue < float >( 6, 0.0f );
	quad.mV [ 3 ].mX = state.GetParamValue < float >( 7, 0.0f );
	quad.mV [ 3 ].mY = state.GetParamValue < float >( 8, 0.0f );

	self->mQuad.SetVerts ( quad.mV [ 0 ], quad.mV [ 1 ], quad.mV [ 2 ], quad.mV [ 3 ]);
	self->SetBoundsDirty ();

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setRect
	@text	Set the model space dimensions of a single tile. When grid drawing, this
			should be a unit rect centered at the origin for tiles that fit each grid
			cell. Growing or shrinking the rect will cause tiles to overlap or leave
			gaps between them.
	
	@in		MOAITileDeck2D self
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@out	nil
*/
mrb_value MOAITileDeck2D::_setRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITileDeck2D, "UNNNN" )
	
	float x0	= state.GetParamValue < float >( 1, 0.0f );
	float y0	= state.GetParamValue < float >( 2, 0.0f );
	float x1	= state.GetParamValue < float >( 3, 0.0f );
	float y1	= state.GetParamValue < float >( 4, 0.0f );
	
	self->mQuad.SetVerts ( x0, y0, x1, y1 );
	self->SetBoundsDirty ();

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setUVQuad
	@text	Set the UV space dimensions of the quad. Vertex order is
			clockwise from upper left (xMin, yMax)
	
	@in		MOAITileDeck2D self
	@in		number x0
	@in		number y0
	@in		number x1
	@in		number y1
	@in		number x2
	@in		number y2
	@in		number x3
	@in		number y3
	@out	nil
*/
mrb_value MOAITileDeck2D::_setUVQuad ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITileDeck2D, "UNNNNNNNN" )

	ZLQuad quad;
	
	quad.mV [ 0 ].mX = state.GetParamValue < float >( 1, 0.0f );
	quad.mV [ 0 ].mY = state.GetParamValue < float >( 2, 0.0f );
	quad.mV [ 1 ].mX = state.GetParamValue < float >( 3, 0.0f );
	quad.mV [ 1 ].mY = state.GetParamValue < float >( 4, 0.0f );
	quad.mV [ 2 ].mX = state.GetParamValue < float >( 5, 0.0f );
	quad.mV [ 2 ].mY = state.GetParamValue < float >( 6, 0.0f );
	quad.mV [ 3 ].mX = state.GetParamValue < float >( 7, 0.0f );
	quad.mV [ 3 ].mY = state.GetParamValue < float >( 8, 0.0f );
	
	self->mQuad.SetUVs ( quad.mV [ 0 ], quad.mV [ 1 ], quad.mV [ 2 ], quad.mV [ 3 ]);
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setUVRect
	@text	Set the UV space dimensions of the quad.
	
	@in		MOAITileDeck2D self
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@out	nil
*/
mrb_value MOAITileDeck2D::_setUVRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITileDeck2D, "UNNNN" )
	
	float u0	= state.GetParamValue < float >( 1, 0.0f );
	float v0	= state.GetParamValue < float >( 2, 0.0f );
	float u1	= state.GetParamValue < float >( 3, 0.0f );
	float v1	= state.GetParamValue < float >( 4, 0.0f );

	self->mQuad.SetUVs ( u0, v0, u1, v1 );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setSize
	@text	Controls how the texture is subdivided into tiles. Default
			behavior is to subdivide the texture into N by M tiles,
			but is tile dimensions are provided (in UV space) then the resulting
			tile set will be N * tileWidth by M * tileHeight in UV
			space. This means the tile set does not have to fill all of the
			texture. The upper left hand corner of the tile set will always be
			at UV 0, 0.
	
	@in		MOAITileDeck2D self
	@in		number width			Width of the tile deck in tiles.
	@in		number height			Height of the tile deck in tiles.
	@opt	number cellWidth		Width of individual tile in UV space. Defaults to 1 / width.
	@opt	number cellHeight		Height of individual tile in UV space. Defaults to 1 / height.
	@opt	number xOff				X offset of the tile from the cell. Defaults to 0.
	@opt	number yOff				Y offset of the tile from the cell. Defaults to 0.
	@opt	number tileWidth		Default value is cellWidth.
	@opt	number tileHeight		Default value is cellHeight.
	@out	nil
*/
mrb_value	MOAITileDeck2D::_setSize ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITileDeck2D, "UNN" )
	
	u32 width			= state.GetParamValue < u32 >( 1, 0 );
	u32 height			= state.GetParamValue < u32 >( 2, 0 );
	
	float cellWidth		= state.GetParamValue < float >( 3, 1.0f / ( float )width );
	float cellHeight	= state.GetParamValue < float >( 4, 1.0f / ( float )height );
	
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
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	transform
	@text	Apply the given MOAITransform to all the vertices in the deck.
	
	@in		MOAITileDeck2D self
	@in		MOAITransform transform
	@out	nil
*/
mrb_value MOAITileDeck2D::_transform ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITileDeck2D, "UU" )
	
	MOAITransformBase* transform = state.GetRubyObject < MOAITransformBase >( 1, true );
	if ( transform ) {
		transform->ForceUpdate ();
		self->Transform ( transform->GetLocalToWorldMtx ());
		self->SetBoundsDirty ();
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	transformUV
	@text	Apply the given MOAITransform to all the uv coordinates in the deck.
	
	@in		MOAITileDeck2D self
	@in		MOAITransform transform
	@out	nil
*/
mrb_value MOAITileDeck2D::_transformUV ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITileDeck2D, "UU" )
	
	MOAITransformBase* transform = state.GetRubyObject < MOAITransformBase >( 1, true );
	if ( transform ) {
		transform->ForceUpdate ();
		self->TransformUV ( transform->GetLocalToWorldMtx ());
	}
	return context;
}

//================================================================//
// MOAITileDeck2D
//================================================================//

//----------------------------------------------------------------//
MOAITileDeck2D::MOAITileDeck2D () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIDeck )
		RTTI_EXTEND ( MOAIMaterialBatchHolder )
		RTTI_EXTEND ( MOAIGridSpace )
	RTTI_END
	
	this->mQuad.SetVerts ( -0.5f, -0.5f, 0.5f, 0.5f );
	this->mQuad.SetUVs ( -0.5f, -0.5f, 0.5f, 0.5f );
}

//----------------------------------------------------------------//
MOAITileDeck2D::~MOAITileDeck2D () {
}

//----------------------------------------------------------------//
void MOAITileDeck2D::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIDeck::RegisterRubyClass ( state, klass );
	MOAIMaterialBatchHolder::RegisterRubyClass ( state, klass );
	MOAIGridSpace::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAITileDeck2D::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIDeck::RegisterRubyFuncs ( state, klass );
	MOAIMaterialBatchHolder::RegisterRubyFuncs ( state, klass );
	MOAIGridSpace::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "setQuad", _setQuad, MRB_ARGS_REQ ( 8 ) );
	state.DefineInstanceMethod ( klass, "setRect", _setRect, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "setUVQuad", _setUVQuad, MRB_ARGS_REQ ( 8 ) );
	state.DefineInstanceMethod ( klass, "setUVRect", _setUVRect, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "setSize", _setSize, MRB_ARGS_ARG ( 2, 6 ) );
	state.DefineInstanceMethod ( klass, "transform", _transform, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "transformUV", _transformUV, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAITileDeck2D::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {

	MOAIGridSpace::SerializeIn ( state, serializer );
	
	//this->mTexture.Set ( *this, serializer.MemberIDToObject < MOAITextureBase >( state.GetFieldValue < MOAISerializerBase::ObjID >( -1, "mTexture", 0 )));
}

//----------------------------------------------------------------//
void MOAITileDeck2D::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {

	MOAIGridSpace::SerializeOut ( state, serializer );
	
	//state.SetField ( -1, "mTexture", serializer.AffirmMemberID ( this->mTexture ));
}

//----------------------------------------------------------------//
void MOAITileDeck2D::Transform ( const ZLAffine3D& mtx ) {

	this->mQuad.TransformVerts ( mtx );
}

//----------------------------------------------------------------//
void MOAITileDeck2D::TransformUV ( const ZLAffine3D& mtx ) {

	this->mQuad.TransformUVs ( mtx );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
ZLBounds MOAITileDeck2D::MOAIDeck_ComputeMaxBounds () {

	return this->MOAIDeck::GetBounds ( 0 );
}

//----------------------------------------------------------------//
void MOAITileDeck2D::MOAIDeck_Draw ( u32 idx ) {
	UNUSED ( idx );
	
	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	MOAIQuadBrush::BindVertexFormat ();
	
	gfxState.SetVertexTransform ( MOAIGfxState::MODEL_TO_CLIP_MTX );
	gfxState.SetUVTransform ( MOAIGfxState::UV_TO_MODEL_MTX );
	
	MOAICellCoord coord = this->GetCellCoord ( idx );
	ZLRect uvRect = this->GetTileRect ( coord );
	
	float uScale = ( uvRect.mXMax - uvRect.mXMin );
	float vScale = -( uvRect.mYMax - uvRect.mYMin );
	
	float uOff = uvRect.mXMin + ( 0.5f * uScale );
	float vOff = uvRect.mYMin - ( 0.5f * vScale );
	
	MOAIMaterialMgr& materialStack = MOAIMaterialMgr::Get ();
	materialStack.Push ( this->GetMaterial ( idx ));
	materialStack.SetShader ( MOAIShaderMgr::DECK2D_SHADER );
	materialStack.LoadGfxState ();
	
	this->mQuad.Draw ( 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, uOff, vOff, uScale, vScale );
	
	materialStack.Pop ();
}

//----------------------------------------------------------------//
ZLBounds MOAITileDeck2D::MOAIDeck_GetBounds ( u32 idx ) {
	UNUSED ( idx );
	
	return ZLBounds ( this->mQuad.GetVtxBounds ());
}

//----------------------------------------------------------------//
MOAICollisionShape* MOAITileDeck2D::MOAIDeck_GetCollisionShape ( u32 idx ) {
	UNUSED ( idx );

	return 0;
}

//----------------------------------------------------------------//
bool MOAITileDeck2D::MOAIDeck_Overlap ( u32 idx, const ZLVec2D& vec, u32 granularity, ZLBounds* result ) {
	UNUSED ( idx );
	UNUSED ( vec );
	UNUSED ( granularity );
	UNUSED ( result );

	return false;
}

//----------------------------------------------------------------//
bool MOAITileDeck2D::MOAIDeck_Overlap ( u32 idx, const ZLVec3D& vec, u32 granularity, ZLBounds* result ) {
	UNUSED ( idx );
	UNUSED ( vec );
	UNUSED ( granularity );
	UNUSED ( result );

	//return this->TestHit ( materials, idx, granularity, this->mQuad.mModelQuad, this->mQuad.mUVQuad, vec.mX, vec.mY );
	return false;
}
