// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIDebugLines.h>
#include <moai-sim/MOAIDraw.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGraphicsPropBase.h>
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAILayoutFrame.h>
#include <moai-sim/MOAIMaterialMgr.h>
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
// TODO: doxygen
mrb_value MOAIGraphicsPropBase::_draw ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGraphicsPropBase, "U" )
	
	u32 subPrimID = state.GetParamValue < u32 >( 1, (u32) MOAIPartitionHull::NO_SUBPRIM_ID );
	
	self->Draw ( (int) subPrimID );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIGraphicsPropBase::_getBillboard ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGraphicsPropBase, "U" )
	
	return state.ToRValue ( self->mBillboard );
}

//----------------------------------------------------------------//
/**	@name	getScissorRect
	@text	Retrieve the prop's scissor rect.
	
	@in		MOAIGraphicsPropBase self
	@out	MOAIScissorRect scissorRect 	Or nil if none exists.
*/
mrb_value MOAIGraphicsPropBase::_getScissorRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGraphicsPropBase, "U" )
	
	if ( self->mScissorRect ) {
		return self->mScissorRect->PushRubyUserdata ( state );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	isVisible
	@text	Returns true if the given prop is visible. An optional
			LOD factor may be passed in to test the prop's LOD
			settings.
	
	@in		MOAIGraphicsPropBase self
	@out	boolean isVisible		Indicates whether the prop is visible.
*/
mrb_value	MOAIGraphicsPropBase::_isVisible ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGraphicsPropBase, "U" )

	// if ( state.ParamIsType ( 2, LUA_TNUMBER )) {
	// 	lua_pushboolean ( state, self->IsVisible ());
	// }
	// else {
	// 	lua_pushboolean ( state, self->IsVisible ());
	// }
	return state.ToRValue ( self->IsVisible () );
}

//----------------------------------------------------------------//
/**	@lua	setBillboard
	@text	Sets the prop's billboarding mode. One of BILLBOARD_NORMAL,
			BILLBOARD_ORTHO, BILLBOARD_COMPASS, BILLBOARD_COMPASS_SCALE,
			BILLBOARD_SCREEN, BILLBOARD_NONE.
	
	@override
	
		@in		MOAIGraphicsPropBase self
		@in		boolean billboard			true == BILLBOARD_NORMAL, false == BILLBOARD_NONE
		@out	nil
	
	@override
	
		@in		MOAIGraphicsPropBase self
		@in		number mode
		@out	nil
*/
mrb_value MOAIGraphicsPropBase::_setBillboard ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGraphicsPropBase, "U" )

	if ( state.ParamIsType ( 1, MRB_TT_FIXNUM )) {
		bool billboard = state.GetParamValue < bool >( 1, false );
		self->mBillboard = billboard ? BILLBOARD_NORMAL : BILLBOARD_NONE;
	}
	else {
		self->mBillboard = state.GetParamValue < u32 >( 1, BILLBOARD_NONE );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setParent
	@text	This method has been deprecated. Use MOAINode setAttrLink instead.
	
	@in		MOAIGraphicsPropBase self
	@opt	MOAINode parent		Default value is nil.
	@out	nil
*/
mrb_value MOAIGraphicsPropBase::_setParent ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGraphicsPropBase, "U" )

	MOAINode* parent = state.GetRubyObject < MOAINode >( 1, true );
	
	self->SetAttrLink ( PACK_ATTR ( MOAIColor, INHERIT_COLOR ), parent, PACK_ATTR ( MOAIColor, COLOR_TRAIT ));
	self->SetAttrLink ( PACK_ATTR ( MOAITransformBase, INHERIT_TRANSFORM ), parent, PACK_ATTR ( MOAITransformBase, TRANSFORM_TRAIT ));
	self->SetAttrLink ( PACK_ATTR ( MOAIGraphicsPropBase, INHERIT_VISIBLE ), parent, PACK_ATTR ( MOAIGraphicsPropBase, ATTR_VISIBLE ));
	
	//MOAILogF ( state, MOAISTRING_FunctionDeprecated_S, "setParent" );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setScissorRect
	@text	Set or clear the prop's scissor rect.
	
	@in		MOAIGraphicsPropBase self
	@opt	MOAIScissorRect scissorRect		Default value is nil.
	@out	nil
*/
mrb_value MOAIGraphicsPropBase::_setScissorRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGraphicsPropBase, "U" )
	
	MOAIScissorRect* scissorRect = state.GetRubyObject < MOAIScissorRect >( 1, true );
	self->mScissorRect.Set ( *self, scissorRect );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setUVTransform
	@text	Sets or clears the prop's UV transform.
	
	@in		MOAIGraphicsPropBase self
	@opt	MOAITransformBase transform	Default value is nil.
	@out	nil
*/
mrb_value MOAIGraphicsPropBase::_setUVTransform ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGraphicsPropBase, "U" )

	MOAITransformBase* transform = state.GetRubyObject < MOAITransformBase >( 1, true );
	self->SetDependentMember < MOAITransformBase >( self->mUVTransform, transform );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setVisible
	@text	Sets or clears the prop's visibility.
	
	@in		MOAIGraphicsPropBase self
	@opt	boolean visible		Default value is true.
	@out	nil
*/
mrb_value MOAIGraphicsPropBase::_setVisible ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGraphicsPropBase, "U" )

	bool visible = state.GetParamValue < bool >( 1, true );
	self->SetVisible ( visible );

	return context;
}

//================================================================//
// MOAIGraphicsPropBase
//================================================================//

//----------------------------------------------------------------//
MOAIRubyClass* MOAIGraphicsPropBase::GetSuperRubyClass () {

	return MOAIRubyObject::GetRubyClass ();
}

//----------------------------------------------------------------//
bool MOAIGraphicsPropBase::IsVisible () {
	return (( this->mDisplayFlags & FLAGS_LOCAL_VISIBLE ) && ( this->mDisplayFlags & FLAGS_VISIBLE ));
}

//----------------------------------------------------------------//
void MOAIGraphicsPropBase::LoadUVTransform () {

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	if ( this->mUVTransform ) {
		ZLAffine3D uvMtx = this->mUVTransform->GetLocalToWorldMtx ();
		gfxState.SetMtx ( MOAIGfxState::MODEL_TO_UV_MTX, uvMtx );
	}
	else {
		gfxState.SetMtx ( MOAIGfxState::MODEL_TO_UV_MTX );
	}
}

//----------------------------------------------------------------//
void MOAIGraphicsPropBase::LoadVertexTransform () {

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	gfxState.SetMtx ( MOAIGfxState::MODEL_TO_WORLD_MTX, this->MOAIGraphicsPropBase_GetWorldDrawingMtx ());
}

//----------------------------------------------------------------//
MOAIGraphicsPropBase::MOAIGraphicsPropBase () :
	mBillboard ( BILLBOARD_NONE ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIPartitionHull )
		RTTI_EXTEND ( MOAIColor )
		RTTI_EXTEND ( MOAIDrawable )
		RTTI_EXTEND ( MOAIDrawable )
		RTTI_EXTEND ( MOAIMaterialBatchHolder )
	RTTI_END
	
	this->mDisplayFlags = DEFAULT_FLAGS;
}

//----------------------------------------------------------------//
MOAIGraphicsPropBase::~MOAIGraphicsPropBase () {
	
	this->mUVTransform.Set ( *this, 0 );
	this->mScissorRect.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAIGraphicsPropBase::PopGfxState () {

	MOAIMaterialMgr::Get ().Pop ();
}

//----------------------------------------------------------------//
void MOAIGraphicsPropBase::PushGfxState () {

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	gfxState.SetPenColor ( this->mColor );

	MOAIMaterialMgr::Get ().Push ( this->GetMaterial ());
	
	if ( this->mScissorRect ) {
		ZLRect scissorRect = this->mScissorRect->GetScissorRect ( gfxState.GetWorldToWndMtx ());
		gfxState.SetScissorRect ( scissorRect );
	}
	else {
		gfxState.SetScissorRect ();
	}
}

//----------------------------------------------------------------//
void MOAIGraphicsPropBase::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIPartitionHull::RegisterRubyClass ( state, klass );
	MOAIColor::RegisterRubyClass ( state, klass );
	MOAIMaterialBatchHolder::RegisterRubyClass ( state, klass );
	
	MOAIDebugLinesMgr::Get ().ReserveStyleSet < MOAIGraphicsPropBase >( TOTAL_DEBUG_LINE_STYLES );
	
	state.DefineClassConst ( klass, "DEBUG_DRAW_GFX_PROP_MASTER",			MOAIDebugLinesMgr::Pack < MOAIGraphicsPropBase >( (u32) -1 ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_PARTITION_CELLS",			MOAIDebugLinesMgr::Pack < MOAIGraphicsPropBase >( DEBUG_DRAW_PARTITION_CELLS ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_PARTITION_PADDED_CELLS",	MOAIDebugLinesMgr::Pack < MOAIGraphicsPropBase >( DEBUG_DRAW_PARTITION_PADDED_CELLS ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_AXIS",						MOAIDebugLinesMgr::Pack < MOAIGraphicsPropBase >( DEBUG_DRAW_AXIS ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_DIAGONALS",					MOAIDebugLinesMgr::Pack < MOAIGraphicsPropBase >( DEBUG_DRAW_DIAGONALS ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_MODEL_BOUNDS",				MOAIDebugLinesMgr::Pack < MOAIGraphicsPropBase >( DEBUG_DRAW_MODEL_BOUNDS ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_WORLD_BOUNDS",				MOAIDebugLinesMgr::Pack < MOAIGraphicsPropBase >( DEBUG_DRAW_WORLD_BOUNDS ));
	
	state.DefineClassConst ( klass, "ATTR_SCISSOR_RECT",			MOAIGraphicsPropBaseAttr::Pack ( ATTR_SCISSOR_RECT ));

	state.DefineClassConst ( klass, "ATTR_LOCAL_VISIBLE",			MOAIGraphicsPropBaseAttr::Pack ( ATTR_LOCAL_VISIBLE ));
	state.DefineClassConst ( klass, "ATTR_VISIBLE",					MOAIGraphicsPropBaseAttr::Pack ( ATTR_VISIBLE ));
	state.DefineClassConst ( klass, "INHERIT_VISIBLE",				MOAIGraphicsPropBaseAttr::Pack ( INHERIT_VISIBLE ));

	state.DefineClassConst ( klass, "INHERIT_FRAME",				MOAIGraphicsPropBaseAttr::Pack ( INHERIT_FRAME ));
	state.DefineClassConst ( klass, "FRAME_TRAIT",					MOAIGraphicsPropBaseAttr::Pack ( FRAME_TRAIT ));
	
	state.DefineClassConst ( klass, "GL_FUNC_ADD",					( u32 )ZGL_BLEND_MODE_ADD );
	state.DefineClassConst ( klass, "GL_FUNC_SUBTRACT",				( u32 )ZGL_BLEND_MODE_SUBTRACT );
	state.DefineClassConst ( klass, "GL_FUNC_REVERSE_SUBTRACT",		( u32 )ZGL_BLEND_MODE_REVERSE_SUBTRACT );
	
	state.DefineClassConst ( klass, "GL_ONE",						( u32 )ZGL_BLEND_FACTOR_ONE );
	state.DefineClassConst ( klass, "GL_ZERO",						( u32 )ZGL_BLEND_FACTOR_ZERO );
	state.DefineClassConst ( klass, "GL_DST_ALPHA",					( u32 )ZGL_BLEND_FACTOR_DST_ALPHA );
	state.DefineClassConst ( klass, "GL_DST_COLOR",					( u32 )ZGL_BLEND_FACTOR_DST_COLOR );
	state.DefineClassConst ( klass, "GL_SRC_COLOR",					( u32 )ZGL_BLEND_FACTOR_SRC_COLOR );
	state.DefineClassConst ( klass, "GL_ONE_MINUS_DST_ALPHA",		( u32 )ZGL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA );
	state.DefineClassConst ( klass, "GL_ONE_MINUS_DST_COLOR",		( u32 )ZGL_BLEND_FACTOR_ONE_MINUS_DST_COLOR );
	state.DefineClassConst ( klass, "GL_ONE_MINUS_SRC_ALPHA",		( u32 )ZGL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA );
	state.DefineClassConst ( klass, "GL_ONE_MINUS_SRC_COLOR",		( u32 )ZGL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR );
	state.DefineClassConst ( klass, "GL_SRC_ALPHA",					( u32 )ZGL_BLEND_FACTOR_SRC_ALPHA );
	state.DefineClassConst ( klass, "GL_SRC_ALPHA_SATURATE",		( u32 )ZGL_BLEND_FACTOR_SRC_ALPHA_SATURATE );
	
	state.DefineClassConst ( klass, "DEPTH_TEST_DISABLE",			( u32 )0 );
	state.DefineClassConst ( klass, "DEPTH_TEST_NEVER",				( u32 )ZGL_DEPTH_NEVER );
	state.DefineClassConst ( klass, "DEPTH_TEST_LESS",				( u32 )ZGL_DEPTH_LESS );
	state.DefineClassConst ( klass, "DEPTH_TEST_EQUAL",				( u32 )ZGL_DEPTH_EQUAL );
	state.DefineClassConst ( klass, "DEPTH_TEST_LESS_EQUAL",		( u32 )ZGL_DEPTH_LEQUAL );
	state.DefineClassConst ( klass, "DEPTH_TEST_GREATER",			( u32 )ZGL_DEPTH_GREATER );
	state.DefineClassConst ( klass, "DEPTH_TEST_NOTEQUAL",			( u32 )ZGL_DEPTH_NOTEQUAL );
	state.DefineClassConst ( klass, "DEPTH_TEST_GREATER_EQUAL",		( u32 )ZGL_DEPTH_GEQUAL );
	state.DefineClassConst ( klass, "DEPTH_TEST_ALWAYS",			( u32 )ZGL_DEPTH_ALWAYS );
	
	state.DefineClassConst ( klass, "CULL_NONE",					( u32 )0 );
	state.DefineClassConst ( klass, "CULL_ALL",						( u32 )ZGL_CULL_ALL );
	state.DefineClassConst ( klass, "CULL_BACK",					( u32 )ZGL_CULL_BACK );
	state.DefineClassConst ( klass, "CULL_FRONT",					( u32 )ZGL_CULL_FRONT );
	
	state.DefineClassConst ( klass, "BILLBOARD_NONE",				( u32 )BILLBOARD_NONE );
	state.DefineClassConst ( klass, "BILLBOARD_NORMAL",				( u32 )BILLBOARD_NORMAL );
	state.DefineClassConst ( klass, "BILLBOARD_ORTHO",				( u32 )BILLBOARD_ORTHO );
	state.DefineClassConst ( klass, "BILLBOARD_COMPASS",			( u32 )BILLBOARD_COMPASS );
	state.DefineClassConst ( klass, "BILLBOARD_COMPASS_SCALE",		( u32 )BILLBOARD_COMPASS_SCALE );
	state.DefineClassConst ( klass, "BILLBOARD_SCREEN",				( u32 )BILLBOARD_SCREEN );
}

//----------------------------------------------------------------//
void MOAIGraphicsPropBase::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIPartitionHull::RegisterRubyFuncs ( state, klass );
	MOAIColor::RegisterRubyFuncs ( state, klass );
	MOAIMaterialBatchHolder::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "draw", _draw, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "getBillboard", _getBillboard, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getScissorRect", _getScissorRect, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isVisible", _isVisible, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setBillboard", _setBillboard, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setParent", _setParent, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setScissorRect", _setScissorRect, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setUVTransform", _setUVTransform, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setVisible", _setVisible, MRB_ARGS_ARG ( 0, 1 ) );

}

//----------------------------------------------------------------//
void MOAIGraphicsPropBase::Render () {

	this->Draw ( MOAIPartitionHull::NO_SUBPRIM_ID );
}

//----------------------------------------------------------------//
void MOAIGraphicsPropBase::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	
	MOAIColor::SerializeIn ( state, serializer );
	MOAIPartitionHull::SerializeIn ( state, serializer );
	MOAIMaterialBatchHolder::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIGraphicsPropBase::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	
	MOAIColor::SerializeOut ( state, serializer );
	MOAIPartitionHull::SerializeOut ( state, serializer );
	MOAIMaterialBatchHolder::SerializeOut ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIGraphicsPropBase::SetVisible ( bool visible ) {

	this->mDisplayFlags = visible ? this->mDisplayFlags | FLAGS_LOCAL_VISIBLE : this->mDisplayFlags & ~FLAGS_LOCAL_VISIBLE;
	this->ScheduleUpdate ();
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAIGraphicsPropBase::MOAIDrawable_DrawDebug ( int subPrimID ) {
	UNUSED ( subPrimID );

	if ( this->GetWorldBoundsStatus () == ZLBounds::ZL_BOUNDS_EMPTY ) return;

	MOAIDebugLinesMgr& debugLines = MOAIDebugLinesMgr::Get ();
	if ( !( debugLines.IsVisible () && debugLines.SelectStyleSet < MOAIGraphicsPropBase >())) return;
	
	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	MOAIDraw& draw = MOAIDraw::Get ();
	UNUSED ( draw ); // mystery warning in vs2008
	
	draw.Bind ();
	
	this->LoadVertexTransform ();
	
	gfxState.SetVertexTransform ( MOAIGfxState::MODEL_TO_DISPLAY_MTX );
	
	ZLBounds modelBounds = this->GetModelBounds ();
	
	// TODO: check bounds status
	
	if ( debugLines.Bind ( DEBUG_DRAW_AXIS )) {
		draw.DrawBoxAxis ( modelBounds );
	}
	
	if ( debugLines.Bind ( DEBUG_DRAW_DIAGONALS )) {
		draw.DrawBoxDiagonals ( modelBounds );
	}
	
	if ( debugLines.Bind ( DEBUG_DRAW_MODEL_BOUNDS )) {
		draw.DrawBoxOutline ( modelBounds );
	}
	
	// clear out the world transform (draw in world space)
	gfxState.SetVertexTransform ( MOAIGfxState::WORLD_TO_DISPLAY_MTX );
	
	if ( debugLines.Bind ( DEBUG_DRAW_WORLD_BOUNDS )) {
		draw.DrawBoxOutline ( this->GetWorldBounds ());
	}
	
	if ( debugLines.IsVisible ( DEBUG_DRAW_PARTITION_CELLS ) || debugLines.IsVisible ( DEBUG_DRAW_PARTITION_CELLS )) {
		
		ZLRect cellRect;
		ZLRect paddedRect;
		
		if ( this->GetCellRect ( &cellRect, &paddedRect )) {
			
			if ( cellRect.Area () != 0.0f ) {
				if ( debugLines.Bind ( DEBUG_DRAW_PARTITION_CELLS )) {
					draw.DrawRectOutline ( cellRect );
				}
			}
			
			if ( paddedRect.Area () != 0.0f ) {
				if ( debugLines.Bind ( DEBUG_DRAW_PARTITION_PADDED_CELLS )) {
					draw.DrawRectOutline ( paddedRect );
				}
			}
		}
	}
}

//----------------------------------------------------------------//
ZLMatrix4x4 MOAIGraphicsPropBase::MOAIGraphicsPropBase_GetWorldDrawingMtx () {

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	ZLMatrix4x4 worldDrawingMtx;

	switch ( this->mBillboard ) {
	
		case BILLBOARD_NORMAL: {
			
			ZLAffine3D billboardMtx ( gfxState.GetMtx ( MOAIGfxState::VIEW_TO_WORLD_MTX )); // inv view mtx sans translation
			
			billboardMtx.m [ ZLAffine3D::C3_R0 ] = 0.0f;
			billboardMtx.m [ ZLAffine3D::C3_R1 ] = 0.0f;
			billboardMtx.m [ ZLAffine3D::C3_R2 ] = 0.0f;
			
			worldDrawingMtx = ZLMatrix4x4 ( this->GetBillboardMtx ( billboardMtx ));
			break;
		}
		
		case BILLBOARD_ORTHO: {
			
			const ZLMatrix4x4& proj				= gfxState.GetMtx ( MOAIGfxState::VIEW_TO_CLIP_MTX );
			const ZLMatrix4x4& invViewProj		= gfxState.GetMtx ( MOAIGfxState::CLIP_TO_WORLD_MTX );
			const ZLMatrix4x4& invWindowMtx		= gfxState.GetMtx ( MOAIGfxState::WINDOW_TO_CLIP_MTX );
			
			ZLMatrix4x4 view					= gfxState.GetMtx ( MOAIGfxState::WORLD_TO_VIEW_MTX );
			ZLMatrix4x4 windowMtx				= gfxState.GetMtx ( MOAIGfxState::CLIP_TO_WINDOW_MTX );
			
			worldDrawingMtx = ZLMatrix4x4 ( this->GetLocalToWorldMtx ());
			
			// world space location for prop
			ZLVec3D worldLoc;
			worldLoc.mX = worldDrawingMtx.m [ ZLMatrix4x4::C3_R0 ];
			worldLoc.mY = worldDrawingMtx.m [ ZLMatrix4x4::C3_R1 ];
			worldLoc.mZ = worldDrawingMtx.m [ ZLMatrix4x4::C3_R2 ];
			
			// view and project the location
			view.Transform ( worldLoc );	// perspective view
			proj.Project ( worldLoc );		// perspective projection
			
			// now we're going to "unproject" the point back, going from window space to *ortho* space
			// this is as though we had projected the point using a 2D window matrix instead of the camera matrix
			
			invWindowMtx.Transform ( worldLoc );

			// prop will start at the origin; will be moved by camera after rotation
			worldDrawingMtx.m [ ZLMatrix4x4::C3_R0 ] = 0.0f;
			worldDrawingMtx.m [ ZLMatrix4x4::C3_R1 ] = 0.0f;
			worldDrawingMtx.m [ ZLMatrix4x4::C3_R2 ] = 0.0f;

			// keep camera rotation, but replace translation with "projected" location of prop
			view.m [ ZLMatrix4x4::C3_R0 ] = worldLoc.mX;
			view.m [ ZLMatrix4x4::C3_R1 ] = worldLoc.mY;
			view.m [ ZLMatrix4x4::C3_R2 ] = 0.0f;
			
			worldDrawingMtx.Append ( view );
			
			// apply the 2D "window" projection to get our final transform
			windowMtx.m [ ZLMatrix4x4::C2_R2 ] = 0.0f;
			worldDrawingMtx.Append ( windowMtx );
			
			// now roll everything back given the current view/proj
			// this will be applied once more by the pipeline, thus cancelling itself out and leaving us with the previous transform
			worldDrawingMtx.Append ( invViewProj );
			
			break;
		}
		
		case BILLBOARD_COMPASS: {
		
			
		
			ZLAffine3D cameraMtx ( gfxState.GetMtx ( MOAIGfxState::VIEW_TO_WORLD_MTX ));
			ZLVec3D	cameraY = cameraMtx.GetYAxis ();
			
			cameraY.mZ = 0.0f;
			cameraY.Norm ();
			
			ZLVec2D mapY ( cameraY.mX, cameraY.mY );
			ZLVec2D worldY ( 0.0f, 1.0f );
			
			float radians = -mapY.Radians ( worldY );
			
			if ( cameraY.mX < 0.0f ) {
				radians = -radians;
			}
		
			ZLMatrix4x4 billboardMtx;
			billboardMtx.Translate ( -this->mPiv.mX, -this->mPiv.mY, -this->mPiv.mZ );
			
			ZLMatrix4x4 mtx;
			mtx.RotateZ ( radians );
			billboardMtx.Append ( mtx );
			
			mtx.Translate ( this->mPiv.mX, this->mPiv.mY, this->mPiv.mZ );
			billboardMtx.Append ( mtx );
			
			// now that we've calculated the compass billboard, here's the tricky part.
			// we need to sandwich it into the local-to-world matrix, after the scale and rotate,
			// but before the translation. it's not sufficient just to prepend as we'd wind up
			// with compass-scale-rotate-translate. in that scenario, if scale != 1, then it will
			// mess up the compass transform. so we need to do scale-rotate-compass-translate instead.
			
			worldDrawingMtx = ZLMatrix4x4 ( this->GetLocalToWorldMtx ());
			
			ZLVec4D localToWorldTranslation = worldDrawingMtx.GetColumn ( 3 ); // extract the translation
			worldDrawingMtx.SetColumn ( 3, ZLVec4D::ORIGIN ); // set translation back to origin
			worldDrawingMtx.Append ( billboardMtx ); // this gives us scale-rotate-billboard
			worldDrawingMtx.SetColumn ( 3, localToWorldTranslation ); // scale-rotate-billboard-translation
			
			break;
		}

		case BILLBOARD_COMPASS_SCALE: {
		
			ZLMatrix4x4 viewToClip = gfxState.GetMtx ( MOAIGfxState::VIEW_TO_CLIP_MTX );
			ZLMatrix4x4 worldToClip = gfxState.GetMtx ( MOAIGfxState::WORLD_TO_CLIP_MTX );
			ZLMatrix4x4 clipToWindow = gfxState.GetMtx ( MOAIGfxState::CLIP_TO_WINDOW_MTX );
			// TODO: The cache for VIEW_TO_WORLD matrix is not working, so manually inverting here, which not efficient
			ZLMatrix4x4 worldToView = gfxState.GetMtx ( MOAIGfxState::WORLD_TO_VIEW_MTX );
			ZLMatrix4x4 worldToViewInv;
			
			worldDrawingMtx = ZLMatrix4x4 ( this->GetLocalToWorldMtx ());
			
			bool worldToViewInvSuccess = worldToViewInv.Inverse(worldToView);
			
			if (!worldToViewInvSuccess) {
				break;
			}
			
			ZLAffine3D cameraMtx ( worldToViewInv );
			
			ZLVec3D	cameraY = cameraMtx.GetYAxis ();
			
			cameraY.mZ = 0.0f;
			cameraY.Norm ();
			
			ZLVec2D mapY ( cameraY.mX, cameraY.mY );
			ZLVec2D worldY ( 0.0f, 1.0f );
			
			float flipped = copysign(1.0f, worldDrawingMtx.GetYAxis().Dot(ZLVec3D(0.0f, 1.0f, 0.0f)));
			float radians = mapY.Radians ( worldY ) * flipped;
			
			if ( cameraY.mX < 0.0f ) {
				radians = -radians;
			}
			
			// Determine scale to match camera
			ZLVec3D loc;
			worldDrawingMtx.GetTranslation ( loc );
			ZLVec4D loc4 (loc.mX, loc.mY, loc.mZ, 1);
			worldToClip.Transform(loc4);
			float sx = loc4.mW * clipToWindow.GetXAxis().Length() / viewToClip.GetXAxis().Length();
			float sy = loc4.mW * clipToWindow.GetYAxis().Length() / viewToClip.GetYAxis().Length();
			
			ZLMatrix4x4 billboardMtx;
			billboardMtx.Translate ( -this->mPiv.mX, -this->mPiv.mY, -this->mPiv.mZ );
			
			ZLMatrix4x4 mtx;
			mtx.RotateZ ( -radians );
			billboardMtx.Append ( mtx );
			
			mtx.Scale ( sx, sy, 1 );
			billboardMtx.Append ( mtx );
			
			mtx.Translate ( this->mPiv.mX, this->mPiv.mY, this->mPiv.mZ );
			billboardMtx.Append ( mtx );
			
			worldDrawingMtx.Prepend ( billboardMtx );
			
			break;
		}
		
		case BILLBOARD_SCREEN: {
			
			//MOAIGfxMgr::Get ().GetWorldToWndMtx ();
			
			//ZLMatrix4x4 viewProjMtx = camera->GetWorldToWndMtx ( *viewport );
			ZLMatrix4x4 viewProjMtx = gfxState.GetMtx ( MOAIGfxState::WORLD_TO_CLIP_MTX );
			viewProjMtx.Append ( gfxState.GetMtx ( MOAIGfxState::CLIP_TO_WINDOW_MTX ));
			
			ZLMatrix4x4 localToWorldMtx ( this->GetLocalToWorldMtx ());
			
			// TODO: check that pivot is supported correctly
			ZLVec3D loc;
			worldDrawingMtx.GetTranslation ( loc );
			viewProjMtx.Project ( loc );
			
			worldDrawingMtx.m [ ZLMatrix4x4::C3_R0 ] = loc.mX;
			worldDrawingMtx.m [ ZLMatrix4x4::C3_R1 ] = loc.mY;
			worldDrawingMtx.m [ ZLMatrix4x4::C3_R2 ] = loc.mZ;
			
			viewProjMtx.Inverse ();
			worldDrawingMtx.Append ( viewProjMtx );
			
			break;
		}
		
		case BILLBOARD_NONE:
		default:
		
			worldDrawingMtx = ZLMatrix4x4 ( this->GetLocalToWorldMtx ());
	}
	
	return worldDrawingMtx;
}

//----------------------------------------------------------------//
bool MOAIGraphicsPropBase::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	if ( MOAIGraphicsPropBaseAttr::Check ( attrID )) {
		
		switch ( UNPACK_ATTR ( attrID )) {
				
			case ATTR_SCISSOR_RECT:
				this->mScissorRect.Set ( *this, attr.ApplyVariantNoAdd < MOAIScissorRect* >( this->mScissorRect, op, MOAIAttribute::ATTR_READ_WRITE ));
				return true;
				
			case ATTR_LOCAL_VISIBLE:
				this->SetVisible ( ZLFloat::ToBoolean ( attr.ApplyNoAdd ( ZLFloat::FromBoolean (( this->mDisplayFlags & FLAGS_LOCAL_VISIBLE ) != 0 ), op, MOAIAttribute::ATTR_READ_WRITE )));
				return true;
				
			case ATTR_VISIBLE:
				attr.ApplyNoAdd ( ZLFloat::FromBoolean ( this->IsVisible ()), op , MOAIAttribute::ATTR_READ );
				return true;
			
			//case FRAME_TRAIT:
			//	attr.Apply < ZLBox >( &this->mFrame, op, MOAIAttribute::ATTR_READ );
			//	return true;
		}
	}
	
	if ( MOAIColor::MOAINode_ApplyAttrOp ( attrID, attr, op )) return true;
	if ( MOAIPartitionHull::MOAINode_ApplyAttrOp ( attrID, attr, op )) return true;
	return false;
}

//----------------------------------------------------------------//
void MOAIGraphicsPropBase::MOAINode_Update () {
	
	MOAIColor::MOAINode_Update ();
	MOAIPartitionHull::MOAINode_Update ();
	
	bool visible = ZLFloat::ToBoolean ( this->GetLinkedValue ( MOAIGraphicsPropBaseAttr::Pack ( INHERIT_VISIBLE ), 1.0f ));
	this->mDisplayFlags = visible && ( this->mDisplayFlags & FLAGS_LOCAL_VISIBLE ) ? this->mDisplayFlags | FLAGS_VISIBLE : this->mDisplayFlags & ~FLAGS_VISIBLE ;
}

//----------------------------------------------------------------//
void MOAIGraphicsPropBase::MOAIPartitionHull_AddToSortBuffer ( MOAIPartitionResultBuffer& buffer, u32 key ) {

	buffer.PushResult ( *this, key, NO_SUBPRIM_ID, this->GetPriority (), this->GetWorldLoc (), this->GetWorldBounds ());
}

//----------------------------------------------------------------//
u32 MOAIGraphicsPropBase::MOAIPartitionHull_AffirmInterfaceMask ( MOAIPartition& partition ) {

	return partition.AffirmInterfaceMask < MOAIDrawable >();
}
