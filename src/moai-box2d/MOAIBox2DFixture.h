// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIBOX2DFIXTURE_H
#define	MOAIBOX2DFIXTURE_H

#include <moai-box2d/MOAIBox2DWorld.h>

class b2Fixture;
class MOAIBox2DArbiter;

//================================================================//
// MOAIBox2DFixture
//================================================================//
/**	@lua MOAIBox2DFixture
	@text Box2D fixture.
*/
class MOAIBox2DFixture :
	public MOAIBox2DPrim {
private:

	b2Fixture*			mFixture;

	MOAIRubyStrongRef	mCollisionHandler;
	u32					mCollisionPhaseMask;
	u32					mCollisionCategoryMask;

	//----------------------------------------------------------------//
	static mrb_value	_destroy				( mrb_state* M, mrb_value context );
	static mrb_value	_getBody				( mrb_state* M, mrb_value context );
	static mrb_value	_getFilter				( mrb_state* M, mrb_value context );
	static mrb_value	_setCollisionHandler	( mrb_state* M, mrb_value context );
	static mrb_value	_setDensity				( mrb_state* M, mrb_value context );
	static mrb_value	_setFilter				( mrb_state* M, mrb_value context );
	static mrb_value	_setFriction			( mrb_state* M, mrb_value context );
	static mrb_value	_setRestitution			( mrb_state* M, mrb_value context );
	static mrb_value	_setSensor				( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	void			Clear				();
	void			HandleCollision		( u32 eventType, MOAIBox2DFixture* other, MOAIBox2DArbiter* arbiter );
	static u32		LoadVerts			( MOAIRubyState& state, int idx, b2Vec2* verts, u32 max, float unitsToMeters  );
	void			SetFixture			( b2Fixture* fixture );


public:
	
	friend class MOAIBox2DArbiter;
	friend class MOAIBox2DBody;
	friend class MOAIBox2DWorld;
	
	DECL_RUBY_FACTORY ( MOAIBox2DFixture, MOAIRubyObject )
	
	//----------------------------------------------------------------//
	void			Destroy					();
					MOAIBox2DFixture		();
					~MOAIBox2DFixture		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
