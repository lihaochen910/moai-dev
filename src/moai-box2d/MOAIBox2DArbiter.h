// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIBOX2DARBITER_H
#define	MOAIBOX2DARBITER_H

#include <Box2D/Box2D.h>

// Forward declaration
class MOAIBox2DWorld;

//================================================================//
// MOAIBox2DArbiter
//================================================================//
/**	@lua	MOAIBox2DArbiter
	@text	Box2D Arbiter.
	
	@flag	BEGIN
	@flag	END
	@flag	POST_SOLVE
	@flag	PRE_SOLVE
	@flag	ALL
*/
class MOAIBox2DArbiter :
	public virtual MOAIRubyObject,
	public b2ContactListener {
private:

	b2Contact*					mContact;
	const b2ContactImpulse*		mImpulse;

	b2Vec2  mContactNormal;
	b2Vec2	mContactPoints [ 2 ];
	u32		mTotalPoints;
	float	mNormalImpulse;
	float	mTangentImpulse;
	bool	mContactDirty;

	/* For reference to get the unitsToMeters value */
	const MOAIBox2DWorld*       mWorld;

	//----------------------------------------------------------------//
	static mrb_value		_getContactNormal		( mrb_state* M, mrb_value context );
	static mrb_value		_getContactPoints		( mrb_state* M, mrb_value context );
	static mrb_value		_getNormalImpulse		( mrb_state* M, mrb_value context );
	static mrb_value		_getTangentImpulse		( mrb_state* M, mrb_value context );
	static mrb_value		_setContactEnabled		( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	void	AffirmContactData ();
	void	BeginContact	( b2Contact* contact );
	void	EndContact		( b2Contact* contact );
	void	PostSolve		( b2Contact* contact, const b2ContactImpulse* impulse );
	void	PreSolve		( b2Contact* contact, const b2Manifold* oldManifold );

	//----------------------------------------------------------------//
	float   GetUnitsToMeters ( ) const;

public:
	
	DECL_RUBY_FACTORY ( MOAIBox2DArbiter, MOAIRubyObject )
	
	enum {
		BEGIN			= 0x00000001,
		END				= 0x00000002,
		POST_SOLVE		= 0x00000004,
		PRE_SOLVE		= 0x00000008,
		ALL				= 0x0000000f,
	};
	
	//----------------------------------------------------------------//
					MOAIBox2DArbiter		();
					MOAIBox2DArbiter        ( const MOAIBox2DWorld &world );
					~MOAIBox2DArbiter		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			SetWorld				( const MOAIBox2DWorld& world );
};

#endif
