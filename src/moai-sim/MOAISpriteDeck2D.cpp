// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAISpriteDeck2D.h>
#include <moai-sim/MOAIMaterialMgr.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAITexture.h>
#include <moai-sim/MOAITransformBase.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getQuad
	@text	Get model space quad given a deck index. 
			Vertex order is clockwiese from upper left.
	
	@in		MOAISpriteDeck2D self
	@in		number idx	Index of the quad.
	@out	number x0
	@out	number y0
	@out	number x1
	@out	number y1
	@out	number x2
	@out	number y2
	@out	number x3
	@out	number y3
*/
mrb_value MOAISpriteDeck2D::_getQuad ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAISpriteDeck2D, "U" )
	
	u32 quadID = 0;
	u32 idx = 2;
	
	if ( state.GetParamsCount () == 10 ) {
		quadID = state.GetParamValue < u32 >( idx++, 1 ) - 1;
	}
	
	return quadID < self->mQuads.Size () ? state.Get ( self->mQuads [ quadID ]) : mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getRect
	@text	Set model space quad given a valid deck index and a rect.
	
	@in		MOAISpriteDeck2D self
	@in		number idx	Index of the quad.
	@out	number xMin
	@out	number yMin
	@out	number xMax
	@out	number yMax
*/
mrb_value MOAISpriteDeck2D::_getRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAISpriteDeck2D, "UN" )
	
	u32 rectID = 0;
	u32 idx = 2;
	
	if ( state.GetParamsCount () == 10 ) {
		rectID = state.GetParamValue < u32 >( idx++, 1 ) - 1;
	}
	
	return rectID < self->mQuads.Size () ? state.Get ( self->mQuads [ rectID ].GetBounds ()) : mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getUVQuad
	@text	Get UV model space quad given a valid deck index.
			Vertex order is clockwise from upper left.

	@in		MOAISpriteDeck2D self
	@in		number idx	Index of the quad.
	@out	number u0
	@out	number v0
	@out	number u1
	@out	number v1
	@out	number u2
	@out	number v2
	@out	number u3
	@out	number v3
*/
mrb_value MOAISpriteDeck2D::_getUVQuad ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAISpriteDeck2D, "UN" )
	
	u32 quadID = 0;
	u32 idx = 2;
	
	if ( state.GetParamsCount () == 10 ) {
		quadID = state.GetParamValue < u32 >( idx++, 1 ) - 1;
	}
	
	return quadID < self->mQuads.Size () ? state.Get ( self->mUVQuads [ quadID ]) : mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getUVRect
	@text	Get UV model space quad given a valid deck index.
	
	@in		MOAISpriteDeck2D self
	@in		number idx	Index of the quad.
	@out	number xMin
	@out	number yMin
	@out	number xMax
	@out	number yMax
*/
mrb_value MOAISpriteDeck2D::_getUVRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAISpriteDeck2D, "U" )
	
	u32 rectID = 0;
	u32 idx = 2;
	
	if ( state.GetParamsCount () == 10 ) {
		rectID = state.GetParamValue < u32 >( idx++, 1 ) - 1;
	}
	
	return rectID < self->mUVQuads.Size () ? state.Get ( self->mUVQuads [ rectID ].GetBounds ()) : mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	reserveQuads
	@text	Reserve quads.
	
	@in		MOAISpriteDeck2D self
	@in		number nQuads
	@out	nil
*/
mrb_value MOAISpriteDeck2D::_reserveQuads ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAISpriteDeck2D, "UN" )

	u32 total = state.GetParamValue < u32 >( 1, 0 );
	self->ReserveQuads ( total );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	reserveSprites
	@text	Reserve sprites.
	
	@in		MOAISpriteDeck2D self
	@in		number nPairs
	@out	nil
*/
mrb_value MOAISpriteDeck2D::_reserveSprites ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAISpriteDeck2D, "UN" )

	u32 total = state.GetParamValue < u32 >( 1, 0 );
	self->ReservePairs ( total );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	reserveSpriteLists
	@text	Reserve quad lists.
	
	@in		MOAISpriteDeck2D self
	@in		number nLists
	@out	nil
*/
mrb_value MOAISpriteDeck2D::_reserveSpriteLists ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAISpriteDeck2D, "UN" )

	u32 total = state.GetParamValue < u32 >( 1, 0 );
	self->ReserveLists ( total );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	reserveUVQuads
	@text	Reserve UV quads.
	
	@in		MOAISpriteDeck2D self
	@in		number nUVQuads
	@out	nil
*/
mrb_value MOAISpriteDeck2D::_reserveUVQuads ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAISpriteDeck2D, "UN" )

	u32 total = state.GetParamValue < u32 >( 1, 0 );
	self->ReserveUVQuads ( total );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setQuad
	@text	Set model space quad given a valid deck index. Vertex order is
			clockwise from upper left (xMin, yMax)
	
	@in		MOAISpriteDeck2D self
	@in		number idx	Index of the quad.
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
mrb_value MOAISpriteDeck2D::_setQuad ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAISpriteDeck2D, "U" )

	u32 quadID = 0;
	u32 idx = 1;
	
	if ( state.GetParamsCount () == 10 ) {
		quadID = state.GetParamValue < u32 >( idx++, 1 ) - 1;
	}
	ZLQuad quad = state.GetParamQuad ( idx );

	self->SetQuad ( quadID, quad );
	self->SetBoundsDirty ();

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setRect
	@text	Set model space quad given a valid deck index and a rect.
	
	@in		MOAISpriteDeck2D self
	@in		number idx	Index of the quad.
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@out	nil
*/
mrb_value MOAISpriteDeck2D::_setRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAISpriteDeck2D, "U" )

	u32 rectID = 0;
	u32 idx = 1;
	
	if ( state.GetParamsCount () == 6 ) {
		rectID = state.GetParamValue < u32 >( idx++, 1 ) - 1;
	}
	ZLRect rect = state.GetRect < float >( idx );

	self->SetRect ( rectID, rect );
	self->SetBoundsDirty ();

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setSprite
	@text	Associates a quad with its UV coordinates.
	
	@in		MOAISpriteDeck2D self
	@in		number idx
	@in		number uvQuadID
	@in		number quadID
	@opt	number materialID
	@out	nil
*/
mrb_value MOAISpriteDeck2D::_setSprite ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAISpriteDeck2D, "UNNN" )

	u32 idx			= state.GetParamValue < u32 >( 1, 1 ) - 1;
	u32 uvQuadID	= state.GetParamValue < u32 >( 2, 1 ) - 1;
	u32 quadID		= state.GetParamValue < u32 >( 3, 1 ) - 1;
	u32 materialID	= state.GetParamValue < u32 >( 4, 1 ) - 1;
	
	self->SetPair ( idx, uvQuadID, quadID, materialID );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setSpriteList
	@text	Initializes quad pair list at index. A list starts at the index
			of a pair and then continues sequentially for n pairs after. So
			a list with base 3 and a run of 4 would display pair 3, 4, 5,
			and 6.
	
	@in		MOAISpriteDeck2D self
	@in		number idx
	@in		number basePairID	The base pair of the list.
	@in		number totalPairs	The run of the list - total pairs to display (including base).
	@out	nil
*/
mrb_value MOAISpriteDeck2D::_setSpriteList ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAISpriteDeck2D, "UNNN" )

	u32 idx			= state.GetParamValue < u32 >( 1, 1 ) - 1;
	u32 basePairID	= state.GetParamValue < u32 >( 2, 1 ) - 1;
	u32 totalPairs	= state.GetParamValue < u32 >( 3, 0 );

	self->SetList ( idx, basePairID, totalPairs );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setUVQuad
	@text	Set UV space quad given a valid deck index. Vertex order is
			clockwise from upper left (xMin, yMax)
	
	@in		MOAISpriteDeck2D self
	@in		number idx	Index of the quad.
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
mrb_value MOAISpriteDeck2D::_setUVQuad ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAISpriteDeck2D, "U" )

	u32 quadID = 0;
	u32 idx = 1;
	
	if ( state.GetParamsCount () == 10 ) {
		quadID = state.GetParamValue < u32 >( idx++, 1 ) - 1;
	}
	ZLQuad quad = state.GetParamQuad ( idx );

	self->SetUVQuad ( quadID, quad );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setUVRect
	@text	Set UV space quad given a valid deck index and a rect.
	
	@in		MOAISpriteDeck2D self
	@in		number idx	Index of the quad.
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@out	nil
*/
mrb_value MOAISpriteDeck2D::_setUVRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAISpriteDeck2D, "U" )

	u32 rectID = 0;
	u32 idx = 1;
	
	if ( state.GetParamsCount () == 6 ) {
		rectID = state.GetParamValue < u32 >( idx++, 1 ) - 1;
	}
	ZLRect rect = state.GetRect < float >( idx );

	self->SetUVRect ( rectID, rect );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	transform
	@text	Apply the given MOAITransform to all the vertices in the deck.
	
	@in		MOAISpriteDeck2D self
	@in		MOAITransform transform
	@out	nil
*/
mrb_value MOAISpriteDeck2D::_transform ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAISpriteDeck2D, "UU" )
	
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
	
	@in		MOAISpriteDeck2D self
	@in		MOAITransform transform
	@out	nil
*/
mrb_value MOAISpriteDeck2D::_transformUV ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAISpriteDeck2D, "UU" )
	
	MOAITransformBase* transform = state.GetRubyObject < MOAITransformBase >( 1, true );
	if ( transform ) {
		transform->ForceUpdate ();
		self->TransformUV ( transform->GetLocalToWorldMtx ());
	}
	return context;
}

//================================================================//
// MOAISpriteDeck2D
//================================================================//

//----------------------------------------------------------------//
MOAIDeck* MOAISpriteDeck2D::AffirmDeck ( MOAIRubyState& state, int idx ) {

	MOAIDeck* deck = state.GetRubyObject < MOAIDeck >( idx, false );
	if ( deck ) return deck;
	
	MOAITextureBase* texture = MOAITexture::AffirmTexture ( state, idx );
		
	if ( texture ) {
		
		MOAISpriteDeck2D* quadDeck = state.CreateClassInstance < MOAISpriteDeck2D >();
		MOAIMaterialBatch* batch = quadDeck->AffirmMaterialBatch ();
	
		assert ( quadDeck );
		assert ( batch );
		
		batch->SetTexture ( 0, texture );
		
		int hWidth = ( int )( texture->GetWidth () / 2 );
		int hHeight = ( int )( texture->GetHeight () / 2 );
		
		ZLRect rect;
		rect.Init ( (float) -hWidth, (float) -hHeight, (float) hWidth, (float) hHeight );
		quadDeck->SetRect( 0, rect );
		
		return quadDeck;
	}
	return 0;
}

//----------------------------------------------------------------//
//bool MOAISpriteDeck2D::Contains ( u32 idx, const ZLVec2D& vec ) {
//	
//	size_t size = this->mSpriteLists.Size ();
//	if ( size ) {
//		
//		idx = ( idx - 1 ) % size;
//		MOAISpriteList& brush = this->mSpriteLists [ idx ];
//		
//		for ( size_t i = 0; i < brush.mTotalSprites; ++i ) {
//			MOAISprite& prim = this->mSprites [ brush.mBaseSprite + i ];
//			if ( this->mQuads [ prim.mQuadID ].Contains ( vec.mX, vec.mY )) {
//				return true;
//			} 
//		}
//	}
//	return false;
//}

//----------------------------------------------------------------//
//bool MOAISpriteDeck2D::Inside ( u32 idx, MOAIMaterialBatch* materials, u32 granularity, ZLVec3D vec, float pad ) {
//	UNUSED ( pad );
//
//	u32 size = ( u32 )this->mSpriteLists.Size (); // TODO: cast
//	if ( size ) {
//	
//		idx = ( idx - 1 ) % size;
//		
//		MOAISpriteList& sprite = this->mSpriteLists [ idx ];
//		
//		for ( u32 i	 = 0; i < sprite.mTotalSprites; ++i ) {
//			MOAISprite& prim = this->mSprites [ sprite.mBaseSprite + i ];
//			if ( this->TestHit ( materials, prim.mMaterialID, idx, granularity, this->mQuads [ prim.mQuadID ], this->mUVQuads [ prim.mUVQuadID ], vec.mX, vec.mY )) return true;
//		}
//	}
//	return false;
//}

//----------------------------------------------------------------//
MOAISpriteDeck2D::MOAISpriteDeck2D () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIDeck )
		RTTI_EXTEND ( MOAIMaterialBatchHolder )
	RTTI_END
}

//----------------------------------------------------------------//
MOAISpriteDeck2D::~MOAISpriteDeck2D () {
}

//----------------------------------------------------------------//
void MOAISpriteDeck2D::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIDeck::RegisterRubyClass ( state, klass );
	MOAIMaterialBatchHolder::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAISpriteDeck2D::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIDeck::RegisterRubyFuncs ( state, klass );
	MOAIMaterialBatchHolder::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getQuad", _getQuad, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getRect", _getRect, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getUVQuad", _getUVQuad, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getUVRect", _getUVRect, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "reserveQuads", _reserveQuads, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "reserveSprites", _reserveSprites, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "reserveSpriteLists", _reserveSpriteLists, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "reserveUVQuads", _reserveUVQuads, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setQuad", _setQuad, MRB_ARGS_REQ ( 9 ) );
	state.DefineInstanceMethod ( klass, "setRect", _setRect, MRB_ARGS_REQ ( 5 ) );
	state.DefineInstanceMethod ( klass, "setSprite", _setSprite, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "setSpriteList", _setSpriteList, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "setUVQuad", _setUVQuad, MRB_ARGS_REQ ( 9 ) );
	state.DefineInstanceMethod ( klass, "setUVRect", _setUVRect, MRB_ARGS_REQ ( 5 ) );
	state.DefineInstanceMethod ( klass, "transform", _transform, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "transformUV", _transformUV, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAISpriteDeck2D::ReserveLists ( u32 total ) {

	this->mSpriteLists.Init ( total );
}

//----------------------------------------------------------------//
void MOAISpriteDeck2D::ReservePairs ( u32 total ) {

	this->mSprites.Init ( total );
	
	MOAISprite zero;
	zero.mQuadID		= 0;
	zero.mUVQuadID		= 0;
	
	this->mSprites.Fill ( zero );
}

//----------------------------------------------------------------//
void MOAISpriteDeck2D::ReserveQuads ( u32 total ) {

	this->mQuads.Init ( total );
}

//----------------------------------------------------------------//
void MOAISpriteDeck2D::ReserveUVQuads ( u32 total ) {

	this->mUVQuads.Init ( total );
}

//----------------------------------------------------------------//
void MOAISpriteDeck2D::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {

	MOAIDeck::SerializeIn ( state, serializer );
	MOAIMaterialBatchHolder::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAISpriteDeck2D::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {

	MOAIDeck::SerializeOut ( state, serializer );
	MOAIMaterialBatchHolder::SerializeOut ( state, serializer );
}

//----------------------------------------------------------------//
void MOAISpriteDeck2D::SetList ( u32 idx, u32 basePairID, u32 totalPairs ) {
	
	this->mSpriteLists.Grow ( idx + 1 );
	MOAISpriteList& sprite = this->mSpriteLists [ idx ];
	
	sprite.mBaseSprite = basePairID;
	sprite.mTotalSprites = totalPairs;
}

//----------------------------------------------------------------//
void MOAISpriteDeck2D::SetPair ( u32 idx, u32 uvQuadID, u32 quadID, u32 materialID ) {
	
	this->mSprites.Grow ( idx + 1 );
	MOAISprite& spritePair = this->mSprites [ idx ];
	
	spritePair.mUVQuadID = uvQuadID;
	spritePair.mQuadID = quadID;
	spritePair.mMaterialID = materialID;
}

//----------------------------------------------------------------//
void MOAISpriteDeck2D::SetQuad ( u32 idx, ZLQuad& quad ) {

	this->mQuads.Grow ( idx + 1 );
	this->mQuads [ idx ] = quad;
}

//----------------------------------------------------------------//
void MOAISpriteDeck2D::SetRect ( u32 idx, ZLRect& rect ) {

	this->mQuads.Grow ( idx + 1 );
	this->mQuads [ idx ].Init ( rect );
}

//----------------------------------------------------------------//
void MOAISpriteDeck2D::SetUVQuad ( u32 idx, ZLQuad& quad ) {

	this->mUVQuads.Grow ( idx + 1 );
	this->mUVQuads [ idx ] = quad;
}

//----------------------------------------------------------------//
void MOAISpriteDeck2D::SetUVRect ( u32 idx, ZLRect& rect ) {

	this->mUVQuads.Grow ( idx + 1 );
	this->mUVQuads [ idx ].Init ( rect );
}

//----------------------------------------------------------------//
void MOAISpriteDeck2D::Transform ( const ZLAffine3D& mtx ) {

	size_t total = this->mQuads.Size ();
	for ( size_t i = 0; i < total; ++i ) {
		this->mQuads [ i ].Transform ( mtx );
	}
}

//----------------------------------------------------------------//
void MOAISpriteDeck2D::TransformUV ( const ZLAffine3D& mtx ) {

	size_t total = this->mQuads.Size ();
	for ( size_t i = 0; i < total; ++i ) {
		this->mUVQuads [ i ].Transform ( mtx );
	}
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
ZLBounds MOAISpriteDeck2D::MOAIDeck_ComputeMaxBounds () {

	size_t size = this->mQuads.Size ();
	
	if ( size ) {
	
		ZLRect rect = this->mQuads [ 0 ].GetBounds ();
		
		for ( size_t i = 1; i < size; ++i ) {
			rect.Grow ( this->mQuads [ i ].GetBounds ());
		}
		
		ZLBounds bounds;
		bounds.Init ( rect );
		bounds.mStatus = ZLBounds::ZL_BOUNDS_OK;
		return bounds;
	}
	return ZLBounds::ZL_BOUNDS_EMPTY;
}

//----------------------------------------------------------------//
void MOAISpriteDeck2D::MOAIDeck_Draw ( u32 idx ) {

	MOAIMaterialMgr& materialStack = MOAIMaterialMgr::Get ();

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	MOAIQuadBrush::BindVertexFormat ();
	
	gfxState.SetVertexTransform ( MOAIGfxState::MODEL_TO_DISPLAY_MTX );
	gfxState.SetUVTransform ( MOAIGfxState::UV_TO_MODEL_MTX );

	size_t totalSprites			= this->mSprites.Size ();
	size_t totalSpriteLists		= this->mSpriteLists.Size ();
	size_t totalQuads			= this->mQuads.Size ();
	
	if ( totalSprites ) {

		u32 base;
		u32 top;
		
		if ( totalSpriteLists ) {
		
			MOAISpriteList& spriteList = this->mSpriteLists [ idx % totalSpriteLists ];
			base = spriteList.mBaseSprite;
			top = base + spriteList.mTotalSprites;
		}
		else {
		
			base = idx % totalSprites;
			top = base + 1;
		}
		
		u32 materialID = MOAIMaterialBatch::UNKNOWN;
		materialStack.Push (); // push an empty stack frame to facilitate "swap" below
		
		for ( size_t i = base; i < top; ++i ) {
			
			MOAISprite spritePair = this->mSprites [ i % totalSprites ];
			
			if (( i == base ) || ( materialID != spritePair.mMaterialID )) {
			
				materialID = spritePair.mMaterialID;
				
				MOAIMaterial* spriteMaterial = this->GetMaterial ( materialID );
				if ( spriteMaterial ) {
					
					materialStack.Pop ();
					materialStack.Push ( spriteMaterial );
					materialStack.SetShader ( MOAIShaderMgr::DECK2D_SHADER );
					materialStack.LoadGfxState ();
				}
			}
			
			MOAIQuadBrush glQuad;
			glQuad.mUVQuad = this->mUVQuads [ spritePair.mUVQuadID ];
			glQuad.mModelQuad = this->mQuads [ spritePair.mQuadID ];
			glQuad.Draw ();
		}
		
		materialStack.Pop ();
	}
	else {
		
		MOAIQuadBrush quadBrush;
		MOAIMaterial* material = 0;
		
		if ( totalQuads ) {
		
			size_t itemIdx = idx % totalQuads;
			material = this->GetMaterial ( itemIdx );

			quadBrush.mModelQuad = this->mQuads [ itemIdx ];

			if ( itemIdx < this->mUVQuads.Size ()) {
				quadBrush.mUVQuad = this->mUVQuads [ itemIdx ];
			}
			else {
				quadBrush.mUVQuad.Init ( 0.0f, 1.0f, 1.0f, 0.0f );
			}
		}
		else {
		
			material = this->GetMaterial ();
			
			quadBrush.mModelQuad.Init ( -0.5f, -0.5f, 0.5f, 0.5f );
			quadBrush.mUVQuad.Init ( 0.0f, 1.0f, 1.0f, 0.0f );
		}
		
		materialStack.Push ( material );
		materialStack.SetShader ( MOAIShaderMgr::DECK2D_SHADER );
		materialStack.LoadGfxState ();
		
		quadBrush.Draw ();
		
		materialStack.Pop ();
	}
}

//----------------------------------------------------------------//
ZLBounds MOAISpriteDeck2D::MOAIDeck_GetBounds ( u32 idx ) {

	ZLBounds bounds = ZLBounds::EMPTY;

	size_t totalSprites			= this->mSprites.Size ();
	size_t totalSpriteLists		= this->mSpriteLists.Size ();
	size_t totalQuads			= this->mQuads.Size ();
	
	if ( totalQuads ) {
	
		if ( totalSprites ) {

			u32 base;
			u32 top;
			
			if ( totalSpriteLists ) {
			
				MOAISpriteList& spriteList = this->mSpriteLists [ idx % totalSpriteLists ];
				base = spriteList.mBaseSprite;
				top = base + spriteList.mTotalSprites;
			}
			else {
			
				base = idx % totalSprites;
				top = base + 1;
			}

			ZLRect rect;

			for ( size_t i = base ; i < top; ++i ) {
				
				MOAISprite sprite = this->mSprites [ i % totalSprites ];
				rect.Grow ( this->mQuads [ sprite.mQuadID ].GetBounds (), i > base );
			}
			bounds.Init ( rect );
		}
		else {
		
			ZLRect rect = this->mQuads [ idx % totalQuads ].GetBounds ();
			bounds.Init ( rect );
		}
	}
	else {
	
		bounds.Init ( -0.5f, 0.5f, 0.5f, -0.5f );
	}
	
	return bounds;
}

//----------------------------------------------------------------//
MOAICollisionShape* MOAISpriteDeck2D::MOAIDeck_GetCollisionShape ( u32 idx ) {
	UNUSED(idx);
	return 0;
}

//----------------------------------------------------------------//
bool MOAISpriteDeck2D::MOAIDeck_Overlap ( u32 idx, const ZLVec2D& vec, u32 granularity, ZLBounds* result ) {

	// TODO: handle granularity

	//return this->GetMaterialBatch ()->TestHit ( this, idx, granularity, this->mQuad.mModelQuad, this->mQuad.mUVQuad, vec.mX, vec.mY );
	UNUSED(result);
	UNUSED(granularity);
	UNUSED(vec);
	UNUSED(idx);
	return true;
}

//----------------------------------------------------------------//
bool MOAISpriteDeck2D::MOAIDeck_Overlap ( u32 idx, const ZLVec3D& vec, u32 granularity, ZLBounds* result ) {

	// TODO: handle granularity

	return (( vec.mZ == 0.0f ) && this->MOAIDeck_Overlap ( idx, vec, granularity, result ));
}
