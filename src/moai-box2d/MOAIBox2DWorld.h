// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIBOX2DWORLD_H
#define	MOAIBOX2DWORLD_H

#include <Box2D/Box2D.h>

class b2World;
class MOAIBox2DArbiter;
class MOAIBox2DBody;
class MOAIBox2DDebugDraw;
class MOAIBox2DFixture;
class MOAIBox2DJoint;
class MOAIBox2DWorld;
class MOAIBox2DRayCastCallback;

//================================================================//
// MOAIBox2DPrim
//================================================================//
class MOAIBox2DPrim :
	public virtual MOAIRubyObject  {
protected:

	MOAIBox2DWorld*		mWorld;
	
	bool				mDestroy;
	MOAIBox2DPrim*		mDestroyNext;

	//----------------------------------------------------------------//
	
public:

	friend class MOAIBox2DWorld;

	GET_SET ( MOAIBox2DWorld*, World, mWorld )

	//----------------------------------------------------------------//
	virtual void	Destroy					() = 0;
	float			GetUnitsToMeters		();
					MOAIBox2DPrim			();
	
	//----------------------------------------------------------------//
	inline bool IsValid () {
		return this->mWorld != NULL;
	}
};

//================================================================//
// MOAIBox2DWorld
//================================================================//
/**	@lua	MOAIBox2DWorld
	@text	Box2D world.
	
	@const DEBUG_DRAW_SHAPES
	@const DEBUG_DRAW_JOINTS
	@const DEBUG_DRAW_BOUNDS
	@const DEBUG_DRAW_PAIRS
	@const DEBUG_DRAW_CENTERS
	@const DEBUG_DRAW_DEFAULT
*/
class MOAIBox2DWorld :
	public MOAIAction,
	public MOAIDrawable,
	public b2DestructionListener {
private:

	bool						mLock;

	b2World*					mWorld;
	MOAIBox2DDebugDraw*			mDebugDraw;
	
	MOAIRubySharedPtr < MOAIBox2DArbiter > mArbiter;

	u32		mVelocityIterations;
	u32		mPositionIterations;
	
	float	mUnitsToMeters; // maps world space units to meters

	MOAIBox2DPrim*		mDestroyBodies;
	MOAIBox2DPrim*		mDestroyFixtures;
	MOAIBox2DPrim*		mDestroyJoints;

	//----------------------------------------------------------------//
	static mrb_value		_addBody					( mrb_state* M, mrb_value context );
	static mrb_value		_addDistanceJoint			( mrb_state* M, mrb_value context );
	static mrb_value		_addFrictionJoint			( mrb_state* M, mrb_value context );
	static mrb_value		_addGearJoint				( mrb_state* M, mrb_value context );
	static mrb_value 		_addMotorJoint				( mrb_state* M, mrb_value context );
	static mrb_value		_addMouseJoint				( mrb_state* M, mrb_value context );
	static mrb_value		_addPrismaticJoint			( mrb_state* M, mrb_value context );
	static mrb_value		_addPulleyJoint				( mrb_state* M, mrb_value context );
	static mrb_value		_addRevoluteJoint			( mrb_state* M, mrb_value context );
	static mrb_value		_addRevoluteJointLocal		( mrb_state* M, mrb_value context );
	static mrb_value		_addRopeJoint				( mrb_state* M, mrb_value context );
	static mrb_value		_addWeldJoint				( mrb_state* M, mrb_value context );
	static mrb_value		_addWheelJoint				( mrb_state* M, mrb_value context );
	static mrb_value		_getAngularSleepTolerance	( mrb_state* M, mrb_value context );
	static mrb_value		_getAutoClearForces			( mrb_state* M, mrb_value context );
	static mrb_value		_getGravity					( mrb_state* M, mrb_value context );
	static mrb_value		_getLinearSleepTolerance	( mrb_state* M, mrb_value context );
	static mrb_value		_getPerformance				( mrb_state* M, mrb_value context );
	static mrb_value		_getRayCast					( mrb_state* M, mrb_value context );
	static mrb_value		_getTimeToSleep				( mrb_state* M, mrb_value context );
	static mrb_value		_setAngularSleepTolerance	( mrb_state* M, mrb_value context );
	static mrb_value		_setAutoClearForces			( mrb_state* M, mrb_value context );
	static mrb_value		_setDebugDrawEnabled		( mrb_state* M, mrb_value context );
	static mrb_value		_setDebugDrawFlags			( mrb_state* M, mrb_value context );
	static mrb_value		_setGravity					( mrb_state* M, mrb_value context );
	static mrb_value		_setIterations				( mrb_state* M, mrb_value context );
	static mrb_value		_setLinearSleepTolerance	( mrb_state* M, mrb_value context );
	static mrb_value		_setTimeToSleep				( mrb_state* M, mrb_value context );
	static mrb_value		_setUnitsToMeters			( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	void			Destroy					();
	void			SayGoodbye				( b2Fixture* fixture ); 
	void			SayGoodbye				( b2Joint* joint );
	void			ScheduleDestruction		( MOAIBox2DBody& body );
	void			ScheduleDestruction		( MOAIBox2DFixture& fixture );
	void			ScheduleDestruction		( MOAIBox2DJoint& joint );

	//----------------------------------------------------------------//
	bool			MOAIAction_IsDone		();
	void			MOAIAction_Update		( double step );
	void			MOAIDrawable_Draw		( int subPrimID );

public:
	
	friend class MOAIBox2DBody;
	friend class MOAIBox2DFixture;
	friend class MOAIBox2DJoint;
	
	DECL_RUBY_FACTORY ( MOAIBox2DWorld, MOAIAction )
	
	GET_SET ( float, UnitsToMeters, mUnitsToMeters )
	GET_CONST ( float, UnitsToMeters, mUnitsToMeters )
	
	// aliases for box2D bitflags
	static const u32 DEBUG_DRAW_SHAPES		= b2Draw::e_shapeBit;
	static const u32 DEBUG_DRAW_JOINTS		= b2Draw::e_jointBit;
	static const u32 DEBUG_DRAW_BOUNDS		= b2Draw::e_aabbBit;
	static const u32 DEBUG_DRAW_PAIRS		= b2Draw::e_pairBit;
	static const u32 DEBUG_DRAW_CENTERS		= b2Draw::e_centerOfMassBit;
	
	static const u32 DEBUG_DRAW_DEFAULT = DEBUG_DRAW_SHAPES | DEBUG_DRAW_JOINTS | DEBUG_DRAW_CENTERS;
	
	//----------------------------------------------------------------//
	bool			IsLocked				();
					MOAIBox2DWorld			();
					~MOAIBox2DWorld			();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
