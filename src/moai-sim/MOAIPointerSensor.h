// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIPOINTERSENSOR_H
#define MOAIPOINTERSENSOR_H

#include <moai-sim/MOAISensor.h>

//================================================================//
// MOAIPointerSensor
//================================================================//
/**	@lua	MOAIPointerSensor
	@text	Pointer sensor.
*/
class MOAIPointerSensor :
	public MOAISensor {
private:

	int mX;
	int mY;

	MOAIRubyStrongRef		mOnMove;

	//----------------------------------------------------------------//
	static mrb_value		_getLoc					( mrb_state* M, mrb_value context );
	static mrb_value		_setCallback			( mrb_state* M, mrb_value context );

public:

	DECL_RUBY_FACTORY ( MOAIPointerSensor, MOAIRubyObject )

	//----------------------------------------------------------------//
	static void			EnqueuePointerEvent		( u8 deviceID, u8 sensorID, int x, int y );
						MOAIPointerSensor		();
						~MOAIPointerSensor		();
	void				ParseEvent				( ZLStream& eventStream );
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
