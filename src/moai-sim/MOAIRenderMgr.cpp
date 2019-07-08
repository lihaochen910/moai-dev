// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIDrawable.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGfxResourceClerk.h>
#include <moai-sim/MOAIRenderMgr.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIRenderMgr::_getRenderCount ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	MOAIRenderMgr& gfxMgr = MOAIRenderMgr::Get ();

	return state.ToRValue ( gfxMgr.mRenderCounter );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIRenderMgr::_getRender ( mrb_state* M, mrb_value context ) {
	UNUSED ( M );
	UNUSED ( context );

	return MOAIRenderMgr::Get ().mRenderRoot;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIRenderMgr::_setRender ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	MOAIRenderMgr::Get ().mRenderRoot.SetRef ( state.GetParamValue ( 1 ) );
	return mrb_nil_value ();
}

//================================================================//
// MOAIRenderMgr
//================================================================//

//----------------------------------------------------------------//
MOAIRenderMgr::MOAIRenderMgr () :
	mRenderCounter ( 0 ),
	mRenderDuration ( 1.0 / 60.0 ),
	mRenderTime ( 0.0 ) {
	
	RTTI_SINGLE ( MOAIRubyObject )
}

//----------------------------------------------------------------//
MOAIRenderMgr::~MOAIRenderMgr () {
}

//----------------------------------------------------------------//
void MOAIRenderMgr::PushDrawable ( MOAIRubyObject* drawable ) {

	MOAIRubyState state = MOAIRubyRuntime::Get ().State ();

	if ( this->mRenderRoot.Empty () ) {
		this->mRenderRoot.SetRef ( mrb_ary_new ( state ) );
	}

	mrb_ary_push ( state, this->mRenderRoot, state.ToRValue ( drawable ) );
}

//----------------------------------------------------------------//
void MOAIRenderMgr::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineStaticMethod ( klass, "getRenderCount", _getRenderCount, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getRender", _getRender, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "setRender", _setRender, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAIRenderMgr::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIRenderMgr::Render () {

	MOAIGfxMgr& gfxMgr = MOAIGfxMgr::Get ();

	gfxMgr.mPipelineMgr.ResetDrawingAPIs ();
	gfxMgr.mResourceMgr.Update ();

	MOAIFrameBuffer* frameBuffer = gfxMgr.mGfxState.GetDefaultFrameBuffer ();
	assert ( frameBuffer );
	frameBuffer->NeedsClear ( true );

	// Measure performance
	double startTime = ZLDeviceTime::GetTimeInSeconds ();
	
	ZLGfx* gfx = gfxMgr.mPipelineMgr.SelectDrawingAPI ( MOAIGfxPipelineClerk::DRAWING_PIPELINE );
	if ( !gfx ) return;

	ZGL_COMMENT ( *gfx, "RENDER MGR RENDER" );

	if ( this->mRenderRoot ) {
	
		MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
		
		MOAIDrawable::Draw ( state, this->mRenderRoot, false );
	}
	
	this->mRenderCounter++;
	
	// Measure performance
	double endTime = ZLDeviceTime::GetTimeInSeconds ();
	this->mRenderDuration = endTime - startTime;
	this->mRenderTime += this->mRenderDuration;
	
	 gfxMgr.mGfxState.FinishFrame ();
}
