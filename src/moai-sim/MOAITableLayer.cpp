// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIColor.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGfxResourceClerk.h>
#include <moai-sim/MOAITableLayer.h>
#include <moai-sim/MOAIRenderMgr.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getRenderTable
	@text	Returns the table currently being used for rendering.
	
	@in		MOAITableLayer self
	@out	table renderTable
*/
mrb_value MOAITableLayer::_getRenderTable ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITableLayer, "U" )
	return self->mRenderTable;
}

//----------------------------------------------------------------//
/**	@lua	setRenderTable
	@text	Sets the table to be used for rendering. This should be
			an array indexed from 1 consisting of MOAIRenderable objects
			and sub-tables. Objects will be rendered in order starting
			from index 1 and continuing until 'nil' is encountered.
	
	@in		MOAITableLayer self
	@in		table renderTable
	@out	nil
*/
mrb_value MOAITableLayer::_setRenderTable ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITableLayer, "U" )
	self->mRenderTable.SetRef ( state.GetParamValue ( 1 ) );
	return mrb_nil_value ();
}

//================================================================//
// MOAITableLayer
//================================================================//

//----------------------------------------------------------------//
MOAITableLayer::MOAITableLayer () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAILayer )
	RTTI_END
}

//----------------------------------------------------------------//
MOAITableLayer::~MOAITableLayer () {
}

//----------------------------------------------------------------//
void MOAITableLayer::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAILayer::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAITableLayer::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAILayer::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getRenderTable",	_getRenderTable, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setRenderTable",	_setRenderTable, MRB_ARGS_REQ ( 1 ) );

}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAITableLayer::MOAIDrawable_Draw ( int subPrimID ) {
	UNUSED ( subPrimID );

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	gfxState.SetFrameBuffer ( this->GetFrameBuffer ());
	
	//disable scissor rect for clear
	gfxState.SetScissorRect ();
	this->ClearSurface ();
	
	MOAIDrawable::Draw ( MOAIRubyRuntime::Get ().State (), this->mRenderTable );
		
	// restore the frame buffer
	gfxState.SetFrameBuffer ( this->GetFrameBuffer ());
}

