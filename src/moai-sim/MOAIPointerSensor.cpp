// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIPointerSensor.h>
#include <moai-sim/MOAIInputMgr.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getLoc
	@text	Returns the location of the pointer on the screen.

	@in		MOAIPointerSensor self
	@out	number x
	@out	number y
*/
mrb_value MOAIPointerSensor::_getLoc ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPointerSensor, "U" )

	mrb_value argv [ 2 ];
	argv [ 0 ] = state.ToRValue ( self->mX );
	argv [ 1 ] = state.ToRValue ( self->mY );

	return mrb_ary_new_from_values ( state, 2, argv );
}

//----------------------------------------------------------------//
/**	@lua	setCallback
	@text	Sets or clears the callback to be issued when the pointer location changes.

	@in		MOAIPointerSensor self
	@opt	function callback			Default value is nil.
	@out	nil
*/
mrb_value MOAIPointerSensor::_setCallback ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPointerSensor, "U" )

	//printf ( "MOAIPointerSensor::_setCallback() %d\n", mrb_type (state.GetParamValue ( 1 )) );
	
	self->mOnMove.SetRef ( state.GetParamValue ( 1 ) );
	return context;
}

//================================================================//
// MOAIPointerSensor
//================================================================//

//----------------------------------------------------------------//
void MOAIPointerSensor::EnqueuePointerEvent ( u8 deviceID, u8 sensorID, int x, int y ) {

	MOAIInputMgr& inputMgr = MOAIInputMgr::Get ();
	if ( inputMgr.WriteEventHeader < MOAIPointerSensor >( deviceID, sensorID )) {
		inputMgr.Write < int >( x );
		inputMgr.Write < int >( y );
	}
}

//----------------------------------------------------------------//
MOAIPointerSensor::MOAIPointerSensor () {

	RTTI_SINGLE ( MOAISensor )
}

//----------------------------------------------------------------//
MOAIPointerSensor::~MOAIPointerSensor () {
}

//----------------------------------------------------------------//
void MOAIPointerSensor::ParseEvent ( ZLStream& eventStream ) {

	int x = eventStream.Read < int >( 0 );
	int y = eventStream.Read < int >( 0 );

	this->mX = x;
	this->mY = y;

	//printf ( "MOAIPointerSensor::ParseEvent() %d, %d\n", x, y );
	
	if ( this->mOnMove ) {
		MOAIRubyState state = MOAIRubyRuntime::Get ().State ();

		mrb_value argv [ 2 ];
		argv [ 0 ] = state.ToRValue ( this->mX );
		argv [ 1 ] = state.ToRValue ( this->mY );

		state.FuncCall ( this->mOnMove, "call", 2, argv );
	}
}

//----------------------------------------------------------------//
void MOAIPointerSensor::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAISensor::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIPointerSensor::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAISensor::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getLoc", _getLoc, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setCallback", _setCallback, MRB_ARGS_REQ ( 1 ) );

}
