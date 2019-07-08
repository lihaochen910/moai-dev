// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAICollisionDeck.h>
#include <moai-sim/MOAICollisionShape.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAICollisionDeck::_reserveShapes ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICollisionDeck, "U" )
	
	self->ReserveShapes ( state.GetParamValue < u32 >( 1, 0 ));
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAICollisionDeck::_setBox ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICollisionDeck, "U" )
	
	u32 idx = state.GetParamValue < u32 >( 1, 1 ) - 1;
	ZLBox box = state.GetBox ( 2 );
	
	self->SetBox ( idx, box );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAICollisionDeck::_setRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICollisionDeck, "U" )
	
	u32 idx = state.GetParamValue < u32 >( 1, 1 ) - 1;
	ZLRect rect = state.GetRect < float >( 2 );
	
	self->SetRect ( idx, rect );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAICollisionDeck::_setQuad ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICollisionDeck, "U" )
	
	u32 idx = state.GetParamValue < u32 >( 1, 1 ) - 1;
	
	ZLQuad quad;
	
	quad.mV [ 0 ].mX = state.GetParamValue < float >( 2, 0.0f );
	quad.mV [ 0 ].mY = state.GetParamValue < float >( 3, 0.0f );
	quad.mV [ 1 ].mX = state.GetParamValue < float >( 4, 0.0f );
	quad.mV [ 1 ].mY = state.GetParamValue < float >( 5, 0.0f );
	quad.mV [ 2 ].mX = state.GetParamValue < float >( 6, 0.0f );
	quad.mV [ 2 ].mY = state.GetParamValue < float >( 7, 0.0f );
	quad.mV [ 3 ].mX = state.GetParamValue < float >( 8, 0.0f );
	quad.mV [ 3 ].mY = state.GetParamValue < float >( 9, 0.0f );
	
	self->SetQuad ( idx, quad );
	
	return mrb_nil_value ();
}

//================================================================//
// MOAICollisionDeck
//================================================================//

//----------------------------------------------------------------//
MOAICollisionShape& MOAICollisionDeck::AffirmShape ( u32 idx ) {

	assert ( idx < this->mShapes.Size ());

	MOAICollisionShape* shape = this->mShapes [ idx ];

	if ( !shape ) {
		shape = new MOAICollisionShape ();
		this->mShapes [ idx ] = shape;
	}
	
	assert ( shape );
	
	return *shape;
}

//----------------------------------------------------------------//
MOAICollisionDeck::MOAICollisionDeck () {

	RTTI_BEGIN
		RTTI_EXTEND ( MOAIDeck )
	RTTI_END
}

//----------------------------------------------------------------//
MOAICollisionDeck::~MOAICollisionDeck () {
}

//----------------------------------------------------------------//
void MOAICollisionDeck::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIDeck::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAICollisionDeck::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIDeck::RegisterRubyFuncs ( state, klass );
	
	state.DefineInstanceMethod ( klass, "reserveShapes",		_reserveShapes, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setBox",				_setBox, MRB_ARGS_REQ ( 7 ) );
	state.DefineInstanceMethod ( klass, "setRect",			_setRect, MRB_ARGS_REQ ( 5 ) );
	state.DefineInstanceMethod ( klass, "setQuad",			_setQuad, MRB_ARGS_REQ ( 9 ) );

}

//----------------------------------------------------------------//
void MOAICollisionDeck::ReserveShapes ( u32 totalShapes ) {
	
	this->mShapes.Resize ( totalShapes );
}

//----------------------------------------------------------------//
void MOAICollisionDeck::SetBox ( u32 idx, const ZLBox& box ) {

	if ( idx < this->mShapes.Size ()) {
		this->AffirmShape ( idx ).Set ( 0, box );
	}
}

//----------------------------------------------------------------//
void MOAICollisionDeck::SetRect ( u32 idx, const ZLRect& rect ) {

	if ( idx < this->mShapes.Size ()) {
		this->AffirmShape ( idx ).Set ( 0, rect );
	}
}

//----------------------------------------------------------------//
void MOAICollisionDeck::SetQuad ( u32 idx, const ZLQuad& quad ) {

	if ( idx < this->mShapes.Size ()) {
		this->AffirmShape ( idx ).Set ( 0, quad );
	}
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
ZLBounds MOAICollisionDeck::MOAIDeck_ComputeMaxBounds () {

	return this->MOAIDeck::GetBounds ( 0 );
}

//----------------------------------------------------------------//
void MOAICollisionDeck::MOAIDeck_Draw ( u32 idx ) {
	UNUSED ( idx );
	
//	u32 size = ( u32 )this->mQuads.Size ();
//	if ( size ) {
//
//		idx = idx - 1;
//		u32 itemIdx = idx % size;
//
//		if ( !this->LoadGfxState ( materials, this->mMaterialIDs [ itemIdx ], idx, MOAIShaderMgr::DECK2D_SHADER )) return;
//
//		MOAIGfxMgr& gfxMgr = MOAIGfxMgr::Get ();
//		MOAIQuadBrush::BindVertexFormat ();
//		
//		gfxState.SetVertexTransform ( MOAIGfxState::MODEL_TO_CLIP_MTX );
//		gfxState.SetUVTransform ( MOAIGfxState::UV_TO_MODEL_MTX );
//		
//		this->mQuads [ itemIdx ].Draw ( offset.mX, offset.mY, offset.mZ, scale.mX, scale.mY  );
//	}
}

//----------------------------------------------------------------//
ZLBounds MOAICollisionDeck::MOAIDeck_GetBounds ( u32 idx ) {
	
	assert ( idx < this->mShapes.Size ());

	return this->mShapes [ idx ]->GetBounds ();
}

//----------------------------------------------------------------//
MOAICollisionShape* MOAICollisionDeck::MOAIDeck_GetCollisionShape ( u32 idx ) {
	UNUSED ( idx );

	if ( idx < this->mShapes.Size ()) {
		return this->mShapes [ idx ];
	}
	return 0;
}

//----------------------------------------------------------------//
bool MOAICollisionDeck::MOAIDeck_Overlap ( u32 idx, const ZLVec2D& vec, u32 granularity, ZLBounds* result ) {
	UNUSED ( idx );
	UNUSED ( vec );
	UNUSED ( granularity );
	UNUSED ( result );

	//	u32 size = ( u32 )this->mQuads.Size ();
	//	if ( size ) {
	//		idx = ( idx - 1 ) % size;
	//		const MOAIQuadBrush& quadBrush = this->mQuads [ idx ];
	//		return this->TestHit ( materials, idx, granularity, quadBrush.mModelQuad, quadBrush.mUVQuad, vec.mX, vec.mY );
	//	}
	//	return false;

	return false;
}

//----------------------------------------------------------------//
bool MOAICollisionDeck::MOAIDeck_Overlap ( u32 idx, const ZLVec3D& vec, u32 granularity, ZLBounds* result ) {
	UNUSED ( idx );
	UNUSED ( vec );
	UNUSED ( granularity );
	UNUSED ( result );

	//return this->TestHit ( materials, idx, granularity, this->mQuad.mModelQuad, this->mQuad.mUVQuad, vec.mX, vec.mY );
	return false;
}
