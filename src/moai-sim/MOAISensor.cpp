// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAISensor.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAISensor::_getTimestamp ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAISensor, "U" )
	
	return state.ToRValue ( self->mTimestamp );
}

//================================================================//
// MOAISensor
//================================================================//

//----------------------------------------------------------------//
MOAISensor::MOAISensor () :
	mType ( 0 ),
	mTimestamp ( 0 ) {

	RTTI_SINGLE ( MOAIRubyObject )
}

//----------------------------------------------------------------//
MOAISensor::~MOAISensor () {
}

//----------------------------------------------------------------//
void MOAISensor::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAISensor::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "getTimestamp", _getTimestamp, MRB_ARGS_NONE () );

}
