// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIBOX2DMOTORJOINT_H
#define	MOAIBOX2DMOTORJOINT_H

#include <moai-box2d/MOAIBox2DJoint.h>

//================================================================//
// MOAIBox2DMotorJoint
//================================================================//
/**	@lua MOAIBox2DMotorJoint
	@text Box2D motor joint.
*/
class MOAIBox2DMotorJoint :
	public MOAIBox2DJoint {
private:

	//----------------------------------------------------------------//
	static mrb_value		_setLinearOffset			( mrb_state* M, mrb_value context );
	static mrb_value		_getLinearOffset			( mrb_state* M, mrb_value context );

	static mrb_value		_setAngularOffset			( mrb_state* M, mrb_value context );
	static mrb_value		_getAngularOffset			( mrb_state* M, mrb_value context );

	static mrb_value		_setMaxForce				( mrb_state* M, mrb_value context );
	static mrb_value		_getMaxForce				( mrb_state* M, mrb_value context );

	static mrb_value		_setMaxTorque				( mrb_state* M, mrb_value context );
	static mrb_value		_getMaxTorque				( mrb_state* M, mrb_value context );

	static mrb_value		_setCorrectionFactor		( mrb_state* M, mrb_value context );
	static mrb_value		_getCorrectionFactor		( mrb_state* M, mrb_value context );
	 
public:
	
	DECL_RUBY_FACTORY ( MOAIBox2DMotorJoint, MOAIRubyObject )
	
	//----------------------------------------------------------------//
					MOAIBox2DMotorJoint		();
					~MOAIBox2DMotorJoint		();
	void			RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
};

#endif
