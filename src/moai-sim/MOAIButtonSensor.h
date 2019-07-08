// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIBUTTONSETSENSOR_H
#define MOAIBUTTONSETSENSOR_H

#include <moai-sim/MOAISensor.h>

//================================================================//
// MOAIButtonSensor
//================================================================//
/**	@lua	MOAIButtonSensor
	@text	Button sensor.
*/
class MOAIButtonSensor :
	public MOAISensor {
private:

	enum {
		IS_DOWN		= 1 << 0x00,
		DOWN		= 1 << 0x01,
		UP			= 1 << 0x02,
	};

	u32	mState;

	MOAIRubyStrongRef	mOnButton;

	//----------------------------------------------------------------//
	static mrb_value		_down				( mrb_state* M, mrb_value context );
	static mrb_value		_isDown				( mrb_state* M, mrb_value context );
	static mrb_value		_isUp				( mrb_state* M, mrb_value context );
	static mrb_value		_up					( mrb_state* M, mrb_value context );
	static mrb_value		_setCallback		( mrb_state* M, mrb_value context );

public:

	DECL_RUBY_FACTORY ( MOAIButtonSensor, MOAIRubyObject )

	//----------------------------------------------------------------//
	bool				ButtonDown				();
	bool				ButtonIsDown			();
	bool				ButtonIsUp				();
	bool				ButtonUp				();
	void				ClearState				();
	static void			EnqueueButtonEvent		( u8 deviceID, u8 sensorID, bool down );
						MOAIButtonSensor		();
						~MOAIButtonSensor		();
	void				ParseEvent				( ZLStream& eventStream );
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void				ResetState				();
};

#endif
