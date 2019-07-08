// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAICompassSensor.h>
#include <moai-sim/MOAIInputMgr.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getHeading
	@text	Returns the current heading according to the built-in compass.

	@in		MOAICompassSensor self
	@out	number heading
*/
mrb_value MOAICompassSensor::_getHeading ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICompassSensor, "U" )
	
	return state.ToRValue ( self->mHeading );
}

//----------------------------------------------------------------//
/**	@lua	setCallback
	@text	Sets or clears the callback to be issued when the heading changes.

	@in		MOAICompassSensor self
	@opt	function callback			Default value is nil.
	@out	nil
*/
mrb_value MOAICompassSensor::_setCallback ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICompassSensor, "U" )
	
	self->mCallback.SetRef ( state.GetParamValue ( 1 ) );
	return context;
}

//================================================================//
// MOAICompassSensor
//================================================================//

//----------------------------------------------------------------//
void MOAICompassSensor::EnqueueCompassEvent ( u8 deviceID, u8 sensorID, float heading ) {

	MOAIInputMgr& inputMgr = MOAIInputMgr::Get ();
	if ( inputMgr.WriteEventHeader < MOAICompassSensor >( deviceID, sensorID )) {
		inputMgr.Write < float >( heading );
	}
}

//----------------------------------------------------------------//
MOAICompassSensor::MOAICompassSensor () :
	mHeading ( 0.0f ) {

	RTTI_SINGLE ( MOAISensor )
}

//----------------------------------------------------------------//
MOAICompassSensor::~MOAICompassSensor () {
}

//----------------------------------------------------------------//
void MOAICompassSensor::ParseEvent ( ZLStream& eventStream ) {

	this->mHeading = eventStream.Read < float >( 0.0f );
	
	if ( this->mCallback ) {
		MOAIRubyState& state = MOAIRubyRuntime::Get ().State ();

		mrb_value argv [ 1 ];
		argv [ 0 ] = state.ToRValue ( this->mHeading );

		state.FuncCall ( this->mCallback, "call", 1, argv );
	}
}

//----------------------------------------------------------------//
void MOAICompassSensor::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAISensor::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAICompassSensor::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAISensor::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getHeading", _getHeading, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setCallback", _setCallback, MRB_ARGS_REQ ( 1 ) );

}
