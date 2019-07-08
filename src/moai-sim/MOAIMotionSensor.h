// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIMOTIONSENSOR_H
#define MOAIMOTIONSENSOR_H

#include <moai-sim/MOAISensor.h>

//================================================================//
// MOAIMotionSensor
//================================================================//
/**	@lua	MOAIMotionSensor
	@text	Gravity/acceleration sensor.
*/
class MOAIMotionSensor :
	public MOAISensor {
private:

	float	mX;
	float	mY;
	float	mZ;

	MOAIRubyStrongRef mCallback;

	//----------------------------------------------------------------//
	static mrb_value	_getLevel		( mrb_state* M, mrb_value context );
	static mrb_value	_setCallback	( mrb_state* M, mrb_value context );

public:

	DECL_RUBY_FACTORY ( MOAIMotionSensor, MOAIRubyObject )

	//----------------------------------------------------------------//
	static void			EnqueueLevelEvent		( u8 deviceID, u8 sensorID, float x, float y, float z );
						MOAIMotionSensor		();
						~MOAIMotionSensor		();
	void				ParseEvent				( ZLStream& eventStream );
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
