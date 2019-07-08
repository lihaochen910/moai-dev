// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAIMaterialMgr.h>
#include <moai-sim/MOAIQuadBrush.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAIStretchPatch2D.h>
#include <moai-sim/MOAITextureBase.h>
#include <moai-sim/MOAITransformBase.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	ninePatch
	@text	Set up a nine patch using Android-style sections (25%, 50%, 25%).
 
	@in		MOAIStretchPatch2D self
	@out	nil
*/
mrb_value MOAIStretchPatch2D::_ninePatch ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStretchPatch2D, "U" )

	self->mRows.Init ( 3 );
	self->SetRow ( 0, 0.25, false );
	self->SetRow ( 1, 0.50, true );
	self->SetRow ( 2, 0.25, false );

	self->mCols.Init ( 3 );
	self->SetColumn ( 0, 0.25, false );
	self->SetColumn ( 1, 0.50, true );
	self->SetColumn ( 2, 0.25, false );

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	reserveColumns
	@text	Reserve total columns in patch.
	
	@in		MOAIStretchPatch2D self
	@in		number nColumns
	@out	nil
*/
mrb_value MOAIStretchPatch2D::_reserveColumns ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStretchPatch2D, "UN" )

	u32 total = state.GetParamValue < u32 >( 1, 0 );
	self->mCols.Init ( total );

	self->mNeedsUpdate = true;
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	reserveRows
	@text	Reserve total rows in patch.
	
	@in		MOAIStretchPatch2D self
	@in		number nRows
	@out	nil
*/
mrb_value MOAIStretchPatch2D::_reserveRows ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStretchPatch2D, "UN" )

	u32 total = state.GetParamValue < u32 >( 1, 0 );
	self->mRows.Init ( total );

	self->mNeedsUpdate = true;
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	reserveUVRects
	@text	Reserve total UV rects in patch. When a patch is indexed
			it will change its UV rects.
	
	@in		MOAIStretchPatch2D self
	@in		number nUVRects
	@out	nil
*/
mrb_value MOAIStretchPatch2D::_reserveUVRects ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStretchPatch2D, "UN" )

	u32 total = state.GetParamValue < u32 >( 1, 0 );
	self->mUVRects.Init ( total );

	for ( u32 i = 0; i < total; ++i ) {
		self->mUVRects [ i ].Init ( 0.0f, 1.0f, 1.0f, 0.0f );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setColumn
	@text	Set the stretch properties of a patch column.
	
	@in		MOAIStretchPatch2D self
	@in		number idx
	@in		number percent
	@in		boolean canStretch
	@out	nil
*/
mrb_value MOAIStretchPatch2D::_setColumn ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStretchPatch2D, "UNNB" )

	u32 idx				= state.GetParamValue < u32 >( 1, 1 ) - 1;
	float percent		= state.GetParamValue < float >( 2, 0.0f );
	bool canStretch		= state.GetParamValue < bool >( 3, false );

	if ( MOAILogMgr::CheckIndexPlusOne ( idx, self->mCols.Size (), M )) {
		self->SetColumn ( idx, percent, canStretch );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setRect
	@text	Set the model space dimensions of the patch.
	
	@in		MOAIStretchPatch2D self
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@out	nil
*/
mrb_value MOAIStretchPatch2D::_setRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStretchPatch2D, "UNNNN" )
	
	self->mRect = state.GetRect < float >( 1 );
	self->SetBoundsDirty ();
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setRow
	@text	Set the stretch properties of a patch row.
	
	@in		MOAIStretchPatch2D self
	@in		number idx
	@in		number percent
	@in		boolean canStretch
	@out	nil
*/
mrb_value MOAIStretchPatch2D::_setRow ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStretchPatch2D, "UNNB" )

	u32 idx				= state.GetParamValue < u32 >( 1, 1 ) - 1;
	float percent		= state.GetParamValue < float >( 2, 0.0f );
	bool canStretch		= state.GetParamValue < bool >( 3, false );

	if ( MOAILogMgr::CheckIndexPlusOne ( idx, self->mRows.Size (), M )) {
		self->SetRow ( idx, percent, canStretch );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setUVRect
	@text	Set the UV space dimensions of the patch.
	
	@in		MOAIStretchPatch2D self
	@in		number idx
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@out	nil
*/
mrb_value MOAIStretchPatch2D::_setUVRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIStretchPatch2D, "UNNNNN" )
	
	u32 idx = state.GetParamValue < u32 >( 1, 1 ) - 1;
	
	if ( MOAILogMgr::CheckIndexPlusOne ( idx, self->mUVRects.Size (), M )) {
		self->mUVRects [ idx ] = state.GetRect < float >( 2 );
	}
	return mrb_nil_value ();
}

//================================================================//
// MOAIStretchPatch2D
//================================================================//

//----------------------------------------------------------------//
void MOAIStretchPatch2D::DrawStretch ( u32 idx, float xStretch, float yStretch ) {

	ZLRect uvRect;
	u32 totalUVRects = ( u32 )this->mUVRects.Size ();
	
	if ( totalUVRects == 0 ) {
		uvRect.Init ( 0.0f, 1.0f, 1.0f, 0.0f );
	}
	else {
		idx = ( idx - 1 ) % totalUVRects;
		uvRect = this->mUVRects [ idx ];
	}

	float nativeWidth = this->mRect.Width ();
	float nativeHeight = this->mRect.Height ();
	
	float rectWidth = nativeWidth * xStretch;
	float rectHeight = nativeHeight * yStretch;
	
	float xMin = this->mRect.mXMin * xStretch;
	float yMin = this->mRect.mYMin * yStretch;

	float uMin = uvRect.mXMin;
	float vMin = uvRect.mYMin;

	// scale for x patches
	float xPatchScale = 1.0f;
	float xStretchPatchScale = 1.0f;
	
	if ( rectWidth > nativeWidth ) {
		xStretchPatchScale = ( rectWidth - ( nativeWidth * this->mXFix )) / ( nativeWidth * this->mXFlex );
	}
	else {
		xPatchScale = rectWidth / nativeWidth;
		xStretchPatchScale = xPatchScale;
	}

	// scale for y patches
	float yPatchScale = 1.0f;
	float yStretchPatchScale = 1.0f;
	
	if ( rectHeight > nativeHeight ) {
		yStretchPatchScale = ( rectHeight - ( nativeHeight * this->mYFix )) / ( nativeHeight * this->mYFlex );
	}
	else {
		yPatchScale = rectHeight / nativeHeight;
		yStretchPatchScale = yPatchScale;
	}
	
	u32 totalRows = ( u32 )this->mRows.Size ();
	u32 totalCols = ( u32 )this->mCols.Size ();
	
	MOAIQuadBrush quad;
	
	float uSpan = uvRect.mXMax - uvRect.mXMin;
	float vSpan = uvRect.mYMax - uvRect.mYMin;
	
	float y = yMin;
	float v = vMin;
	
	for ( u32 i = 0; i < totalRows; ++i ) {
		
		MOAIStretchPatchSpan& row = this->mRows [ i ];
		float vStep = row.mPercent * vSpan;
		
		float h = nativeHeight * row.mPercent;
		if ( row.mCanStretch ) {
			h *= yStretchPatchScale;
		}
		else {
			h *= yPatchScale;
		}
		
		float x = xMin;
		float u = uMin;
		
		for ( u32 j = 0; j < totalCols; ++j ) {
			
			MOAIStretchPatchSpan& col = this->mCols [ j ];
			float uStep = col.mPercent * uSpan;
			
			float w = nativeWidth * col.mPercent;
			if ( col.mCanStretch ) {
				w *= xStretchPatchScale;
			}
			else {
				w *= xPatchScale;
			}
			
			quad.SetVerts ( x, y, x + w, y + h );
			quad.SetUVs ( u, v, u + uStep, v + vStep );
			quad.Draw ();
			
			x += w;
			u += uStep;
		}
		
		y += h;
		v += vStep;
	}
}

//----------------------------------------------------------------//
MOAIStretchPatch2D::MOAIStretchPatch2D () :
	mNeedsUpdate ( true ) {

	RTTI_BEGIN
		RTTI_EXTEND ( MOAIStretchDeck )
		RTTI_EXTEND ( MOAIMaterialBatchHolder )
	RTTI_END
	
	this->mRect.Init ( 0.0f, 0.0f, 0.0f, 0.0f );
}

//----------------------------------------------------------------//
MOAIStretchPatch2D::~MOAIStretchPatch2D () {
}

//----------------------------------------------------------------//
void MOAIStretchPatch2D::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIStretchDeck::RegisterRubyClass ( state, klass );
	MOAIMaterialBatchHolder::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIStretchPatch2D::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIStretchDeck::RegisterRubyFuncs ( state, klass );
	MOAIMaterialBatchHolder::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "ninePatch",			_ninePatch, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "reserveColumns",		_reserveColumns, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "reserveRows",		_reserveRows, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "reserveUVRects",		_reserveUVRects, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setColumn",			_setColumn, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "setRect",			_setRect, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "setRow",				_setRow, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "setUVRect",			_setUVRect, MRB_ARGS_REQ ( 5 ) );

}

//----------------------------------------------------------------//
void MOAIStretchPatch2D::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAIStretchPatch2D::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAIStretchPatch2D::UpdateParams () {
	
	if ( !this->mNeedsUpdate ) return;
	
	this->mYFix = 0.0f;
	this->mYFlex = 0.0f;
	
	size_t totalRows = this->mRows.Size ();
	for ( size_t i = 0; i < totalRows; ++i ) {
		MOAIStretchPatchSpan& span = this->mRows [ i ];
		if ( span.mCanStretch ) {
			this->mYFlex += span.mPercent;
		}
		else {
			this->mYFix += span.mPercent;
		}
	}
	
	this->mXFix = 0.0f;
	this->mXFlex = 0.0f;
	
	size_t totalCols = this->mCols.Size ();
	for ( size_t i = 0; i < totalCols; ++i ) {
		MOAIStretchPatchSpan& span = this->mCols [ i ];
		if ( span.mCanStretch ) {
			this->mXFlex += span.mPercent;
		}
		else {
			this->mXFix += span.mPercent;
		}
	}
	
	this->mNeedsUpdate = false;
}

//----------------------------------------------------------------//
void MOAIStretchPatch2D::SetColumn ( u32 idx, float percent, bool canStretch ) {

	assert ( idx < this->mCols.Size ());

	this->mCols [ idx ].mPercent = percent;
	this->mCols [ idx ].mCanStretch = canStretch;
	this->mNeedsUpdate = true;
}

//----------------------------------------------------------------//
void MOAIStretchPatch2D::SetRow ( u32 idx, float percent, bool canStretch ) {

	assert ( idx < this->mRows.Size ());

	this->mRows [ idx ].mPercent = percent;
	this->mRows [ idx ].mCanStretch = canStretch;
	this->mNeedsUpdate = true;
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
ZLBounds MOAIStretchPatch2D::MOAIDeck_ComputeMaxBounds () {

	return this->GetBounds ( 0 );
}

//----------------------------------------------------------------//
ZLBounds MOAIStretchPatch2D::MOAIDeck_GetBounds ( u32 idx ) {
	UNUSED ( idx );

	return ZLBounds ( this->mRect );
}

//----------------------------------------------------------------//
void MOAIStretchPatch2D::MOAIDeck_Draw ( u32 idx ) {
	
	this->UpdateParams ();
	
	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	ZLVec3D stretch = this->BindStretchVertexTransform ();
	gfxState.SetUVTransform ( MOAIGfxState::UV_TO_MODEL_MTX );
	
	MOAIQuadBrush::BindVertexFormat ();
	
	MOAIMaterialMgr& materialStack = MOAIMaterialMgr::Get ();
	materialStack.Push ( this->GetMaterial ( idx ));
	materialStack.SetShader ( MOAIShaderMgr::DECK2D_SHADER );
	materialStack.LoadGfxState ();
	
	this->DrawStretch ( idx, stretch.mX, stretch.mY );
	
	materialStack.Pop ();
}

//----------------------------------------------------------------//
MOAICollisionShape* MOAIStretchPatch2D::MOAIDeck_GetCollisionShape ( u32 idx ) {
	UNUSED ( idx );

	return 0;
}

//----------------------------------------------------------------//
bool MOAIStretchPatch2D::MOAIDeck_Overlap ( u32 idx, const ZLVec2D& vec, u32 granularity, ZLBounds* result ) {
	UNUSED ( idx );
	UNUSED ( vec );
	UNUSED ( granularity );
	UNUSED ( result );

	return false;
}

//----------------------------------------------------------------//
bool MOAIStretchPatch2D::MOAIDeck_Overlap ( u32 idx, const ZLVec3D& vec, u32 granularity, ZLBounds* result ) {
	UNUSED ( idx );
	UNUSED ( vec );
	UNUSED ( granularity );
	UNUSED ( result );

	return false;
}
