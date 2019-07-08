// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIDECKPROPBASE_H
#define	MOAIDECKPROPBASE_H

#include <moai-sim/MOAIBlendMode.h>
#include <moai-sim/MOAIColor.h>
#include <moai-sim/MOAIDrawable.h>
#include <moai-sim/MOAIPartitionHull.h>
#include <moai-sim/MOAITransform.h>
#include <moai-core/MOAIRubySharedPtr.h>

class MOAIDeck;

//================================================================//
// MOAIDeckPropBase
//================================================================//
/**	@lua	MOAIDeckPropBase
	@text	Base class for props.
	
	@attr	ATTR_INDEX
	@attr	ATTR_PARTITION
*/
class MOAIDeckPropBase :
	public virtual MOAINode {
private:
	
	//----------------------------------------------------------------//
	static mrb_value			_getDeck					( mrb_state* M, mrb_value context );
	static mrb_value			_setDeck					( mrb_state* M, mrb_value context );

protected:

	MOAIRubySharedPtr < MOAIDeck >			mDeck;

public:
	
	GET ( MOAIDeck*, Deck, mDeck )

	//----------------------------------------------------------------//
						MOAIDeckPropBase		();
						~MOAIDeckPropBase		();
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void				SerializeIn				( MOAIRubyState& state, MOAIDeserializer& serializer );
	void				SerializeOut			( MOAIRubyState& state, MOAISerializer& serializer );
};

#endif
