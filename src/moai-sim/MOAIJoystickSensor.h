// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIJOYSTICKSENSOR_H
#define MOAIJOYSTICKSENSOR_H

#include <moai-sim/MOAISensor.h>

//================================================================//
// MOAIJoystickSensor
//================================================================//
/**	@lua	MOAIJoystickSensor
	@text	Analog and digital joystick sensor.
*/
class MOAIJoystickSensor :
	public MOAISensor {
private:

	float mX;
	float mY;
	
	MOAIRubyStrongRef	mOnStick;

	//----------------------------------------------------------------//
	static mrb_value		_getVector				( mrb_state* M, mrb_value context );
	static mrb_value		_setCallback			( mrb_state* M, mrb_value context );

public:

	DECL_RUBY_FACTORY ( MOAIJoystickSensor, MOAIRubyObject )

	//----------------------------------------------------------------//
	static void			EnqueueJoystickEvent	( u8 deviceID, u8 sensorID, float x, float y );
						MOAIJoystickSensor		();
						~MOAIJoystickSensor		();
	void				ParseEvent				( ZLStream& eventStream );
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
