// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAISTRETCHDECK_H
#define	MOAISTRETCHDECK_H

#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIMaterialBatchHolder.h>

//================================================================//
// MOAIStretchDeck
//================================================================//
// TODO: doxygen
class MOAIStretchDeck :
	public MOAIDeck {
protected:

	float			mStretchFactor;

	//----------------------------------------------------------------//
	static mrb_value		_setStretchFactor			( mrb_state* M, mrb_value context );
	
public:
		
	//----------------------------------------------------------------//
	ZLVec3D			BindStretchVertexTransform				() const;
					MOAIStretchDeck							();
					~MOAIStretchDeck						();
	void			RegisterRubyClass						( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs						( MOAIRubyState& state, RClass* klass );
};

#endif
