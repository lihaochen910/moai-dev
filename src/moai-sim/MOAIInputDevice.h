// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIINPUTDEVICE_H
#define MOAIINPUTDEVICE_H

class MOAISensor;

//================================================================//
// MOAIInputDevice
//================================================================//
/**	@lua	MOAIInputDevice
	@text	Manager class for input bindings. Has no public methods.
*/
class MOAIInputDevice :
	public virtual MOAIRubyObject {
private:

	STLString	mName;
	STLString	mHardwareInfo;
	bool		mIsActive;

	ZLLeanArray < MOAISensor* > mSensors;

	MOAIRubyStrongRef			mSensorsHash;

	//----------------------------------------------------------------//
	static mrb_value		_getHardwareInfo		( mrb_state* M, mrb_value context );
	static mrb_value		_getSensors				( mrb_state* M, mrb_value context );
	static mrb_value		_methodMissing			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	MOAISensor*		GetSensor				( u8 sensorID );
	void			SetSensor				( u8 sensorID, cc8* name, MOAISensor* sensor );

public:

	friend class MOAIInputContext;
	friend class MOAIInputMgr;

	DECL_RUBY_FACTORY ( MOAIInputDevice, MOAIRubyObject )

	GET_SET ( STLString, Name, mName );
	GET_SET ( bool, Active, mIsActive );

	//----------------------------------------------------------------//
	void			ClearSensorState		();
					MOAIInputDevice			();
					~MOAIInputDevice		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			ReserveSensors			( u8 total );
	void			ResetSensorState		();
	void			SetHardwareInfo			( cc8* hardwareInfo );
};

#endif
