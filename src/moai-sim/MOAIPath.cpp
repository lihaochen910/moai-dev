// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIPath.h>

#define DEFAULT_FLATNESS 0.125f
#define DEFAULT_ANGLE 15.0f

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: path
mrb_value MOAIPath::_bless ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPath, "U" );

	self->Bless ();
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: path
mrb_value MOAIPath::_evaluate ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPath, "U" );

	float t			= state.GetParamValue < float >( 1, 0.0f );
	
	ZLVec2D result = self->Evaluate ( t );

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( result.mX );
	ret [ 1 ] = state.ToRValue ( result.mY );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
// TODO: path
mrb_value MOAIPath::_getLength ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPath, "U" );
	
	return state.ToRValue ( self->GetLength () );
}

//----------------------------------------------------------------//
// TODO: path
mrb_value MOAIPath::_reserve ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPath, "U" );

	size_t size = state.GetParamValue < u32 >( 1, 0 );
	self->Reserve ( size );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: path
mrb_value MOAIPath::_setPoint ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPath, "U" );

	size_t idx		= state.GetParamValue < u32 >( 1, 1 ) - 1;
	float x			= state.GetParamValue < float >( 2, 0.0f );
	float y			= state.GetParamValue < float >( 3, 0.0f );
	
	self->SetPoint ( idx, x, y );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: path
mrb_value MOAIPath::_setThresholds ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPath, "U" );

	float flatness	= state.GetParamValue < float >( 1, DEFAULT_FLATNESS );
	float angle		= state.GetParamValue < float >( 2, DEFAULT_ANGLE );
	
	self->SetFlatness ( flatness );
	self->SetAngle ( angle );
	
	return mrb_nil_value ();
}

//================================================================//
// MOAIPath
//================================================================//

//----------------------------------------------------------------//
void MOAIPath::Bless () {

	size_t totalSegments = this->CountSegments ();
	
	this->mSegmentLengths.Init ( totalSegments );
	
	this->mLength = 0.0f;
	
	for ( size_t i = 0; i < totalSegments; ++i ) {
	
		ZLCubicBezier2D curve = this->GetSegment ( i );
		
		float length = curve.GetFlattenedLength ( this->mFlatness, this->mAngle );
		this->mSegmentLengths [ i ] = length;
		this->mLength += length;
	}
}

//----------------------------------------------------------------//
size_t MOAIPath::CountSegments () {

	return ( this->mControlPoints.Size () - 1 ) / 3;
}

//----------------------------------------------------------------//
ZLVec2D MOAIPath::Evaluate ( float t ) {

	ZLCubicBezier2D curve = this->GetSegmentForTime ( t, &t );
	return curve.Evaluate ( ZLFloat::Clamp ( t, 0.0f, 1.0f ));
}

//----------------------------------------------------------------//
ZLCubicBezier2D MOAIPath::GetSegment ( size_t idx ) {

	size_t basePoint = idx > 0 ? idx * 3 : 0;
	
	ZLCubicBezier2D curve;
	
	curve.Init (
		this->mControlPoints [ basePoint ],
		this->mControlPoints [ basePoint + 1 ],
		this->mControlPoints [ basePoint + 2 ],
		this->mControlPoints [ basePoint + 3 ]
	);
	
	return curve;
}

//----------------------------------------------------------------//
ZLCubicBezier2D MOAIPath::GetSegmentForTime ( float t, float* st ) {

	size_t totalSegments = this->CountSegments ();
	
	t = ZLFloat::Clamp ( t, 0.0f, 1.0f ) * ( float )totalSegments;
	
	float s = ZLFloat::Floor ( t );
	
	if ( st ) {
		( *st ) = t - s;
	}
	
	return this->GetSegment (( size_t )s );
}

//----------------------------------------------------------------//
MOAIPath::MOAIPath () :
	mFlatness ( DEFAULT_FLATNESS ),
	mAngle ( DEFAULT_ANGLE ),
	mLength ( 0.0f ) {
	
	RTTI_SINGLE ( MOAIRubyObject )
}

//----------------------------------------------------------------//
MOAIPath::~MOAIPath () {
}

//----------------------------------------------------------------//
void MOAIPath::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIPath::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "bless",			_bless, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "evaluate",			_evaluate, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getLength",		_getLength, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "reserve",			_reserve, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setPoint",			_setPoint, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "setThresholds",	_setThresholds, MRB_ARGS_ARG ( 0, 2 ) );

}

//----------------------------------------------------------------//
void MOAIPath::Reserve ( size_t size ) {

	this->mControlPoints.Init ( size );
}

//----------------------------------------------------------------//
void MOAIPath::SetPoint ( size_t idx, float x, float y ) {

	this->mControlPoints [ idx ].Init ( x, y );
}
