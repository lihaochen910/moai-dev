// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIDeckRemapper.h>
#include <moai-sim/MOAITileFlags.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	reserve
	@text	The total number of indices to remap. Index remaps will be
			initialized from 1 to N.
	
	@in		MOAIDeckRemapper self
	@opt	number size		Default value is 0.
	@out	nil
*/
mrb_value MOAIDeckRemapper::_reserve ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDeckRemapper, "U" )

	u32 size = state.GetParamValue < u32 >( 1, 0 );
	self->mRemap.Init ( size );
	
	for ( u32 i = 0; i < size; ++i ) {
		self->mRemap [ i ] = i;
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setBase
	@text	Set the base offset for the range of indices to remap.
			Used when remapping only a portion of the indices in
			the original deck.
	
	@in		MOAIDeckRemapper self
	@opt	number base		Default value is 0.
	@out	nil
*/
mrb_value MOAIDeckRemapper::_setBase ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDeckRemapper, "U" )
	
	self->mBase = state.GetParamValue < u32 >( 1, 0 );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setRemap
	@text	Remap a single index to a new value.
	
	@in		MOAIDeckRemapper self
	@in		number index		Index to remap.
	@opt	number remap		New value for index. Default value is index (i.e. remove the remap).
	@out	nil
*/
mrb_value MOAIDeckRemapper::_setRemap ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDeckRemapper, "UN" )

	u32 idx		= state.GetParamValue < u32 >( 1, 1 );
	u32 remap	= state.GetParamValue < u32 >( 2, idx );
	
	idx			= idx - 1;
	remap		= remap - 1;
	
	u32 code = idx - self->mBase;
	
	if ( code < self->mRemap.Size ()) {
		self->mRemap [ code ] = remap;
	}
	return context;
}

//================================================================//
// MOAIDeckRemapper
//================================================================//

//----------------------------------------------------------------//
MOAIDeckRemapper::MOAIDeckRemapper () :
	mBase ( 0 ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAINode )
		RTTI_EXTEND ( MOAIDeckProxy )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIDeckRemapper::~MOAIDeckRemapper () {
}

//----------------------------------------------------------------//
void MOAIDeckRemapper::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAINode::RegisterRubyClass ( state, klass );
	MOAIDeckProxy::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIDeckRemapper::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAINode::RegisterRubyFuncs ( state, klass );
	MOAIDeckProxy::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "reserve", _reserve, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setBase", _setBase, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setRemap", _setRemap, MRB_ARGS_ARG ( 0, 1 ) );
	
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
u32 MOAIDeckRemapper::MOAIDeckProxy_Remap ( u32 idx ) {

	u32 code = idx - this->mBase;
	
	return ( code < this->mRemap.Size ()) ? this->mRemap [ code ] : idx;
}

//----------------------------------------------------------------//
bool MOAIDeckRemapper::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	u32 code = attrID - this->mBase - 1;

	if ( code < this->mRemap.Size ()) {
		this->mRemap [ code ] = ZLFloat::ToIndex ( attr.Apply (( float )this->mRemap [ code ], op, MOAIAttribute::ATTR_READ_WRITE )) - 1;
		return true;
	}
	return false;
}
