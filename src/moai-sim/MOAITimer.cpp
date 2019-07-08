// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIAnimCurve.h>
#include <moai-sim/MOAISim.h>
#include <moai-sim/MOAITimer.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getSpeed
	@text	Return the playback speed.

	@in		MOAITimer self
	@out	number speed
*/
mrb_value MOAITimer::_getSpeed ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITimer, "U" )

	return state.ToRValue ( self->mSpeed );
}

//----------------------------------------------------------------//
/**	@lua	getTime
	@text	Return the current time.

	@in		MOAITimer self
	@out	number time
*/
mrb_value MOAITimer::_getTime( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITimer, "U" )

	return state.ToRValue ( self->GetTime () );
}

//----------------------------------------------------------------//
/**	@lua	getTimesExecuted
	@text	Gets the number of times the timer has completed a cycle.

	@in		MOAITimer self
	@out	number nTimes
*/
mrb_value MOAITimer::_getTimesExecuted ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITimer, "U" )

	return state.ToRValue ( self->mTimesExecuted );
}

//----------------------------------------------------------------//
/**	@lua	setCurve
	@text	Set or clear the curve to use for event generation.
	
	@in		MOAITimer self
	@opt	MOAIAnimCurve curve		Default value is nil.
	@out	nil
*/
mrb_value MOAITimer::_setCurve ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITimer, "U" );

	self->mCurve.Set ( *self, state.GetRubyObject < MOAIAnimCurve >( 1, true ));
	self->ScheduleUpdate ();

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setMode
	@text	Sets the playback mode of the timer.

	@in		MOAITimer self
	@in		number mode		One of: MOAITimer.NORMAL, MOAITimer.REVERSE, MOAITimer.LOOP, MOAITimer.LOOP_REVERSE, MOAITimer.PING_PONG
	@out	nil
*/
mrb_value MOAITimer::_setMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITimer, "UN" )

	self->mMode = state.GetParamValue < int >( 1, NORMAL );
	
	if( self->mMode == REVERSE || self->mMode == LOOP_REVERSE || self->mMode == CONTINUE_REVERSE ){
		self->mDirection = -1.0f;
	}
	else {
		self->mDirection = 1.0f;
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setSpan
	@text	Sets the playback mode of the timer.

	@overload	Span will be 0 to endTime.

		@in		MOAITimer self
		@in		number endTime
		@out	nil
	
	@overload	Span will be startTime to endTime.
	
		@in		MOAITimer self
		@in		number startTime
		@in		number endTime
		@out	nil
*/
mrb_value MOAITimer::_setSpan ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITimer, "UN" )

	if ( state.ParamIsType ( 2, MRB_TT_FLOAT ) ) {
		
		float startTime		= state.GetParamValue < float >( 1, 0.0f );
		float endTime		= state.GetParamValue < float >( 2, 1.0f );
		
		self->SetSpan ( startTime, endTime );
	}
	else {
		float span			= state.GetParamValue < float >( 1, 1.0f );
		
		self->SetSpan ( span );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setSpeed
	@text	Sets the playback speed. This affects only the timer, not
			its children in the action tree.

	@in		MOAITimer self
	@in		number speed
	@out	nil
*/
mrb_value MOAITimer::_setSpeed ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITimer, "UN" )

	self->mSpeed = state.GetParamValue < float >( 1, 1.0f );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setTime
	@text	Manually set the current time. This will be wrapped
			into the current span.

	@in		MOAITimer self
	@opt	number time			Default value is 0.
	@out	nil
*/
mrb_value MOAITimer::_setTime ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITimer, "U" )
	
	float time = state.GetParamValue < float >( 1, 0.0f );
	self->SetTime ( time );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	toggleDirection
	@text	Reverses the current direction of the timer.

	@in		MOAITimer self
	@out	nil
*/
mrb_value MOAITimer::_toggleDirection ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITimer, "U" )
	
	self->ToggleDirection ();
	return context;
}

//================================================================//
// MOAITimer
//================================================================//

//----------------------------------------------------------------//
void MOAITimer::DoStep ( float step ) {

	if ( step == 0.0f ) return;

	float length = this->mEndTime - this->mStartTime;

	if ( length == 0.0f ) {
		this->Stop ();
		this->ScheduleUpdate ();
		return;
	}

	float t0 = this->mTime;
	this->mTime += step * this->mSpeed * this->mDirection;
	
	//float t1 = this->mTime;
	//float result = 0.0f;
	
	switch ( this->mMode ) {
	
		case NORMAL: {
			
			if ( this->mTime >= this->mEndTime ) {
			
				this->mTime = this->mEndTime;
				this->GenerateKeyframeCallbacks ( t0, this->mTime, true );
				this->mCycle = 1.0f;
				this->OnEndSpan ();
				this->Stop ();
			}
			else {
				this->GenerateKeyframeCallbacks ( t0, this->mTime, false );
			}
			//result = this->mTime - t0;
			break;
		}
		
		case REVERSE: {
		
			if ( this->mTime < this->mStartTime ) {
			
				this->mTime = this->mStartTime ;
				this->GenerateKeyframeCallbacks ( t0, this->mTime, true );
				this->mCycle = -1.0f;
				this->OnEndSpan ();
				this->Stop ();
			}
			else {
				this->GenerateKeyframeCallbacks ( t0, this->mTime, false );
			}
			//result = this->mTime - t0;
			break;
		}
		
		case CONTINUE:
		case LOOP: {
			
			if ( this->mTime >= this->mEndTime ) {
				
				this->GenerateKeyframeCallbacks ( t0, this->mEndTime, true );
				
				while ( this->mTime >= this->mEndTime ) {
				
					this->mTime -= length;

					if ( this->mMode == CONTINUE ) {
						this->mCycle += 1.0f;
					}

					this->OnEndSpan ();
					this->OnLoop ();
					this->OnBeginSpan ();
					
					float end = this->mTime < this->mEndTime ? this->mTime : this->mEndTime;
					this->GenerateKeyframeCallbacks ( this->mStartTime, end, end == this->mEndTime );
				}
			}
			else {
				this->GenerateKeyframeCallbacks ( t0, this->mTime, false );
			}
			//result = t1 - t0;
			break;
		}
		
		case CONTINUE_REVERSE:
		case LOOP_REVERSE: {
		
			if ( this->mTime <= this->mStartTime ) {
				
				this->GenerateKeyframeCallbacks ( t0, this->mStartTime, true );
				
				while ( this->mTime <= this->mStartTime ) {
				
					this->mTime += length;

					if ( this->mMode == CONTINUE_REVERSE ) {
						this->mCycle -= 1.0f;
					}

					this->OnEndSpan ();
					this->OnLoop ();
					this->OnBeginSpan ();
					
					float end = this->mTime > this->mStartTime ? this->mTime : this->mStartTime;
					this->GenerateKeyframeCallbacks ( this->mEndTime, end, end == this->mStartTime );
				}
			}
			else {
				this->GenerateKeyframeCallbacks ( t0, this->mTime, false );
			}
			//result = t1 - t0;
			break;
		}
		
		case PING_PONG: {
			
			if (( this->mTime < this->mStartTime ) || ( this->mTime >= this->mEndTime )) {
			
				while (( this->mTime < this->mStartTime ) || ( this->mTime >= this->mEndTime )) {
					
					if ( this->mTime < this->mStartTime ) {
						this->mTime = this->mStartTime + ( this->mStartTime - this->mTime );
						
						float end = this->mTime < this->mEndTime ? this->mTime : this->mEndTime;
						this->GenerateKeyframeCallbacks ( this->mStartTime, end, end == this->mEndTime );
					}
					else {
						this->mTime = this->mEndTime - ( this->mTime - this->mEndTime );
						
						float end = this->mTime > this->mStartTime ? this->mTime : this->mStartTime;
						this->GenerateKeyframeCallbacks ( this->mEndTime, end, end == this->mStartTime );
					}
					
					this->mDirection *= -1.0f;
					this->OnEndSpan ();
					this->OnLoop ();
					this->OnBeginSpan ();

					if ( this->mTime == this->mEndTime ) {

						break;
					}
				}
			}
			else {
				this->GenerateKeyframeCallbacks ( t0, this->mTime, false );
			}
			//result = this->mTime - t0;
			break;
		}
	}
	
	this->ScheduleUpdate ();
	return;
}

//----------------------------------------------------------------//
void MOAITimer::GenerateKeyframeCallbacks ( float t0, float t1, bool end ) {

	if ( t0 == t1 ) return;
	u32 size = this->mCurve ? this->mCurve->Size () : 0;
	if ( !size ) return;
		
	u32 keyID = ( int )this->mCurve->FindKeyID ( t0 );
	if ( size <= keyID ) {
		keyID = 0;
	}
	
	if ( t0 < t1 ) {
	
		for ( ; keyID < size; ++keyID ) {
			const MOAIAnimKey& key = this->mCurve->GetKey ( keyID );
			
			if (( end && ( key.mTime >= t1 )) || (( key.mTime >= t0 ) && ( key.mTime < t1 ))) {
				this->OnKeyframe ( keyID, key.mTime, this->mCurve->GetSample ( keyID ));
			}
			
			if ( key.mTime >= t1 ) break;
		}
	}
	else {
		
		for ( ; ( int )keyID > -1; --keyID ) {
			const MOAIAnimKey& key = this->mCurve->GetKey ( keyID );
		
			if (( end && ( key.mTime <= t1 )) || (( key.mTime <= t0 ) && ( key.mTime > t1 ))) {
				this->OnKeyframe ( keyID, key.mTime, this->mCurve->GetSample ( keyID ));
			}
			
			if ( key.mTime <= t1 ) break;
		}
	}
}

//----------------------------------------------------------------//
float MOAITimer::GetCycle () {

	return this->mCycle;
}

//----------------------------------------------------------------//
float MOAITimer::GetLength () {

	return this->mEndTime - this->mStartTime;
}

//----------------------------------------------------------------//
float MOAITimer::GetNormalizedTime () {

	float length = this->mEndTime - this->mStartTime;
	return this->GetTime () / length;
}

//----------------------------------------------------------------//
float MOAITimer::GetTime () {

	if (( this->mMode == CONTINUE ) || ( this->mMode == CONTINUE_REVERSE )) {
		float length = this->mEndTime - this->mStartTime;
		return this->mTime + ( length * this->mCycle );
	}
	return this->mTime;
}

//----------------------------------------------------------------//
MOAITimer::MOAITimer () :
	mTime ( 0.0f ),
	mCycle ( 0.0f ),
	mSpeed ( 1.0f ),
	mDirection ( 1.0f ),
	mMode ( NORMAL ),
	mTimesExecuted ( 0.0f ),
	mStartTime ( 0.0f ),
	mEndTime ( 1.0f ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAINode )
		RTTI_EXTEND ( MOAIAction )
	RTTI_END
}

//----------------------------------------------------------------//
MOAITimer::~MOAITimer () {

	this->mCurve.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAITimer::OnBeginSpan () {
	
	MOAIRubyState state = MOAIRubyRuntime::Get ().State ();

	mrb_value args [ 1 ];
	args [ 0 ] = state.ToRValue ( this->mTimesExecuted );

	MOAIInstanceEventSource::InvokeListener ( EVENT_TIMER_BEGIN_SPAN, 1, args );
}

//----------------------------------------------------------------//
void MOAITimer::OnEndSpan () {
	
	MOAIRubyState state = MOAIRubyRuntime::Get ().State ();

	mrb_value args [ 1 ];
	args [ 0 ] = state.ToRValue ( this->mTimesExecuted );

	MOAIInstanceEventSource::InvokeListener ( EVENT_TIMER_END_SPAN, 1, args );
	
	this->mTimesExecuted += 1.0f;
}

//----------------------------------------------------------------//
void MOAITimer::OnKeyframe ( u32 idx, float time, float value ) {

	MOAIRubyState state = MOAIRubyRuntime::Get ().State ();

	mrb_value args [ 4 ];
	args [ 0 ] = state.ToRValue ( idx + 1 );
	args [ 1 ] = state.ToRValue ( mTimesExecuted );
	args [ 2 ] = state.ToRValue ( time );
	args [ 3 ] = state.ToRValue ( value );

	MOAIInstanceEventSource::InvokeListener ( EVENT_TIMER_LOOP, 4, args );
}

//----------------------------------------------------------------//
void MOAITimer::OnLoop () {
	
	MOAIRubyState state = MOAIRubyRuntime::Get ().State ();

	MOAIInstanceEventSource::InvokeListener ( EVENT_TIMER_LOOP );
}

//----------------------------------------------------------------//
void MOAITimer::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "ATTR_TIME",				MOAITimerAttr::Pack ( ATTR_TIME ) );
	
	state.DefineClassConst ( klass, "EVENT_TIMER_KEYFRAME",		( u32 )EVENT_TIMER_KEYFRAME );
	state.DefineClassConst ( klass, "EVENT_TIMER_LOOP",			( u32 )EVENT_TIMER_LOOP );
	state.DefineClassConst ( klass, "EVENT_TIMER_BEGIN_SPAN",	( u32 )EVENT_TIMER_BEGIN_SPAN );
	state.DefineClassConst ( klass, "EVENT_TIMER_END_SPAN",		( u32 )EVENT_TIMER_END_SPAN );
	
	state.DefineClassConst ( klass, "NORMAL",					( u32 )NORMAL );
	state.DefineClassConst ( klass, "REVERSE",					( u32 )REVERSE );
	state.DefineClassConst ( klass, "CONTINUE",					( u32 )CONTINUE );
	state.DefineClassConst ( klass, "CONTINUE_REVERSE",			( u32 )CONTINUE_REVERSE );
	state.DefineClassConst ( klass, "LOOP",						( u32 )LOOP );
	state.DefineClassConst ( klass, "LOOP_REVERSE",				( u32 )LOOP_REVERSE );
	state.DefineClassConst ( klass, "PING_PONG",				( u32 )PING_PONG );

}

//----------------------------------------------------------------//
void MOAITimer::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAINode::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getSpeed", _getSpeed, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getTime", _getTime, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getTimesExecuted", _getTimesExecuted, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setCurve", _setCurve, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setMode", _setMode, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setSpan", _setSpan, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "setSpeed", _setSpeed, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setTime", _setTime, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "toggleDirection", _toggleDirection, MRB_ARGS_NONE () );

}

//----------------------------------------------------------------//
void MOAITimer::SetSpan ( float span ) {

	this->mStartTime = 0.0f;
	this->mEndTime = span;
}

//----------------------------------------------------------------//
void MOAITimer::SetSpan ( float startTime, float endTime ) {

	this->mStartTime = startTime;
	this->mEndTime = endTime;
}

//----------------------------------------------------------------//
void MOAITimer::SetTime ( float time ) {

	float length = ZLFloat::Abs ( this->mEndTime - this->mStartTime );

	float transformedTime = ( time - this->mStartTime ) / length;

	switch ( this->mMode ) {
		case NORMAL: 
		case REVERSE: {
			time = ZLFloat::Clamp ( time, this->mStartTime, this->mEndTime );
			this->mTime = time;
			break;
		}
		case CONTINUE:
		case CONTINUE_REVERSE: {
			float wrappedT = transformedTime - ZLFloat::Floor ( transformedTime );
			this->mTime = wrappedT * length + this->mStartTime;
			this->mCycle = ZLFloat::Floor ( transformedTime );
			break;
		}
		case LOOP:
		case LOOP_REVERSE: {
			float wrappedT = transformedTime - ZLFloat::Floor ( transformedTime );
			this->mTime = wrappedT * length + this->mStartTime;
			this->mCycle = 0.0f;
			break;
		}
		case PING_PONG: {
			float wrappedT = transformedTime - ZLFloat::Floor ( transformedTime );
			this->mTime = wrappedT * length + this->mStartTime;
			if ( ( u32 ) ZLFloat::Floor ( transformedTime ) % 2 ) { //switch direction if odd
				this->mDirection = -1.0f;
			}
			else {
				this->mDirection = 1.0f;
			}
			this->mCycle = 0.0f;
			break;
		}
	}

	if ( this->mTime + EPSILON > time && this->mTime - EPSILON < time ) { 
		this->mTime = time; 
	}

	this->ScheduleUpdate ();
}

//----------------------------------------------------------------//
void MOAITimer::ToggleDirection () {

	switch ( this->mMode ) {
		
		case NORMAL:
			this->mMode = REVERSE;
			this->mDirection = -1.0f;
			break;
		
		case REVERSE:
			this->mMode = NORMAL;
			this->mDirection = 1.0f;
			break;
		
		case CONTINUE:
			this->mMode = CONTINUE_REVERSE;
			this->mDirection = -1.0f;
			break;
		
		case CONTINUE_REVERSE:
			this->mMode = CONTINUE;
			this->mDirection = 1.0f;
			break;
		
		case LOOP:
			this->mMode = LOOP_REVERSE;
			this->mDirection = -1.0f;
			break;
		
		case LOOP_REVERSE:
			this->mMode = LOOP;
			this->mDirection = 1.0f;
			break;
			
		case PING_PONG:
			this->mDirection = -1.0f;
			break;
	}
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAITimer::MOAIAction_IsDone () {

	if ( this->mMode == NORMAL ) {
		return (( this->mTime < this->mStartTime ) || ( this->mTime >= this->mEndTime ));
	}
	
	if ( this->mMode == REVERSE ) {
		return (( this->mTime <= this->mStartTime ) || ( this->mTime > this->mEndTime ));
	}
	return false;
}

//----------------------------------------------------------------//
void MOAITimer::MOAIAction_Start () {
	MOAIAction::MOAIAction_Start ();

	if( this->mDirection > 0.0f ) {
		this->mTime = this->mStartTime;
	}
	else {
		this->mTime = this->mEndTime;
	}
	this->mCycle = 0.0f;
	this->mTimesExecuted = 0.0f;
	
	this->OnBeginSpan ();
}

//----------------------------------------------------------------//
void MOAITimer::MOAIAction_Update ( double step ) {

	this->DoStep (( float )step ); // TODO: change everything to doubles
}

//----------------------------------------------------------------//
bool MOAITimer::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	if ( MOAITimerAttr::Check ( attrID )) {
		attrID = UNPACK_ATTR ( attrID );
		
		if ( attrID == ATTR_TIME ) {
			attr.Apply ( this->GetTime (), op, MOAIAttribute::ATTR_READ );
			return true;
		}
	}
	return false;
}
