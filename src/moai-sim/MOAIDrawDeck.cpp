// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIDrawDeck.h>
#include <moai-sim/MOAIDraw.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAIMaterialMgr.h>
#include <moai-sim/MOAIShader.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAISurfaceSampler2D.h>
#include <moai-sim/MOAITransform.h>
#include <moai-sim/MOAIVertexFormatMgr.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDrawDeck::_setBounds ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDrawDeck, "U" )

	int top = state.GetParamsCount ();

	switch ( top ) {
	
		case 5:
			self->mBounds.Init ( state.GetRect < float >( 1 ));
			break;
	
		case 7:
			self->mBounds.Init ( state.GetBox ( 1 ));
			break;
			
		default:
			self->mBounds = ZLBounds::GLOBAL;
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setBoundsCallback
	@text	Sets the callback to be issued when the size of a deck item
			needs to be determined.
			The callback's parameters are ( number index ).

	@in		MOAIDrawDeck self
	@in		function callback
	@out	nil
*/
mrb_value MOAIDrawDeck::_setBoundsCallback ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDrawDeck, "U" )
	
	self->mOnBounds.SetRef ( state.GetParamValue ( 1 ) );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setDrawCallback
	@text	Sets the callback to be issued when draw events occur.
			The callback's parameters are ( number index, number xOff, number yOff, number xScale, number yScale ).

	@in		MOAIDrawDeck self
	@in		function callback
	@out	nil
*/
mrb_value MOAIDrawDeck::_setDrawCallback ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDrawDeck, "U" )
	
	self->mOnDraw.SetRef ( state.GetParamValue ( 1 ) );
	return context;
}

//================================================================//
// MOAIDrawDeck
//================================================================//

//----------------------------------------------------------------//
MOAIDrawDeck::MOAIDrawDeck () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIStretchDeck )
	RTTI_END
	
	this->mStretchFactor = 0.0;
	this->mBounds = ZLBounds::GLOBAL;
}

//----------------------------------------------------------------//
MOAIDrawDeck::~MOAIDrawDeck () {
}

//----------------------------------------------------------------//
void MOAIDrawDeck::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIStretchDeck::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIDrawDeck::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIStretchDeck::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "setBounds", _setBounds, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "setBoundsCallback", _setBoundsCallback, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setDrawCallback", _setDrawCallback, MRB_ARGS_REQ ( 1 ) );

}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
ZLBounds MOAIDrawDeck::MOAIDeck_ComputeMaxBounds () {

	return this->mBounds;
}

//----------------------------------------------------------------//
void MOAIDrawDeck::MOAIDeck_Draw ( u32 idx ) {
	
	if ( this->mOnDraw ) {
	
		MOAIDraw::Get ().Bind ();
	
		MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
		ZLVec3D stretch = this->BindStretchVertexTransform ();
	
		MOAIRubyState state = MOAIRubyRuntime::Get ().State ();

		mrb_value ret [ 4 ];
		ret [ 0 ] = state.ToRValue ( idx + 1 );
		ret [ 1 ] = state.ToRValue ( stretch.mX );
		ret [ 2 ] = state.ToRValue ( stretch.mY );
		ret [ 3 ] = state.ToRValue ( stretch.mZ );

		state.FuncCall ( this->mOnDraw, "call", 4, ret );
	}
}

//----------------------------------------------------------------//
ZLBounds MOAIDrawDeck::MOAIDeck_GetBounds ( u32 idx ) {
	
	if ( this->mOnBounds ) {
	
		MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
		
		mrb_value ret [ 1 ];
		ret [ 0 ] = state.ToRValue ( idx + 1 );

		state.FuncCall ( this->mOnBounds, "call", 1, ret );
		
		// ZLBounds bounds;
		// bounds.Init ( state.GetBox ( -6 ));
		// return bounds;
	}
	return this->mBounds;
}

//----------------------------------------------------------------//
MOAICollisionShape* MOAIDrawDeck::MOAIDeck_GetCollisionShape ( u32 idx ) {
	UNUSED ( idx );

	return 0;
}

//----------------------------------------------------------------//
bool MOAIDrawDeck::MOAIDeck_Overlap ( u32 idx, const ZLVec2D& vec, u32 granularity, ZLBounds* result ) {
	UNUSED ( idx );
	UNUSED ( vec );
	UNUSED ( granularity );
	UNUSED ( result );

	return false;
}

//----------------------------------------------------------------//
bool MOAIDrawDeck::MOAIDeck_Overlap ( u32 idx, const ZLVec3D& vec, u32 granularity, ZLBounds* result ) {
	UNUSED ( idx );
	UNUSED ( vec );
	UNUSED ( granularity );
	UNUSED ( result );

	return false;
}
