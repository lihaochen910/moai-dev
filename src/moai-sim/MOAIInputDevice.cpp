// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <moai-sim/MOAIButtonSensor.h>
#include <moai-sim/MOAICompassSensor.h>
#include <moai-sim/MOAIInputDevice.h>
#include <moai-sim/MOAIJoystickSensor.h>
#include <moai-sim/MOAIKeyboardSensor.h>
#include <moai-sim/MOAILocationSensor.h>
#include <moai-sim/MOAIMotionSensor.h>
#include <moai-sim/MOAIPointerSensor.h>
#include <moai-sim/MOAITouchSensor.h>
#include <moai-sim/MOAISensor.h>
#include <moai-sim/MOAIWheelSensor.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getHardwareInfo
	@text	Get any hardware metadate string specified by the host.
	
	@in		MOAIInputDevice self
	@out	string hardwareInfo
*/
mrb_value MOAIInputDevice::_getHardwareInfo ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIInputDevice, "U" )
	
	return state.ToRValue ( self->mHardwareInfo.c_str () );
}

//----------------------------------------------------------------//
mrb_value MOAIInputDevice::_getSensors ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIInputDevice, "" )

	return self->mSensorsHash;
}

//----------------------------------------------------------------//
mrb_value MOAIInputDevice::_methodMissing ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIInputDevice, "" )

	return mrb_hash_get ( M, self->mSensorsHash, mrb_sym2str ( M, mrb_symbol ( state.GetParamValue ( 1 ) ) ) );
}

//================================================================//
// MOAIInputDevice
//================================================================//

//----------------------------------------------------------------//
void MOAIInputDevice::ClearSensorState () {

	for ( u32 i = 0; i < this->mSensors.Size (); ++i ) {
		MOAISensor* sensor = this->mSensors [ i ];
		if ( sensor ) {
			sensor->ClearState ();
		}
	}
}

//----------------------------------------------------------------//
MOAISensor* MOAIInputDevice::GetSensor ( u8 sensorID ) {
	
	if ( sensorID < this->mSensors.Size ()) {
		return this->mSensors [ sensorID ];
	}
	return 0;
}

//----------------------------------------------------------------//
MOAIInputDevice::MOAIInputDevice () :
	mIsActive ( true ) {
	
	RTTI_SINGLE ( MOAIRubyObject )

	this->mSensorsHash.SetRef ( mrb_hash_new ( MOAIRubyRuntime::Get ().State () ) );
}

//----------------------------------------------------------------//
MOAIInputDevice::~MOAIInputDevice () {

	for ( u32 i = 0; i < this->mSensors.Size (); ++i ) {
		this->RubyRelease ( this->mSensors [ i ]);
	}
}

//----------------------------------------------------------------//
void MOAIInputDevice::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineStaticMethod ( klass, "getHardwareInfo", _getHardwareInfo, MRB_ARGS_NONE () );
	
}

//----------------------------------------------------------------//
void MOAIInputDevice::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );

	state.DefineInstanceMethod ( klass, "getSensors", _getSensors, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "method_missing", _methodMissing, MRB_ARGS_ANY () );

}

//----------------------------------------------------------------//
void MOAIInputDevice::ReserveSensors ( u8 total ) {

	this->mSensors.Init ( total );
	this->mSensors.Fill ( 0 );
}

//----------------------------------------------------------------//
void MOAIInputDevice::ResetSensorState () {

	for ( u32 i = 0; i < this->mSensors.Size (); ++i ) {
		MOAISensor* sensor = this->mSensors [ i ];
		if ( sensor ) {
			sensor->ResetState ();
		}
	}
}

//----------------------------------------------------------------//
void MOAIInputDevice::SetHardwareInfo ( cc8* hardwareInfo ) {

	this->mHardwareInfo = hardwareInfo;
}

//----------------------------------------------------------------//
void MOAIInputDevice::SetSensor ( u8 sensorID, cc8* name, MOAISensor* sensor ) {

	assert ( sensor );
	
	sensor->mName = name;
	
	this->RubyRelease ( this->mSensors [ sensorID ]);
	
	this->mSensors [ sensorID ] = sensor;
	this->RubyRetain ( sensor );
	
	/*MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
	this->PushRubyUserdata ( state );

	sensor->PushRubyUserdata ( state );
	lua_setfield ( state, -2, name );*/

	// TODO:
	MOAIRubyState& state = MOAIRubyRuntime::Get ().GetMainState ();

	mrb_hash_set ( state, this->mSensorsHash, state.ToRValue ( name ), state.ToRValue < MOAIRubyObject* >( sensor ));
}
