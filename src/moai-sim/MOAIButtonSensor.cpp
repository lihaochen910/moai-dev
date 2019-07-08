// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIButtonSensor.h>
#include <moai-sim/MOAIInputMgr.h>

//================================================================//
// MOAIButtonEvent
//================================================================//
class MOAIButtonEvent {
public:

	bool	mDown;
};

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	down
	@text	Checks to see if the button was pressed during the last iteration.

	@in		MOAIButtonSensor self
	@out	boolean wasPressed
*/
mrb_value MOAIButtonSensor::_down ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIButtonSensor, "U" )
	
	return state.ToRValue ( self->ButtonDown () );
}

//----------------------------------------------------------------//
/**	@lua	isDown
	@text	Checks to see if the button is currently down.

	@in		MOAIButtonSensor self
	@out	boolean isDown
*/
mrb_value MOAIButtonSensor::_isDown ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIButtonSensor, "U" )
	
	return state.ToRValue ( self->ButtonIsDown () );
}

//----------------------------------------------------------------//
/**	@lua	isUp
	@text	Checks to see if the button is currently up.

	@in		MOAIButtonSensor self
	@out	boolean isUp
*/
mrb_value MOAIButtonSensor::_isUp ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIButtonSensor, "U" )
	
	return state.ToRValue ( self->ButtonIsUp () );
}

//----------------------------------------------------------------//
/**	@lua	up
	@text	Checks to see if the button was released during the last iteration.

	@in		MOAIButtonSensor self
	@out	boolean wasReleased
*/
mrb_value MOAIButtonSensor::_up ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIButtonSensor, "U" )
	
	return state.ToRValue ( self->ButtonUp () );
}

//----------------------------------------------------------------//
/**	@lua	setCallback
	@text	Sets or clears the callback to be issued when button events occur.

	@in		MOAIButtonSensor self
	@opt	function callback		Default value is nil.
	@out	nil
*/
mrb_value MOAIButtonSensor::_setCallback ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIButtonSensor, "U" )
	
	self->mOnButton.SetRef ( state.GetParamValue ( 1 ) );
	return context;
}

//================================================================//
// MOAIButtonSensor
//================================================================//

//----------------------------------------------------------------//
bool MOAIButtonSensor::ButtonDown () {

	return (( mState & DOWN ) == DOWN );
}

//----------------------------------------------------------------//
bool MOAIButtonSensor::ButtonIsDown () {

	return (( mState & IS_DOWN ) == IS_DOWN );
}

//----------------------------------------------------------------//
bool MOAIButtonSensor::ButtonIsUp () {

	return (( mState & IS_DOWN ) == 0 );
}

//----------------------------------------------------------------//
bool MOAIButtonSensor::ButtonUp () {

	return (( mState & UP ) == UP );
}

//----------------------------------------------------------------//
void MOAIButtonSensor::ClearState () {

	this->mState = 0;
}

//----------------------------------------------------------------//
void MOAIButtonSensor::EnqueueButtonEvent ( u8 deviceID, u8 sensorID, bool down ) {

	MOAIInputMgr& inputMgr = MOAIInputMgr::Get ();
	if ( inputMgr.WriteEventHeader < MOAIButtonSensor >( deviceID, sensorID )) {
		inputMgr.Write < bool >( down );
	}
}

//----------------------------------------------------------------//
MOAIButtonSensor::MOAIButtonSensor () :
	mState ( 0 ) {

	RTTI_SINGLE ( MOAISensor )
}

//----------------------------------------------------------------//
MOAIButtonSensor::~MOAIButtonSensor () {
}

//----------------------------------------------------------------//
void MOAIButtonSensor::ParseEvent ( ZLStream& eventStream ) {

	bool down = eventStream.Read < bool >( false );
	
	if ( down ) {
		this->mState |= IS_DOWN | DOWN;
	}
	else {
		this->mState &= ~IS_DOWN;
		this->mState |= UP;
	}

	//printf ( "MOAIButtonSensor::ParseEvent() %d\n", down );
	
	if ( this->mOnButton ) {
		MOAIRubyState& state = MOAIRubyRuntime::Get ().State ();

		mrb_value argv [ 1 ];
		argv [ 0 ] = state.ToRValue ( down );

		state.FuncCall ( this->mOnButton, "call", 1, argv );
	}
}

//----------------------------------------------------------------//
void MOAIButtonSensor::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAISensor::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIButtonSensor::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAISensor::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "down", _down, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isDown", _isDown, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isUp", _isUp, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "up", _up, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setCallback", _setCallback, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAIButtonSensor::ResetState () {

	// clear out the old events
	this->mState &= ~( DOWN | UP );	
}
