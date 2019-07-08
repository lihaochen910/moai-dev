// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIDECKREMAPPER_H
#define	MOAIDECKREMAPPER_H

#include <moai-sim/MOAIDeckProxy.h>
#include <moai-sim/MOAINode.h>

//================================================================//
// MOAIDeckRemapper
//================================================================//
/**	@lua	MOAIDeckRemapper
	@text	Remap deck indices. Most useful for controlling animated tiles in tilemaps.
			All indices are exposed as attributes that may be connected by setAttrLink
			or driven using MOAIAnim or MOAIAnimCurve.
*/
class MOAIDeckRemapper :
	public virtual MOAINode,
	public virtual MOAIDeckProxy {
protected:
	
	u32						mBase;
	ZLLeanArray < u32 >		mRemap;
	
	//----------------------------------------------------------------//
	static mrb_value			_reserve				( mrb_state* M, mrb_value context );
	static mrb_value			_setBase				( mrb_state* M, mrb_value context );
	static mrb_value			_setRemap				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	u32					MOAIDeckProxy_Remap				( u32 idx );
	bool				MOAINode_ApplyAttrOp			( u32 attrID, MOAIAttribute& attr, u32 op );

public:
	
	DECL_RUBY_FACTORY ( MOAIDeckRemapper, MOAIDeck )
	
	//----------------------------------------------------------------//
						MOAIDeckRemapper				();
						~MOAIDeckRemapper				();
	void				RegisterRubyClass				( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs				( MOAIRubyState& state, RClass* klass );
};

#endif
