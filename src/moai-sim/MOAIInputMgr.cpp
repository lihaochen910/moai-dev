// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <moai-sim/MOAIButtonSensor.h>
#include <moai-sim/MOAICompassSensor.h>
#include <moai-sim/MOAIInputDevice.h>
#include <moai-sim/MOAIInputMgr.h>
#include <moai-sim/MOAIJoystickSensor.h>
#include <moai-sim/MOAIKeyboardSensor.h>
#include <moai-sim/MOAILocationSensor.h>
#include <moai-sim/MOAIMotionSensor.h>
#include <moai-sim/MOAIPointerSensor.h>
#include <moai-sim/MOAISensor.h>
#include <moai-sim/MOAITouchSensor.h>
#include <moai-sim/MOAIWheelSensor.h>

#define LUAVAR_CONFIGURATION	"configuration"

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
mrb_value MOAIInputMgr::_autoTimestamp ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIInputMgr, "" )
	
	self->SetAutotimestamp ( state.GetParamValue < bool >( 1, true ));
	
	return context;
}

//----------------------------------------------------------------//
mrb_value MOAIInputMgr::_deferEvents ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIInputMgr, "" )

	bool defer = state.GetParamValue < bool >( 1, false );
	self->DeferEvents ( defer );
	
	return context;
}

//----------------------------------------------------------------//
mrb_value MOAIInputMgr::_discardEvents ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIInputMgr, "" )
	
	self->DiscardAll ();
	
	return context;
}

//----------------------------------------------------------------//
mrb_value MOAIInputMgr::_getDevices ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIInputMgr, "" )

	return self->mDevicesHash;
}

//----------------------------------------------------------------//
mrb_value MOAIInputMgr::_methodMissing ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIInputMgr, "" )

	return mrb_hash_get ( M, self->mDevicesHash, mrb_sym2str ( M, mrb_symbol ( state.GetParamValue ( 1 ) )));
}

//----------------------------------------------------------------//
mrb_value MOAIInputMgr::_playback ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIInputMgr, "" )
	
	self->mPlayback = state.GetParamValue < bool >( 1, true );
	
	return context;
}

//----------------------------------------------------------------//
mrb_value MOAIInputMgr::_setAutosuspend ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIInputMgr, "" )
	
	self->mAutosuspend = state.GetParamValue < double >( 1, 0 );
	
	return context;
}

//----------------------------------------------------------------//
mrb_value MOAIInputMgr::_setEventCallback ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIInputMgr, "" )
	
	self->mEventCallback.SetRef ( state.GetParamValue ( 1 ) );
	
	return context;
}

//----------------------------------------------------------------//
mrb_value MOAIInputMgr::_setRecorder ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIInputMgr, "" )
	
	self->mRecorder.Set ( *self, state.GetRubyObject < MOAIStream >( 1, true ));
	
	return context;

}

//----------------------------------------------------------------//
mrb_value MOAIInputMgr::_suspendEvents ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIInputMgr, "" )
	
	self->SuspendEvents ( state.GetParamValue ( 1, false ));
	
	return context;
}

//================================================================//
// MOAIInputMgr
//================================================================//

//----------------------------------------------------------------//
u8 MOAIInputMgr::AddDevice ( cc8* name  ) {

	u8 id = ( u8 )this->mDevices.GetTop ();
	
	this->mDevices.Push ();
	this->SetDevice ( id, name );
	return id;
}

//----------------------------------------------------------------//
bool MOAIInputMgr::CanWrite () {

	if (( this->mAutosuspend > 0 ) && ( this->mAutosuspend <= ( ZLDeviceTime::GetTimeInSeconds () - this->mLastUpdate ))) {
		this->DiscardAll ();
		this->mAutosuspended = true;
	}
	return !( this->mPlayback || this->mSuspended || this->mAutosuspended );
}

//----------------------------------------------------------------//
void MOAIInputMgr::ClearSensorState () {

	for ( u32 i = 0; i < this->mDevices.Size (); ++i ) {
		MOAIInputDevice* device = this->mDevices [ i ];
		if ( device ) {
			device->ClearSensorState ();
		}
	}
}

//----------------------------------------------------------------//
bool MOAIInputMgr::CheckSensor ( u8 deviceID, u8 sensorID, u32 type ) {

	MOAIInputDevice* device = this->GetDevice ( deviceID );
	if ( device && device->mIsActive ) {
		MOAISensor* sensor = device->GetSensor ( sensorID );
		return ( sensor && ( sensor->mType == type ));
	}
	return false;
}

//----------------------------------------------------------------//
void MOAIInputMgr::DeferEvents ( bool defer ) {

	this->mDefer = defer;
}

//----------------------------------------------------------------//
void MOAIInputMgr::FlushEvents ( double skip ) {

	this->mTimebase += skip;
	this->DiscardAll ();
	this->ClearSensorState ();
}

//----------------------------------------------------------------//
MOAIInputDevice* MOAIInputMgr::GetDevice ( u8 deviceID ) {

	if ( deviceID < this->mDevices.Size ()) {
		return this->mDevices [ deviceID ];
	}
	return 0;
}

//----------------------------------------------------------------//
MOAISensor* MOAIInputMgr::GetSensor ( u8 deviceID, u8 sensorID ) {

	MOAIInputDevice* device = this->GetDevice ( deviceID );
	if ( device ) {
		return device->GetSensor ( sensorID );
	}
	return 0;
}

//----------------------------------------------------------------//
bool MOAIInputMgr::HasEvents () {

	// TODO: this ignores the case where there are pending events that would not be processed until after the current frame
	return this->GetCursor () > 0;
}

//----------------------------------------------------------------//
void MOAIInputMgr::InvokeCallback ( u32 event, double timestamp ) {

	if ( this->mEventCallback ) {
		
		MOAIRubyState& state = MOAIRubyRuntime::Get ().State ();

		mrb_value argv [ 2 ];
		argv [ 0 ] = state.ToRValue ( event );
		argv [ 1 ] = state.ToRValue ( timestamp );

		state.FuncCall ( this->mEventCallback, "call", 2, argv );
	}
}

//----------------------------------------------------------------//
//bool MOAIInputMgr::IsDone () {
//	return false;
//}

//----------------------------------------------------------------//
MOAIInputMgr::MOAIInputMgr () :
	mTimebase ( 0 ),
	mTimestamp ( 0 ),
	mAutoTimestamp ( false ),
	mDefer ( false ),
	mSuspended ( false ),
	mAutosuspend ( 0 ),
	mAutosuspended ( false ),
	mLastUpdate ( 0 ),
	mPlayback ( false ) {
	
	RTTI_SINGLE ( MOAIAction )
	
	this->SetChunkSize ( CHUNK_SIZE );

	this->mDevicesHash.SetRef ( mrb_hash_new ( MOAIRubyRuntime::Get ().State ()));
}

//----------------------------------------------------------------//
MOAIInputMgr::~MOAIInputMgr () {

	if ( this->mRecorder ) {
		this->mRecorder->Flush ();
	}

	for ( u32 i = 0; i < this->mDevices.Size (); ++i ) {
		this->RubyRelease ( this->mDevices [ i ]);
	}
	this->mRecorder.Set ( *this, 0 );
}

//----------------------------------------------------------------//
size_t MOAIInputMgr::ParseEvents ( ZLStream& stream, double timestep ) {
	UNUSED ( timestep ); // TODO: fix this

	bool first = true;
	//double timebase = 0;
	size_t cursor = stream.GetCursor ();
	
	// parse events until we run out or hit an event after the current sim time
	while ( stream.GetCursor () < stream.GetLength ()) {
		
		u8 deviceID			= stream.Read < u8 >( 0 );
		u8 sensorID			= stream.Read < u8 >( 0 );
		double timestamp	= stream.Read < double >( 0 );
		
		if ( first ) {
			//timebase = timestamp;
			first = false;
		}
		
		//if ( timestep < ( timestamp - timebase )) break; // TODO: come back to this
		
		MOAISensor* sensor = this->GetSensor ( deviceID, sensorID );
		assert ( sensor );
		sensor->mTimestamp = timestamp;
		sensor->ParseEvent ( stream );
		
		this->InvokeCallback ( INPUT_EVENT, timestamp );
		
		cursor = stream.GetCursor ();
	}
	
	this->InvokeCallback ( FINISHED_UPDATE, ZLDeviceTime::GetTimeInSeconds () - this->mTimebase );
	
	return cursor;
}

//----------------------------------------------------------------//
void MOAIInputMgr::Record ( size_t size ) {

	if ( this->mRecorder && size ) {
		this->SetCursor ( 0 );
		this->mRecorder->WriteStream ( *this, size );
		this->SetCursor ( size );
	}
}

//----------------------------------------------------------------//
void MOAIInputMgr::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "INPUT_EVENT",			( u32 )INPUT_EVENT );
	state.DefineClassConst ( klass, "FINISHED_UPDATE",		( u32 )FINISHED_UPDATE );

	state.DefineStaticMethod ( klass, "autoTimestamp", _autoTimestamp, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "deferEvents", _deferEvents, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "discardEvents", _discardEvents, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getDevices", _getDevices, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "method_missing", _methodMissing, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "playback", _playback, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "setAutosuspend", _setAutosuspend, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "setEventCallback", _setEventCallback, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "setRecorder", _setRecorder, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "suspendEvents", _suspendEvents, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAIInputMgr::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIInputMgr::ReserveDevices ( u8 total ) {

	this->mDevices.SetTop ( total, 0 );
}

//----------------------------------------------------------------//
void MOAIInputMgr::ReserveSensors ( u8 deviceID, u8 total ) {

	MOAIInputDevice* device = this->GetDevice ( deviceID );
	if ( device ) {
		device->ReserveSensors ( total );
	}
}

//----------------------------------------------------------------//
void MOAIInputMgr::ResetSensorState () {

	for ( u32 i = 0; i < this->mDevices.Size (); ++i ) {
		MOAIInputDevice* device = this->mDevices [ i ];
		if ( device ) {
			device->ResetSensorState ();
		}
	}
}

//----------------------------------------------------------------//
void MOAIInputMgr::SetAutosuspend ( double autosuspend ) {

	this->mAutosuspend = autosuspend >= 0 ? autosuspend : 0;
}

//----------------------------------------------------------------//
void MOAIInputMgr::SetAutotimestamp ( bool autotimestamp ) {

	this->mAutoTimestamp = autotimestamp;
	if ( this->mAutoTimestamp ) {
		this->mTimebase = ZLDeviceTime::GetTimeInSeconds ();
	}
}

//----------------------------------------------------------------//
void MOAIInputMgr::SetConfigurationName ( cc8* name ) {

	MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
	//this->PushRubyClassTable ( state );
	
	state.SetClassField ( this->GetRubyClass ()->GetRClass (), LUAVAR_CONFIGURATION, state.ToRValue ( name ) );
}

//----------------------------------------------------------------//
void MOAIInputMgr::SetDevice ( u8 deviceID, cc8* name ) {

	if ( !( deviceID < this->mDevices.Size ())) return;

	this->RubyRelease ( this->mDevices [ deviceID ]);

	MOAIRubyState& state = MOAIRubyRuntime::Get ().GetMainState ();
	
	MOAIInputDevice* device = state.CreateClassInstance < MOAIInputDevice >();
	this->mDevices [ deviceID ] = device;
	this->RubyRetain ( device );
	
	if ( name ) {
	
		device->SetName ( name );
	
		/*MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
		this->PushMemberTable ( state );
		device->PushRubyUserdata ( state );
		lua_setfield ( state, -2, name );*/

		mrb_hash_set ( state, this->mDevicesHash, state.ToRValue ( name ), state.ToRValue < MOAIRubyObject* >( device ));

		//state.SetClassField ( this->GetRubyClass ()->GetRClass (), name, state.ToRValue < MOAIRubyObject* >( device ) );
	}
}

//----------------------------------------------------------------//
void MOAIInputMgr::SetDeviceActive ( u8 deviceID, bool active ) {

	MOAIInputDevice* device = this->GetDevice ( deviceID );
	if ( device ) {
		device->SetActive ( active );
	}
}

//----------------------------------------------------------------//
void MOAIInputMgr::SetDeviceHardwareInfo ( u8 deviceID, cc8* hardwareInfo ) {

	MOAIInputDevice* device = this->GetDevice ( deviceID );
	if ( device ) {
		device->SetHardwareInfo ( hardwareInfo );
	}
}

//----------------------------------------------------------------//
void MOAIInputMgr::SuspendEvents ( bool suspend ) {

	if ( suspend ) {
		this->DiscardAll ();
	}
	this->mSuspended = suspend;
}

//----------------------------------------------------------------//
void MOAIInputMgr::Update ( double timestep ) {

	//ZLStream* eventStream = this;

	if ( this->mPlayback ) {
		if ( this->mRecorder ) {
			size_t cursor = this->ParseEvents ( *this->mRecorder, timestep );
			this->mRecorder->SetCursor ( cursor );
		}
	}
	else {

		this->mLastUpdate = ZLDeviceTime::GetTimeInSeconds ();
		this->mAutosuspended = false;

		// reset the input sensors
		this->ResetSensorState ();

		if ( !this->mDefer ) {
			
			// rewind the event queue
			this->Seek ( 0, SEEK_SET );
			
			size_t cursor = this->ParseEvents ( *this, timestep );
			
			// record the processed events
			this->Record ( cursor );
			
			// discard processed events
			this->DiscardFront ( cursor );
			
			// back to the end of the queue
			this->SetCursor ( this->GetLength ());
		}
	}
}

//----------------------------------------------------------------//
bool MOAIInputMgr::WriteEventHeader ( u8 deviceID, u8 sensorID, u32 type ) {

	if ( this->CanWrite () && this->CheckSensor ( deviceID, sensorID, type )) {

		this->Write < u8 >(( u8 )deviceID );
		this->Write < u8 >(( u8 )sensorID );
		
		double timestamp = this->mAutoTimestamp ? ZLDeviceTime::GetTimeInSeconds () : this->mTimestamp;
		
		this->Write < double >( timestamp - this->mTimebase );
		
		return true;
	}
	return false;
}
