// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAICOMPASSSENSOR_H
#define MOAICOMPASSSENSOR_H

#include <moai-sim/MOAISensor.h>

//================================================================//
// MOAICompassSensor
//================================================================//
/**	@lua	MOAICompassSensor
	@text	Device heading sensor.
*/
class MOAICompassSensor :
	public MOAISensor {
private:

	float mHeading;

	MOAIRubyStrongRef mCallback;

	//----------------------------------------------------------------//
	static mrb_value	_getHeading		( mrb_state* M, mrb_value context );
	static mrb_value	_setCallback	( mrb_state* M, mrb_value context );

public:

	DECL_RUBY_FACTORY ( MOAICompassSensor, MOAIRubyObject )

	//----------------------------------------------------------------//
	static void			EnqueueCompassEvent		( u8 deviceID, u8 sensorID, float heading );
						MOAICompassSensor		();
						~MOAICompassSensor		();
	void				ParseEvent				( ZLStream& eventStream );
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
