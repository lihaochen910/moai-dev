// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAISCRIPTNODE_H
#define	MOAISCRIPTNODE_H

#include <moai-sim/MOAINode.h>


//================================================================//
// MOAIScriptNode
//================================================================//
/**	@lua	MOAIScriptNode
	@text	User scriptable dependency node. User may specify Ruby
			callback to handle node updating as well as custom floating
			point attributes.
*/
class MOAIScriptNode :
	public virtual MOAINode {
private:

	MOAIRubyStrongRef		mOnUpdate;
	ZLLeanArray < float >	mAttributes;
	ZLLeanArray < cc8* >	mAttrNames;

	//----------------------------------------------------------------//
	static mrb_value		_reserveAttrs			( mrb_state* M, mrb_value context );
	static mrb_value		_setCallback			( mrb_state* M, mrb_value context );
	static mrb_value		_setAttrName			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	bool			MOAINode_ApplyAttrOp	( u32 attrID, MOAIAttribute& attr, u32 op );
	void			MOAINode_Update			();

public:
	
	DECL_RUBY_FACTORY ( MOAIScriptNode, MOAINode )
	
	//----------------------------------------------------------------//
					MOAIScriptNode			();
					~MOAIScriptNode			();
	void			NamedAttrAdd			( u32 attrID, MOAIAttribute& attr );
	void			NamedAttrGet			( u32 attrID, MOAIAttribute& attr );
	void			NamedAttrSet			( u32 attrID, MOAIAttribute& attr );
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
