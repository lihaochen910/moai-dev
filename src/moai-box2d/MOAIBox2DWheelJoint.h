// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIBOX2DWHEELJOINT_H
#define	MOAIBOX2DWHEELJOINT_H

#include <moai-box2d/MOAIBox2DJoint.h>

//================================================================//
// MOAIBox2DWheelJoint
//================================================================//
/**	@lua MOAIBox2DWheelJoint
	@text Box2D wheel joint.
 */
class MOAIBox2DWheelJoint :
public MOAIBox2DJoint {
private:
	
	//----------------------------------------------------------------//
	static mrb_value		_getJointTranslation	( mrb_state* M, mrb_value context );
	static mrb_value		_getJointSpeed			( mrb_state* M, mrb_value context );
	static mrb_value		_isMotorEnabled			( mrb_state* M, mrb_value context );	
	static mrb_value		_getMotorSpeed			( mrb_state* M, mrb_value context );
	static mrb_value		_getSpringDampingRatio	( mrb_state* M, mrb_value context );	
	static mrb_value		_getMaxMotorTorque		( mrb_state* M, mrb_value context );
	static mrb_value		_getMotorTorque			( mrb_state* M, mrb_value context );	
	static mrb_value		_getSpringFrequencyHz	( mrb_state* M, mrb_value context );
	static mrb_value		_setMotor				( mrb_state* M, mrb_value context );
	static mrb_value		_setMotorSpeed			( mrb_state* M, mrb_value context );
	static mrb_value		_setMotorEnabled		( mrb_state* M, mrb_value context );
	static mrb_value		_setMaxMotorTorque		( mrb_state* M, mrb_value context );	
	static mrb_value		_setSpringDampingRatio	( mrb_state* M, mrb_value context );
	static mrb_value		_setSpringFrequencyHz	( mrb_state* M, mrb_value context );

	
public:
	
	DECL_RUBY_FACTORY ( MOAIBox2DWheelJoint, MOAIRubyObject )

	//----------------------------------------------------------------//
					MOAIBox2DWheelJoint		();
					~MOAIBox2DWheelJoint	();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
