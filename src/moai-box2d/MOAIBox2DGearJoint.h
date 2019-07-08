// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIBOX2DGEARJOINT_H
#define	MOAIBOX2DGEARJOINT_H

#include <moai-box2d/MOAIBox2DJoint.h>

//================================================================//
// MOAIBox2DGearJoint
//================================================================//
/**	@lua MOAIBox2DGearJoint
	@text Box2D gear joint.
*/
class MOAIBox2DGearJoint :
	public MOAIBox2DJoint {
private:

	friend class MOAIBox2DWorld;

	MOAIRubySharedPtr < MOAIBox2DJoint > mJointA;
	MOAIRubySharedPtr < MOAIBox2DJoint > mJointB;

	//----------------------------------------------------------------//
	static mrb_value		_getJointA				( mrb_state* M, mrb_value context );
	static mrb_value		_getJointB				( mrb_state* M, mrb_value context );
	static mrb_value		_getRatio				( mrb_state* M, mrb_value context );
	static mrb_value		_setRatio				( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_FACTORY ( MOAIBox2DGearJoint, MOAIRubyObject )
	
	//----------------------------------------------------------------//
					MOAIBox2DGearJoint		();
					~MOAIBox2DGearJoint		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
