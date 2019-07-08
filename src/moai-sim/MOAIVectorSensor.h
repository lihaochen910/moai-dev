// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIVECTORSENSOR_H
#define MOAIVECTORSENSOR_H

#include <moai-sim/MOAISensor.h>

//================================================================//
// MOAIVectorSensor
//================================================================//
/**	@lua	MOAIVectorSensor
	@text	General purpose 3-tuple
*/
class MOAIVectorSensor :
	public MOAISensor {
private:

	float	mX;
	float	mY;
	float	mZ;

	MOAIRubyStrongRef mCallback;

	//----------------------------------------------------------------//
	static mrb_value		_getVector				( mrb_state* M, mrb_value context );
	static mrb_value		_setCallback			( mrb_state* M, mrb_value context );

public:

	DECL_RUBY_FACTORY ( MOAIVectorSensor, MOAIRubyObject )

	//----------------------------------------------------------------//
	static void			EnqueueVectorEvent		( u8 deviceID, u8 sensorID, float x, float y, float z );
						MOAIVectorSensor		();
						~MOAIVectorSensor		();
	void				ParseEvent				( ZLStream& eventStream );
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
