// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAICAMERAANCHOR2D_H
#define	MOAICAMERAANCHOR2D_H

#include <moai-sim/MOAINode.h>

class MOAITransformBase;

//================================================================//
// MOAICameraAnchor2D
//================================================================//
/**	@lua	MOAICameraAnchor2D
	@text	Attaches fitting information to a transform. Used by
			MOAICameraFitter2D.
*/
class MOAICameraAnchor2D :
	public virtual MOAINode {
private:

	ZLRect			mRect;
	ZLVec3D			mLoc;
	
	//----------------------------------------------------------------//
	static mrb_value		_setParent				( mrb_state* M, mrb_value context );
	static mrb_value		_setRect				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	ZLRect			GetRect					();
	
	//----------------------------------------------------------------//
	void			MOAINode_Update			();

public:

	DECL_RUBY_FACTORY ( MOAICameraAnchor2D, MOAINode )
	DECL_ATTR_HELPER ( MOAICameraAnchor2D )

	friend class MOAICameraFitter2D;

	enum {
		INHERIT_LOC,
		TOTAL_ATTR,
	};

	//----------------------------------------------------------------//
					MOAICameraAnchor2D		();
	virtual			~MOAICameraAnchor2D		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
