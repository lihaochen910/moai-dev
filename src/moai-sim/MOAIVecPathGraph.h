// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIVECPATHGRAPH_H
#define	MOAIVECPATHGRAPH_H

#include <moai-sim/MOAIPathGraph.h>

class MOAIPathFinder;

class MOAIVecPathGraph :
	public MOAIPathGraph {
private:

	friend class MOAIPathFinder;
	
	ZLLeanArray < ZLVec3D >	mNodes;
	ZLLeanArray < bool >	mNeighbors;
	
	//----------------------------------------------------------------//
	static mrb_value		_areNeighbors		( mrb_state* M, mrb_value context );
	static mrb_value		_getNode			( mrb_state* M, mrb_value context );
	static mrb_value		_getNodeCount		( mrb_state* M, mrb_value context );
	static mrb_value		_reserveNodes		( mrb_state* M, mrb_value context );
	static mrb_value		_setNeighbors		( mrb_state* M, mrb_value context );
	static mrb_value		_setNode			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	bool			AreNeighbors				( u32 id1, u32 id2 );
	ZLVec3D			GetNode						( u32 id );
	u32				GetNodeCount				();
	void			PushNeighbors				( MOAIPathFinder& pathFinder, int nodeID );
	void			ReserveNodes				( u32 total );
	void			SetNeighbors				( u32 id1, u32 id2, bool value );
	void			SetNode						( u32 id, const ZLVec3D& node );

public:
	
	DECL_RUBY_FACTORY ( MOAIVecPathGraph, MOAIRubyObject )
	
	//----------------------------------------------------------------//
					MOAIVecPathGraph			();
					~MOAIVecPathGraph			();
	void			RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
};

#endif
