// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIBOX2DFRICTIONJOINT_H
#define	MOAIBOX2DFRICTIONJOINT_H

#include <moai-box2d/MOAIBox2DJoint.h>

class b2Joint;

//================================================================//
// MOAIBox2DFrictionJoint
//================================================================//
/**	@lua MOAIBox2DFrictionJoint
	@text Box2D friction joint.
*/
class MOAIBox2DFrictionJoint :
	public MOAIBox2DJoint {
private:

	//----------------------------------------------------------------//
	static mrb_value		_getMaxForce			( mrb_state* M, mrb_value context );
	static mrb_value		_getMaxTorque			( mrb_state* M, mrb_value context );
	static mrb_value		_setMaxForce			( mrb_state* M, mrb_value context );
	static mrb_value		_setMaxTorque			( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_FACTORY ( MOAIBox2DFrictionJoint, MOAIRubyObject )
	
	//----------------------------------------------------------------//
					MOAIBox2DFrictionJoint		();
					~MOAIBox2DFrictionJoint		();
	void			RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
};

#endif
