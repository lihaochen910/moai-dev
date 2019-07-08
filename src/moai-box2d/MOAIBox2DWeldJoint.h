// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIBOX2DWELDJOINT_H
#define	MOAIBOX2DWELDJOINT_H

#include <moai-box2d/MOAIBox2DJoint.h>

//================================================================//
// MOAIBox2DWeldJoint
//================================================================//
/**	@lua MOAIBox2DWeldJoint
	@text Box2D weld joint.
*/
class MOAIBox2DWeldJoint :
	public MOAIBox2DJoint {
private:

	//----------------------------------------------------------------//
	static mrb_value		_getDampingRatio		( mrb_state* M, mrb_value context );
	static mrb_value		_getFrequency			( mrb_state* M, mrb_value context );
	static mrb_value		_setDampingRatio		( mrb_state* M, mrb_value context );
	static mrb_value		_setFrequency			( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_FACTORY ( MOAIBox2DWeldJoint, MOAIRubyObject )
	
	//----------------------------------------------------------------//
					MOAIBox2DWeldJoint		();
					~MOAIBox2DWeldJoint		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
