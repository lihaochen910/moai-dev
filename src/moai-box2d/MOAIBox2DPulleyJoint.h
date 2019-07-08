// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIBOX2DPULLEYJOINT_H
#define	MOAIBOX2DPULLEYJOINT_H

#include <moai-box2d/MOAIBox2DJoint.h>

//================================================================//
// MOAIBox2DPulleyJoint
//================================================================//
/**	@lua MOAIBox2DPulleyJoint
	@text Box2D pulley joint.
*/
class MOAIBox2DPulleyJoint :
	public MOAIBox2DJoint {
private:

	//----------------------------------------------------------------//
	static mrb_value		_getGroundAnchorA		( mrb_state* M, mrb_value context );
	static mrb_value		_getGroundAnchorB		( mrb_state* M, mrb_value context );
	static mrb_value		_getLength1				( mrb_state* M, mrb_value context );
	static mrb_value		_getLength2				( mrb_state* M, mrb_value context );
	static mrb_value		_getRatio				( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_FACTORY ( MOAIBox2DPulleyJoint, MOAIRubyObject )
	
	//----------------------------------------------------------------//
					MOAIBox2DPulleyJoint	();
					~MOAIBox2DPulleyJoint	();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
