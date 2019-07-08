// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIJoystickSensor.h>
#include <moai-sim/MOAIInputMgr.h>

//================================================================//
// MOAIJoystickEvent
//================================================================//
class MOAIJoystickEvent {
public:

	float	mX;
	float	mY;
};

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getVector
	@text	Returns the joystick vector.

	@in		MOAIJoystickSensor self
	@out	number x
	@out	number y
*/
mrb_value MOAIJoystickSensor::_getVector ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIJoystickSensor, "U" )
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( self->mX );
	ret [ 1 ] = state.ToRValue ( self->mY );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	setCallback
	@text	Sets or clears the callback to be issued when the joystick vector changes.

	@in		MOAIJoystickSensor self
	@opt	function callback			Default value is nil.
	@out	nil
*/
mrb_value MOAIJoystickSensor::_setCallback ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIJoystickSensor, "U" )
	
	self->mOnStick.SetRef ( state.GetParamValue ( 1 ) );
	return context;
}

//================================================================//
// MOAIJoystickSensor
//================================================================//

//----------------------------------------------------------------//
void MOAIJoystickSensor::EnqueueJoystickEvent( u8 deviceID, u8 sensorID, float x, float y ) {

	MOAIInputMgr& inputMgr = MOAIInputMgr::Get ();
	if ( inputMgr.WriteEventHeader < MOAIJoystickSensor >( deviceID, sensorID )) {
		inputMgr.Write < float >( x );
		inputMgr.Write < float >( y );
	}
}

//----------------------------------------------------------------//
MOAIJoystickSensor::MOAIJoystickSensor () {

	RTTI_SINGLE ( MOAISensor )
}

//----------------------------------------------------------------//
MOAIJoystickSensor::~MOAIJoystickSensor () {
}

//----------------------------------------------------------------//
void MOAIJoystickSensor::ParseEvent ( ZLStream& eventStream ) {

	this->mX = eventStream.Read < float >( 0.0f );
	this->mY = eventStream.Read < float >( 0.0f );
	
	if ( this->mOnStick ) {
		MOAIRubyState& state = MOAIRubyRuntime::Get ().State ();

		mrb_value argv [ 2 ];
		argv [ 0 ] = state.ToRValue ( this->mX );
		argv [ 1 ] = state.ToRValue ( this->mY );

		state.FuncCall ( this->mOnStick, "call", 1, argv );
	}
}

//----------------------------------------------------------------//
void MOAIJoystickSensor::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAISensor::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIJoystickSensor::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAISensor::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getVector", _getVector, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setCallback", _setCallback, MRB_ARGS_REQ ( 1 ) );

}
