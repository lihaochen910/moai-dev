// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIBOX2DREVOLUTEJOINT_H
#define	MOAIBOX2DREVOLUTEJOINT_H

#include <moai-box2d/MOAIBox2DJoint.h>

//================================================================//
// MOAIBox2DRevoluteJoint
//================================================================//
/**	@lua MOAIBox2DRevoluteJoint
	@text Box2D revolute joint.
*/
class MOAIBox2DRevoluteJoint :
	public MOAIBox2DJoint {
private:

	//----------------------------------------------------------------//
	static mrb_value		_getJointAngle			( mrb_state* M, mrb_value context );
	static mrb_value		_getJointSpeed			( mrb_state* M, mrb_value context );
	static mrb_value		_getLowerLimit			( mrb_state* M, mrb_value context );
	static mrb_value		_getMotorSpeed			( mrb_state* M, mrb_value context );
	static mrb_value		_getMotorTorque			( mrb_state* M, mrb_value context );
	static mrb_value		_getUpperLimit			( mrb_state* M, mrb_value context );
	static mrb_value		_isLimitEnabled			( mrb_state* M, mrb_value context );
	static mrb_value		_isMotorEnabled			( mrb_state* M, mrb_value context );
	static mrb_value		_setLimit				( mrb_state* M, mrb_value context );
	static mrb_value		_setLimitEnabled		( mrb_state* M, mrb_value context );
	static mrb_value		_setMaxMotorTorque		( mrb_state* M, mrb_value context );
	static mrb_value		_setMotor				( mrb_state* M, mrb_value context );
	static mrb_value		_setMotorSpeed			( mrb_state* M, mrb_value context );
	static mrb_value		_setMotorEnabled		( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_FACTORY ( MOAIBox2DRevoluteJoint, MOAIRubyObject )
	
	//----------------------------------------------------------------//
					MOAIBox2DRevoluteJoint		();
					~MOAIBox2DRevoluteJoint		();
	void			RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
};

#endif
