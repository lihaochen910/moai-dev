// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIInputMgr.h>
#include <moai-sim/MOAIVectorSensor.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getVector
	@text	Polls the current status of the vector sensor.

	@in		MOAIVectorSensor self
	@out	number x
	@out	number y
	@out	number z
*/
mrb_value MOAIVectorSensor::_getVector ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorSensor, "U" )
	
	mrb_value argv [ 3 ];
	argv [ 0 ] = state.ToRValue ( self->mX );
	argv [ 0 ] = state.ToRValue ( self->mY );
	argv [ 0 ] = state.ToRValue ( self->mZ );
	
	return mrb_ary_new_from_values ( M, 3, argv );
}

//----------------------------------------------------------------//
/**	@lua	setCallback
	@text	Sets or clears the callback to be issued when the level changes.

	@in		MOAIMotionSensor self
	@opt	function callback		Default value is nil.
	@out	nil
*/
mrb_value MOAIVectorSensor::_setCallback ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorSensor, "U" )
	
	self->mCallback.SetRef ( state.GetParamValue ( 1 ) );
	
	return context;
}

//================================================================//
// MOAIVectorSensor
//================================================================//

//----------------------------------------------------------------//
void MOAIVectorSensor::EnqueueVectorEvent ( u8 deviceID, u8 sensorID, float x, float y, float z ) {

	MOAIInputMgr& inputMgr = MOAIInputMgr::Get ();
	if ( inputMgr.WriteEventHeader < MOAIVectorSensor >( deviceID, sensorID )) {
		inputMgr.Write < float >( x );
		inputMgr.Write < float >( y );
		inputMgr.Write < float >( z );
	}
}

//----------------------------------------------------------------//
MOAIVectorSensor::MOAIVectorSensor () :
	mX ( 0.0f ),
	mY ( 0.0f ),
	mZ ( 0.0f ) {

	RTTI_SINGLE ( MOAISensor )
}

//----------------------------------------------------------------//
MOAIVectorSensor::~MOAIVectorSensor () {
}

//----------------------------------------------------------------//
void MOAIVectorSensor::ParseEvent ( ZLStream& eventStream ) {

	this->mX = eventStream.Read < float >( 0.0f );
	this->mY = eventStream.Read < float >( 0.0f );
	this->mZ = eventStream.Read < float >( 0.0f );
	
	if ( this->mCallback ) {
		MOAIRubyState& state = MOAIRubyRuntime::Get ().State ();

		mrb_value argv [ 3 ];
		argv [ 0 ] = state.ToRValue ( this->mX );
		argv [ 0 ] = state.ToRValue ( this->mY );
		argv [ 0 ] = state.ToRValue ( this->mZ );

		state.FuncCall ( this->mCallback, "call", 3, argv );
	}
}

//----------------------------------------------------------------//
void MOAIVectorSensor::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAISensor::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIVectorSensor::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "getVector", _getVector, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setCallback", _setCallback, MRB_ARGS_REQ ( 1 ) );

}
