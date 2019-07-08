// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPINTRANSFORM_H
#define	MOAIPINTRANSFORM_H

#include <moai-sim/MOAITransform.h>
#include <moai-sim/MOAINode.h>

class MOAIViewLayer;

//================================================================//
// MOAIPinTransform
//================================================================//
/**	@lua	MOAIPinTransform
	@text	2D transform for connecting transforms across layers. Useful for
			HUD overlay items and map pins.
*/
class MOAIPinTransform :
	public MOAITransform {
private:

	MOAIRubySharedPtr < MOAIViewLayer > mSourceLayer;
	MOAIRubySharedPtr < MOAIViewLayer > mDestLayer;
		
	float			mFront;
	
	//----------------------------------------------------------------//
	static mrb_value		_init						( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	bool			MOAINode_ApplyAttrOp		( u32 attrID, MOAIAttribute& attr, u32 op );
	void			MOAINode_Update				();

public:
	
	DECL_RUBY_FACTORY ( MOAIPinTransform, MOAITransform )
	DECL_ATTR_HELPER ( MOAIPinTransform )
		
	enum {
		ATTR_FRONT,
	};
		
	//----------------------------------------------------------------//
					MOAIPinTransform			();
					~MOAIPinTransform			();
	void			RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
};

#endif
