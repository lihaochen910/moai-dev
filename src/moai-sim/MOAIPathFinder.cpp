// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAIGridPathGraph.h>
#include <moai-sim/MOAIPathFinder.h>
#include <moai-sim/MOAIPathGraph.h>
#include <moai-sim/MOAIPathTerrainDeck.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	findPath
	@text	Attempts to find an efficient path from the start node
			to the finish node. May be called incrementally.

	@in		MOAIPathFinder self
	@opt	number iterations
	@out	boolean more
*/
mrb_value MOAIPathFinder::_findPath ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathFinder, "U" )
	
	int iterations = state.GetParamValue < int >( 1, -1 );
	
	return state.ToRValue ( self->FindPath ( iterations ) );
}

//----------------------------------------------------------------//
/**	@lua	getGraph
	@text	Returns the attached graph (if any).

	@in		MOAIPathFinder self
	@out	MOAIPathGraph graph
*/
mrb_value MOAIPathFinder::_getGraph ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathFinder, "U" )
	
	if ( self->mGraph ) {
		return state.ToRValue < MOAIRubyObject* >( ( MOAIPathGraph* )self->mGraph );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getPathEntry
	@text	Returns a path entry. This is a node ID that may be
			passed back to the graph to get a location.

	@in		MOAIPathFinder self
	@in		number index
	@out	number entry
*/
mrb_value MOAIPathFinder::_getPathEntry ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathFinder, "UN" )
	
	u32 index = state.GetParamValue < u32 >( 1, 1 ) - 1;
	
	if ( index < self->mPath.Size ()) {
	
		return state.ToRValue ( self->mPath [ index ] + 1 );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getPathSize
	@text	Returns the size of the path (in nodes).

	@in		MOAIPathFinder self
	@out	number size
*/
mrb_value MOAIPathFinder::_getPathSize ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathFinder, "U" )
	
	return state.ToRValue ( ( u32 )self->mPath.Size () ); // TODO: overflow?
}

//----------------------------------------------------------------//
/**	@lua	init
	@text	Specify the ID of the start and target node.

	@in		MOAIPathFinder self
	@in		number startNodeID
	@in		number targetNodeID
	@out	nil
*/
mrb_value MOAIPathFinder::_init ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathFinder, "UNN" )
	
	self->mStartNodeID = state.GetParamValue < int >( 1, 1 ) - 1;
	self->mTargetNodeID = state.GetParamValue < int >( 2, 1 ) - 1;
	
	self->Reset ();
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	reserveTerrainWeights
	@text	Specify the size of the terrain weight vector. 

	@in		MOAIPathFinder self
	@opt	number size				Default value is 0.
	@out	nil
*/
mrb_value MOAIPathFinder::_reserveTerrainWeights ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathFinder, "UN" )
	
	u32 size = state.GetParamValue < u32 >( 1, 0 );
	
	self->mWeights.Init ( size );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setFlags
	@text	Set flags to use for pathfinding. These are graph specific
			flags provided by the graph implementation.

	@in		MOAIPathFinder self
	@opt	number heuristic
	@out	nil
*/
mrb_value MOAIPathFinder::_setFlags ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathFinder, "U" )

	self->mFlags = state.GetParamValue < u32 >( 1, 0 );

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setGraph
	@text	Set graph data to use for pathfinding. 

	@overload

		@in		MOAIPathFinder self
		@opt	MOAIGrid grid						Default value is nil.
		@out	nil
	
	@overload
	
		@in		MOAIPathFinder self
		@opt	MOAIGridPathGraph gridPathGraph		Default value is nil.
		@out	nil
*/
mrb_value MOAIPathFinder::_setGraph ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathFinder, "U" )

	self->mGraph.Set ( *self, 0 );

	MOAIGrid* grid = state.GetRubyObject < MOAIGrid >( 1, false );
	if ( grid ) {
		MOAIGridPathGraph* gridPathGraph = state.CreateClassInstance < MOAIGridPathGraph >();
		gridPathGraph->SetGrid ( grid );
		self->mGraph.Set ( *self, gridPathGraph );
		return mrb_nil_value ();
	}
	
	MOAIPathGraph* pathGraph = state.GetRubyObject < MOAIPathGraph >( 1, false );
	if ( pathGraph ) {
		self->mGraph.Set ( *self, pathGraph );
		return mrb_nil_value ();
	}
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setHeuristic
	@text	Set heuristic to use for pathfinding. This is a const provided
			by the graph implementation being used.

	@in		MOAIPathFinder self
	@opt	number heuristic
	@out	nil
*/
mrb_value MOAIPathFinder::_setHeuristic ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathFinder, "U" )

	self->mHeuristic = state.GetParamValue < u32 >( 1, 0 );

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setTerrainDeck
	@text	Set terrain deck to use with graph.

	@in		MOAIPathFinder self
	@opt	MOAIPathTerrainDeck terrainDeck		Default value is nil.
	@out	nil
*/
mrb_value MOAIPathFinder::_setTerrainDeck ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathFinder, "U" )
	
	self->mTerrainDeck.Set ( *self, state.GetRubyObject < MOAIPathTerrainDeck >( 1, true ));
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setTerrainMask
	@text	Set 32-bit mask to apply to terrain samples.

	@in		MOAIPathFinder self
	@opt	number mask					Default value is 0xffffffff.
	@out	nil
*/
mrb_value MOAIPathFinder::_setTerrainMask ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathFinder, "U" )

	self->mMask = state.GetParamValue < u32 >( 1, 0xffffffff );

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setTerrainWeight
	@text	Set a component of the terrain weight vector.

	@in		MOAIPathFinder self
	@in		number index
	@opt	number deltaScale			Default value is 0.
	@opt	number penaltyScale			Default value is 0.
	@out	nil
*/
mrb_value MOAIPathFinder::_setTerrainWeight ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathFinder, "UNNN" )
	
	u32 index = state.GetParamValue < u32 >( 1, 1 ) - 1;

	if ( index < self->mWeights.Size ()) {
	
		MOAIPathWeight& weight = self->mWeights [ index ];
		weight.mDeltaScale = state.GetParamValue < float >( 2, 0.0f );
		weight.mPenaltyScale = state.GetParamValue < float >( 3, 0.0f );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setWeight
	@text	Sets weights to be applied to G and H.

	@in		MOAIPathFinder self
	@opt	number gWeight				Default value is 1.0.
	@opt	number hWeight				Default value is 1.0.
	@out	nil
*/
mrb_value MOAIPathFinder::_setWeight ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathFinder, "U" )
	
	self->mGWeight = state.GetParamValue < float >( 1, 1.0f );
	self->mHWeight = state.GetParamValue < float >( 2, 1.0f );
	
	return mrb_nil_value ();
}

//================================================================//
// MOAIPathFinder
//================================================================//

//----------------------------------------------------------------//
void MOAIPathFinder::BuildPath ( MOAIPathState* state ) {

	u32 size = 0;
	for ( MOAIPathState* cursor = state; cursor; cursor = cursor->mParent, ++size );
	
	this->mPath.Init ( size );
	for ( MOAIPathState* cursor = state; cursor; cursor = cursor->mParent ) {
		this->mPath [ --size ] = cursor->mNodeID;
	}
	
	this->ClearVisitation ();
}

//----------------------------------------------------------------//
void MOAIPathFinder::ClearVisitation () {

	while ( this->mOpen ) {
		MOAIPathState* state = this->mOpen;
		this->mOpen = this->mOpen->mNext;
		delete state;
	}
	
	while ( this->mClosed ) {
		MOAIPathState* state = this->mClosed;
		this->mClosed = this->mClosed->mNext;
		delete state;
	}
}

//----------------------------------------------------------------//
void MOAIPathFinder::CloseState ( MOAIPathState* stateToClose ) {
	
	MOAIPathState* cursor = this->mOpen;
	this->mOpen = 0;
	
	while ( cursor ) {
		MOAIPathState* state = cursor;
		cursor = cursor->mNext;
		
		if ( state == stateToClose ) {
			state->mNext = this->mClosed;
			this->mClosed = state;
		}
		else {
			state->mNext = this->mOpen;
			this->mOpen = state;
		}
	}
}

//----------------------------------------------------------------//
bool MOAIPathFinder::CheckMask ( u32 terrain ) {

	if ( !terrain || ( terrain & MOAITileFlags::HIDDEN )) return false;

	if ( this->mTerrainDeck ) {
		return ( this->mMask & this->mTerrainDeck->GetMask ( terrain & MOAITileFlags::CODE_MASK ) ) ? true : false;
	}
	return true;
}

//----------------------------------------------------------------//
float MOAIPathFinder::ComputeTerrainCost ( float moveCost, u32 terrain0, u32 terrain1 ) {

	if ( !this->mTerrainDeck ) return 0.0f;

	if ( terrain0 & MOAITileFlags::HIDDEN ) return 0.0f;
	if ( terrain1 & MOAITileFlags::HIDDEN ) return 0.0f;

	u32 total = MIN ( this->mTerrainDeck->GetVectorSize (), ( u32 )this->mWeights.Size ());
	
	float* v0 = this->mTerrainDeck->GetVector ( terrain0 & MOAITileFlags::CODE_MASK );
	float* v1 = this->mTerrainDeck->GetVector ( terrain1 & MOAITileFlags::CODE_MASK );
	
	float terrainCost = 0.0f;
	for ( u32 i = 0; i < total; ++i ) {
		
		const MOAIPathWeight& weight = this->mWeights [ i ];
		
		float c0 = v0 [ i ];
		float c1 = v1 [ i ];
		
		float delta = ( c1 - c0 ) * weight.mDeltaScale;
		float penalty = ( c1 + c0 ) * 0.5f * weight.mPenaltyScale;
		
		terrainCost += ( moveCost * delta ) + ( moveCost * penalty );
	}
	return terrainCost;
}

//----------------------------------------------------------------//
bool MOAIPathFinder::FindPath ( int iterations ) {
	
	if ( !this->mState ) {
		this->PushState ( this->mStartNodeID, 0.0f, 0.0f );
	}
	
	bool noIterations = iterations <= 0;
	
	for ( ; this->mOpen && (( iterations > 0 ) || noIterations ); iterations-- ) {
		
		this->mState = this->NextState ();

		if ( this->mState->mNodeID == this->mTargetNodeID ) {
			this->BuildPath ( this->mState );
			return false;
		}

		this->CloseState ( this->mState );
		this->mGraph->PushNeighbors ( *this, this->mState->mNodeID );
	}
	return this->mOpen ? true : false;
}

//----------------------------------------------------------------//
bool MOAIPathFinder::IsVisited ( int nodeID ) {

	for ( MOAIPathState* state = this->mOpen; state; state = state->mNext ) {
		if ( state->mNodeID == nodeID ) return true;
	}
	
	for ( MOAIPathState* state = this->mClosed; state; state = state->mNext ) {
		if ( state->mNodeID == nodeID ) return true;
	}
	
	return false;
}

//----------------------------------------------------------------//
MOAIPathFinder::MOAIPathFinder () :
	mOpen ( 0 ),
	mClosed ( 0 ),
	mStartNodeID ( 0 ),
	mTargetNodeID ( 0 ),
	mState ( 0 ),
	mMask ( 0xffffffff ),
	mHeuristic ( 0 ),
	mFlags ( 0 ),
	mGWeight ( 1.0f ),
	mHWeight ( 1.0f ) {
	
	RTTI_SINGLE ( MOAIRubyObject )
}

//----------------------------------------------------------------//
MOAIPathFinder::~MOAIPathFinder () {

	this->ClearVisitation ();
	
	this->mGraph.Set ( *this, 0 );
	this->mTerrainDeck.Set ( *this, 0 );
}

//----------------------------------------------------------------//
MOAIPathState* MOAIPathFinder::NextState () {

	MOAIPathState* state = this->mOpen;
	MOAIPathState* best = state;
	state = state->mNext;

	for ( ; state; state = state->mNext ) {
		if ( state->mEstimatedScore < best->mEstimatedScore ) {
			best = state;
		}
	}
	return best;
}

//----------------------------------------------------------------//
void MOAIPathFinder::PushState ( int nodeID, float cost, float estimate ) {
	
	MOAIPathState* state = new MOAIPathState ();
	state->mNodeID = nodeID;
	state->mParent = this->mState;

	state->mNext = this->mOpen;
	this->mOpen = state;
	
	float parentScore = state->mParent ? state->mParent->mCumulatedScore : 0.0f;
	state->mCumulatedScore = parentScore + cost;
	state->mEstimatedScore = state->mCumulatedScore + estimate;
}

//----------------------------------------------------------------//
void MOAIPathFinder::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIPathFinder::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "findPath",					_findPath, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "getGraph",					_getGraph, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getPathEntry",				_getPathEntry, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getPathSize",				_getPathSize, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "init",						_init, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "reserveTerrainWeights",	_reserveTerrainWeights, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setFlags",					_setFlags, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setGraph",					_setGraph, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setHeuristic",				_setHeuristic, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setTerrainDeck",			_setTerrainDeck, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setTerrainMask",			_setTerrainMask, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setTerrainWeight",			_setTerrainWeight, MRB_ARGS_ARG ( 1, 2 ) );
	state.DefineInstanceMethod ( klass, "setWeight",				_setWeight, MRB_ARGS_ARG ( 0, 2 ) );

}

//----------------------------------------------------------------//
void MOAIPathFinder::Reset () {

	this->mState = 0;
	this->mPath.Clear ();

	this->ClearVisitation ();
}
