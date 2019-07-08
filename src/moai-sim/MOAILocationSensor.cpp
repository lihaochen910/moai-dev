// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAILocationSensor.h>
#include <moai-sim/MOAIInputMgr.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getLocation
	@text	Returns the current information about the physical location.

	@in		MOAILocationSensor self
	@out	number longitude
	@out	number latitude
	@out	number haccuracy		The horizontal (long/lat) accuracy.
	@out	number altitude
	@out	number vaccuracy		The vertical (altitude) accuracy.
	@out	number speed
*/
mrb_value MOAILocationSensor::_getLocation ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAILocationSensor, "U" )
	
	mrb_value argv [ 6 ];
	argv [ 0 ] = state.ToRValue ( self->mLongitude );
	argv [ 1 ] = state.ToRValue ( self->mLatitude );
	argv [ 2 ] = state.ToRValue ( self->mHAccuracy );
	argv [ 3 ] = state.ToRValue ( self->mAltitude );
	argv [ 4 ] = state.ToRValue ( self->mVAccuracy );
	argv [ 5 ] = state.ToRValue ( self->mSpeed );
	
	return mrb_ary_new_from_values ( state, 6, argv );
}

//----------------------------------------------------------------//
/**	@lua	setCallback
	@text	Sets or clears the callback to be issued when the location changes.

	@in		MOAILocationSensor self
	@opt	function callback			Default value is nil.
	@out	nil
*/
mrb_value MOAILocationSensor::_setCallback ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAILocationSensor, "U" )
	
	self->mCallback.SetRef ( state.GetParamValue ( 1 ) );
	return context;
}

//================================================================//
// MOAILocationSensor
//================================================================//

//----------------------------------------------------------------//
void MOAILocationSensor::EnqueueLocationEvent ( u8 deviceID, u8 sensorID, double longitude, double latitude, double altitude, float hAccuracy, float vAccuracy, float speed ) {

	MOAIInputMgr& inputMgr = MOAIInputMgr::Get ();
	if ( inputMgr.WriteEventHeader < MOAILocationSensor >( deviceID, sensorID )) {
		inputMgr.Write < double >( longitude );
		inputMgr.Write < double >( latitude );
		inputMgr.Write < double >( altitude );
		inputMgr.Write < float >( hAccuracy );
		inputMgr.Write < float >( vAccuracy );
		inputMgr.Write < float >( speed );
	}
}

//----------------------------------------------------------------//
MOAILocationSensor::MOAILocationSensor () :
	mLongitude ( 0.0 ),
	mLatitude ( 0.0 ),
	mAltitude ( 0.0 ),
	mHAccuracy ( 0.0f ),
	mVAccuracy ( 0.0f ),
	mSpeed ( 0.0f ) {

	RTTI_SINGLE ( MOAISensor )
}

//----------------------------------------------------------------//
MOAILocationSensor::~MOAILocationSensor () {
}

//----------------------------------------------------------------//
void MOAILocationSensor::ParseEvent ( ZLStream& eventStream ) {

	this->mLongitude	= eventStream.Read < double >( 0.0 );
	this->mLatitude		= eventStream.Read < double >( 0.0 );
	this->mAltitude		= eventStream.Read < double >( 0.0 );
	this->mHAccuracy	= eventStream.Read < float >( 0.0f );
	this->mVAccuracy	= eventStream.Read < float >( 0.0f );
	this->mSpeed		= eventStream.Read < float >( 0.0f );
	
	if ( this->mCallback ) {
		MOAIRubyState& state = MOAIRubyRuntime::Get ().State ();

		mrb_value argv [ 6 ];
		argv [ 0 ] = state.ToRValue ( this->mLongitude );
		argv [ 1 ] = state.ToRValue ( this->mLatitude );
		argv [ 2 ] = state.ToRValue ( this->mHAccuracy );
		argv [ 3 ] = state.ToRValue ( this->mAltitude );
		argv [ 4 ] = state.ToRValue ( this->mVAccuracy );
		argv [ 5 ] = state.ToRValue ( this->mSpeed );

		state.FuncCall ( this->mCallback, "call", 6, argv );
	}
}

//----------------------------------------------------------------//
void MOAILocationSensor::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAISensor::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAILocationSensor::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAISensor::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getLocation", _getLocation, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setCallback", _setCallback, MRB_ARGS_REQ ( 1 ) );

}
