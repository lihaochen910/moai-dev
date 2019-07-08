// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIDRAWDECK_H
#define	MOAIDRAWDECK_H

#include <moai-sim/MOAIStretchDeck.h>

//================================================================//
// MOAIDrawDeck
//================================================================//
/**	@lua MOAIDrawDeck
	@text Scriptable deck objectf for immediate mode drawing with MOAIDraw.
*/
class MOAIDrawDeck :
	public MOAIStretchDeck {
private:

	ZLBounds				mBounds;

	MOAIRubyStrongRef		mOnBounds;
	MOAIRubyStrongRef		mOnDraw;

	//----------------------------------------------------------------//
	static mrb_value		_setBounds						( mrb_state* M, mrb_value context );
	static mrb_value		_setBoundsCallback				( mrb_state* M, mrb_value context );
	static mrb_value		_setDrawCallback				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void					SetBounds						( MOAIRubyState& state, int idx );

	//----------------------------------------------------------------//
	ZLBounds				MOAIDeck_ComputeMaxBounds		();
	void					MOAIDeck_Draw					( u32 idx );
	ZLBounds				MOAIDeck_GetBounds				( u32 idx );
	MOAICollisionShape*		MOAIDeck_GetCollisionShape		( u32 idx );
	bool					MOAIDeck_Overlap				( u32 idx, const ZLVec2D& vec, u32 granularity, ZLBounds* result );
	bool					MOAIDeck_Overlap				( u32 idx, const ZLVec3D& vec, u32 granularity, ZLBounds* result );

public:
	
	DECL_RUBY_FACTORY ( MOAIDrawDeck, MOAIStretchDeck )
	
	//----------------------------------------------------------------//
							MOAIDrawDeck					();
							~MOAIDrawDeck					();
	void					RegisterRubyClass				( MOAIRubyState& state, RClass* klass );
	void					RegisterRubyFuncs				( MOAIRubyState& state, RClass* klass );
};

#endif
