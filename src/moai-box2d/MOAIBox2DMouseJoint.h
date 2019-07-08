// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIBOX2DMOUSEJOINT_H
#define	MOAIBOX2DMOUSEJOINT_H

#include <moai-box2d/MOAIBox2DJoint.h>

//================================================================//
// MOAIBox2DMouseJoint
//================================================================//
/**	@lua MOAIBox2DMouseJoint
	@text Box2D 'mouse' joint.
*/
class MOAIBox2DMouseJoint :
	public MOAIBox2DJoint {
private:

	//----------------------------------------------------------------//
	static mrb_value		_getDampingRatio		( mrb_state* M, mrb_value context );
	static mrb_value		_getFrequency			( mrb_state* M, mrb_value context );
	static mrb_value		_getMaxForce			( mrb_state* M, mrb_value context );
	static mrb_value		_getTarget				( mrb_state* M, mrb_value context );
	static mrb_value		_setDampingRatio		( mrb_state* M, mrb_value context );
	static mrb_value		_setFrequency			( mrb_state* M, mrb_value context );
	static mrb_value		_setMaxForce			( mrb_state* M, mrb_value context );
	static mrb_value		_setTarget				( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_FACTORY ( MOAIBox2DMouseJoint, MOAIRubyObject )
	
	//----------------------------------------------------------------//
					MOAIBox2DMouseJoint		();
					~MOAIBox2DMouseJoint	();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
