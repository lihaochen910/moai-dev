// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIScriptNode.h>

//================================================================//
// MOAIMemberTableAttr
//================================================================//
class MOAIMemberTableAttr {
protected:
	friend class MOAIScriptNode;
	MOAIScriptNode*		mSource;
	cc8*				mFieldName;
	
	//----------------------------------------------------------------//
	MOAIMemberTableAttr () :
		mSource ( 0 ),
		mFieldName ( 0 ) {
		}
};

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	reserveAttrs
	@text	Reserve memory for custom attributes and initializes them to 0.
	
	@in		MOAIScriptNode self
	@in		number nAttributes
	@out	nil
*/
mrb_value MOAIScriptNode::_reserveAttrs ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIScriptNode, "UN" );

	u32 size = state.GetParamValue < u32 >( 1, 0 );
	self->mAttributes.Init ( size );
	self->mAttributes.Fill ( 0.0f );
	
	self->mAttrNames.Init ( size );
	self->mAttrNames.Fill ( 0 );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setCallback
	@text	Sets a Ruby function to be called whenever the node is updated.
	
	@in		MOAIScriptNode self
	@in		function onUpdate
	@out	nil
*/
mrb_value MOAIScriptNode::_setCallback ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIScriptNode, "UF" );

	self->mOnUpdate.SetRef ( state.GetParamValue ( 1 ) );
	return context;
}


mrb_value MOAIScriptNode::_setAttrName ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIScriptNode, "U" );
	
	u32 idx = state.GetParamValue < u32 >( 1, 1 ) - 1;
	self->mAttrNames [ idx ] = state.GetParamValue < cc8* >( 2, 0 );
	
	return context;
}

//================================================================//
// MOAIScriptNode
//================================================================//

//----------------------------------------------------------------//
MOAIScriptNode::MOAIScriptNode () {
	
	RTTI_SINGLE ( MOAINode )
}

//----------------------------------------------------------------//
MOAIScriptNode::~MOAIScriptNode () {
}

//----------------------------------------------------------------//
void MOAIScriptNode::NamedAttrAdd ( u32 attrID, MOAIAttribute &attr ) {
	
	/*cc8* attrName = this->mAttrNames [ attrID ];
	switch ( attr.GetTypeID ()) {
		case MOAIAttribute::ATTR_TYPE_FLOAT_32: {
			float value = attr.GetValue ( 0.0f );
			
			if ( value != 0.0f ) {
				
				MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
				this->PushMemberTable ( state );
				float cur = state.GetFieldValue < float >( -1, attrName, 0.0f );
				state.SetField ( -1, attrName, cur + value );
			}
			break;
		}
		case MOAIAttribute::ATTR_TYPE_INT_32: {
			int value = ( int )attr.GetValue ( 0 );
			
			if ( value != 0 ) {
				MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
				this->PushMemberTable ( state );
				int cur = state.GetFieldValue < int >( -1, attrName, 0 );
				state.SetField ( -1, attrName, cur + value );
			}
			break;
		}
	}*/
}

//----------------------------------------------------------------//
void MOAIScriptNode::NamedAttrGet ( u32 attrID, MOAIAttribute &attr ) {
	
	/*cc8* attrName = this->mAttrNames [ attrID ];
	
	MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
	this->PushMemberTable ( state );
	state.PushField ( -1, attrName );
	
	if ( state.IsType ( -1, LUA_TNUMBER )) {
		
		float value = state.GetValue < float >( -1, 0.0f );
		attr.Apply ( value, MOAIAttribute::GET, MOAIAttribute::ATTR_WRITE );
	}
	else {
		
		MOAIMemberTableAttr value;
		value.mSource = this;
		value.mFieldName = attrName;
		attr.ApplyVariantNoAdd ( value, MOAIAttribute::GET, MOAIAttribute::ATTR_WRITE );
	}*/
}

//----------------------------------------------------------------//
void MOAIScriptNode::NamedAttrSet ( u32 attrID, MOAIAttribute &attr ) {
	
	/*cc8* attrName = this->mAttrNames [ attrID ];
	switch ( attr.GetTypeID ()) {
		case MOAIAttribute::ATTR_TYPE_FLOAT_32: {

			float value = attr.GetValue ( 0.0f );
			MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
			this->PushMemberTable ( state );
			state.SetField ( -1, attrName, value );

			break;
		}
		case MOAIAttribute::ATTR_TYPE_INT_32: {

			int value = attr.GetValue ( 0 );
			MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
			this->PushMemberTable ( state );
			state.SetField ( -1, attrName, value );

			break;
		}
		case MOAIAttribute::ATTR_TYPE_VARIANT: {

			MOAIMemberTableAttr value;
			value = attr.GetVariant < MOAIMemberTableAttr >( value );
			MOAIScriptNode* source = value.mSource;
			cc8* sourceField = value.mFieldName;

			if ( source && sourceField ) {

				MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
				source->PushMemberTable ( state );
				this->PushMemberTable ( state );
				state.PushField ( -2, sourceField );

				lua_setfield ( state, -2, attrName );
			}
			break;
		}
	}*/
}

//----------------------------------------------------------------//
void MOAIScriptNode::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIScriptNode::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "reserveAttrs",	_reserveAttrs, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setAttrName",	_setAttrName, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setCallback",	_setCallback, MRB_ARGS_REQ ( 1 ) );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAIScriptNode::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {
	attrID = UNPACK_ATTR(attrID) - 1;
	
	if ( attrID >= this->mAttributes.Size()) {
		return false;
	}
	
	if ( this->mAttrNames [ attrID ] == 0 ) {
		this->mAttributes [ attrID ] = attr.Apply ( this->mAttributes [ attrID ], op, MOAIAttribute::ATTR_READ_WRITE );
		return true;
	}
	else {
		switch ( op ) {
			case MOAIAttribute::CHECK:
				attr.SetFlags ( MOAIAttribute::ATTR_READ_WRITE );
				break;
				
			case MOAIAttribute::ADD:
				this->NamedAttrAdd ( attrID, attr );
				break;
				
			case MOAIAttribute::SET:
				this->NamedAttrSet ( attrID, attr );
				break;
				
			case MOAIAttribute::GET:
				this->NamedAttrGet ( attrID, attr );
				break;
		}
		return true;
	}
}

//----------------------------------------------------------------//
void MOAIScriptNode::MOAINode_Update () {

	if ( this->mOnUpdate ) {
		
		MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
		
		if ( state.IsType ( this->mOnUpdate, MRB_TT_PROC ) ) {
			state.FuncCall ( this->mOnUpdate, "call", 0, 0 );
		}
		else if ( state.IsType ( this->mOnUpdate, MRB_TT_FIBER ) ) {
			state.FiberResume ( this->mOnUpdate );
		}
	}
}
