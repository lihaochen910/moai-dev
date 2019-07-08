// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIKEYBOARDSENSOR_H
#define MOAIKEYBOARDSENSOR_H

#include <moai-sim/MOAISensor.h>
#include <moai-sim/MOAIKeyCode.h>
#include <moai-sim/MOAIKeyCodeEnum.h>

//================================================================//
// MOAIKeyboardSensor
//================================================================//
/**	@lua	MOAIKeyboardSensor
	@text	Hardware keyboard sensor.


*/
class MOAIKeyboardSensor :
	public MOAISensor {
private:

	enum {
		IS_DOWN		= 1 << 0x00,	// The key is currently pressed
		DOWN		= 1 << 0x01,	// The key was pressed during the last iteration
		UP			= 1 << 0x02,	// The key was released during the last iteration
	};

	u32 mState [ MOAI_KEY_TOTAL ];

	u32 mClearCount;
	u32 mClearQueue [ MOAI_KEY_TOTAL ];	// The keys whose DOWN or UP flags are set
	
	MOAIRubyStrongRef		mOnKey;
	MOAIRubyStrongRef		mOnChar;
	MOAIRubyStrongRef		mOnEdit;

	//----------------------------------------------------------------//
	static mrb_value		_keyDown				( mrb_state* M, mrb_value context );
	static mrb_value		_keyIsDown				( mrb_state* M, mrb_value context );
	static mrb_value		_keyIsUp				( mrb_state* M, mrb_value context );
	static mrb_value		_keyUp					( mrb_state* M, mrb_value context );
	static mrb_value		_setCallback			( mrb_state* M, mrb_value context );
	static mrb_value		_setCharCallback		( mrb_state* M, mrb_value context );
	static mrb_value		_setEditCallback		( mrb_state* M, mrb_value context );
	static mrb_value		_setKeyCallback			( mrb_state* M, mrb_value context );

public:

	DECL_RUBY_FACTORY ( MOAIKeyboardSensor, MOAIRubyObject )

	//----------------------------------------------------------------//
	void				ClearState					();
	static void			EnqueueKeyboardCharEvent	( u8 deviceID, u8 sensorID, u32 unicodeChar );
	static void			EnqueueKeyboardEditEvent	( u8 deviceID, u8 sensorID, char const* text, u32 start, u32 editLength, u32 maxLength );
	static void			EnqueueKeyboardKeyEvent		( u8 deviceID, u8 sensorID, u32 keyID, bool down );
	static void			EnqueueKeyboardTextEvent	( u8 deviceID, u8 sensorID, cc8* text );
	static mrb_value	CheckKeys					( mrb_state* M, mrb_value context, bool ( MOAIKeyboardSensor::*predicate )( u32 keyCode ));
	bool				KeyDown						( u32 keyID );
	bool				KeyIsDown					( u32 keyID );
	bool				KeyIsUp						( u32 keyID );
	bool				KeyUp						( u32 keyID );
						MOAIKeyboardSensor			();
						~MOAIKeyboardSensor			();
	void				ParseEvent					( ZLStream& eventStream );
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void				ResetState					();
};

#endif
