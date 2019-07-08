// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIAnimCurveBone.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIAnimCurveBone::_getValueAtTime ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAnimCurveBone, "UN" );

	float time = state.GetParamValue < float >( 1, 0 );
	
	MOAIAnimKeySpan span = self->GetSpan ( time );
	ZLAffine3D mtx = self->GetValue ( span );

	//return state.ToRValue ( mtx );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIAnimCurveBone::_setKey ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAnimCurveBone, "UNNNN" );

	u32 index				= state.GetParamValue < u32 >( 1, 1 ) - 1;
	float time				= state.GetParamValue < float >( 2, 0.0f );
	ZLVec3D position		= state.GetParamValue < ZLVec3D >( 3, ZLVec3D::ORIGIN );
	ZLQuaternion rotation	= state.GetParamValue < ZLQuaternion >( 6, ZLQuaternion::IDENT );
	ZLVec3D scale			= state.GetParamValue < ZLVec3D >( 10, ZLVec3D::AXIS );
	u32 mode				= state.GetParamValue < u32 >( 13, ZLInterpolate::kSmooth );
	float weight			= state.GetParamValue < float >( 14, 1.0f );
	
	if ( MOAILogMgr::CheckIndexPlusOne ( index, self->mKeys.Size (), M )) {
		
		self->SetKey ( index, time, mode, weight );
		
		self->SetSamplePosition ( index, position.mX, position.mY, position.mZ );
		self->SetSampleRotation ( index, rotation.mV.mX, rotation.mV.mY, rotation.mV.mZ, rotation.mS );
		self->SetSampleScale ( index, scale.mX, scale.mY, scale.mZ );
	}
	return context;
}

//================================================================//
// MOAIAnimCurveBone
//================================================================//

//----------------------------------------------------------------//
void MOAIAnimCurveBone::ApplyValueAttrOp ( MOAIAttribute& attr, u32 op ) {

	this->mValue = attr.ApplyNoAdd ( this->mValue, op, MOAIAttribute::ATTR_READ_WRITE );
}

//----------------------------------------------------------------//
ZLAffine3D MOAIAnimCurveBone::Compose ( const ZLVec3D& pos, const ZLQuaternion& rot, const ZLVec3D& scl ) {

	ZLAffine3D value;
	value.Scale ( scl );
	
	ZLAffine3D mtx;
	rot.Get ( mtx );
	value.Append ( mtx );
	
	mtx.Translate ( pos );
	value.Append ( mtx );
	
	return value;
}

//----------------------------------------------------------------//
void MOAIAnimCurveBone::GetCurveDelta ( ZLVec3D& pos, ZLQuaternion& rot, ZLVec3D& scl ) const {

	u32 size = ( u32 )this->mKeys.Size ();
	if ( size > 1 ) {
	
		pos = this->mPositionSamples [ size - 1 ];
		pos.Sub ( this->mPositionSamples [ 0 ]);
	
		rot = this->mRotationSamples [ size - 1 ];
		rot.Sub ( this->mRotationSamples [ 0 ]);
		
		scl = this->mScaleSamples [ size - 1 ];
		scl.Sub ( this->mScaleSamples [ 0 ]);
	}
	else {
		pos = ZLVec3D::ORIGIN;
		rot = ZLQuaternion::IDENT;
		scl = ZLVec3D::AXIS;
	}
}

//----------------------------------------------------------------//
void MOAIAnimCurveBone::GetDelta ( MOAIAttribute& attr, const MOAIAnimKeySpan& span0, const MOAIAnimKeySpan& span1 ) const {

	ZLVec3D p0;
	ZLQuaternion q0;
	ZLVec3D s0;
	
	this->GetValue ( span0, p0, q0, s0 );
	
	ZLVec3D p1;
	ZLQuaternion q1;
	ZLVec3D s1;
	
	this->GetValue ( span1, p1, q1, s1 );

	p1.Sub ( p0 );
	q1.Sub ( q0 );
	s1.Sub ( s0 );
	
	attr.SetValue ( MOAIAnimCurveBone::Compose ( p1, q1, s1 ));
}

//----------------------------------------------------------------//
ZLAffine3D MOAIAnimCurveBone::GetValue ( float time ) const {

	MOAIAnimKeySpan span = this->GetSpan ( time );
	return this->GetValue ( span );
}

//----------------------------------------------------------------//
ZLAffine3D MOAIAnimCurveBone::GetValue ( const MOAIAnimKeySpan& span ) const {
	
	ZLVec3D p0;
	ZLQuaternion q0;
	ZLVec3D s0;
		
	this->GetValue ( span, p0, q0, s0 );
	
	// TODO: test this
//	if ( span.mCycle != 0.0f ) {
//	
//		ZLVec3D p1;
//		ZLQuaternion q1;
//		ZLVec3D s1;
//	
//		this->GetCurveDelta ( p1, q1, s1 );
//		
//		p1.Scale ( span.mCycle );
//		q1.Scale ( span.mCycle );
//		s1.Scale ( span.mCycle );
//		
//		p0.Add ( p1 );
//		q0.Add ( q1 );
//		s0.Add ( s1 );
//	}
	
	return MOAIAnimCurveBone::Compose ( p0, q0, s0 );
}

//----------------------------------------------------------------//
void MOAIAnimCurveBone::GetValue ( const MOAIAnimKeySpan& span, ZLVec3D& pos, ZLQuaternion& rot, ZLVec3D& scl ) const {

	MOAIAnimKey& key = this->mKeys [ span.mKeyID ];
	
	ZLVec3D p0			= this->mPositionSamples [ span.mKeyID ];
	ZLQuaternion q0		= this->mRotationSamples [ span.mKeyID ];
	ZLVec3D s0			= this->mScaleSamples [ span.mKeyID ];
	
	if ( span.mTime > 0.0f ) {
	
		ZLVec3D p1			= this->mPositionSamples [ span.mKeyID + 1 ];
		ZLQuaternion q1		= this->mRotationSamples [ span.mKeyID + 1 ];
		ZLVec3D s1			= this->mScaleSamples [ span.mKeyID + 1 ];
		
		p0.Lerp ( p1, ZLInterpolate::Curve ( key.mMode, span.mTime, key.mWeight ));
		q0.Slerp ( q0, q1, ZLInterpolate::Curve ( key.mMode, span.mTime, key.mWeight ));
		s0.Lerp ( s1, ZLInterpolate::Curve ( key.mMode, span.mTime, key.mWeight ));
	}
		
	pos = p0;
	rot = q0;
	scl = s0;
}

//----------------------------------------------------------------//
void MOAIAnimCurveBone::GetValue ( MOAIAttribute& attr, const MOAIAnimKeySpan& span ) const {

	attr.SetValue ( this->GetValue ( span ));
}

//----------------------------------------------------------------//
void MOAIAnimCurveBone::GetZero ( MOAIAttribute& attr ) const {

	attr.SetValue ( ZLQuaternion ( 0.0f, 0.0f, 0.0f, 0.0f ));
}

//----------------------------------------------------------------//
MOAIAnimCurveBone::MOAIAnimCurveBone () {
	
	RTTI_SINGLE ( MOAIAnimCurveBase )
	
	this->mValue.Ident ();
}

//----------------------------------------------------------------//
MOAIAnimCurveBone::~MOAIAnimCurveBone () {
}

//----------------------------------------------------------------//
void MOAIAnimCurveBone::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIAnimCurveBase::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIAnimCurveBone::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIAnimCurveBase::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getValueAtTime", _getValueAtTime, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setKey", _setKey, MRB_ARGS_ARG ( 0, 1 ) | MRB_ARGS_REQ ( 2 ) );

}

//----------------------------------------------------------------//
void MOAIAnimCurveBone::ReserveSamples ( u32 total ) {

	this->mPositionSamples.Init ( total );
	this->mRotationSamples.Init ( total );
	this->mScaleSamples.Init ( total );
}

//----------------------------------------------------------------//
void MOAIAnimCurveBone::SetSamplePosition ( u32 idx, float x, float y, float z ) {

	if ( idx < this->mKeys.Size ()) {
		this->mPositionSamples [ idx ] = ZLVec3D ( x, y, z );
	}
}

//----------------------------------------------------------------//
void MOAIAnimCurveBone::SetSampleRotation ( u32 idx, float x, float y, float z, float w ) {

	if ( idx < this->mKeys.Size ()) {
		this->mRotationSamples [ idx ] = ZLQuaternion ( w, x, y, z );
	}
}

//----------------------------------------------------------------//
void MOAIAnimCurveBone::SetSampleScale ( u32 idx, float x, float y, float z ) {

	if ( idx < this->mKeys.Size ()) {
		this->mScaleSamples [ idx ] = ZLVec3D ( x, y, z );
	}
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAIAnimCurveBone::MOAINode_Update () {

	this->mValue = this->GetValue ( this->mTime );
}
