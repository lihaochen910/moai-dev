// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPATHFINDER_H
#define	MOAIPATHFINDER_H

class MOAIGrid;
class MOAIPathGraph;
class MOAIPathTerrainDeck;

//================================================================//
// MOAIPathWeight
//================================================================//
class MOAIPathWeight {
public:

	float			mDeltaScale;
	float			mPenaltyScale;
};

//================================================================//
// MOAIPathState
//================================================================//
class MOAIPathState {
private:

	friend class MOAIPathFinder;

	int					mNodeID;
	MOAIPathState*		mParent;
	MOAIPathState*		mNext;
	
	float				mCumulatedScore;
	float				mEstimatedScore;
};

//================================================================//
// MOAIPathFinder
//================================================================//
/**	@lua	MOAIPathFinder
	@text	Object for maintaining pathfinding state.
*/
class MOAIPathFinder :
	public virtual MOAIRubyObject {
private:

	friend class MOAIPathGraph;

	MOAIRubySharedPtr < MOAIPathTerrainDeck > mTerrainDeck;
	MOAIRubySharedPtr < MOAIPathGraph > mGraph;

	ZLLeanArray < MOAIPathWeight > mWeights;
	ZLLeanArray < int >	mPath;

	// TODO: optimize implementation with memory pool of path states
	// and binary heap of open paths

	MOAIPathState*		mOpen;
	MOAIPathState*		mClosed;
	
	int					mStartNodeID;
	int					mTargetNodeID;

	MOAIPathState*		mState; // used while expanding open set

	u32					mMask;

	u32					mHeuristic;
	u32					mFlags;

	float				mGWeight;
	float				mHWeight;

	//----------------------------------------------------------------//
	static mrb_value			_findPath					( mrb_state* M, mrb_value context );
	static mrb_value			_getGraph					( mrb_state* M, mrb_value context );
	static mrb_value			_getPathEntry				( mrb_state* M, mrb_value context );
	static mrb_value			_getPathSize				( mrb_state* M, mrb_value context );
	static mrb_value			_init						( mrb_state* M, mrb_value context );
	static mrb_value			_reserveTerrainWeights		( mrb_state* M, mrb_value context );
	static mrb_value			_setFlags					( mrb_state* M, mrb_value context );
	static mrb_value			_setGraph					( mrb_state* M, mrb_value context );
	static mrb_value			_setHeuristic				( mrb_state* M, mrb_value context );
	static mrb_value			_setTerrainMask				( mrb_state* M, mrb_value context );
	static mrb_value			_setTerrainDeck				( mrb_state* M, mrb_value context );
	static mrb_value			_setTerrainWeight			( mrb_state* M, mrb_value context );
	static mrb_value			_setWeight					( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void				BuildPath			( MOAIPathState* state );
	void				ClearVisitation		();
	void				CloseState			( MOAIPathState* stateToClose );
	MOAIPathState*		NextState			();
	void				Reset				();

public:
	
	DECL_RUBY_FACTORY ( MOAIPathFinder, MOAIRubyObject )
	
	GET ( const ZLLeanArray < MOAIPathWeight >&, Weights, mWeights );
	GET ( int, TargetNodeID, mTargetNodeID );
	GET ( u32, Mask, mMask );
	GET ( u32, Heuristic, mHeuristic )
	GET ( u32, Flags, mFlags )
	GET ( float, GWeight, mGWeight )
	GET ( float, HWeight, mHWeight )
	
	//----------------------------------------------------------------//
	bool		CheckMask				( u32 terrain );
	float		ComputeTerrainCost		( float moveCost, u32 terrain0, u32 terrain1 );
	bool		FindPath				( int iterations );
	bool		IsVisited				( int nodeID );
				MOAIPathFinder			();
				~MOAIPathFinder			();
	void		PushState				( int nodeID, float cost, float estimate );
	void		RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void		RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
