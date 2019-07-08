// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPATHSTEPPER_H
#define	MOAIPATHSTEPPER_H

#include <moai-sim/MOAIPath.h>

class MOAIPath;

//================================================================//
// MOAIPathStepper
//================================================================//
// TODO: doxygen
class MOAIPathStepper :
	public virtual MOAIRubyObject {
private:

	u32								mStepCount;
	u32								mTotalSteps;
	
	float							mStepSize;
	
	float							mEdgeBase;
	float							mEdgeLength;
	
	ZLVec2D							mEdge [ 3 ];
	ZLVec2D							mNormal;
	
	MOAIRubySharedPtr < MOAIPath>	mPath;
	
	ZLCubicBezierFlattener2D		mFlattener;
	size_t							mSegmentIdx;
	size_t							mTotalSegments;
	float							mLength;

	//----------------------------------------------------------------//
	static mrb_value		_getLength		( mrb_state* M, mrb_value context );
	static mrb_value		_more			( mrb_state* M, mrb_value context );
	static mrb_value		_next			( mrb_state* M, mrb_value context );
	static mrb_value		_start			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	bool			MoreVertices			();
	ZLVec2D			NextVertex				();

public:
	
	DECL_RUBY_FACTORY ( MOAIPathStepper, MOAIRubyObject )
	
	GET ( float, Length, mLength )
	
	//----------------------------------------------------------------//
					MOAIPathStepper			();
					~MOAIPathStepper		();
	bool			More					();
	ZLVec2D			Next					();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			Start					( MOAIPath& path, float stepSize );
};

#endif
