// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIColor.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGfxResourceClerk.h>
#include <moai-sim/MOAILayer.h>
#include <moai-sim/MOAIRenderMgr.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAILayer::_draw ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionViewLayer, "U" )

	self->Draw ( MOAIPartitionHull::NO_SUBPRIM_ID );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAILayer::_getClearMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAILayer, "U" )
	return state.ToRValue ( self->mClearMode );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAILayer::_getFrameBuffer ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAILayer, "U" )
	return state.ToRValue < MOAIRubyObject* >( self->GetFrameBuffer () );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAILayer::_pushRenderPass ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAILayer, "U" )
	MOAIRenderMgr::Get ().PushDrawable ( self );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setClearColor
	@text	At the start of each frame the device will by default automatically
			render a background color.  Using this function you can set the
			background color that is drawn each frame.  If you specify no arguments
			to this function, then automatic redraw of the background color will
			be turned off (i.e. the previous render will be used as the background).

	@overload

		@in		MOAILayer self
		@opt	number red			The red value of the color.
		@opt	number green		The green value of the color.
		@opt	number blue			The blue value of the color.
		@opt	number alpha		The alpha value of the color.
		@out	nil
	
	@overload
		
		@in		MOAILayer self
		@in		MOAIColor color
		@out	nil
*/
mrb_value MOAILayer::_setClearColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAILayer, "U" )
	
	MOAIColor* color = state.GetRubyObject < MOAIColor >( 1, true );
	if ( color ) {
		self->SetClearColor ( color );
		self->mClearFlags |= ZGL_CLEAR_COLOR_BUFFER_BIT;
		return mrb_nil_value ();
	}
	
	// don't clear the color
	self->mClearFlags &= ~ZGL_CLEAR_COLOR_BUFFER_BIT;
	self->SetClearColor ( 0 );

	if ( state.GetParamsCount () > 1 ) {
	
		float r = state.GetParamValue < float >( 1, 0.0f );
		float g = state.GetParamValue < float >( 2, 0.0f );
		float b = state.GetParamValue < float >( 3, 0.0f );
		float a = state.GetParamValue < float >( 4, 1.0f );
		
		self->mClearColor = ZLColor::PackRGBA ( r, g, b, a );
		self->mClearFlags |= ZGL_CLEAR_COLOR_BUFFER_BIT;
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setClearDepth
	@text	At the start of each frame the buffer will by default automatically
			clear the depth buffer.  This function sets whether or not the depth
			buffer should be cleared at the start of each frame.

	@in		MOAILayer self
	@in		boolean clearDepth	Whether to clear the depth buffer each frame.
	@out	nil
*/
mrb_value MOAILayer::_setClearDepth ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAILayer, "U" )
	
	bool clearDepth = state.GetParamValue < bool >( 1, false );
	
	if ( clearDepth ) {
		self->mClearFlags |= ZGL_CLEAR_DEPTH_BUFFER_BIT;
	}
	else {
		self->mClearFlags &= ~ZGL_CLEAR_DEPTH_BUFFER_BIT;
	}
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAILayer::_setClearMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAILayer, "U" )
	self->mClearMode = state.GetParamValue < u32 >( 1, CLEAR_ON_BUFFER_FLAG );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAILayer::_setFrameBuffer ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAILayer, "U" )
	
	MOAIFrameBuffer* frameBuffer = state.GetRubyObject < MOAIFrameBuffer >( 1, true );
	self->SetFrameBuffer ( frameBuffer );
	return context;
}

//================================================================//
// MOAILayer
//================================================================//

//----------------------------------------------------------------//
void MOAILayer::ClearSurface () {

	MOAIFrameBuffer* frameBuffer = this->GetFrameBuffer ();
	assert ( frameBuffer );

	if (( this->mClearMode == CLEAR_NEVER ) || (( this->mClearMode == CLEAR_ON_BUFFER_FLAG ) && ( !frameBuffer->NeedsClear ()))) return;

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	if ( this->mClearFlags & ZGL_CLEAR_COLOR_BUFFER_BIT ) {
	
		ZLColorVec clearColor;
		
		if ( this->mClearColorNode ) {
			clearColor = this->mClearColorNode->GetColorTrait ();
		}
		else {
			clearColor.SetRGBA ( this->mClearColor );
		}
		
		gfxState.SetClearColor ( clearColor );
	}
	
	gfxState.SetClearFlags ( this->mClearFlags );
	MOAIGfxMgr::Get ().mGfxState.ClearSurface ();
	
	frameBuffer->NeedsClear ( false );
}

//----------------------------------------------------------------//
MOAIFrameBuffer* MOAILayer::GetFrameBuffer () {

	return this->mFrameBuffer ? this->mFrameBuffer : MOAIGfxMgr::Get ().mGfxState.GetDefaultFrameBuffer ();
}

//----------------------------------------------------------------//
MOAILayer::MOAILayer () :
	mClearFlags ( ZGL_CLEAR_COLOR_BUFFER_BIT | ZGL_CLEAR_DEPTH_BUFFER_BIT ),
	mClearColor ( ZLColor::PackRGBA ( 0.0f, 0.0f, 0.0f, 1.0f )),
	mClearMode ( CLEAR_ON_BUFFER_FLAG ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
		RTTI_EXTEND ( MOAIDrawable )
	RTTI_END
}

//----------------------------------------------------------------//
MOAILayer::~MOAILayer () {

	this->mClearColorNode.Set ( *this, 0 );
	this->mFrameBuffer.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAILayer::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "CLEAR_ALWAYS",			( u32 )CLEAR_ALWAYS );
	state.DefineClassConst ( klass, "CLEAR_NEVER",			( u32 )CLEAR_NEVER );
	state.DefineClassConst ( klass, "CLEAR_ON_BUFFER_FLAG",	( u32 )CLEAR_ON_BUFFER_FLAG );
}

//----------------------------------------------------------------//
void MOAILayer::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "draw",				_draw, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getClearMode",		_getClearMode, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getFrameBuffer",	_getFrameBuffer, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "pushRenderPass",	_pushRenderPass, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setClearColor",	_setClearColor, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "setClearDepth",	_setClearDepth, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setClearMode",		_setClearMode, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setFrameBuffer",	_setFrameBuffer, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAILayer::SetClearColor ( MOAIColor* color ) {

	if ( this->mClearColorNode != color ) {
		this->mClearColorNode.Set ( *this, color );
	}
}

//----------------------------------------------------------------//
void MOAILayer::SetFrameBuffer ( MOAIFrameBuffer* frameBuffer ) {

	//this->mFrameBuffer.Set ( *this, frameBuffer ? frameBuffer : MOAIGfxMgr::Get ().mGfxState.GetDefaultFrameBuffer ());
	this->mFrameBuffer.Set ( *this, frameBuffer );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAILayer::MOAIDrawable_Draw ( int subPrimID ) {
	UNUSED ( subPrimID );

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	gfxState.SetFrameBuffer ( this->GetFrameBuffer ());
	
	//disable scissor rect for clear
	gfxState.SetScissorRect ();
	this->ClearSurface ();
}
