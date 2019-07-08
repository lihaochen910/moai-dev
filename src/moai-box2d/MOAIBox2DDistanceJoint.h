// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIBOX2DDISTANCEJOINT_H
#define	MOAIBOX2DDISTANCEJOINT_H

#include <moai-box2d/MOAIBox2DJoint.h>

class b2Joint;

//================================================================//
// MOAIBox2DDistanceJoint
//================================================================//
/**	@lua MOAIBox2DDistanceJoint
	@text Box2D distance joint.
*/
class MOAIBox2DDistanceJoint :
	public MOAIBox2DJoint {
private:

	//----------------------------------------------------------------//
	static mrb_value		_getDampingRatio			( mrb_state* M, mrb_value context );
	static mrb_value		_getFrequency				( mrb_state* M, mrb_value context );
	static mrb_value		_getLength					( mrb_state* M, mrb_value context );
	static mrb_value		_setDampingRatio			( mrb_state* M, mrb_value context );
	static mrb_value		_setFrequency				( mrb_state* M, mrb_value context );
	static mrb_value		_setLength					( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_FACTORY ( MOAIBox2DDistanceJoint, MOAIRubyObject )
	
	//----------------------------------------------------------------//
					MOAIBox2DDistanceJoint		();
					~MOAIBox2DDistanceJoint		();
	void			RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
};

#endif
