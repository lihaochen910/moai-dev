// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAICOLLISIONDECK_H
#define	MOAICOLLISIONDECK_H

#include <moai-sim/MOAIDeck.h>

class MOAICollisionShape;

//================================================================//
// MOAICollisionDeck
//================================================================//
// TODO: doxygen
class MOAICollisionDeck :
	public MOAIDeck {
private:

	ZLLeanArray < ZLStrongPtr < MOAICollisionShape > >		mShapes;
	
	//----------------------------------------------------------------//
	static mrb_value		_reserveShapes			( mrb_state* M, mrb_value context );
	static mrb_value		_setBox					( mrb_state* M, mrb_value context );
	static mrb_value		_setRect				( mrb_state* M, mrb_value context );
	static mrb_value		_setQuad				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	MOAICollisionShape&		AffirmShape						( u32 idx );
	
	//----------------------------------------------------------------//
	ZLBounds				MOAIDeck_ComputeMaxBounds		();
	void					MOAIDeck_Draw					( u32 idx );
	ZLBounds				MOAIDeck_GetBounds				( u32 idx );
	MOAICollisionShape*		MOAIDeck_GetCollisionShape		( u32 idx );
	bool					MOAIDeck_Overlap				( u32 idx, const ZLVec2D& vec, u32 granularity, ZLBounds* result );
	bool					MOAIDeck_Overlap				( u32 idx, const ZLVec3D& vec, u32 granularity, ZLBounds* result );
	
public:
	
	DECL_RUBY_FACTORY ( MOAICollisionDeck, MOAIDeck )
	
	//----------------------------------------------------------------//
					MOAICollisionDeck		();
					~MOAICollisionDeck		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			ReserveShapes			( u32 totalShapes );
	void			SetBox					( u32 idx, const ZLBox& box );
	void			SetRect					( u32 idx, const ZLRect& rect );
	void			SetQuad					( u32 idx, const ZLQuad& quad );
};

#endif
