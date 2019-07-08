// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIAnimCurveBase.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@ruby	getLength
	@text	Return the largest key frame time value in the curve.
	
	@in		MOAIAnimCurveBase self
	@out	number length
*/
mrb_value MOAIAnimCurveBase::_getLength ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAnimCurveBase, "U" );

	return state.ToRValue ( self->GetLength () );
}

//----------------------------------------------------------------//
/**	@ruby	reserveKeys
	@text	Reserve key frames.
	
	@in		MOAIAnimCurveBase self
	@in		number nKeys
	@out	nil
*/
mrb_value MOAIAnimCurveBase::_reserveKeys ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAnimCurveBase, "UN" );

	u32 total	= state.GetParamValue < u32 >( 1, 0 );

	self->ReserveKeys ( total );

	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIAnimCurveBase::_setTime ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAnimCurveBase, "UN" );

	self->mTime = state.GetParamValue < float >( 1, 0.0f );
	self->ScheduleUpdate ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@ruby	setWrapMode
	@text	Sets the wrap mode for values above 1.0 and below 0.0.
			CLAMP sets all values above and below 1.0 and 0.0 to
			values at 1.0 and 0.0 respectively
	        
	
	@in		MOAIAnimCurveBase self
	@opt	number mode			One of MOAIAnimCurveBase.CLAMP, MOAIAnimCurveBase.WRAP, MOAIAnimCurveBase.MIRROR,
								MOAIAnimCurveBase.APPEND. Default value is MOAIAnimCurveBase.CLAMP.

	@out	nil
*/
mrb_value MOAIAnimCurveBase::_setWrapMode	( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAnimCurveBase, "U" );

	u32 mode = state.GetParamValue < u32 >( 1, CLAMP );

	self->mWrapMode = mode;
	
	return context;
}

//================================================================//
// MOAIAnimCurveBase
//================================================================//

//----------------------------------------------------------------//
void MOAIAnimCurveBase::Clear () {

	this->mKeys.Clear ();
}

//----------------------------------------------------------------//
void MOAIAnimCurveBase::Draw ( u32 resolution ) const {
	UNUSED ( resolution );
}

//----------------------------------------------------------------//
u32 MOAIAnimCurveBase::FindKeyID ( float time ) const {
	
	MOAIAnimKey key;
	key.mTime = time;
	
	u32 index = USBinarySearchNearest < MOAIAnimKey >( this->mKeys.Data (), key, ( u32 )this->mKeys.Size ());
	
	return index;
}

//----------------------------------------------------------------//
void MOAIAnimCurveBase::GetDelta ( MOAIAttribute& attr, float t0, float t1 ) {
	
	if (( t0 == t1 ) || ( this->mKeys.Size () < 2 )) {
		this->GetZero ( attr );
	}
	else {

		MOAIAnimKeySpan s0 = this->GetSpan ( t0 );
		MOAIAnimKeySpan s1 = this->GetSpan ( t1 );
		
		this->GetDelta ( attr, s0, s1 );
	}
}

//----------------------------------------------------------------//
const MOAIAnimKey& MOAIAnimCurveBase::GetKey ( u32 id ) const {

	assert ( id < this->mKeys.Size ());
	return this->mKeys [ id ];
}

//----------------------------------------------------------------//
float MOAIAnimCurveBase::GetLength () const {

	u32 total = ( u32 )this->mKeys.Size ();
	if ( total == 0 ) return 0.0f;
	return this->mKeys [ total - 1 ].mTime - this->mKeys [ 0 ].mTime;
}

//----------------------------------------------------------------//
MOAIAnimKeySpan MOAIAnimCurveBase::GetSpan ( float time ) const {

	MOAIAnimKeySpan span;
	span.mKeyID = NULL_KEY_ID;
	span.mTime = 0.0f;
	span.mCycle = 0.0f;

	u32 total = ( u32 )this->mKeys.Size ();
	u32 endID = total - 1;
	assert ( total );
	
	float wrapTime = this->WrapTime ( time, span.mCycle );
	span.mKeyID = this->FindKeyID ( wrapTime );

	if ( span.mKeyID == endID ) {
		return span;
	}
	
	MOAIAnimKey k0 = this->mKeys [ span.mKeyID ];
	
	if ( k0.mMode == ZLInterpolate::kFlat ) {
		return span;
	}
	
	if ( k0.mTime == wrapTime ) {
		return span;
	}
	
	MOAIAnimKey k1 = this->mKeys [ span.mKeyID + 1 ];
	
	if ( k1.mTime > k0.mTime ) {
		span.mTime = ( wrapTime - k0.mTime ) / ( k1.mTime - k0.mTime );
	}
	return span;
}

//----------------------------------------------------------------//
void MOAIAnimCurveBase::GetValue ( MOAIAttribute& attr, float time ) {
	
	MOAIAnimKeySpan span = this->GetSpan ( time );
	this->GetValue ( attr, span );
}

//----------------------------------------------------------------//
MOAIAnimCurveBase::MOAIAnimCurveBase () :
	mTime ( 0.0f ),
	mWrapMode ( CLAMP ) {
	
	RTTI_SINGLE ( MOAINode )
}

//----------------------------------------------------------------//
MOAIAnimCurveBase::~MOAIAnimCurveBase () {

	this->Clear ();
}

//----------------------------------------------------------------//
void MOAIAnimCurveBase::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "ATTR_TIME", MOAIAnimCurveBaseAttr::Pack ( ATTR_TIME ) );
	state.DefineClassConst ( klass, "ATTR_VALUE", MOAIAnimCurveBaseAttr::Pack ( ATTR_VALUE ) );

	state.DefineClassConst ( klass, "CLAMP", ( u32 )CLAMP );
	state.DefineClassConst ( klass, "WRAP", ( u32 )WRAP );
	state.DefineClassConst ( klass, "MIRROR", ( u32 )MIRROR );
	state.DefineClassConst ( klass, "APPEND", ( u32 )APPEND );
}

//----------------------------------------------------------------//
void MOAIAnimCurveBase::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "getLength", _getLength, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "reserveKeys", _reserveKeys, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setTime", _setTime, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setWrapMode", _setWrapMode, MRB_ARGS_ARG ( 0, 1 ) );

}

//----------------------------------------------------------------//
void MOAIAnimCurveBase::ReserveKeys ( u32 total ) {

	this->mKeys.Init ( total );
	this->ReserveSamples ( total );
}

//----------------------------------------------------------------//
void MOAIAnimCurveBase::SetKey ( u32 id, float time, u32 mode, float weight ) {

	if ( id < this->mKeys.Size ()) {
		this->mKeys [ id ].mTime = time;
		this->mKeys [ id ].mMode = mode;
		this->mKeys [ id ].mWeight = weight;
	}
}

//----------------------------------------------------------------//
u32 MOAIAnimCurveBase::Size () const {

	return ( u32 )this->mKeys.Size ();
}

//----------------------------------------------------------------//
// return the time wrapped to fall within the time span defined by the curve
// in APPEND mode, 'repeat' gives the cycle (how many complete spans have elapsed)
float MOAIAnimCurveBase::WrapTime ( float t, float &repeat ) const {

	float startTime = this->mKeys [ 0 ].mTime;
	float length = GetLength ();

	float time = ( t - startTime ) / length; // normalize time
	float wrappedT = 0.0f;
	repeat = 0.0f;

	switch ( mWrapMode ) {
		case CLAMP: {
			wrappedT = ZLFloat::Clamp ( time, 0.0f, 1.0f );
		}
		break;

		case WRAP: {
			wrappedT = time - ZLFloat::Floor ( time );
		}
		break;

		case MIRROR: {
			u32 tFloor = ( u32 ) ZLFloat::Floor ( time );
			if ( tFloor % 2 ) {
				wrappedT = 1.0f - ( time - tFloor );
			}
			else {
				wrappedT = ( time - tFloor );
			}
		}
		break;

		case APPEND: {
			wrappedT = time - ZLFloat::Floor ( time );
			repeat = ZLFloat::Floor ( time );
		}
		break;
	}

	float result = wrappedT * length + startTime;
	
	if( wrappedT != 0.0f && wrappedT != 1.0f ) {
		if ( result + EPSILON > t && result - EPSILON < t ) { 
			result = t; 
		}
	}

	return result;
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAIAnimCurveBase::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	if ( MOAIAnimCurveBaseAttr::Check ( attrID )) {

		switch ( UNPACK_ATTR ( attrID )) {
			case ATTR_TIME:
				this->mTime = attr.Apply ( this->mTime, op, MOAIAttribute::ATTR_READ_WRITE );
				return true;
			case ATTR_VALUE:
				this->ApplyValueAttrOp ( attr, op );
				return true;
		}
	}
	return false;
}
