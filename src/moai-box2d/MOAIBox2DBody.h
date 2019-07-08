// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIBOX2DBODY_H
#define	MOAIBOX2DBODY_H

#include <moai-box2d/MOAIBox2DWorld.h>

class b2Body;

//================================================================//
// MOAIBox2DBody
//================================================================//
/**	@lua	MOAIBox2DBody
	@text	Box2D body.

	@const	DYNAMIC
	@const	KINEMATIC
	@const	STATIC

*/
class MOAIBox2DBody :
	public MOAIBox2DPrim,
	public MOAITransformBase {
private:

	b2Body*			mBody;

	//----------------------------------------------------------------//
	static mrb_value		_addChain				( mrb_state* M, mrb_value context );
	static mrb_value		_addCircle				( mrb_state* M, mrb_value context );
	static mrb_value		_addEdges               ( mrb_state* M, mrb_value context );
	static mrb_value		_addPolygon				( mrb_state* M, mrb_value context );
	static mrb_value		_addRect				( mrb_state* M, mrb_value context );
	static mrb_value		_applyAngularImpulse	( mrb_state* M, mrb_value context );
	static mrb_value		_applyForce				( mrb_state* M, mrb_value context );
	static mrb_value		_applyLinearImpulse		( mrb_state* M, mrb_value context );
	static mrb_value		_applyTorque			( mrb_state* M, mrb_value context );
	static mrb_value		_destroy				( mrb_state* M, mrb_value context );
	static mrb_value		_getAngle				( mrb_state* M, mrb_value context );
	static mrb_value		_getAngularVelocity		( mrb_state* M, mrb_value context );
	static mrb_value 		_getContactList			( mrb_state* M, mrb_value context );
	static mrb_value		_getInertia				( mrb_state* M, mrb_value context );
	static mrb_value		_getGravityScale		( mrb_state* M, mrb_value context );
	static mrb_value		_getLinearVelocity		( mrb_state* M, mrb_value context );
	static mrb_value		_getLocalCenter			( mrb_state* M, mrb_value context );
	static mrb_value		_getMass				( mrb_state* M, mrb_value context );
	static mrb_value		_getPosition			( mrb_state* M, mrb_value context );
	static mrb_value		_getWorldCenter			( mrb_state* M, mrb_value context );
	static mrb_value		_isActive				( mrb_state* M, mrb_value context );
	static mrb_value		_isAwake				( mrb_state* M, mrb_value context );
	static mrb_value		_isBullet				( mrb_state* M, mrb_value context );
	static mrb_value		_isFixedRotation		( mrb_state* M, mrb_value context );
	static mrb_value		_resetMassData			( mrb_state* M, mrb_value context );
	static mrb_value		_setActive				( mrb_state* M, mrb_value context );
	static mrb_value		_setAngularDamping		( mrb_state* M, mrb_value context );
	static mrb_value		_setAngularVelocity		( mrb_state* M, mrb_value context );
	static mrb_value		_setAwake				( mrb_state* M, mrb_value context );
	static mrb_value		_setBullet				( mrb_state* M, mrb_value context );
	static mrb_value		_setFixedRotation		( mrb_state* M, mrb_value context );
	static mrb_value		_setGravityScale		( mrb_state* M, mrb_value context );
	static mrb_value		_setLinearDamping		( mrb_state* M, mrb_value context );
	static mrb_value		_setLinearVelocity		( mrb_state* M, mrb_value context );
	static mrb_value		_setMassData			( mrb_state* M, mrb_value context );
	static mrb_value		_setTransform			( mrb_state* M, mrb_value context );
	static mrb_value		_setType				( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	void			Clear					();
	void			SetBody					( b2Body* body );
	
	//----------------------------------------------------------------//
	bool			MOAINode_ApplyAttrOp						( u32 attrID, MOAIAttribute& attr, u32 op );
	void			MOAINode_Update								();
	void			MOAITransformBase_BuildLocalToWorldMtx		( ZLAffine3D& localToWorldMtx );

public:
	
	friend class MOAIBox2DWorld;
	
	DECL_RUBY_FACTORY ( MOAIBox2DBody, MOAINode )

	//----------------------------------------------------------------//
	void			Destroy					();
					MOAIBox2DBody			();
					~MOAIBox2DBody			();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
