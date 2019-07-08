// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIAnimCurveQuat.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getValueAtTime
	@text	Return the interpolated value (as Euler angles) given a point in
			time along the curve. This does not change the curve's built in TIME
			attribute (it simply performs the requisite computation on demand).
	
	@in		MOAIAnimCurveQuat self
	@in		number time
	@out	number xRot
	@out	number yRot
	@out	number zRot
*/
mrb_value MOAIAnimCurveQuat::_getValueAtTime ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAnimCurveQuat, "UN" );

	float time = state.GetParamValue < float >( 1, 0 );
	
	MOAIAnimKeySpan span = self->GetSpan ( time );
	ZLQuaternion quat = self->GetValue ( span );
	
	ZLVec3D value;
	quat.Get ( value.mX, value.mY, value.mZ );

	mrb_value ret [ 4 ];
	ret [ 0 ] = state.ToRValue ( value.mX );
	ret [ 1 ] = state.ToRValue ( value.mY );
	ret [ 2 ] = state.ToRValue ( value.mZ );
	ret [ 3 ] = state.ToRValue ( span.mKeyID + 1 );

	return mrb_ary_new_from_values ( state, 4, ret );
}

//----------------------------------------------------------------//
/**	@lua	setKey
	@text	Initialize a key frame at a given time with a give value
			(as Euler angles). Also set the transition type between
			the specified key frame and the next key frame.
	
	@in		MOAIAnimCurveQuat self
	@in		number index			Index of the keyframe.
	@in		number time				Location of the key frame along the curve.
	@in		number xRot				X rotation at time.
	@in		number yRot				Y rotation at time.
	@in		number zRot				Z rotation at time.
	@opt	number mode				The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
									MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.
	@opt	number weight			Blends between chosen ease type (of any) and a linear transition. Defaults to 1.
	@out	nil
*/
mrb_value MOAIAnimCurveQuat::_setKey ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAnimCurveQuat, "UNNNN" );

	u32 index		= state.GetParamValue < u32 >( 1, 1 ) - 1;
	float time		= state.GetParamValue < float >( 2, 0.0f );
	ZLVec3D value	= state.GetVec3D < float >( 3 );
	u32 mode		= state.GetParamValue < u32 >( 6, ZLInterpolate::kSmooth );
	float weight	= state.GetParamValue < float >( 7, 1.0f );
	
	if ( MOAILogMgr::CheckIndexPlusOne ( index, self->mKeys.Size (), M )) {
		
		self->SetKey ( index, time, mode, weight );
		self->SetSample ( index, value.mX, value.mY, value.mZ );
	}
	return context;
}

//================================================================//
// MOAIAnimCurveQuat
//================================================================//

//----------------------------------------------------------------//
void MOAIAnimCurveQuat::ApplyValueAttrOp ( MOAIAttribute& attr, u32 op ) {

	this->mValue = attr.Apply ( this->mValue, op, MOAIAttribute::ATTR_READ_WRITE );
}

//----------------------------------------------------------------//
ZLQuaternion MOAIAnimCurveQuat::GetCurveDelta () const {

	ZLQuaternion delta = ZLQuaternion::IDENT;

	u32 size = ( u32 )this->mKeys.Size ();
	if ( size > 1 ) {
		delta = this->mSamples [ size - 1 ];
		delta.Sub ( this->mSamples [ 0 ]);
	}
	return delta;
}

//----------------------------------------------------------------//
void MOAIAnimCurveQuat::GetDelta ( MOAIAttribute& attr, const MOAIAnimKeySpan& span0, const MOAIAnimKeySpan& span1 ) const {

	ZLQuaternion v0 = this->GetValue ( span0 );
	ZLQuaternion v1 = this->GetValue ( span1 );
	
	v1.Sub ( v0 );
	
	attr.SetValue ( v1 );
}

//----------------------------------------------------------------//
ZLQuaternion MOAIAnimCurveQuat::GetValue ( float time ) const {

	MOAIAnimKeySpan span = this->GetSpan ( time );
	return this->GetValue ( span );
}

//----------------------------------------------------------------//
ZLQuaternion MOAIAnimCurveQuat::GetValue ( const MOAIAnimKeySpan& span ) const {

	MOAIAnimKey& key = this->mKeys [ span.mKeyID ];
	ZLQuaternion v0 = this->mSamples [ span.mKeyID ];
	
	if ( span.mTime > 0.0f ) {
	
		ZLQuaternion v1 = this->mSamples [ span.mKeyID + 1 ];
		
		v0.Slerp ( v0, v1, ZLInterpolate::Curve ( key.mMode, span.mTime, key.mWeight ));
	}
	
	if ( span.mCycle != 0.0f ) {
		ZLQuaternion curveDelta = this->GetCurveDelta ();
		curveDelta.Scale ( span.mCycle );
		v0.Add ( curveDelta );
	}
	return v0;
}

//----------------------------------------------------------------//
void MOAIAnimCurveQuat::GetValue ( MOAIAttribute& attr, const MOAIAnimKeySpan& span ) const {

	attr.SetValue ( this->GetValue ( span ));
}

//----------------------------------------------------------------//
void MOAIAnimCurveQuat::GetZero ( MOAIAttribute& attr ) const {

	attr.SetValue ( ZLQuaternion ( 0.0f, 0.0f, 0.0f, 0.0f ));
}

//----------------------------------------------------------------//
MOAIAnimCurveQuat::MOAIAnimCurveQuat () {
	
	RTTI_SINGLE ( MOAIAnimCurveBase )
	
	this->mValue = ZLQuaternion::IDENT;
}

//----------------------------------------------------------------//
MOAIAnimCurveQuat::~MOAIAnimCurveQuat () {
}

//----------------------------------------------------------------//
void MOAIAnimCurveQuat::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIAnimCurveBase::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIAnimCurveQuat::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIAnimCurveBase::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getValueAtTime", _getValueAtTime, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setKey", _setKey, MRB_ARGS_ARG ( 5, 2 ) );

}

//----------------------------------------------------------------//
void MOAIAnimCurveQuat::ReserveSamples ( u32 total ) {

	this->mSamples.Init ( total );
}

//----------------------------------------------------------------//
void MOAIAnimCurveQuat::SetSample ( u32 id, float x, float y, float z ) {

	if ( id < this->mKeys.Size ()) {
		this->mSamples [ id ] = ZLQuaternion ( x, y, z );
	}
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAIAnimCurveQuat::MOAINode_Update () {

	this->mValue = this->GetValue ( this->mTime );
}
