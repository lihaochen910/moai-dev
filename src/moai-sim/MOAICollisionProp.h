// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAICOLLISIONFACET_H
#define	MOAICOLLISIONFACET_H

#include <moai-sim/MOAICollisionPrim.h>
#include <moai-sim/MOAIIndexedPropBase.h>
#include <moai-sim/MOAIPartitionHull.h>

class MOAICollisionShape;
class MOAICollisionProp;
class MOAICollisionWorld;
class MOAIContactPoint2D;
class MOAIContactPointAccumulator2D;
class MOAIAbstractDrawShape;
class MOAIMoveConstraint2D;
class MOAIOverlapHandler;
class MOAIPropOverlap;
class MOAIVectorAccumulator;

//================================================================//
// MOAIPropOverlapLink
//================================================================//
class MOAIPropOverlapLink {
private:

	friend class MOAICollisionProp;
	friend class MOAICollisionWorld;

	u32						mType;
	MOAICollisionProp*		mProp;
	MOAIPropOverlapLink*	mOtherLink;
	MOAIPropOverlap*		mOverlap;
	MOAIPropOverlapLink*	mNext;
};

//================================================================//
// MOAIPropOverlap
//================================================================//
class MOAIPropOverlap {
private:

	friend class MOAICollisionProp;
	friend class MOAICollisionWorld;

	MOAIPropOverlapLink		mLeft;
	MOAIPropOverlapLink		mRight;
	
	ZLBounds				mBounds;
	
	bool					mIsValid;
	
	ZLLeanLink < MOAIPropOverlap* >	mOverlapListLink;
};

//================================================================//
// MOAICollisionProp
//================================================================//
// TODO: doxygen
class MOAICollisionProp :
	public virtual MOAIPartitionHull,
	public virtual MOAIDrawable,
	public virtual MOAIIndexedPropBase {
private:
	
	friend class MOAICollisionWorld;
	friend class MOAIOverlapHandler;
	
	static const u32 DEFAULT_MAX_MOVE_STEPS = 8;
	
	u32									mCategory;			// type flags for collision object
	u32									mMask;				// mask of type flags this object collides with
	
	u32									mOverlapFlags;
	u32									mOverlapPass;		// used to identify if prop has been processed in current cycle
	MOAIPropOverlapLink*				mOverlapLinks;		// singly-linked list of links to overlaps with this prop (if caching)
	
	ZLLeanLink < MOAICollisionProp* >	mActiveListLink;	// link in collision world's list of props with overlaps or in need of update
	
	bool								mStayActive;
	u32									mTouched;			// only for debug drawing
	
	MOAICollisionWorld*					mCollisionWorld;
	
	//----------------------------------------------------------------//
	static mrb_value				_collisionMove			( mrb_state* M, mrb_value context );
	static mrb_value				_getOverlaps			( mrb_state* M, mrb_value context );
	static mrb_value				_hasOverlaps			( mrb_state* M, mrb_value context );
	//static mrb_value				_setGroupMask			( mrb_state* M, mrb_value context );
	static mrb_value				_setOverlapFlags		( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	void					ClearOverlapLink		( MOAIPropOverlap& overlap );
	static void				DrawContactPoints		( MOAIAbstractDrawShape& draw, const MOAIMoveConstraint2D* contacts, u32 nContacts );
	void					GatherAndProcess		( MOAICollisionPrimVisitor& visitor, const ZLBox& worldBounds );
	bool					IsActive				();
	
	//----------------------------------------------------------------//
	void					MOAIDrawable_DrawDebug							( int subPrimID );
	bool					MOAINode_ApplyAttrOp							( u32 attrID, MOAIAttribute& attr, u32 op );
	void					MOAINode_Update									();
	void					MOAIPartitionHull_AddToSortBuffer				( MOAIPartitionResultBuffer& buffer, u32 key = 0 );
	u32						MOAIPartitionHull_AffirmInterfaceMask			( MOAIPartition& partition );
	ZLBounds				MOAIPartitionHull_GetModelBounds				();
	bool					MOAIPartitionHull_PrepareForInsertion			( const MOAIPartition& partition );
	void					MOAIPartitionHull_WasRemovedFromPartition		();
	
public:

	enum {
		DEBUG_DRAW_COLLISION_ACTIVE_PROP_BOUNDS,
		DEBUG_DRAW_COLLISION_ACTIVE_OVERLAP_PROP_BOUNDS,
		DEBUG_DRAW_COLLISION_ACTIVE_TOUCHED_PROP_BOUNDS,
		
		DEBUG_DRAW_COLLISION_CONTACT_NORMAL,
		DEBUG_DRAW_COLLISION_CONTACT_POINT_CORNER,
		DEBUG_DRAW_COLLISION_CONTACT_POINT_CORNER_EDGE_NORMAL,
		DEBUG_DRAW_COLLISION_CONTACT_POINT_CORNER_TANGENT,
		DEBUG_DRAW_COLLISION_CONTACT_POINT_CROSSING,
		DEBUG_DRAW_COLLISION_CONTACT_POINT_LEAVING,
		DEBUG_DRAW_COLLISION_CONTACT_POINT_PARALLEL,
		
		DEBUG_DRAW_COLLISION_MOVE_RETICLE,
		DEBUG_DRAW_COLLISION_OVERLAP_PROP_BOUNDS,
		DEBUG_DRAW_COLLISION_OVERLAPS,
		DEBUG_DRAW_COLLISION_WORLD_BOUNDS,
		
		TOTAL_DEBUG_LINE_STYLES,
	};

	enum {
		SURFACE_MOVE_DETACH,
		SURFACE_MOVE_SLIDE,
		SURFACE_MOVE_LOCK,
	};

	DECL_RUBY_FACTORY ( MOAICollisionProp, MOAITransform )

	static const u32 OVERLAP_ENABLE					= 0x01;		// prop behaves as collision region
	static const u32 OVERLAP_EVENTS_ON_UPDATE		= 0x02;		// will send overlap update events
	static const u32 OVERLAP_EVENTS_CONTINUOUS		= 0x04;		// both props will stay active as long as there's an overlap
	static const u32 OVERLAP_EVENTS_LIFECYCLE		= 0x08;		// will create overlap links and receive begin/end events
	static const u32 OVERLAP_GRANULARITY_FINE		= 0x10;		// will use higher resolution primitives if available
	static const u32 OVERLAP_CALCULATE_BOUNDS		= 0x20;		// will calculate bounds instead of just a point between both props

	static const u32 CATEGORY_MASK_ALL				= 0xffffffff;

	static const u32 DEFAULT_OVERLAP_FLAGS			= 0;

	//----------------------------------------------------------------//
	MOAICollisionShape*			GetCollisionShape				();
	void						Move							( ZLVec3D move, u32 detach, u32 maxSteps );
								MOAICollisionProp				();
	virtual						~MOAICollisionProp				();
	void						RegisterRubyClass				( MOAIRubyState& state, RClass* klass );
	void						RegisterRubyFuncs				( MOAIRubyState& state, RClass* klass );
	void						SerializeIn						( MOAIRubyState& state, MOAIDeserializer& serializer );
	void						SerializeOut					( MOAIRubyState& state, MOAISerializer& serializer );
};

#endif
