// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIAnimCurveVec.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@ruby	getValueAtTime
	@text	Return the interpolated vector components given a point in
			time along the curve. This does not change the curve's built in TIME
			attribute (it simply performs the requisite computation on demand).
	
	@in		MOAIAnimCurveVec self
	@in		number time
	@out	number x
	@out	number y
	@out	number z
*/
mrb_value MOAIAnimCurveVec::_getValueAtTime ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAnimCurveVec, "UN" );

	float time = state.GetParamValue < float >( 1, 0 );
	
	MOAIAnimKeySpan span = self->GetSpan ( time );
	ZLVec3D value = self->GetValue ( span );

	mrb_value ret [ 4 ];
	ret [ 0 ] = state.ToRValue ( value.mX );
	ret [ 1 ] = state.ToRValue ( value.mY );
	ret [ 2 ] = state.ToRValue ( value.mZ );
	ret [ 3 ] = state.ToRValue ( span.mKeyID + 1 );

	return mrb_ary_new_from_values ( state, 4, ret );
}

//----------------------------------------------------------------//
/**	@ruby	setKey
	@text	Initialize a key frame at a given time with a give vector.
			Also set the transition type between the specified key frame
			and the next key frame.
	
	@in		MOAIAnimCurveVec self
	@in		number index			Index of the keyframe.
	@in		number time				Location of the key frame along the curve.
	@in		number x				X component at time.
	@in		number y				Y component at time.
	@in		number z				Z component at time.
	@opt	number mode				The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
									MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.
	@opt	number weight			Blends between chosen ease type (of any) and a linear transition. Defaults to 1.
	@out	nil
*/
mrb_value MOAIAnimCurveVec::_setKey ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAnimCurveVec, "UNNNN" );

	u32 index		= state.GetParamValue < u32 >( 1, 1 ) - 1;
	float time		= state.GetParamValue < float >( 2, 0.0f );
	ZLVec3D value	= state.GetVec3D < float >( 3 );
	u32 mode		= state.GetParamValue < u32 >( 6, ZLInterpolate::kSmooth );
	float weight	= state.GetParamValue < float >( 7, 1.0f );
	
	if ( MOAILogMgr::CheckIndexPlusOne ( index, self->mKeys.Size (), M )) {
		
		self->SetKey ( index, time, mode, weight );
		self->SetSample ( index, value );
	}
	return context;
}

//================================================================//
// MOAIAnimCurveVec
//================================================================//

//----------------------------------------------------------------//
void MOAIAnimCurveVec::ApplyValueAttrOp ( MOAIAttribute& attr, u32 op ) {

	this->mValue = attr.Apply ( this->mValue, op, MOAIAttribute::ATTR_READ_WRITE );
}

//----------------------------------------------------------------//
ZLVec3D MOAIAnimCurveVec::GetCurveDelta () const {

	ZLVec3D delta;

	u32 size = ( u32 )this->mKeys.Size ();
	if ( size > 1 ) {
		delta = this->mSamples [ size - 1 ];
		delta.Sub ( this->mSamples [ 0 ]);
	}
	else {
		delta.Init ( 0.0f, 0.0f, 0.0f );
	}
	return delta;
}

//----------------------------------------------------------------//
void MOAIAnimCurveVec::GetDelta ( MOAIAttribute& attr, const MOAIAnimKeySpan& span0, const MOAIAnimKeySpan& span1 ) const {

	ZLVec3D v0 = this->GetValue ( span0 );
	ZLVec3D v1 = this->GetValue ( span1 );
	
	v1.Sub ( v0 );
	
	attr.SetValue ( v1 );
}

//----------------------------------------------------------------//
ZLVec3D MOAIAnimCurveVec::GetValue ( float time ) const {

	MOAIAnimKeySpan span = this->GetSpan ( time );
	return this->GetValue ( span );
}

//----------------------------------------------------------------//
ZLVec3D MOAIAnimCurveVec::GetValue ( const MOAIAnimKeySpan& span ) const {

	MOAIAnimKey& key = this->mKeys [ span.mKeyID ];
	ZLVec3D v0 = this->mSamples [ span.mKeyID ];
	
	if ( span.mTime > 0.0f ) {
	
		ZLVec3D v1 = this->mSamples [ span.mKeyID + 1 ];
	
		v0.mX = ZLInterpolate::Interpolate ( key.mMode, v0.mX, v1.mX, span.mTime, key.mWeight );
		v0.mY = ZLInterpolate::Interpolate ( key.mMode, v0.mY, v1.mY, span.mTime, key.mWeight );
		v0.mZ = ZLInterpolate::Interpolate ( key.mMode, v0.mZ, v1.mZ, span.mTime, key.mWeight );
	}
	
	if ( span.mCycle != 0.0f ) {
		ZLVec3D curveDelta = this->GetCurveDelta ();
		curveDelta.Scale ( span.mCycle );
		v0.Add ( curveDelta );
	}
	return v0;
}

//----------------------------------------------------------------//
void MOAIAnimCurveVec::GetValue ( MOAIAttribute& attr, const MOAIAnimKeySpan& span ) const {

	attr.SetValue ( this->GetValue ( span ));
}

//----------------------------------------------------------------//
void MOAIAnimCurveVec::GetZero ( MOAIAttribute& attr ) const {

	ZLVec3D zero ( 0.0f, 0.0f, 0.0f );
	attr.SetValue ( zero );
}

//----------------------------------------------------------------//
MOAIAnimCurveVec::MOAIAnimCurveVec () :
	mValue ( 0.0f, 0.0f, 0.0f ) {
	
	RTTI_SINGLE ( MOAIAnimCurveBase )
}

//----------------------------------------------------------------//
MOAIAnimCurveVec::~MOAIAnimCurveVec () {
}

//----------------------------------------------------------------//
void MOAIAnimCurveVec::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIAnimCurveBase::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIAnimCurveVec::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIAnimCurveBase::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getValueAtTime", _getValueAtTime, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setKey", _setKey, MRB_ARGS_ARG ( 5, 2 ) );

}

//----------------------------------------------------------------//
void MOAIAnimCurveVec::ReserveSamples ( u32 total ) {

	this->mSamples.Init ( total );
}

//----------------------------------------------------------------//
void MOAIAnimCurveVec::SetSample ( u32 id, const ZLVec3D& value ) {

	if ( id < this->mKeys.Size ()) {
		this->mSamples [ id ] = value;
	}
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAIAnimCurveVec::MOAINode_Update () {

	this->mValue = this->GetValue ( this->mTime );
}
