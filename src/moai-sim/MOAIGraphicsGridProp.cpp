// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIDebugLines.h>
#include <moai-sim/MOAIFancyGrid.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGraphicsGridProp.h>
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAILayoutFrame.h>
#include <moai-sim/MOAIMaterialBatch.h>
#include <moai-sim/MOAIPartition.h>
#include <moai-sim/MOAIPartitionResultBuffer.h>
#include <moai-sim/MOAIRenderMgr.h>
#include <moai-sim/MOAIScissorRect.h>
#include <moai-sim/MOAIShader.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAISurfaceSampler2D.h>
#include <moai-sim/MOAITexture.h>
#include <moai-sim/MOAITextureBase.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//

//================================================================//
// MOAIGraphicsGridProp
//================================================================//

//----------------------------------------------------------------//
//ZLAffine3D MOAIGraphicsGridProp::AppendRot90SclTr ( const ZLAffine3D& mtx, const ZLAffine3D& append ) {
//
//	// don't need a general purpose matrix mult to handle just scale and offset.
//	// can omit a lot of the multiplications.
//
//	// 0 s 0 z
//	// s 0 0 y
//	// 0 0 1 0
//
//	ZLAffine3D result;
//
//	result.m [ ZLAffine3D::C0_R0 ]	=	( append.m [ ZLAffine3D::C1_R0 ] * mtx.m [ ZLAffine3D::C0_R1 ]);
//	result.m [ ZLAffine3D::C0_R1 ]	=	( append.m [ ZLAffine3D::C1_R1 ] * mtx.m [ ZLAffine3D::C0_R1 ]);
//	result.m [ ZLAffine3D::C0_R2 ]	=	( append.m [ ZLAffine3D::C1_R2 ] * mtx.m [ ZLAffine3D::C0_R1 ]);
//
//	result.m [ ZLAffine3D::C1_R0 ]	=	( append.m [ ZLAffine3D::C0_R0 ] * mtx.m [ ZLAffine3D::C1_R0 ]);
//	result.m [ ZLAffine3D::C1_R1 ]	=	( append.m [ ZLAffine3D::C0_R1 ] * mtx.m [ ZLAffine3D::C1_R0 ]);
//	result.m [ ZLAffine3D::C1_R2 ]	=	( append.m [ ZLAffine3D::C0_R2 ] * mtx.m [ ZLAffine3D::C1_R0 ]);
//	
//	result.m [ ZLAffine3D::C2_R0 ]	=	append.m [ ZLAffine3D::C2_R0 ];
//	result.m [ ZLAffine3D::C2_R1 ]	=	append.m [ ZLAffine3D::C2_R1 ];
//	result.m [ ZLAffine3D::C2_R2 ]	=	append.m [ ZLAffine3D::C2_R2 ];
//	
//	result.m [ ZLAffine3D::C3_R0 ]	=	( append.m [ ZLAffine3D::C0_R0 ] * mtx.m [ ZLAffine3D::C3_R0 ])	+
//										( append.m [ ZLAffine3D::C1_R0 ] * mtx.m [ ZLAffine3D::C3_R1 ])	+
//										( append.m [ ZLAffine3D::C3_R0 ]);
//	
//	result.m [ ZLAffine3D::C3_R1 ]	=	( append.m [ ZLAffine3D::C0_R1 ] * mtx.m [ ZLAffine3D::C3_R0 ])	+
//										( append.m [ ZLAffine3D::C1_R1 ] * mtx.m [ ZLAffine3D::C3_R1 ])	+
//										( append.m [ ZLAffine3D::C3_R1 ]);
//	
//	result.m [ ZLAffine3D::C3_R2 ]	=	( append.m [ ZLAffine3D::C0_R2 ] * mtx.m [ ZLAffine3D::C3_R0 ])	+
//										( append.m [ ZLAffine3D::C1_R2 ] * mtx.m [ ZLAffine3D::C3_R1 ])	+
//										( append.m [ ZLAffine3D::C3_R2 ]);
//	
//	return result;
//}

//----------------------------------------------------------------//
//ZLAffine3D MOAIGraphicsGridProp::AppendSclTr ( const ZLAffine3D& mtx, const ZLAffine3D& append ) {
//
//	// don't need a general purpose matrix mult to handle just scale and offset.
//	// can omit a lot of the multiplications.
//
//	// s 0 0 x
//	// 0 s 0 y
//	// 0 0 1 0
//
//	ZLAffine3D result;
//
//	result.m [ ZLAffine3D::C0_R0 ]	=	( append.m [ ZLAffine3D::C0_R0 ] * mtx.m [ ZLAffine3D::C0_R0 ]);
//	result.m [ ZLAffine3D::C0_R1 ]	=	( append.m [ ZLAffine3D::C0_R1 ] * mtx.m [ ZLAffine3D::C0_R0 ]);
//	result.m [ ZLAffine3D::C0_R2 ]	=	( append.m [ ZLAffine3D::C0_R2 ] * mtx.m [ ZLAffine3D::C0_R0 ]);
//
//	result.m [ ZLAffine3D::C1_R0 ]	=	( append.m [ ZLAffine3D::C1_R0 ] * mtx.m [ ZLAffine3D::C1_R1 ]);
//	result.m [ ZLAffine3D::C1_R1 ]	=	( append.m [ ZLAffine3D::C1_R1 ] * mtx.m [ ZLAffine3D::C1_R1 ]);
//	result.m [ ZLAffine3D::C1_R2 ]	=	( append.m [ ZLAffine3D::C1_R2 ] * mtx.m [ ZLAffine3D::C1_R1 ]);
//	
//	result.m [ ZLAffine3D::C2_R0 ]	=	append.m [ ZLAffine3D::C2_R0 ];
//	result.m [ ZLAffine3D::C2_R1 ]	=	append.m [ ZLAffine3D::C2_R1 ];
//	result.m [ ZLAffine3D::C2_R2 ]	=	append.m [ ZLAffine3D::C2_R2 ];
//	
//	result.m [ ZLAffine3D::C3_R0 ]	=	( append.m [ ZLAffine3D::C0_R0 ] * mtx.m [ ZLAffine3D::C3_R0 ])	+
//										( append.m [ ZLAffine3D::C1_R0 ] * mtx.m [ ZLAffine3D::C3_R1 ])	+
//										( append.m [ ZLAffine3D::C3_R0 ]);
//	
//	result.m [ ZLAffine3D::C3_R1 ]	=	( append.m [ ZLAffine3D::C0_R1 ] * mtx.m [ ZLAffine3D::C3_R0 ])	+
//										( append.m [ ZLAffine3D::C1_R1 ] * mtx.m [ ZLAffine3D::C3_R1 ])	+
//										( append.m [ ZLAffine3D::C3_R1 ]);
//	
//	result.m [ ZLAffine3D::C3_R2 ]	=	( append.m [ ZLAffine3D::C0_R2 ] * mtx.m [ ZLAffine3D::C3_R0 ])	+
//										( append.m [ ZLAffine3D::C1_R2 ] * mtx.m [ ZLAffine3D::C3_R1 ])	+
//										( append.m [ ZLAffine3D::C3_R2 ]);
//	
//	return result;
//}

//----------------------------------------------------------------//
void MOAIGraphicsGridProp::DrawGrid ( const MOAICellCoord &c0, const MOAICellCoord &c1 ) {

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	ZLVec3D offset	= ZLVec3D::ORIGIN;
	ZLVec3D scale	= ZLVec3D::AXIS;
	
	assert ( this->mGrid );
	MOAIGrid& grid = *this->mGrid;
	
	float tileWidth = grid.GetTileWidth ();
	float tileHeight = grid.GetTileHeight ();
	
	// is this grid fancy?
	MOAIFancyGrid* fancyGrid = this->mGrid->AsType < MOAIFancyGrid >();
	
	const ZLAffine3D& modelToWorldMtx = this->MOAIGraphicsPropBase_GetWorldDrawingMtx ();
	ZLColorVec penColor = gfxState.GetPenColor ();
	
	for ( int y = c0.mY; y <= c1.mY; ++y ) {
		for ( int x = c0.mX; x <= c1.mX; ++x ) {
			
			int addr = grid.GetCellAddr ( x, y );
			u32 idx = grid.GetTile ( addr );
			
			if ( !idx || ( idx & MOAITileFlags::HIDDEN )) continue;
			
			MOAICellCoord coord ( x, y );
			ZLVec2D loc = grid.GetTilePoint ( coord, MOAIGridSpace::TILE_CENTER );

			float xScale = ( idx & MOAITileFlags::XFLIP ) ? -tileWidth : tileWidth;
			float yScale = ( idx & MOAITileFlags::YFLIP ) ? -tileHeight : tileHeight;

			ZLAffine3D mtx = modelToWorldMtx;

			if ( idx & MOAITileFlags::ROT_90 ) {
				mtx.PrependRot90SclTr2D ( xScale, yScale, loc.mX, loc.mY );
			}
			else {
				mtx.PrependSclTr2D ( xScale, yScale, loc.mX, loc.mY );
			}

			gfxState.SetMtx ( MOAIGfxState::MODEL_TO_WORLD_MTX, mtx );
			
			if ( fancyGrid ) {
				gfxState.SetPenColor ( penColor * fancyGrid->GetTileColor ( addr ));
			}
			
			this->mDeck->Draw (( idx & MOAITileFlags::CODE_MASK ) - 1 );
		}
	}
}

//----------------------------------------------------------------//
MOAIGraphicsGridProp::MOAIGraphicsGridProp () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIGridPropBase )
		RTTI_EXTEND ( MOAIGraphicsPropBase )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIGraphicsGridProp::~MOAIGraphicsGridProp () {
}

//----------------------------------------------------------------//
void MOAIGraphicsGridProp::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIGridPropBase::RegisterRubyClass ( state, klass );
	MOAIGraphicsPropBase::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIGraphicsGridProp::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIGridPropBase::RegisterRubyFuncs ( state, klass );
	MOAIGraphicsPropBase::RegisterRubyFuncs ( state, klass );
}

//----------------------------------------------------------------//
void MOAIGraphicsGridProp::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	
	MOAIGridPropBase::SerializeIn ( state, serializer );
	MOAIGraphicsPropBase::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIGraphicsGridProp::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	
	MOAIGridPropBase::SerializeOut ( state, serializer );
	MOAIGraphicsPropBase::SerializeOut ( state, serializer );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAIGraphicsGridProp::MOAIDrawable_Draw ( int subPrimID ) {
	UNUSED ( subPrimID );

	if ( !this->IsVisible ()) return;
	if ( !this->mDeck ) return;
	if ( this->IsClear ()) return;

	this->PushGfxState ();
	this->LoadUVTransform ();
	
	MOAICellCoord c0, c1;

	if ( subPrimID == MOAIPartitionHull::NO_SUBPRIM_ID ) {
		this->GetGridBoundsInView ( this->GetWorldToLocalMtx (), c0, c1 );
	}
	else {
		c0 = c1 = this->mGrid->GetCellCoord ( subPrimID );
	}
	
	this->DrawGrid ( c0, c1 );
	
	this->PopGfxState ();
}

//----------------------------------------------------------------//
bool MOAIGraphicsGridProp::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {
	
	if ( MOAIGraphicsPropBase::MOAINode_ApplyAttrOp ( attrID, attr, op )) return true;
	if ( MOAIPartitionHull::MOAINode_ApplyAttrOp ( attrID, attr, op )) return true;
	return false;
}

//----------------------------------------------------------------//
void MOAIGraphicsGridProp::MOAINode_Update () {
	
	MOAIGraphicsPropBase::MOAINode_Update ();
}

//----------------------------------------------------------------//
void MOAIGraphicsGridProp::MOAIPartitionHull_AddToSortBuffer ( MOAIPartitionResultBuffer& buffer, u32 key ) {

	if (( this->mFlags & FLAGS_EXPAND_FOR_SORT ) && this->mGrid && this->mDeck ) {
		
		// add a sub-prim for each visible grid cell
		const ZLAffine3D& mtx = this->GetLocalToWorldMtx ();
		
		MOAIGrid& grid = *this->mGrid;
		
		MOAICellCoord c0;
		MOAICellCoord c1;
		
		// TODO: this needs to be pushed up one level to GatherHulls
		// should not assume anything about the view or rendering
		// only need to do this if we have a frustum - will break
		// expected results for other queries
		this->GetGridBoundsInView ( this->GetWorldToLocalMtx (), c0, c1 );

		for ( int y = c0.mY; y <= c1.mY; ++y ) {
			for ( int x = c0.mX; x <= c1.mX; ++x ) {
				
				MOAICellCoord wrap = grid.WrapCellCoord ( x, y );
				u32 idx = grid.GetTile ( wrap.mX, wrap.mY );
				if ( !idx || ( idx & MOAITileFlags::HIDDEN )) continue;
				
				MOAICellCoord coord ( x, y );
				int subPrimID = grid.GetCellAddr ( coord );
				
				ZLVec3D loc;
				loc.Init ( grid.GetTilePoint ( coord, MOAIGridSpace::TILE_CENTER ));
				
				ZLBox bounds = this->mDeck->GetBounds ( idx );
				bounds.Offset ( loc );
				
				mtx.Transform ( loc );
				bounds.Transform ( mtx );
				
				buffer.PushResult ( *this, key, subPrimID, this->GetPriority (), loc, this->GetWorldBounds ()); // TODO: should use tile bounds for expand mode
			}
		}
	}
	else {
		buffer.PushResult ( *this, key, NO_SUBPRIM_ID, this->GetPriority (), this->GetWorldLoc (), this->GetWorldBounds ());
	}
}

//----------------------------------------------------------------//
ZLBounds MOAIGraphicsGridProp::MOAIPartitionHull_GetModelBounds () {
	
	if ( this->mGrid ) {
		
		if ( this->mGrid->GetRepeat ()) {
			return ZLBounds::GLOBAL;
		}
		ZLBounds bounds;
		bounds.Init ( this->mGrid->GetBounds ());
		return bounds;
	}
	return ZLBounds::EMPTY;
}
