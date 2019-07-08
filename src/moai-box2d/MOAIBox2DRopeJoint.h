// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIBOX2DROPEJOINT_H
#define	MOAIBOX2DROPEJOINT_H

#include <moai-box2d/MOAIBox2DJoint.h>

//================================================================//
// MOAIBox2DRopeJoint
//================================================================//
/**	@lua MOAIBox2DRopeJoint
	@text Box2D rope joint.
 */
class MOAIBox2DRopeJoint :
public MOAIBox2DJoint {
private:
	
	//----------------------------------------------------------------//
	static mrb_value _getLimitState			( mrb_state* M, mrb_value context );
	static mrb_value _getMaxLength			( mrb_state* M, mrb_value context );
	static mrb_value _setMaxLength			( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_FACTORY ( MOAIBox2DRopeJoint, MOAIRubyObject )
	
	//----------------------------------------------------------------//
					MOAIBox2DRopeJoint		();
					~MOAIBox2DRopeJoint		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
