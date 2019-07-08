// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAITouchSensor.h>
#include <moai-sim/MOAIInputMgr.h>

const float MOAITouchSensor::DEFAULT_TAPTIME = 0.6f;
const float MOAITouchSensor::DEFAULT_TAPMARGIN = 50.0f;

//#define MOAI_TOUCH_SENSOR_DEBUG_LOG

#ifdef MOAI_TOUCH_SENSOR_DEBUG_LOG
	#define DEBUG_LOG(format,...) ZLLog ( format, ##__VA_ARGS__ )
#else
	#define DEBUG_LOG
#endif

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	countTouches
	@text	Return the number of currently active touches.

	@in		MOAITouchSensor self
	@out	number count
*/
mrb_value MOAITouchSensor::_countTouches ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITouchSensor, "U" )
	
	return state.ToRValue ( self->mTop );
}

//----------------------------------------------------------------//
/**	@lua	down
	@text	Checks to see if the screen was touched during the last iteration.

	@in		MOAITouchSensor self
	@opt	number idx				Index of touch to check.
	@out	boolean wasPressed
*/
mrb_value MOAITouchSensor::_down ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITouchSensor, "U" )
	
	u32 idx = state.GetParamValue < u32 >( 1, self->mActiveStack [ 0 ] );
	
	if ( idx < MAX_TOUCHES ) {
		return state.ToRValue ( ( self->mTouches [ idx ].mState & DOWN ) == DOWN );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	getActiveTouches
	@text	Returns the IDs of all of the touches currently occurring (for use with getTouch).

	@in		MOAITouchSensor self
	@out	number idx1
	@out	...
	@out	number idxN
*/
mrb_value MOAITouchSensor::_getActiveTouches ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITouchSensor, "U" )
	
	u32 count = self->mTop;
	mrb_value ary = mrb_ary_new ( state );
	
	for ( u32 i = 0; i < count; ++i ) {
		mrb_ary_push ( M, ary, state.ToRValue ( self->mActiveStack [ i ] ) );
	}
	return ary;
}

//----------------------------------------------------------------//
/**	@lua	getCenterLoc
	@text	Get the center coordinate of all currently active touches (as
			a simple average of all active touch coordinates).
	
			Returns nil if no current active touches.

	@in		MOAITouchSensor self
	@out	number x
	@out	number y
*/
mrb_value MOAITouchSensor::_getCenterLoc ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITouchSensor, "U" )
	
	u32 count = self->mTop;
	if ( count == 0 ) return context;
	
	ZLVec2D loc ( 0.0f, 0.0f );
	
	for ( u32 i = 0; i < count; ++i ) {
		u32 touchID = self->mActiveStack [ i ];
		const MOAITouch& touch = self->mTouches [ touchID ];
		
		if ( i == 0 ) {
			loc.mX = touch.mX;
			loc.mY = touch.mY;
		}
		else {
			loc.mX += touch.mX;
			loc.mY += touch.mY;
		}
	}
	
	loc.Scale ( 1.0f / ( float )count );
	
	mrb_value argv [ 2 ];
	argv [ 0 ] = state.ToRValue ( loc.mX );
	argv [ 1 ] = state.ToRValue ( loc.mY );
	
	return mrb_ary_new_from_values ( M, 2, argv );
}

//----------------------------------------------------------------//
/**	@lua	getTouch
	@text	Checks to see if there are currently touches being made on the screen.

	@in		MOAITouchSensor self
	@in		number id				The ID of the touch.
	@out	number x
	@out	number y
	@out	number tapCount
*/
mrb_value MOAITouchSensor::_getTouch ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITouchSensor, "U" )
	
	u32 idx = state.GetParamValue < u32 >( 2, 0 );
	
	if ( idx < MAX_TOUCHES ) {
		
		MOAITouch& touch = self->mTouches [ idx ];
		
		mrb_value argv [ 3 ];
		argv [ 0 ] = state.ToRValue ( touch.mX );
		argv [ 1 ] = state.ToRValue ( touch.mY );
		argv [ 2 ] = state.ToRValue ( touch.mTapCount );

		return mrb_ary_new_from_values ( M, 3, argv );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	hasTouches
	@text	Checks to see if there are currently touches being made on the screen.

	@in		MOAITouchSensor self
	@out	boolean hasTouches
*/
mrb_value MOAITouchSensor::_hasTouches ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITouchSensor, "U" )

	return state.ToRValue ( self->mTop > 0 );
}

//----------------------------------------------------------------//
/**	@lua	isDown
	@text	Checks to see if the touch status is currently down.

	@in		MOAITouchSensor self
	@opt	number idx				Index of touch to check.
	@out	boolean isDown
*/
mrb_value MOAITouchSensor::_isDown ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITouchSensor, "U" )
	
	u32 idx = state.GetParamValue < u32 >( 2, self->mActiveStack [ 0 ] );
	
	if ( idx < MAX_TOUCHES ) {
		return state.ToRValue ( ( self->mTouches [ idx ].mState & IS_DOWN ) == IS_DOWN );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setAcceptCancel
	@text	Sets whether or not to accept cancel events ( these happen on iOS backgrounding ), default value is false
 
	@in		MOAITouchSensor self
	@in		boolean accept	true then touch cancel events will be sent 
	@out	nil
*/
mrb_value MOAITouchSensor::_setAcceptCancel ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITouchSensor, "UB" )
	
	self->mAcceptCancel = state.GetParamValue < bool >( 1, self->mAcceptCancel );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setCallback
	@text	Sets or clears the callback to be issued when the pointer location changes.

	@in		MOAITouchSensor self
	@opt	function callback		Default value is nil.
	@out	nil
*/
mrb_value MOAITouchSensor::_setCallback ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITouchSensor, "U" )
	
	self->mCallback.SetRef ( state.GetParamValue ( 1 ) );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setTapMargin
	@text	Sets maximum distance between two touches for them to be considered a tap
	 
	@in		MOAITouchSensor self
	@in		number margin			Max difference on x and y between taps
	@out	nil
 */
mrb_value MOAITouchSensor::_setTapMargin ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITouchSensor, "UN" )
	
	float tapMargin = state.GetParamValue < float >( 1, DEFAULT_TAPMARGIN );
	self->mTapMargin = tapMargin;
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setTapTime
	@text	Sets the time between each touch for it to be counted as a tap
	 
	@in		MOAITouchSensor self
	@in		number time				New time between taps
	@out	nil
*/
mrb_value MOAITouchSensor::_setTapTime ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITouchSensor, "UN" )
	
	float tapTime = state.GetParamValue < float >( 1, DEFAULT_TAPTIME );
	self->mTapTime = tapTime;
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	up
	@text	Checks to see if the screen was untouched (is no longer being touched) during the last iteration.

	@in		MOAITouchSensor self
	@opt	number idx				Index of touch to check.
	@out	boolean wasPressed
*/
mrb_value MOAITouchSensor::_up ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITouchSensor, "U" )
	
	u32 idx = state.GetParamValue < u32 >( 1, self->mActiveStack [ 0 ] );
	
	if ( idx < MAX_TOUCHES ) {
		return state.ToRValue ( ( self->mTouches [ idx ].mState & UP ) == UP );
	}
	return context;
}

//================================================================//
// MOAITouchSensor
//================================================================//

//----------------------------------------------------------------//
void MOAITouchSensor::EnqueueTouchEvent ( u8 deviceID, u8 sensorID, u32 touchID, bool down, float x, float y ) {

	MOAIInputMgr& inputMgr = MOAIInputMgr::Get ();
	if ( inputMgr.WriteEventHeader < MOAITouchSensor >( deviceID, sensorID )) {
	
		float time = ( float )ZLDeviceTime::GetTimeInSeconds ();
		
		u32 eventType = down ? TOUCH_DOWN : TOUCH_UP;

		inputMgr.Write < u32 >( eventType );
		inputMgr.Write < u32 >( touchID );
		inputMgr.Write < float >( x );
		inputMgr.Write < float >( y );
		inputMgr.Write < float >( time );
	}
}

//----------------------------------------------------------------//
void MOAITouchSensor::EnqueueTouchEventCancel ( u8 deviceID, u8 sensorID ) {

	DEBUG_LOG ( "ENQUEUE TOUCHES CANCELLED\n" );

	MOAIInputMgr& inputMgr = MOAIInputMgr::Get ();
	if ( inputMgr.WriteEventHeader < MOAITouchSensor >( deviceID, sensorID )) {
		inputMgr.Write < u32 >( TOUCH_CANCEL );
	}
}

//----------------------------------------------------------------//
void MOAITouchSensor::AddLingerTouch ( MOAITouchLinger& touch ) {

	if ( this->mLingerTop < MAX_TOUCHES ) {
		this->mLingerTouches [ this->mLingerTop ] = touch;
		++this->mLingerTop;
	}
}

//----------------------------------------------------------------//
u32 MOAITouchSensor::AddTouch () {

	u32 idx = UNKNOWN_TOUCH;

	if ( this->mTop < MAX_TOUCHES ) {
		idx = this->mAllocStack [ this->mTop ];
		this->mActiveStack [ this->mTop ] = idx;
		this->mTop++;
	}
	return idx;
}

//----------------------------------------------------------------//
s32 MOAITouchSensor::CheckLingerList ( float x, float y, float time ) {

	u32 top = this->mLingerTop;
	float margin = this->mTapMargin;
	float testTime = time - this->mTapTime;
	
	s32 maxTapCount = 0;
	for ( u32 i = 0; i < top; ++i ) {
		if ( this->mLingerTouches [ i ].mX > ( x - margin ) &&
			 this->mLingerTouches [ i ].mX < ( x + margin ) &&
			 this->mLingerTouches [ i ].mY > ( y - margin ) &&
			 this->mLingerTouches [ i ].mY < ( y + margin ) &&
			 this->mLingerTouches [ i ].mTime > testTime &&
			 maxTapCount < this->mLingerTouches [ i ].mTapCount ) {

			maxTapCount = this->mLingerTouches [ i ].mTapCount;
		}
	}
	
	return maxTapCount;
}

//----------------------------------------------------------------//
void MOAITouchSensor::ClearState () {

	this->mTop = 0;
	this->mLingerTop = 0;

	for ( u32 i = 0; i < MAX_TOUCHES; ++i ) {
		this->mTouches [ i ].mState = 0;
		this->mAllocStack [ i ] = i;
		this->mActiveStack [ i ] = MAX_TOUCHES;
	}
}

//----------------------------------------------------------------//
u32 MOAITouchSensor::FindTouch ( u32 touchID ) {

	for ( u32 i = 0; i < this->mTop; ++i ) {
		u32 idx = this->mActiveStack [ i ];
		if ( this->mTouches [ idx ].mTouchID == touchID ) {
			return idx;
		}
	}
	return UNKNOWN_TOUCH;
}

//----------------------------------------------------------------//
MOAITouchSensor::MOAITouchSensor () {

	RTTI_SINGLE ( MOAISensor )
	
	mTapTime = DEFAULT_TAPTIME;
	mTapMargin = DEFAULT_TAPMARGIN;
	
	mAcceptCancel = false;
	
	this->ClearState ();
}

//----------------------------------------------------------------//
MOAITouchSensor::~MOAITouchSensor () {
}

//----------------------------------------------------------------//
void MOAITouchSensor::ParseEvent ( ZLStream& eventStream ) {

	u32 eventType = eventStream.Read < u32 >( 0 );
	
	// we only get TOUCH_DOWN, TOUCH_UP and TOUCH_CANCEL events
	// so we have to infer TOUCH_MOVE

	if ( eventType == TOUCH_CANCEL ) {
		
		DEBUG_LOG ( "TOUCHES CANCELLED\n" );
		
		// for now, TOUCH_CANCEL clobbers all touches
		this->ClearState ();
		
		if ( this->mCallback && this->mAcceptCancel ) {
			MOAIRubyState& state = MOAIRubyRuntime::Get ().State ();

			mrb_value argv [ 1 ];
			argv [ 0 ] = state.ToRValue ( eventType );

			state.FuncCall ( this->mCallback, "call", 1, argv );
		}
	}
	else {
		
		MOAITouch touch;
		
		touch.mState		= 0;
		touch.mTouchID		= eventStream.Read < u32 >( 0 );
		touch.mX			= eventStream.Read < float >( 0.0f );
		touch.mY			= eventStream.Read < float >( 0.0f );
		touch.mTime			= eventStream.Read < float >( 0.0f );
		touch.mTapCount     = 0;
		
		// see if there's already a record for this touch event
		u32 idx = this->FindTouch ( touch.mTouchID );
		
		if ( eventType == TOUCH_DOWN ) { // TOUCH_DOWN or TOUCH_MOVE
			
			// if it's a new touch, this is really a TOUCH_DOWN
			if ( idx == UNKNOWN_TOUCH ) {
				
				DEBUG_LOG ( "TOUCH DOWN: " );
				
				// if we're maxed out on touches, this touch will be ignored until something
				// frees up. after that it will be counted as a TOUCH_DOWN
				idx = this->AddTouch ();
				if ( idx == UNKNOWN_TOUCH ) {
					DEBUG_LOG ( "COUND NOT ADD TOUCH\n" );
					return;
				}
				
				DEBUG_LOG ( "%x->%d\n", touch.mTouchID, idx );
				
				// see if we touched down in a linger
				touch.mTapCount = CheckLingerList ( touch.mX, touch.mY, touch.mTime ) + 1;

				touch.mState = IS_DOWN | DOWN;
				
				this->mTouches [ idx ] = touch;
				
				#ifdef MOAI_TOUCH_SENSOR_DEBUG_LOG
					this->PrintStacks ();
				#endif
			}
			else {
			
				// we already knew about this touch, so it's a move
				touch.mState = this->mTouches [ idx ].mState | IS_DOWN;
				touch.mTapCount = this->mTouches [ idx ].mTapCount;
				eventType = TOUCH_MOVE;
			}
		}
		else if ( idx != UNKNOWN_TOUCH ) {
			
			DEBUG_LOG ( "TOUCH UP: %x->%d\n", touch.mTouchID, idx );
			
			// we know about the touch and it's not a TOUCH_DOWN, so it must be a TOUCH_UP
			
			// create a little cloud of linger to remember where the touch lifted up
			MOAITouchLinger linger;
			linger.mX = this->mTouches [ idx ].mX;
			linger.mY = this->mTouches [ idx ].mY;
			linger.mTapCount = this->mTouches [ idx ].mTapCount;
			linger.mTime = this->mTouches [ idx ].mTime;

			this->AddLingerTouch ( linger );

			touch.mState &= ~IS_DOWN;
			touch.mState |= UP;
			touch.mTapCount = CheckLingerList ( touch.mX, touch.mY, touch.mTime );
			
			#ifdef MOAI_TOUCH_SENSOR_DEBUG_LOG
				this->PrintStacks ();
			#endif
		}
		
		if ( idx != UNKNOWN_TOUCH ) {
			
			// if we have a valid touch, invoke the callback
			
			this->mTouches [ idx ] = touch;
			
			if ( this->mCallback ) {
				
				MOAIRubyState& state = MOAIRubyRuntime::Get ().State ();

				mrb_value argv [ 5 ];
				argv [ 0 ] = state.ToRValue ( eventType );
				argv [ 0 ] = state.ToRValue ( idx );
				argv [ 0 ] = state.ToRValue ( touch.mX );
				argv [ 0 ] = state.ToRValue ( touch.mY );
				argv [ 0 ] = state.ToRValue ( touch.mTapCount );

				state.FuncCall ( this->mCallback, "call", 5, argv );
			}
		}
	}
}

//----------------------------------------------------------------//
void MOAITouchSensor::PrintStacks () {

	STLString stacks = "";

	stacks.write ( "[" );

	for ( u32 i = 0; i < MAX_TOUCHES; ++i ) {
	
		if ( i == this->mTop ) {
			stacks.write ( "|" );
		}
		else {
			stacks.write ( " " );
		}
	
		if ( i < this->mTop ) {
			stacks.write ( "-" );
		}
		else {
			stacks.write ( "%d", ( int )this->mAllocStack [ i ]);
		}
	}
	
	stacks.write ( " ] [" );
	
	for ( u32 i = 0; i < MAX_TOUCHES; ++i ) {
	
		if ( i == this->mTop ) {
			stacks.write ( "|" );
		}
		else {
			stacks.write ( " " );
		}
		
		if ( i < this->mTop ) {
			int touch = ( int )this->mActiveStack [ i ];
			stacks.write ( "%d(%x)", touch, this->mTouches [ touch ].mTouchID );
		}
		else {
			stacks.write ( "-" );
		}
	}
	
	stacks.write ( " ]\n" );
	
	ZLLogF ( ZLLog::CONSOLE, stacks.c_str ());
}

//----------------------------------------------------------------//
void MOAITouchSensor::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAISensor::RegisterRubyClass ( state, klass );

	state.DefineClassConst ( klass, "TOUCH_DOWN", ( u32 )TOUCH_DOWN );
	state.DefineClassConst ( klass, "TOUCH_MOVE", ( u32 )TOUCH_MOVE );
	state.DefineClassConst ( klass, "TOUCH_UP", ( u32 )TOUCH_UP );
	state.DefineClassConst ( klass, "TOUCH_CANCEL", ( u32 )TOUCH_CANCEL );
}

//----------------------------------------------------------------//
void MOAITouchSensor::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAISensor::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "countTouches", _countTouches, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "down", _down, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "getActiveTouches", _getActiveTouches, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getCenterLoc", _getCenterLoc, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getTouch", _getTouch, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "hasTouches", _hasTouches, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isDown", _isDown, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setAcceptCancel", _setAcceptCancel, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setCallback", _setCallback, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setTapMargin", _setTapMargin, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setTapTime", _setTapTime, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "up", _up, MRB_ARGS_ARG ( 0, 1 ) );

}

//----------------------------------------------------------------//
void MOAITouchSensor::ResetState () {

	u32 top = this->mTop;
	u32 j = 0;
	for ( u32 i = 0; i < top; ++i ) {
		
		u32 idx = this->mActiveStack [ i ];
		MOAITouch& touch = this->mTouches [ idx ];
		
		if (( touch.mState & IS_DOWN ) == 0 ) {

			touch.mState = 0;
			--this->mTop;
			this->mAllocStack [ this->mTop ] = idx;
		}
		else {
			touch.mState &= ~( DOWN | UP );
			this->mActiveStack [ j++ ] = this->mActiveStack [ i ];
		}
	}
	
	#ifdef MOAI_TOUCH_SENSOR_DEBUG_LOG
		if ( top != this->mTop ) {
			DEBUG_LOG ( "REMOVED TOUCH(ES)\n" );
			this->PrintStacks ();
		}
		assert ( j == this->mTop );
	#endif
	
	bool changed = true;
	float time = ( float ) ZLDeviceTime::GetTimeInSeconds () - mTapTime;

	while ( changed ) {
	
		changed = false;
		top = this->mLingerTop;

		for ( u32 i = 0; i < top; ++i ) {
			if ( this->mLingerTouches [ i ].mTime < time ) {

				this->mLingerTouches [ i ] = this->mLingerTouches [ top ];
				this->mLingerTop--;

				changed = true;
				break;
			}
		}
	}

	if ( this->mTop == 0 && this->mLingerTop == 0 ) {
		this->ClearState ();
	}
}
