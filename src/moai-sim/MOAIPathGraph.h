// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPATHGRAPH_H
#define	MOAIPATHGRAPH_H

class MOAIPathFinder;

//================================================================//
// MOAIPathGraph
//================================================================//
class MOAIPathGraph :
	public virtual MOAIRubyObject {
protected:

	friend class MOAIPathFinder;

	u32		mHeuristic;

	//----------------------------------------------------------------//
	static mrb_value		_setHeuristic			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	virtual void	PushNeighbors			( MOAIPathFinder& pathFinder, int nodeID ) = 0;

public:

	//----------------------------------------------------------------//
					MOAIPathGraph			();
					~MOAIPathGraph			();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
