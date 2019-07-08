// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAICOLLISIONWORLD_H
#define	MOAICOLLISIONWORLD_H

#include <moai-sim/MOAIAction.h>
#include <moai-sim/MOAICollisionProp.h>
#include <moai-sim/MOAIDrawable.h>
#include <moai-sim/MOAIDrawShapeRetained.h>
#include <moai-sim/MOAIOverlap.h>
#include <moai-sim/MOAIPartition.h>

class MOAIPartitionHull;
class MOAICollisionWorld;

//================================================================//
// MOAICollisionWorld
//================================================================//
class MOAICollisionWorld :
	public virtual MOAIAction,
	public virtual MOAIDrawShapeRetained,
	public virtual MOAIPartition {
private:

	friend class MOAICollisionProp;
	friend class MOAIOverlapHandler;

	bool	mUpdated;
	u32		mOverlapPass;

	typedef ZLLeanList < MOAICollisionProp* >::Iterator ActiveListIt;
	ZLLeanList < MOAICollisionProp* > mActiveList;
	
	typedef ZLLeanList < MOAIPropOverlap* >::Iterator OverlapListIt;
	ZLLeanList < MOAIPropOverlap* > mOverlapList;
	
	ZLLeanPool < MOAIPropOverlap > mOverlapPool;

	MOAIRubyStrongRef mCallback;

	//----------------------------------------------------------------//
	static mrb_value			_insertProp				( mrb_state* M, mrb_value context );
	static mrb_value			_processOverlaps		( mrb_state* M, mrb_value context );
	static mrb_value			_setCallback			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void				AffirmOverlap			( MOAICollisionProp& prop0, u32 type0, MOAICollisionProp& prop1, u32 type1, const ZLBounds& bounds );
	void				ClearOverlap			( MOAICollisionProp& prop0, MOAICollisionProp& prop1 );
	void				ClearOverlaps			( MOAICollisionProp& prop );
	void				DoCallback				( u32 eventID, MOAICollisionProp& prop0, MOAICollisionProp& prop1 );
	void				DoCallback				( u32 eventID, MOAICollisionProp& prop0, MOAICollisionProp& prop1, const ZLBounds& bounds );
	void				HandleOverlap			( MOAICollisionProp& prop0, u32 type0, MOAICollisionProp& prop1, u32 type1, const ZLBounds& bounds );
	void				InvalidateOverlaps		( MOAICollisionProp& prop, u32 nextPass );
	void				MakeActive				( MOAICollisionProp& prop0 );
	void				MakeInactive			( MOAICollisionProp& prop0 );
	void				ProcessOverlaps			();
	void				PruneOverlaps			( MOAICollisionProp& prop );
	void				Render					();
	void				RemoveHull				( MOAICollisionProp& prop );

	//----------------------------------------------------------------//
	bool				MOAIAction_IsDone					();
	void				MOAIAction_Update					( double step );
	void				MOAIPartition_DrawDebugFront		();
	void				MOAIPartition_OnInsertHull			( MOAIPartitionHull& hull );
	void				MOAIPartition_OnRemoveHull			( MOAIPartitionHull& hull );
	void				MOAIPartition_OnUpdateHull			( MOAIPartitionHull& hull );

public:
	
	DECL_RUBY_FACTORY ( MOAICollisionWorld, MOAIAction )
	
	enum {
		OVERLAP_BEGIN,
		OVERLAP_END,
		OVERLAP_UPDATE,
	};
	
	static const u32 OVERLAP_PASS_INIT = 0;
	
	GET ( u32, CurrentPass, mOverlapPass )
	GET ( u32, NextPass, ( mOverlapPass + 1 ))
	
	//----------------------------------------------------------------//
					MOAICollisionWorld		();
					~MOAICollisionWorld		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			SerializeIn				( MOAIRubyState& state, MOAIDeserializer& serializer );
	void			SerializeOut			( MOAIRubyState& state, MOAISerializer& serializer );
};

#endif
