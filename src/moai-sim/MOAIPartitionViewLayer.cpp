// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIDebugLines.h>
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIFrameBufferTexture.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIPartitionViewLayer.h>
#include <moai-sim/MOAIMaterialMgr.h>
#include <moai-sim/MOAIPartition.h>
#include <moai-sim/MOAIPartitionResultBuffer.h>
#include <moai-sim/MOAIPartitionResultMgr.h>
#include <moai-sim/MOAIRenderMgr.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAITextureBase.h>
#include <moai-sim/MOAITransform.h>
#include <moai-sim/MOAIVertexFormatMgr.h>
#include <moai-sim/MOAIViewProj.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getPropViewList
	@text	Return a list of props gathered and sorted by layer.
	
	@in		MOAIPartitionViewLayer self
	@opt	number sortMode					Default is layer's current value.
	@opt	boolean sortInViewSpace			Default is layer's current value.
	@opt	number xSortScale				Default is layer's current value.
	@opt	number ySortScale				Default is layer's current value.
	@opt	number zSortScale				Default is layer's current value.
	@opt	number pSortScale				Priority sort scale. Default is layer's current value.
	@out	...								Gathered props.
*/
mrb_value	MOAIPartitionViewLayer::_getPropViewList ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionViewLayer, "U" )
	
	MOAIPartition* partition = self->MOAIPartitionHolder::mPartition;
	
	if ( partition && self->mViewport ) {
		
		u32 interfaceMask = partition->GetInterfaceMask < MOAIDrawable >();
		if ( !interfaceMask ) return mrb_nil_value ();
		
		float sortScale [ 4 ];
		
		u32 sortMode			= state.GetParamValue < u32 >( 1, self->mSortMode );
		bool sortInViewSpace	= state.GetParamValue < bool >( 2, self->mSortInViewSpace );
		
		sortScale [ 0 ]			= state.GetParamValue < float >( 3, self->mSortScale [ 0 ]);
		sortScale [ 1 ]			= state.GetParamValue < float >( 4, self->mSortScale [ 1 ]);
		sortScale [ 2 ]			= state.GetParamValue < float >( 5, self->mSortScale [ 2 ]);
		sortScale [ 3 ]			= state.GetParamValue < float >( 6, self->mSortScale [ 3 ]);
		
		ZLMatrix4x4 viewMtx = MOAIViewProj::GetViewMtx ( self->mCamera, self->mParallax );
		ZLMatrix4x4 invViewProjMtx = viewMtx;
		invViewProjMtx.Append ( MOAIViewProj::GetProjectionMtx ( self->mViewport, self->mCamera ));
		invViewProjMtx.Inverse ();
	
		ZLFrustum viewVolume;
		viewVolume.Init ( invViewProjMtx );
		
		MOAIScopedPartitionResultBufferHandle scopedBufferHandle = MOAIPartitionResultMgr::Get ().GetBufferHandle ();
		MOAIPartitionResultBuffer& buffer = scopedBufferHandle;
		
		u32 totalResults = 0;
		
		if ( self->mPartitionCull2D ) {
			totalResults = partition->GatherHulls ( buffer, 0, viewVolume.mAABB, interfaceMask );
		}
		else {
			totalResults = partition->GatherHulls ( buffer, 0, viewVolume, interfaceMask );
		}
		
		if ( !totalResults ) return mrb_nil_value ();
		
		if ( sortInViewSpace ) {
			buffer.Transform ( viewMtx, false );
		}
		
		buffer.GenerateKeys (
			sortMode,
			sortScale [ 0 ],
			sortScale [ 1 ],
			sortScale [ 2 ],
			sortScale [ 3 ]
		);
		
		buffer.Sort ( self->mSortMode );
	
		return buffer.PushHulls ( M );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getSortMode
	@text	Get the sort mode for rendering.
	
	@in		MOAIPartitionViewLayer self
	@out	number sortMode
*/
mrb_value MOAIPartitionViewLayer::_getSortMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionViewLayer, "U" )
	
	return state.ToRValue ( self->mSortMode );
}

//----------------------------------------------------------------//
/**	@lua	getSortScale
	@text	Return the scalar applied to axis sorts.
	
	@in		MOAIPartitionViewLayer self
	@out	number x
	@out	number y
	@out	number priority
*/
mrb_value	MOAIPartitionViewLayer::_getSortScale ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionViewLayer, "U" )

	mrb_value ret [ 3 ];
	ret [ 0 ] = state.ToRValue ( self->mSortScale [ 0 ] );
	ret [ 1 ] = state.ToRValue ( self->mSortScale [ 1 ] );
	ret [ 2 ] = state.ToRValue ( self->mSortScale [ 3 ] );

	return mrb_ary_new_from_values ( state, 3, ret );
}

//----------------------------------------------------------------//
/**	@lua	setPartitionCull2D
	@text	Enables 2D partition cull (projection of frustum AABB will
			be used instead of AABB or frustum).
	
	@in		MOAIPartitionViewLayer self
	@in		boolean partitionCull2D		Default value is false.
	@out	nil
*/
mrb_value	MOAIPartitionViewLayer::_setPartitionCull2D ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionViewLayer, "U" )

	self->mPartitionCull2D = state.GetParamValue < bool >( 1, false );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setSortMode
	@text	Set the sort mode for rendering.
	
	@in		MOAIPartitionViewLayer self
	@in		number sortMode				One of MOAIPartitionViewLayer.SORT_NONE, MOAIPartitionViewLayer.SORT_PRIORITY_ASCENDING,
										MOAIPartitionViewLayer.SORT_PRIORITY_DESCENDING, MOAIPartitionViewLayer.SORT_X_ASCENDING,
										MOAIPartitionViewLayer.SORT_X_DESCENDING, MOAIPartitionViewLayer.SORT_Y_ASCENDING,
										MOAIPartitionViewLayer.SORT_Y_DESCENDING, MOAIPartitionViewLayer.SORT_Z_ASCENDING,
										MOAIPartitionViewLayer.SORT_Z_DESCENDING
	@in		boolean sortInViewSpace		Default value is 'false'.
	@out	nil
*/
mrb_value MOAIPartitionViewLayer::_setSortMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionViewLayer, "U" )
	
	self->mSortMode			= state.GetParamValue < u32 >( 1, MOAIPartitionResultBuffer::SORT_PRIORITY_ASCENDING );
	self->mSortInViewSpace	= state.GetParamValue < bool >( 2, false );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setSortScale
	@text	Set the scalar applied to axis sorts.
	
	@in		MOAIPartitionViewLayer self
	@opt	number x			Default value is 0.
	@opt	number y			Default value is 0.
	@opt	number z			Default value is 0.
	@opt	number priority		Default value is 1.
	@out	nil
*/
mrb_value	MOAIPartitionViewLayer::_setSortScale ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionViewLayer, "U" )

	self->mSortScale [ 0 ] = state.GetParamValue < float >( 1, 0.0f );
	self->mSortScale [ 1 ] = state.GetParamValue < float >( 2, 0.0f );
	self->mSortScale [ 2 ] = state.GetParamValue < float >( 3, 0.0f );
	self->mSortScale [ 3 ] = state.GetParamValue < float >( 4, 1.0f );

	return context;
}

//================================================================//
// MOAIPartitionViewLayer
//================================================================//

//----------------------------------------------------------------//
void MOAIPartitionViewLayer::DrawPartition ( MOAIPartition& partition ) {

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	u32 interfaceMask = partition.GetInterfaceMask < MOAIDrawable >();
	if ( !interfaceMask ) return;
	
	MOAIScopedPartitionResultBufferHandle scopedBufferHandle = MOAIPartitionResultMgr::Get ().GetBufferHandle ();
	MOAIPartitionResultBuffer& buffer = scopedBufferHandle;
	
	const ZLFrustum& viewVolume = gfxState.GetViewVolume ();
	
	u32 totalResults = 0;
		
	if ( this->mPartitionCull2D ) {
		totalResults = partition.GatherHulls ( buffer, 0, viewVolume.mAABB, interfaceMask );
	}
	else {
		totalResults = partition.GatherHulls ( buffer, 0, viewVolume, interfaceMask );
	}
	
	if ( !totalResults ) return;
	
	if ( this->mSortInViewSpace ) {
		buffer.Transform ( gfxState.GetMtx ( MOAIGfxState::WORLD_TO_VIEW_MTX ), false );
	}
	
	buffer.GenerateKeys (
		this->mSortMode,
		this->mSortScale [ 0 ],
		this->mSortScale [ 1 ],
		this->mSortScale [ 2 ],
		this->mSortScale [ 3 ]
	);
	
	buffer.Sort ( this->mSortMode );
	
	MOAIMaterialMgr& materialStack = MOAIMaterialMgr::Get ();
	materialStack.Push ( this->GetMaterial ());
	
	this->DrawProps ( buffer );
	
	materialStack.Pop ();
	
	if ( MOAIDebugLinesMgr::Get ().IsVisible () && this->mShowDebugLines ) {
		
		partition.DrawDebugBack ();
		this->DrawPropsDebug ( buffer );
		partition.DrawDebugFront ();
	}
}

//----------------------------------------------------------------//
void MOAIPartitionViewLayer::DrawProps ( MOAIPartitionResultBuffer& buffer ) {

	u32 totalResults = buffer.GetTotalResults ();

	for ( u32 i = 0; i < totalResults; ++i ) {
		MOAIPartitionResult* result = buffer.GetResultUnsafe ( i );
		MOAIDrawable* drawable = result->AsType < MOAIDrawable >();
		drawable->Draw ( result->mSubPrimID );
	}
}

//----------------------------------------------------------------//
void MOAIPartitionViewLayer::DrawPropsDebug ( MOAIPartitionResultBuffer& buffer ) {

	u32 totalResults = buffer.GetTotalResults ();

	for ( u32 i = 0; i < totalResults; ++i ) {
		MOAIPartitionResult* result = buffer.GetResultUnsafe ( i );
		MOAIDrawable* drawable = result->AsType < MOAIDrawable >();
		drawable->DrawDebug ( result->mSubPrimID );
	}
}

//----------------------------------------------------------------//
MOAIPartitionViewLayer::MOAIPartitionViewLayer () :
	mSortMode ( MOAIPartitionResultBuffer::SORT_PRIORITY_ASCENDING ),
	mSortInViewSpace ( false ),
	mPartitionCull2D ( true ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIPartitionHolder )
		RTTI_EXTEND ( MOAIViewLayer )
	RTTI_END
	
	this->mSortScale [ 0 ] = 0.0f;
	this->mSortScale [ 1 ] = 0.0f;
	this->mSortScale [ 2 ] = 0.0f;
	this->mSortScale [ 3 ] = 1.0f;
}

//----------------------------------------------------------------//
MOAIPartitionViewLayer::~MOAIPartitionViewLayer () {
}

//----------------------------------------------------------------//
void MOAIPartitionViewLayer::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIPartitionHolder::RegisterRubyClass ( state, klass );
	MOAIViewLayer::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "SORT_NONE",						( u32 )MOAIPartitionResultBuffer::SORT_NONE );
	state.DefineClassConst ( klass, "SORT_ISO",							( u32 )MOAIPartitionResultBuffer::SORT_ISO );
	state.DefineClassConst ( klass, "SORT_PRIORITY_ASCENDING",			( u32 )MOAIPartitionResultBuffer::SORT_PRIORITY_ASCENDING );
	state.DefineClassConst ( klass, "SORT_PRIORITY_DESCENDING",			( u32 )MOAIPartitionResultBuffer::SORT_PRIORITY_DESCENDING );
	state.DefineClassConst ( klass, "SORT_X_ASCENDING",					( u32 )MOAIPartitionResultBuffer::SORT_X_ASCENDING );
	state.DefineClassConst ( klass, "SORT_X_DESCENDING",				( u32 )MOAIPartitionResultBuffer::SORT_X_DESCENDING );
	state.DefineClassConst ( klass, "SORT_Y_ASCENDING",					( u32 )MOAIPartitionResultBuffer::SORT_Y_ASCENDING );
	state.DefineClassConst ( klass, "SORT_Y_DESCENDING",				( u32 )MOAIPartitionResultBuffer::SORT_Y_DESCENDING );
	state.DefineClassConst ( klass, "SORT_Z_ASCENDING",					( u32 )MOAIPartitionResultBuffer::SORT_Z_ASCENDING );
	state.DefineClassConst ( klass, "SORT_Z_DESCENDING",				( u32 )MOAIPartitionResultBuffer::SORT_Z_DESCENDING );
	state.DefineClassConst ( klass, "SORT_VECTOR_ASCENDING",			( u32 )MOAIPartitionResultBuffer::SORT_VECTOR_ASCENDING );
	state.DefineClassConst ( klass, "SORT_VECTOR_DESCENDING",			( u32 )MOAIPartitionResultBuffer::SORT_VECTOR_DESCENDING );
	state.DefineClassConst ( klass, "SORT_DIST_SQUARED_ASCENDING",		( u32 )MOAIPartitionResultBuffer::SORT_DIST_SQUARED_ASCENDING );
	state.DefineClassConst ( klass, "SORT_DIST_SQUARED_DESCENDING",		( u32 )MOAIPartitionResultBuffer::SORT_DIST_SQUARED_DESCENDING );

}

//----------------------------------------------------------------//
void MOAIPartitionViewLayer::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIPartitionHolder::RegisterRubyFuncs ( state, klass );
	MOAIViewLayer::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getLayerPartition", MOAIPartitionHolder::_getPartition, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getPartition", MOAIViewLayer::_getPartition, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getPropViewList", _getPropViewList, MRB_ARGS_ARG ( 0, 6 ) );
	state.DefineInstanceMethod ( klass, "getSortMode", _getSortMode, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getSortScale", _getSortScale, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setLayerPartition", MOAIPartitionHolder::_setPartition, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setPartition", MOAIViewLayer::_setPartition, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setPartitionCull2D", _setPartitionCull2D, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setSortMode", _setSortMode, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setSortScale", _setSortScale, MRB_ARGS_ARG ( 0, 4 ) );
	
}

//----------------------------------------------------------------//
void MOAIPartitionViewLayer::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	MOAIGraphicsProp::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIPartitionViewLayer::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	MOAIGraphicsProp::SerializeOut ( state, serializer );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAIPartitionViewLayer::MOAIViewLayer_Draw () {
	
	if ( this->MOAIPartitionHolder::mPartition ) {
		this->DrawPartition ( *this->MOAIPartitionHolder::mPartition );
	}
}
