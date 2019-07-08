// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAITOUCHSENSOR_H
#define MOAITOUCHSENSOR_H

#include <moai-sim/MOAISensor.h>

//================================================================//
// MOAITouch
//================================================================//
class MOAITouch {
private:

	friend class MOAITouchSensor;
	
	u32		mState;
	
	float	mTime;
	
	float	mX;
	float	mY;
	
	u32		mTouchID;
	u32		mTapCount;
};

//================================================================//
// MOAITouch
//================================================================//
class MOAITouchLinger {
private:

	friend class MOAITouchSensor;
	
	float	mTime;
	
	float	mX;
	float	mY;
	
	s32		mTapCount;
};

//================================================================//
// MOAITouchSensor
//================================================================//
/**	@lua	MOAITouchSensor
	@text	Multitouch sensor. Tracks up to 16 simultaneous touches.

	@const	TOUCH_DOWN
	@const	TOUCH_MOVE
	@const	TOUCH_UP
	@const	TOUCH_CANCEL
*/
class MOAITouchSensor :
	public MOAISensor {
private:

	enum {
		IS_DOWN		= 1 << 0x00,
		DOWN		= 1 << 0x01,
		UP			= 1 << 0x02,
	};

	static const u32 MAX_TOUCHES		= 16;
	static const u32 UNKNOWN_TOUCH		= 0xffffffff;
		
	static const float DEFAULT_TAPTIME;
	static const float DEFAULT_TAPMARGIN;

	bool				mAcceptCancel;
		
	u32					mActiveStack [ MAX_TOUCHES ];
	u32					mAllocStack [ MAX_TOUCHES ];
		
	MOAIRubyStrongRef	mCallback;
		
	u32					mLingerTop;
	MOAITouchLinger		mLingerTouches [ MAX_TOUCHES ];
		
	MOAITouch			mTouches [ MAX_TOUCHES ];

	u32					mTop;
		
	float				mTapMargin;
	float				mTapTime;

	//----------------------------------------------------------------//
	static mrb_value		_countTouches			( mrb_state* M, mrb_value context );
	static mrb_value		_down					( mrb_state* M, mrb_value context );
	static mrb_value		_getActiveTouches		( mrb_state* M, mrb_value context );
	static mrb_value		_getCenterLoc			( mrb_state* M, mrb_value context );
	static mrb_value		_getTouch				( mrb_state* M, mrb_value context );
	static mrb_value		_hasTouches				( mrb_state* M, mrb_value context );
	static mrb_value		_isDown					( mrb_state* M, mrb_value context );
	static mrb_value		_setAcceptCancel		( mrb_state* M, mrb_value context );
	static mrb_value		_setCallback			( mrb_state* M, mrb_value context );
	static mrb_value		_setTapMargin			( mrb_state* M, mrb_value context ); 
	static mrb_value		_setTapTime				( mrb_state* M, mrb_value context );
	static mrb_value		_up						( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	u32				AddTouch				();
	void			AddLingerTouch			( MOAITouchLinger& touch );
	s32				CheckLingerList			( float x, float y, float time );
	u32				FindTouch				( u32 touchID );
	void			PrintStacks				();

public:

	enum {
		TOUCH_DOWN,
		TOUCH_MOVE,
		TOUCH_UP,
		TOUCH_CANCEL,
	};

	DECL_RUBY_FACTORY ( MOAITouchSensor, MOAIRubyObject )

	//----------------------------------------------------------------//
	void				ClearState					();
	static void			EnqueueTouchEvent			( u8 deviceID, u8 sensorID, u32 touchID, bool down, float x, float y );
	static void			EnqueueTouchEventCancel		( u8 deviceID, u8 sensorID );
						MOAITouchSensor				();
						~MOAITouchSensor			();
	void				ParseEvent					( ZLStream& eventStream );
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void				ResetState					();
};

#endif
