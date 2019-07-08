// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIPathFinder.h>
#include <moai-sim/MOAIVecPathGraph.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	areNeighbors
	@text	Checks if two nodes are neighbors.

	@in		MOAIVecPathGraph self
	@in		number nodeID1
	@in		number nodeID2
	@out	boolean
*/
mrb_value MOAIVecPathGraph::_areNeighbors ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVecPathGraph, "UNN" )

	u32 id1 = state.GetParamValue < u32 >( 1, 1 ) - 1;
	u32 id2 = state.GetParamValue < u32 >( 2, 1 ) - 1;

	if ( MOAILogMgr::CheckIndexPlusOne ( id1, self->mNodes.Size (), M ) &&
		MOAILogMgr::CheckIndexPlusOne ( id2, self->mNodes.Size (), M )) {

		bool result = self->AreNeighbors ( id1, id2 );

		return state.ToRValue ( result );
	}

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getNode
	@text	Returns the coordinates of a node.

	@in		MOAIVecPathGraph self
	@in		number nodeID
	@out	number x
	@out	number y
	@out	number z
*/
mrb_value MOAIVecPathGraph::_getNode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVecPathGraph, "UN" )

	u32 id = state.GetParamValue < u32 >( 1, 1 ) - 1;

	if ( MOAILogMgr::CheckIndexPlusOne ( id, self->mNodes.Size (), M )) {
		ZLVec3D vec = self->GetNode( id );

		mrb_value ret [ 3 ];
		ret [ 0 ] = state.ToRValue ( vec.mX );
		ret [ 1 ] = state.ToRValue ( vec.mY );
		ret [ 2 ] = state.ToRValue ( vec.mZ );

		return mrb_ary_new_from_values ( state, 3, ret );
	}

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getNodeCount
	@text	Returns the number of nodes in the graph.

	@in		MOAIVecPathGraph self
	@out	number count
*/
mrb_value MOAIVecPathGraph::_getNodeCount ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVecPathGraph, "U" )

	u32 count = self->GetNodeCount ();

	return state.ToRValue ( count );
}

//----------------------------------------------------------------//
/**	@lua	reserveNodes
	@text	Reserves memory for a given number of nodes.

	@in		MOAIVecPathGraph self
	@in		number nNodes
	@out	nil
*/
mrb_value MOAIVecPathGraph::_reserveNodes ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVecPathGraph, "UN" )

	u32 total = state.GetParamValue < u32 >( 1, 0 );
	self->ReserveNodes ( total );

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setNeighbors
	@text	Sets the neighborhood relation for two nodes.

	@in		MOAIVecPathGraph self
	@in		number nodeID1
	@in		number nodeID2
	@opt	boolean value			Whether the nodes are neighbors (true) or not (false). Defaults to true.
	@out	nil
*/
mrb_value MOAIVecPathGraph::_setNeighbors ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVecPathGraph, "UNN" )

	u32 id1 = state.GetParamValue < u32 >( 1, 1 ) - 1;
	u32 id2 = state.GetParamValue < u32 >( 2, 1 ) - 1;
	bool neighbors = state.GetParamValue < bool >( 3, true );

	if ( MOAILogMgr::CheckIndexPlusOne ( id1, self->mNodes.Size (), M ) &&
		MOAILogMgr::CheckIndexPlusOne ( id2, self->mNodes.Size (), M )) {

		self->SetNeighbors ( id1, id2, neighbors );
	}

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setNode
	@text	Sets the coordinates of a node.

	@in		MOAIVecPathGraph self
	@in		number nodeID
	@opt	number x				Defaults to 0.
	@opt	number y				Defaults to 0.
	@opt	number z				Defaults to 0.
	@out	nil
*/
mrb_value MOAIVecPathGraph::_setNode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVecPathGraph, "UN" )

	u32 id = state.GetParamValue < u32 >( 1, 1 ) - 1;
	float x = state.GetParamValue < float >( 2, 0.0f );
	float y = state.GetParamValue < float >( 3, 0.0f );
	float z = state.GetParamValue < float >( 4, 0.0f );

	if ( MOAILogMgr::CheckIndexPlusOne ( id, self->mNodes.Size (), M )) {
		self->SetNode ( id, ZLVec3D ( x, y, z ));
	}

	return mrb_nil_value ();
}

//================================================================//
// MOAIVecPathGraph
//================================================================//

//----------------------------------------------------------------//
bool MOAIVecPathGraph::AreNeighbors ( u32 id1, u32 id2 ) {
	
	size_t total = this->mNodes.Size ();

	if ( id1 < total && id2 < total ) {
		return this->mNeighbors [ id1 * total + id2 ];
	}

	return false;
}

//----------------------------------------------------------------//
ZLVec3D MOAIVecPathGraph::GetNode ( u32 id ) {
	
	if ( id < this->mNodes.Size ()) {
		return this->mNodes [ id ];
	}

	return ZLVec3D ( 0.0f, 0.0f, 0.0f );
}

//----------------------------------------------------------------//
u32 MOAIVecPathGraph::GetNodeCount () {
	
	return ( u32 )this->mNodes.Size ();
}

//----------------------------------------------------------------//
void MOAIVecPathGraph::PushNeighbors ( MOAIPathFinder& pathFinder, int nodeID ) {

	u32 total = ( u32 )this->mNodes.Size (); // TODO: cast

	ZLVec3D currentNode = this->GetNode ( nodeID );
	ZLVec3D targetNode = this->GetNode ( pathFinder.GetTargetNodeID ());

	for ( u32 neighborID = 0; neighborID < total; ++neighborID ) {
		if ( this->AreNeighbors ( nodeID, neighborID ) &&
			!pathFinder.IsVisited ( neighborID )) {

			ZLVec3D neighbor = this->GetNode ( neighborID );
			float g = neighbor.Dist ( currentNode ) * pathFinder.GetGWeight ();
			float h = neighbor.Dist ( targetNode ) * pathFinder.GetHWeight ();
			pathFinder.PushState ( neighborID, g, h );
		}
	}
}

//----------------------------------------------------------------//
void MOAIVecPathGraph::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIVecPathGraph::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIPathGraph::RegisterRubyFuncs ( state, klass );
	
	state.DefineInstanceMethod ( klass, "areNeighbors",			_areNeighbors, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "getNode",				_getNode, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getNodeCount",			_getNodeCount, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "reserveNodes",			_reserveNodes, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setNeighbors",			_setNeighbors, MRB_ARGS_ARG ( 2, 1 ) );
	state.DefineInstanceMethod ( klass, "setNode",				_setNode, MRB_ARGS_ARG ( 1, 3 ) );

}

//----------------------------------------------------------------//
void MOAIVecPathGraph::ReserveNodes ( u32 total ) {

	this->mNodes.Init ( total );
	this->mNeighbors.Init ( total * total );
	this->mNeighbors.Fill ( false );
}

//----------------------------------------------------------------//
void MOAIVecPathGraph::SetNeighbors ( u32 id1, u32 id2, bool value ) {
	
	size_t total = this->mNodes.Size ();

	if (( id1 < total ) && ( id2 < total )) {
		this->mNeighbors [ id1 * total + id2 ] = value;
		this->mNeighbors [ id2 * total + id1 ] = value;
	}
}

//----------------------------------------------------------------//
void MOAIVecPathGraph::SetNode ( u32 id, const ZLVec3D& node ) {
	
	if ( id < this->mNodes.Size ()) {
		this->mNodes [ id ] = node;
	}
}

//----------------------------------------------------------------//
MOAIVecPathGraph::MOAIVecPathGraph () {
	
	RTTI_SINGLE ( MOAIPathGraph )
}

//----------------------------------------------------------------//
MOAIVecPathGraph::~MOAIVecPathGraph () {

}
