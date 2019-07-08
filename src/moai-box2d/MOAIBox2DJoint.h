// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIBOX2DJOINT_H
#define	MOAIBOX2DJOINT_H

#include <moai-box2d/MOAIBox2DWorld.h>

class b2Joint;

//================================================================//
// MOAIBox2DJoint
//================================================================//
/**	@lua MOAIBox2DJoint
	@text Box2D joint.
*/
class MOAIBox2DJoint :
	public MOAIBox2DPrim {
protected:
	
	b2Joint*	mJoint;

	//----------------------------------------------------------------//
	static mrb_value			_destroy				( mrb_state* M, mrb_value context );
	static mrb_value			_getAnchorA				( mrb_state* M, mrb_value context );
	static mrb_value			_getAnchorB				( mrb_state* M, mrb_value context );
	static mrb_value			_getBodyA				( mrb_state* M, mrb_value context );
	static mrb_value			_getBodyB				( mrb_state* M, mrb_value context );
	static mrb_value			_getReactionForce		( mrb_state* M, mrb_value context );
	static mrb_value			_getReactionTorque		( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	void				Clear					();
	void				SetJoint				( b2Joint* joint );

public:
	
	friend class MOAIBox2DWorld;
	
	//----------------------------------------------------------------//
	void				Destroy					();
						MOAIBox2DJoint			();
						~MOAIBox2DJoint			();
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
