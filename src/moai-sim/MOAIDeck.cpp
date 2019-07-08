// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAICollisionShape.h>
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGfxResource.h>
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAIImage.h>
#include <moai-sim/MOAIShader.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAISurfaceSampler2D.h>
#include <moai-sim/MOAITexture.h>
#include <moai-sim/MOAITextureBase.h>
#include <moai-sim/MOAITransform.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDeck::_draw ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDeck, "U" )

	u32 index						= state.GetParamValue < u32 >( 1, 1 );
	//MOAIMaterialBatch* materials	= state.GetRubyObject < MOAIMaterialBatch >( 2, false );
	//ZLVec3D offset					= state.GetParamValue < ZLVec3D >( 3, ZLVec3D::ORIGIN );
	//ZLVec3D scale					= state.GetParamValue < ZLVec3D >( 6, ZLVec3D::AXIS );

	self->Draw ( index );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	getBounds
	@text	Return bounds for an item or the maximum bounds for the
			deck.

	@overload

		@in		number idx
		@out	xMin
		@out	yMin
		@out	zMin
		@out	xMax
		@out	yMax
		@out	zMax
	
	@overload
		@out	xMin
		@out	yMin
		@out	zMin
		@out	xMax
		@out	yMax
		@out	zMax
*/
mrb_value MOAIDeck::_getBounds ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDeck, "U" )
	
	ZLBox box;
	
	if ( state.ParamIsType ( 1, MRB_TT_FIXNUM )) {
	
		u32 idx = state.GetParamValue < u32 >( 1, 1 ) - 1;
		box = self->GetBounds ( idx );
	}
	else {
	
		box = self->GetBounds ();
	}
	
	return state.Get ( box );
}

//================================================================//
// MOAIDeck
//================================================================//

//----------------------------------------------------------------//
void MOAIDeck::Draw ( u32 idx ) {

	this->MOAIDeck_Draw ( idx );
}

//----------------------------------------------------------------//
ZLBounds MOAIDeck::GetBounds () {

	return this->GetBounds ( 0 );
	/*
	if ( this->mBoundsDirty ) {
	
		this->mMaxBounds = this->MOAIDeck_ComputeMaxBounds ();
		
		// flip and expand to account for flip flags
		//ZLBox bounds = this->mMaxBounds;
		//bounds.Scale ( -1.0f );
		//bounds.Bless ();
		
		//this->mMaxBounds.Grow ( bounds );
		this->mBoundsDirty = false;
	}
	return this->mMaxBounds;*/
}

//----------------------------------------------------------------//
ZLBounds MOAIDeck::GetBounds ( u32 idx ) {

	return this->MOAIDeck_GetBounds ( idx );
}

//----------------------------------------------------------------//
MOAICollisionShape* MOAIDeck::GetCollisionShape ( u32 idx ) {

	return MOAIDeck_GetCollisionShape ( idx );
}

//----------------------------------------------------------------//
MOAIDeck::MOAIDeck () :
	mBoundsDirty ( true ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIDeck::~MOAIDeck () {
}

//----------------------------------------------------------------//
bool MOAIDeck::Overlap ( u32 idx, const ZLVec2D& vec, u32 granularity, ZLBounds* result ) {

	return this->MOAIDeck_Overlap ( idx, vec, granularity, result );
}


//----------------------------------------------------------------//
void MOAIDeck::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIDeck::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "draw", _draw, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "getBounds", _getBounds, MRB_ARGS_ARG ( 0, 1 ) );

}

//----------------------------------------------------------------//
void MOAIDeck::SetBoundsDirty () {

	this->mBoundsDirty = true;
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
ZLBounds MOAIDeck::MOAIDeck_ComputeMaxBounds () {

	return ZLBounds::EMPTY;
}

//----------------------------------------------------------------//
void MOAIDeck::MOAIDeck_Draw ( u32 idx ) {
	UNUSED ( idx );
}

//----------------------------------------------------------------//
ZLBounds MOAIDeck::MOAIDeck_GetBounds ( u32 idx ) {
	UNUSED ( idx );

	return ZLBounds::EMPTY;
}

//----------------------------------------------------------------//
MOAICollisionShape* MOAIDeck::MOAIDeck_GetCollisionShape ( u32 idx ) {
	UNUSED ( idx );

	return 0;
}

//----------------------------------------------------------------//
bool MOAIDeck::MOAIDeck_Overlap ( u32 idx, const ZLVec2D& vec, u32 granularity, ZLBounds* result ) {
	UNUSED ( result );
	UNUSED ( granularity );
	
	ZLBounds bounds = this->GetBounds ( idx );
	return (( bounds.mStatus == ZLBounds::ZL_BOUNDS_OK ) && bounds.Contains ( ZLVec3D ( vec.mY, vec.mY, 0.0f ), ZLBox::PLANE_XY ));
}

//----------------------------------------------------------------//
bool MOAIDeck::MOAIDeck_Overlap ( u32 idx, const ZLVec3D& vec, u32 granularity, ZLBounds* result ) {
	UNUSED ( result );
	UNUSED ( granularity );
	
	ZLBounds bounds = this->GetBounds ( idx );
	return (( bounds.mStatus == ZLBounds::ZL_BOUNDS_OK ) && bounds.Contains ( vec ));
}
