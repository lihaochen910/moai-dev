// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIINDEXEDPROPBASE_H
#define	MOAIINDEXEDPROPBASE_H

#include <moai-sim/MOAIBlendMode.h>
#include <moai-sim/MOAIColor.h>
#include <moai-sim/MOAIDeckPropBase.h>
#include <moai-sim/MOAIDrawable.h>
#include <moai-sim/MOAIPartitionHull.h>
#include <moai-sim/MOAITransform.h>
#include <moai-core/MOAIRubySharedPtr.h>

//================================================================//
// MOAIIndexedPropBase
//================================================================//
// TODO: doxygen
class MOAIIndexedPropBase :
	public virtual MOAIDeckPropBase {
protected:
	
	u32				mIndex;
	
	//----------------------------------------------------------------//
	static mrb_value		_getIndex					( mrb_state* M, mrb_value context );
	static mrb_value		_setIndex					( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	bool			MOAINode_ApplyAttrOp		( u32 attrID, MOAIAttribute& attr, u32 op );

public:

	DECL_ATTR_HELPER ( MOAIIndexedPropBase )

	enum {
		ATTR_INDEX,
		TOTAL_ATTR,
	};

	GET_SET ( u32, Index, mIndex )

	//----------------------------------------------------------------//
					MOAIIndexedPropBase			();
					~MOAIIndexedPropBase		();
	void			RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void			SerializeIn					( MOAIRubyState& state, MOAIDeserializer& serializer );
	void			SerializeOut				( MOAIRubyState& state, MOAISerializer& serializer );
};

#endif
