// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIPathTerrainDeck.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getMask
	@text	Returns mask for cell.

	@in		MOAIPathTerrainDeck self
	@in		number idx
	@out	number mask
*/
mrb_value MOAIPathTerrainDeck::_getMask ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathTerrainDeck, "UN" )
	
	u32 idx = state.GetParamValue < u32 >( 1, 1 ) - 1;
	
	if ( idx < self->mMasks.Size ()) {
		return state.ToRValue( self->mMasks [ idx ] );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getTerrainVec
	@text	Returns terrain vector for cell.

	@in		MOAIPathTerrainDeck self
	@in		number idx
	@out	...
*/
mrb_value MOAIPathTerrainDeck::_getTerrainVec ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathTerrainDeck, "UN" )
	
	u32 idx = state.GetParamValue < u32 >( 1, 1 ) - 1;
	float* vector = self->GetVector ( idx + 1 );

	u32 size = self->mVectorSize;

	mrb_value ary = mrb_ary_new ( M );
	
	for ( u32 i = 0; i < size; ++i ) {
		mrb_ary_push ( M, ary, state.ToRValue ( vector [ i ] ) );
	}
	return ary;
}

//----------------------------------------------------------------//
/**	@lua	setMask
	@text	Returns mask for cell.

	@in		MOAIPathTerrainDeck self
	@in		number idx
	@in		number mask
	@out	nil
*/
mrb_value MOAIPathTerrainDeck::_setMask ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathTerrainDeck, "UNN" )
	
	u32 idx		= state.GetParamValue < u32 >( 1, 1 ) - 1;
	u32 mask	= state.GetParamValue < int >( 2, 0 );
	
	if ( idx < self->mMasks.Size ()) {
		self->mMasks [ idx ] = mask;
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setTerrainVec
	@text	Sets terrain vector for cell.

	@in		MOAIPathTerrainDeck self
	@in		number idx
	@in		float... values
	@out	nil
*/
mrb_value MOAIPathTerrainDeck::_setTerrainVec ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathTerrainDeck, "UN" )
	
	u32 idx		= state.GetParamValue < u32 >( 1, 1 ) - 1;
	u32 total	= state.GetParamsCount () - 2;
	
	if ( total > self->mVectorSize ) {
		total = self->mVectorSize;
	}
	
	float* vector = self->GetVector ( idx + 1 );
	
	for ( u32 i = 0; i < total; ++i ) {
		vector [ i ] = state.GetParamValue < float >( 2 + i, 0.0f );
	}
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	reserve
	@text	Allocates terrain vectors.

	@in		MOAIPathTerrainDeck self
	@in		number deckSize
	@in		number terrainVecSize
	@out	nil
*/
mrb_value MOAIPathTerrainDeck::_reserve ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPathTerrainDeck, "UN" )
	
	self->mDeckSize		= state.GetParamValue < u32 >( 1, 0 );
	self->mVectorSize	= state.GetParamValue < u32 >( 2, 0 );
	
	self->mMasks.Init ( self->mDeckSize );
	self->mMasks.Fill ( 0xffffffff );
	
	self->mVectors.Init ( self->mDeckSize * self->mVectorSize );
	self->mVectors.Fill ( 0.0f );

	return mrb_nil_value ();
}

//================================================================//
// MOAIPathTerrainDeck
//================================================================//

//----------------------------------------------------------------//
u32 MOAIPathTerrainDeck::GetMask ( u32 idx ) {

	return this->mMasks [ idx - 1 ];
}

//----------------------------------------------------------------//
float* MOAIPathTerrainDeck::GetVector ( u32 idx ) {

	return &this->mVectors [( idx - 1 ) * this->mVectorSize ];
}

//----------------------------------------------------------------//
MOAIPathTerrainDeck::MOAIPathTerrainDeck () :
	mDeckSize ( 0 ),
	mVectorSize ( 0 ) {

	RTTI_SINGLE ( MOAIRubyObject )
}

//----------------------------------------------------------------//
MOAIPathTerrainDeck::~MOAIPathTerrainDeck () {
}

//----------------------------------------------------------------//
void MOAIPathTerrainDeck::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
}

//----------------------------------------------------------------//
void MOAIPathTerrainDeck::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "getMask",				_getMask, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getTerrainVec",		_getTerrainVec, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setMask",				_setMask, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setTerrainVec",		_setTerrainVec, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "reserve",				_reserve, MRB_ARGS_REQ ( 2 ) );

}
