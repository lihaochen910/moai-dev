// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPATH_H
#define	MOAIPATH_H

//================================================================//
// MOAIPath
//================================================================//
// TODO: doxygen
class MOAIPath :
	public virtual MOAIRubyObject {
private:

	ZLLeanArray < ZLVec2D >				mControlPoints;
	ZLLeanArray < float >				mSegmentLengths;

	float			mFlatness;
	float			mAngle;

	float			mLength;

	//----------------------------------------------------------------//
	static mrb_value		_bless				( mrb_state* M, mrb_value context );
	static mrb_value		_evaluate			( mrb_state* M, mrb_value context );
	static mrb_value		_getLength			( mrb_state* M, mrb_value context );
	static mrb_value		_reserve			( mrb_state* M, mrb_value context );
	static mrb_value		_setPoint			( mrb_state* M, mrb_value context );
	static mrb_value		_setThresholds		( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//

public:
	
	DECL_RUBY_FACTORY ( MOAIPath, MOAIRubyObject )
	
	GET_SET ( float, Flatness, mFlatness )
	GET_SET ( float, Angle, mAngle )
	
	GET ( float, Length, mLength )
	
	//----------------------------------------------------------------//
	void				Bless					();
	size_t				CountSegments			();
	ZLVec2D				Evaluate				( float t );
	ZLCubicBezier2D		GetSegment				( size_t idx );
	ZLCubicBezier2D		GetSegmentForTime		( float t, float* st = 0 );
						MOAIPath				();
						~MOAIPath				();
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void				Reserve					( size_t size );
	void				SetPoint				( size_t idx, float x, float y );
};

#endif
