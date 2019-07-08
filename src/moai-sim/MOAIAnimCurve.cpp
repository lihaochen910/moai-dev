// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIAnimCurve.h>
//#include <moai-sim/MOAIGfxMgr.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getValueAtTime
	@text	Return the interpolated value given a point in time along the curve. This does not change
	        the curve's built in TIME attribute (it simply performs the requisite computation on demand).
	
	@in		MOAIAnimCurve self
	@in		number time
	@out	number value	The interpolated value
*/
mrb_value MOAIAnimCurve::_getValueAtTime ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAnimCurve, "UN" );

	float time = state.GetParamValue < float >( 1, 0 );
	
	MOAIAnimKeySpan span = self->GetSpan ( time );
	float value = self->GetValue ( span );
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( value );
	ret [ 1 ] = state.ToRValue ( span.mKeyID + 1 );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getValueRange
	@text	Returns the minimum and maximum values in the given time range. 
	
	@in		MOAIAnimCurve self
	@in		number	start time
	@in		number	end time
	@out	number 	min value
	@out	number 	max value
*/
mrb_value MOAIAnimCurve::_getValueRange ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAnimCurve, "UNN" );
	
	float t0 = state.GetParamValue < float >( 1, 0.0f );
	float t1 = state.GetParamValue < float >( 2, 0.0f );
	
	float min = 0.0f;
	float max = 0.0f;
	
	if ( t0 > t1 ) {
		float tmp = t0; t0 = t1; t1 = tmp;
	}
	
	self->GetValueRange ( t0, t1, min, max );

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( min );
	ret [ 1 ] = state.ToRValue ( max );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	setKey
	@text	Initialize a key frame at a given time with a give value. Also set the transition type between
			the specified key frame and the next key frame.
	
	@in		MOAIAnimCurve self
	@in		number index			Index of the keyframe.
	@in		number time				Location of the key frame along the curve.
	@in		number value			Value of the curve at time.
	@opt	number mode				The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
									MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.
	@opt	number weight			Blends between chosen ease type (of any) and a linear transition. Defaults to 1.
	@out	nil
*/
mrb_value MOAIAnimCurve::_setKey ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAnimCurve, "UNN" );

	u32 index		= state.GetParamValue < u32 >( 1, 1 ) - 1;
	float time		= state.GetParamValue < float >( 2, 0.0f );
	float value		= state.GetParamValue < float >( 3, 0.0f );
	u32 mode		= state.GetParamValue < u32 >( 4, ZLInterpolate::kSmooth );
	float weight	= state.GetParamValue < float >( 5, 1.0f );
	
	if ( MOAILogMgr::CheckIndexPlusOne ( index, self->mKeys.Size (), M )) {
	
		self->SetKey ( index, time, mode, weight );
		self->SetSample ( index, value );
	}
	return context;
}

//================================================================//
// MOAIAnimCurve
//================================================================//

//----------------------------------------------------------------//
void MOAIAnimCurve::ApplyValueAttrOp ( MOAIAttribute& attr, u32 op ) {

	this->mValue = attr.Apply ( this->mValue, op, MOAIAttribute::ATTR_READ_WRITE );
}

//----------------------------------------------------------------//
void MOAIAnimCurve::Draw ( u32 resolution ) const {

	// TODO: this isn't entirely correct. the value of each key frame should be drawn
	// and then the spans between keys should be filled in with an approximation of
	// the resolution.
	
	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	float length = this->GetLength ();
	float step = length / ( float )resolution;

	gfxState.BeginPrim ( ZGL_PRIM_LINE_STRIP, resolution );

	for ( u32 i = 0; i < resolution; ++i ) {

		float t = step * ( float )i;
		float v = this->GetValue ( t );

		gfxState.WriteVtx ( t, v, 0.0f );
		gfxState.WritePenColor4b ();
	}

	float t = length;
	float v = this->GetValue ( t );

	gfxState.WriteVtx ( t, v, 0.0f );
	gfxState.WritePenColor4b ();

	gfxState.EndPrim ();
}

//----------------------------------------------------------------//
float MOAIAnimCurve::GetCurveDelta () const {

	size_t size = this->mKeys.Size ();
	if ( size > 1 ) {
		return this->mSamples [ size - 1 ] - this->mSamples [ 0 ];
	}
	return 0.0f;
}

//----------------------------------------------------------------//
void MOAIAnimCurve::GetDelta ( MOAIAttribute& attr, const MOAIAnimKeySpan& span0, const MOAIAnimKeySpan& span1 ) const {

	float v0 = this->GetValue ( span0 );
	float v1 = this->GetValue ( span1 );
	
	attr.SetValue ( v1 - v0 );
}

//----------------------------------------------------------------//
float MOAIAnimCurve::GetSample ( u32 id ) {

	if ( id < this->mKeys.Size ()) {
		return this->mSamples [ id ];
	}
	return 0.0f;
}

//----------------------------------------------------------------//
float MOAIAnimCurve::GetValue ( float time ) const {

	MOAIAnimKeySpan span = this->GetSpan ( time );
	return this->GetValue ( span );
}

//----------------------------------------------------------------//
float MOAIAnimCurve::GetValue ( const MOAIAnimKeySpan& span ) const {

	MOAIAnimKey& key = this->mKeys [ span.mKeyID ];
	float v0 = this->mSamples [ span.mKeyID ];
	
	if ( span.mTime > 0.0f ) {
		v0 = ZLInterpolate::Interpolate ( key.mMode, v0, this->mSamples [ span.mKeyID + 1 ], span.mTime, key.mWeight );
	}
	return v0 + ( this->GetCurveDelta () * span.mCycle );
}

//----------------------------------------------------------------//
void MOAIAnimCurve::GetValue ( MOAIAttribute& attr, const MOAIAnimKeySpan& span ) const {

	attr.SetValue ( this->GetValue ( span ));
}

//----------------------------------------------------------------//
void MOAIAnimCurve::GetValueRange ( float t0, float t1, float &min, float &max ) {
	
	u32 key0 = 0;
	u32 key1 = 0;
	
	MOAIAnimKeySpan span0 = this->GetSpan ( t0 );
	MOAIAnimKeySpan span1 = this->GetSpan ( t1 );
	
	if ( t1 - t0 > this->GetLength ()) {
		key0 = 0;
		key1 = this->Size () - 1;
	}
	else {
		if ( span0.mKeyID > span1.mKeyID ) {
			
			key0 = span1.mKeyID;
			key1 = span0.mKeyID;
		}
		else {
			
			key0 = span0.mKeyID;
			key1 = span1.mKeyID;
		}
	}
	
	min = this->GetValue ( span0 );
	max = this->GetValue ( span0 );
	
	for ( u32 id = key0 + 1; id <= key1; id++ ) {
		
		float val = this->mSamples [ id ];
		
		if ( val < min ) {
			min = val;
		}
		if ( val > max ) {
			max = val;
		}
	}
	
	float last = this->GetValue ( span1 );
	
	if ( last < min ) {
		min = last;
	}
	if ( last > max ) {
		max = last;
	}
}

//----------------------------------------------------------------//
void MOAIAnimCurve::GetZero ( MOAIAttribute& attr ) const {

	attr.SetValue ( 0.0f );
}

//----------------------------------------------------------------//
MOAIAnimCurve::MOAIAnimCurve () :
	mValue ( 0.0f ) {
	
	RTTI_SINGLE ( MOAIAnimCurveBase )
}

//----------------------------------------------------------------//
MOAIAnimCurve::~MOAIAnimCurve () {
}

//----------------------------------------------------------------//
void MOAIAnimCurve::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIAnimCurveBase::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIAnimCurve::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIAnimCurveBase::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getValueAtTime", _getValueAtTime, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getValueRange", _getValueRange, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setKey", _setKey, MRB_ARGS_ARG ( 3, 2 ) );

}

//----------------------------------------------------------------//
void MOAIAnimCurve::ReserveSamples ( u32 total ) {

	this->mSamples.Init ( total );
}

//----------------------------------------------------------------//
void MOAIAnimCurve::SetSample ( u32 id, float value ) {

	if ( id < this->mKeys.Size ()) {
		this->mSamples [ id ] = value;
	}
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAIAnimCurve::MOAINode_Update () {

	this->mValue = this->GetValue ( this->mTime );
}
