// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIGRIDPATHGRAPH_H
#define	MOAIGRIDPATHGRAPH_H

#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAIPathGraph.h>

class MOAIGridPathGraphParams;
class MOAIPathFinder;

//================================================================//
// MOAIGridPathGraph
//================================================================//
/**	@lua	MOAIGridPathGraph
	@text	Pathfinder graph adapter for MOAIGrid.
*/
class MOAIGridPathGraph :
	public MOAIPathGraph {
private:

	friend class MOAIPathFinder;

	enum {
		H_MOVE_COST,
		V_MOVE_COST,
		D_MOVE_COST,
	};

	MOAIRubySharedPtr < MOAIGrid > mGrid;

	//----------------------------------------------------------------//
	static mrb_value		_setGrid			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	float			ComputeHeuristic			( MOAIGridPathGraphParams& params, const MOAICellCoord& c0, const MOAICellCoord& c1 );
	void			PushNeighbor				( MOAIPathFinder& pathFinder, MOAIGridPathGraphParams& params, u32 tile0, int xTile, int yTile, float moveCost );
	void			PushNeighbors				( MOAIPathFinder& pathFinder, int nodeID );

public:
	
	enum DISTANCE {
		EUCLIDEAN_DISTANCE,
		MANHATTAN_DISTANCE,
		DIAGONAL_DISTANCE,
	};
	
	static const u32 NO_DIAGONALS = 0x00000001;
	
	DECL_RUBY_FACTORY ( MOAIGridPathGraph, MOAIRubyObject )
	
	//----------------------------------------------------------------//
					MOAIGridPathGraph			();
					~MOAIGridPathGraph			();
	void			RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void			SetGrid						( MOAIGrid* grid );
};

#endif
