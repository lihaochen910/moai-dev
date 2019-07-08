// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIINPUTMGR_H
#define MOAIINPUTMGR_H

#include <moai-sim/MOAIAction.h>
#include <moai-sim/MOAIInputDevice.h>
#include <moai-sim/MOAISensor.h>

//================================================================//
// MOAIInputMgr
//================================================================//
/**	@lua	MOAIInputMgr
	@text	Base class for input streams and device sets.
*/
class MOAIInputMgr :
	public ZLContextClass < MOAIInputMgr, MOAIRubyObject >,
	public virtual ZLMemStream {
private:

	static const size_t CHUNK_SIZE = 256;

	enum {
		INPUT_EVENT,
		FINISHED_UPDATE,
	};

	ZLLeanStack < MOAIInputDevice*, 8 > mDevices;

	double	mTimebase;				// used to position timestamps against sim timeline
	double	mTimestamp;				// timestamp for next event
	
	bool	mAutoTimestamp;
	
	bool	mDefer;
	bool	mSuspended;
	
	double	mAutosuspend;	// if > 0, use as a timeout to susped input
	bool	mAutosuspended;

	double	mLastUpdate;	// last update in *device* time

	MOAIRubySharedPtr < MOAIStream > mRecorder;
	bool mPlayback;
	
	MOAIRubyStrongRef	mEventCallback;
	MOAIRubyStrongRef	mDevicesHash;

	//----------------------------------------------------------------//
	static mrb_value			_autoTimestamp				( mrb_state* M, mrb_value context );
	static mrb_value			_deferEvents				( mrb_state* M, mrb_value context );
	static mrb_value			_discardEvents				( mrb_state* M, mrb_value context );
	static mrb_value			_getDevices					( mrb_state* M, mrb_value context );
	static mrb_value			_methodMissing				( mrb_state* M, mrb_value context );
	static mrb_value			_playback					( mrb_state* M, mrb_value context );
	static mrb_value			_setAutosuspend				( mrb_state* M, mrb_value context );
	static mrb_value			_setEventCallback			( mrb_state* M, mrb_value context );
	static mrb_value			_setRecorder				( mrb_state* M, mrb_value context );
	static mrb_value			_suspendEvents				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	bool				CanWrite					();
	bool				CheckSensor					( u8 deviceID, u8 sensorID, u32 type );
	void				InvokeCallback				( u32 event, double timestamp );
	size_t				ParseEvents					( ZLStream& stream, double timestep );
	void				Record						( size_t size );
	bool				WriteEventHeader			( u8 deviceID, u8 sensorID, u32 type );

public:

	DECL_RUBY_SINGLETON ( MOAIInputMgr )

	SET ( double, Timebase, mTimebase )
	SET ( double, Timestamp, mTimestamp )

	//----------------------------------------------------------------//
	u8					AddDevice					( cc8* name );
	void				ClearSensorState			();
	void				DeferEvents					( bool defer );
	void				FlushEvents					( double skip );
	MOAIInputDevice*	GetDevice					( u8 deviceID );
	MOAISensor*			GetSensor					( u8 deviceID, u8 sensorID );
	bool				HasEvents					();
	//bool				IsDone						();
						MOAIInputMgr				();
						~MOAIInputMgr				();
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void				ReserveDevices				( u8 total );
	void				ReserveSensors				( u8 deviceID, u8 total );
	void				ResetSensorState			();
	void				SetAutosuspend				( double autosuspend );
	void				SetAutotimestamp			( bool autotimestamp );
	void				SetConfigurationName		( cc8* name );
	void				SetDevice					( u8 deviceID, cc8* name ); // back compat
	void				SetDeviceActive				( u8 deviceID, bool active );
	void				SetDeviceHardwareInfo		( u8 deviceID, cc8* hardwareInfo );
	void				SuspendEvents				( bool suspend );
	void				Update						( double timestep );

	//----------------------------------------------------------------//
	template < typename TYPE >
	void SetSensor ( u8 deviceID, u8 sensorID, cc8* name ) {
	
		MOAIInputDevice* device = this->GetDevice ( deviceID );
		if ( device ) {
			if ( sensorID < device->mSensors.Size ()) {
				MOAISensor* sensor = MOAIRubyRuntime::Get ().GetMainState ().CreateClassInstance < TYPE >();
				sensor->SetType ( ZLTypeID < TYPE >::GetID ());
				device->SetSensor ( sensorID, name, sensor );
			}
		}
	}
	
	//----------------------------------------------------------------//
	template < typename TYPE >
	bool WriteEventHeader ( u8 deviceID, u8 sensorID ) {
		return this->WriteEventHeader ( deviceID, sensorID, ZLTypeID < TYPE >::GetID ());
	}
};

#endif
