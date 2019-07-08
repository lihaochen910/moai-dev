// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAILOCATIONSENSOR_H
#define MOAILOCATIONSENSOR_H

#include <moai-sim/MOAISensor.h>

//================================================================//
// MOAILocationSensor
//================================================================//
/**	@lua	MOAILocationSensor
	@text	Location services sensor.
*/
class MOAILocationSensor :
	public MOAISensor {
private:

	double mLongitude;
	double mLatitude;
	double mAltitude;
	
	float mHAccuracy;
	float mVAccuracy;

	float mSpeed;

	MOAIRubyStrongRef mCallback;

	//----------------------------------------------------------------//
	static mrb_value	_getLocation	( mrb_state* M, mrb_value context );
	static mrb_value	_setCallback	( mrb_state* M, mrb_value context );

public:

	DECL_RUBY_FACTORY ( MOAILocationSensor, MOAIRubyObject )

	//----------------------------------------------------------------//
	static void			EnqueueLocationEvent	( u8 deviceID, u8 sensorID, double longitude, double latitude, double altitude, float hAccuracy, float vAccuracy, float speed );
						MOAILocationSensor		();
						~MOAILocationSensor		();
	void				ParseEvent				( ZLStream& eventStream );
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
