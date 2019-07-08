// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAICameraAnchor2D.h>
#include <moai-sim/MOAITransformBase.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	setParent
	@text	Attach the anchor to a transform.
	
	@in		MOAICameraAnchor2D self
	@opt	MOAINode parent
	@out	nil
*/
mrb_value MOAICameraAnchor2D::_setParent ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraAnchor2D, "U" )
	
	MOAINode* parent = state.GetRubyObject < MOAINode >( 1, true );
	self->SetAttrLink ( PACK_ATTR ( MOAICameraAnchor2D, INHERIT_LOC ), parent, PACK_ATTR ( MOAITransformBase, TRANSFORM_TRAIT ));
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setRect
	@text	Set the dimensions (in world units) of the anchor.
	
	@in		MOAICameraAnchor2D self
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@out	nil
*/
mrb_value MOAICameraAnchor2D::_setRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraAnchor2D, "UNNNN" )

	float x0	= state.GetParamValue < float >( 1, 0.0f );
	float y0	= state.GetParamValue < float >( 2, 0.0f );
	float x1	= state.GetParamValue < float >( 3, 0.0f );
	float y1	= state.GetParamValue < float >( 4, 0.0f );
	
	self->mRect.Init ( x0, y0, x1, y1 );
	
	return context;
}

//================================================================//
// MOAICameraAnchor2D
//================================================================//

//----------------------------------------------------------------//
ZLRect MOAICameraAnchor2D::GetRect () {

	ZLRect rect = this->mRect;
	rect.Offset ( this->mLoc.mX, this->mLoc.mY );
	rect.Bless ();
	
	return rect;
}

//----------------------------------------------------------------//
MOAICameraAnchor2D::MOAICameraAnchor2D () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAINode )
	RTTI_END
	
	this->mRect.Init ( 0.0f, 0.0f, 0.0f, 0.0f );
	this->mLoc.Init ( 0.0f, 0.0f, 0.0f );
}

//----------------------------------------------------------------//
MOAICameraAnchor2D::~MOAICameraAnchor2D () {
}

//----------------------------------------------------------------//
void MOAICameraAnchor2D::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	MOAINode::RegisterRubyClass ( state, klass );

	state.DefineClassConst ( klass, "INHERIT_LOC", MOAICameraAnchor2DAttr::Pack ( INHERIT_LOC ));
}

//----------------------------------------------------------------//
void MOAICameraAnchor2D::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAINode::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "setParent", _setParent, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setRect", _setRect, MRB_ARGS_REQ ( 4 ) );

}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAICameraAnchor2D::MOAINode_Update () {
	
	const ZLAffine3D inherit = this->GetLinkedValue ( MOAICameraAnchor2DAttr::Pack ( INHERIT_LOC ), ZLAffine3D::IDENT );
	this->mLoc = inherit.GetTranslation ();
}
