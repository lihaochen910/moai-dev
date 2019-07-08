// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAICOLOR_H
#define	MOAICOLOR_H

#include <moai-sim/MOAINode.h>

class MOAIDeck;
class MOAIPartitionViewLayer;

//================================================================//
// MOAIColor
//================================================================//
/**	@lua	MOAIColor
	@text	Color vector with animation helper methods.
	
	@attr	ATTR_R_COL	The R (red) channel
	@attr	ATTR_G_COL	The G (green) channel
	@attr	ATTR_B_COL	The B (blue) channel
	@attr	ATTR_A_COL	The A (alpha) channel
	
	@attr	INHERIT_COLOR
	@attr	COLOR_TRAIT
*/
class MOAIColor :
	public virtual MOAINode,
	public ZLColorVec {
protected:
	
	ZLColorVec	mColor;
	
	//----------------------------------------------------------------//
	static mrb_value		_getColor			( mrb_state* M, mrb_value context );
	static mrb_value		_moveColor			( mrb_state* M, mrb_value context );
	static mrb_value		_packRGBA			( mrb_state* M, mrb_value context );
	static mrb_value		_seekColor			( mrb_state* M, mrb_value context );
	static mrb_value		_setColor			( mrb_state* M, mrb_value context );
	static mrb_value		_setParent			( mrb_state* M, mrb_value context );
	static mrb_value		_unpackRGBA			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	bool			MOAINode_ApplyAttrOp		( u32 attrID, MOAIAttribute& attr, u32 op );
	void			MOAINode_Update				();

public:
	
	DECL_RUBY_FACTORY ( MOAIColor, MOAINode )
	DECL_ATTR_HELPER ( MOAIColor )
	
	enum {
		ATTR_R_COL,
		ATTR_G_COL,
		ATTR_B_COL,
		ATTR_A_COL,
		
		ADD_COLOR,
		INHERIT_COLOR,
		COLOR_TRAIT,
		
		TOTAL_ATTR,
	};
	
	//----------------------------------------------------------------//
	static MOAIColor*	AffirmColor			( MOAIRubyState& state, int idx );
	ZLColorVec			GetColorTrait		() const;
	bool				IsClear				();
						MOAIColor			();
						~MOAIColor			();
	void				RegisterRubyClass	( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs	( MOAIRubyState& state, RClass* klass );
};

#endif
