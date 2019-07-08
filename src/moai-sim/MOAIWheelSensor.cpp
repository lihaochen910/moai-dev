// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIWheelSensor.h>
#include <moai-sim/MOAIInputMgr.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getValue
	@text	Returns the current value of the wheel, based on delta events

	@in		MOAIWheelSensor self
	@out	number value
*/
mrb_value MOAIWheelSensor::_getValue ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIWheelSensor, "U" )
	
	return state.ToRValue ( self->mValue );
}

//----------------------------------------------------------------//
/**	@lua	getDelta
	@text	Returns the delta of the wheel

	@in		MOAIWheelSensor self
	@out	number delta
*/
mrb_value MOAIWheelSensor::_getDelta ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIWheelSensor, "U" )
	
	return state.ToRValue ( self->mDelta );
}

//----------------------------------------------------------------//
/**	@lua	setCallback
	@text	Sets or clears the callback to be issued on a wheel delta event

	@in		MOAIWheelSensor self
	@opt	function callback		Default value is nil.
	@out	nil
*/
mrb_value MOAIWheelSensor::_setCallback ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIWheelSensor, "U" )
	
	self->mCallback.SetRef ( state.GetParamValue ( 1 ) );
	
	return context;
}

//================================================================//
// MOAIWheelSensor
//================================================================//

//----------------------------------------------------------------//
void MOAIWheelSensor::EnqueueWheelEvent ( u8 deviceID, u8 sensorID, float value ) {

	MOAIInputMgr& inputMgr = MOAIInputMgr::Get ();
	if ( inputMgr.WriteEventHeader < MOAIWheelSensor >( deviceID, sensorID )) {
		inputMgr.Write < float >( value );
	}
}

//----------------------------------------------------------------//
MOAIWheelSensor::MOAIWheelSensor () :
	mValue ( 0 ),
	mDelta ( 0 ) {
	
	RTTI_SINGLE ( MOAISensor )

}

//----------------------------------------------------------------//
MOAIWheelSensor::~MOAIWheelSensor () {
}

//----------------------------------------------------------------//
void MOAIWheelSensor::ParseEvent ( ZLStream& eventStream ) {
	
	this->mDelta = eventStream.Read < float >( 0.0f );
	this->mValue += this->mDelta;
	
	if ( this->mCallback ) {
		MOAIRubyState& state = MOAIRubyRuntime::Get ().State ();

		mrb_value argv [ 1 ];
		argv [ 0 ] = state.ToRValue ( this->mDelta );

		state.FuncCall ( this->mCallback, "call", 1, argv );
	}
}

//----------------------------------------------------------------//
void MOAIWheelSensor::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAISensor::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIWheelSensor::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAISensor::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getValue", _getValue, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getDelta", _getDelta, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setCallback", _setCallback, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAIWheelSensor::ResetState () {

	this->mDelta = 0;
}
