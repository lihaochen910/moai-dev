// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIWHEELSENSOR_H
#define MOAIWHEELSENSOR_H

#include <moai-sim/MOAISensor.h>

//================================================================//
// MOAIWheelSensor
//================================================================//
/**	@lua	MOAIWheelSensor
	@text	Hardware wheel sensor.
*/
class MOAIWheelSensor :
	public MOAISensor {
private:

	float mValue;
	float mDelta;

	MOAIRubyStrongRef mCallback;

	//----------------------------------------------------------------//
	static mrb_value	_getValue		( mrb_state* M, mrb_value context );
	static mrb_value	_getDelta		( mrb_state* M, mrb_value context );
	static mrb_value	_setCallback	( mrb_state* M, mrb_value context );

public:

	DECL_RUBY_FACTORY ( MOAIWheelSensor, MOAIRubyObject )

	//----------------------------------------------------------------//
	static void			EnqueueWheelEvent		( u8 deviceID, u8 sensorID, float value );
						MOAIWheelSensor			();
						~MOAIWheelSensor		();
	void				ParseEvent				( ZLStream& eventStream );
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void				ResetState				();
};

#endif
