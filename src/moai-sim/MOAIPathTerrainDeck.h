// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPATHTERRAINDECK_H
#define	MOAIPATHTERRAINDECK_H

#include <moai-sim/MOAIGridSpace.h>
#include <moai-sim/MOAIPathFinder.h>

//================================================================//
// MOAIPathTerrainDeck
//================================================================//
/**	@lua	MOAIPathTerrainDeck
	@text	Terrain specifications for use with pathfinding graphs. Contains
			indexed terrain types for graph nodes.
*/
class MOAIPathTerrainDeck :
	public MOAIRubyObject {
private:

	friend class MOAIPathFinder;

	ZLLeanArray < u32 >		mMasks;
	ZLLeanArray < float >	mVectors;
	
	u32						mDeckSize;
	u32						mVectorSize;

	//----------------------------------------------------------------//
	static mrb_value		_getMask			( mrb_state* M, mrb_value context );
	static mrb_value		_getTerrainVec		( mrb_state* M, mrb_value context );
	static mrb_value		_setMask			( mrb_state* M, mrb_value context );
	static mrb_value		_setTerrainVec		( mrb_state* M, mrb_value context );
	static mrb_value		_reserve			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	u32				GetMask						( u32 idx );
	float*			GetVector					( u32 idx );

public:
	
	DECL_RUBY_FACTORY ( MOAIPathTerrainDeck, MOAIRubyObject )
	
	GET ( u32, VectorSize, mVectorSize )
	
	//----------------------------------------------------------------//
					MOAIPathTerrainDeck			();
					~MOAIPathTerrainDeck		();
	void			RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
};

#endif
