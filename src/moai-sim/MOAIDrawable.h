// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIDRAWABLE_H
#define	MOAIDRAWABLE_H

#include <moai-sim/MOAIPartitionHull.h>

//================================================================//
// MOAIDrawable
//================================================================//
class MOAIDrawable :
	public virtual RTTIBase {
private:

	//----------------------------------------------------------------//
	virtual void	MOAIDrawable_Draw					( int subPrimID );
	virtual void	MOAIDrawable_DrawDebug				( int subPrimID );

public:

	//----------------------------------------------------------------//
	static void		Draw				( MOAIRubyMemberRef& ref, bool debug = false );
	static void		Draw				( MOAIRubyState& state, int idx, bool debug = false );
	static void		Draw				( MOAIRubyState& state, mrb_value val, bool debug = false );
	void			Draw				( int subPrimID = MOAIPartitionHull::NO_SUBPRIM_ID ) { MOAIDrawable_Draw ( subPrimID ); }
	void			DrawDebug			( int subPrimID = MOAIPartitionHull::NO_SUBPRIM_ID ) { MOAIDrawable_DrawDebug ( subPrimID ); }
};

#endif
