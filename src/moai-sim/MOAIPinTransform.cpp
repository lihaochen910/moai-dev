// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIViewLayer.h>
#include <moai-sim/MOAIPinTransform.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	init
	@text	Initialize the bridge transform (map coordinates in one layer onto
			another; useful for rendering screen space objects tied to world
			space coordinates - map pins, for example).
	
	@in		MOAIPinTransform self
	@in		MOAITransformBase sourceTransform
	@in		MOAIViewLayer sourceLayer
	@in		MOAIViewLayer destLayer
	@out	nil
*/
mrb_value MOAIPinTransform::_init ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPinTransform, "UUU" );
	
	MOAIViewLayer* sourceLayer = state.GetRubyObject < MOAIViewLayer >( 1, true );
	if ( !sourceLayer ) return mrb_nil_value ();
	
	MOAIViewLayer* destLayer = state.GetRubyObject < MOAIViewLayer >( 2, true );
	if ( !destLayer ) return mrb_nil_value ();
	
	self->SetDependentMember ( self->mSourceLayer, sourceLayer );
	self->SetDependentMember ( self->mDestLayer, destLayer );
	
	return mrb_nil_value ();
}

//================================================================//
// MOAIPinTransform
//================================================================//

//----------------------------------------------------------------//
MOAIPinTransform::MOAIPinTransform () :
	mFront ( 1.0f ) {
	
	RTTI_SINGLE ( MOAITransform )
}

//----------------------------------------------------------------//
MOAIPinTransform::~MOAIPinTransform () {

	this->mSourceLayer.Set ( *this, 0 );
	this->mDestLayer.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAIPinTransform::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAITransform::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "ATTR_FRONT", MOAIPinTransformAttr::Pack ( ATTR_FRONT ) );
}

//----------------------------------------------------------------//
void MOAIPinTransform::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAITransform::RegisterRubyFuncs ( state, klass );
	
	state.DefineInstanceMethod ( klass, "init", _init, MRB_ARGS_REQ ( 2 ) );

}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAIPinTransform::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	if ( MOAIPinTransformAttr::Check ( attrID )) {
		switch ( UNPACK_ATTR ( attrID )) {
			case ATTR_FRONT:
				attr.Apply ( this->mFront, op, MOAIAttribute::ATTR_READ );
				return true;
		}
	}
	
	return MOAITransform::MOAINode_ApplyAttrOp ( attrID, attr, op );
}

//----------------------------------------------------------------//
void MOAIPinTransform::MOAINode_Update () {
	
	MOAITransform::MOAINode_Update ();
	
	if ( !( this->mSourceLayer && this->mDestLayer )) return;
	
	ZLVec3D loc = this->mLocalToWorldMtx.GetTranslation ();
	
	ZLMatrix4x4 mtx;
	
	this->mSourceLayer->GetWorldToWndMtx ().Project ( loc );
	this->mDestLayer->GetWndToWorldMtx ().Transform ( loc );
	
	this->mLocalToWorldMtx.Translate ( loc.mX, loc.mY, loc.mZ );
	this->mWorldToLocalMtx.Translate ( -loc.mX, -loc.mY, -loc.mZ );
	
	// Z component is at the back of the NDC's near plane
	this->mFront = loc.mZ < -1.0f ? 0.0f : 1.0f;
}
