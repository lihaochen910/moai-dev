// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIDeckPropBase.h>
#include <moai-sim/MOAISpriteDeck2D.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getDeck
	@text	Get the deck.
               
	@in		MOAIDeckPropBase self
	@out	MOAIDeck deck
*/
mrb_value MOAIDeckPropBase::_getDeck ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDeckPropBase, "U" )

	if ( self->mDeck ) {
		return self->mDeck->PushRubyUserdata ( state );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setDeck
	@text	Sets or clears the deck to be indexed by the prop.
	
	@in		MOAIDeckPropBase self
	@opt	MOAIDeck deck		Default value is nil.
	@out	nil
*/
mrb_value MOAIDeckPropBase::_setDeck ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIDeckPropBase, "U" )

	MOAIDeck* deck = MOAISpriteDeck2D::AffirmDeck ( state, 1 );

	self->mDeck.Set ( *self, deck );
	self->ScheduleUpdate ();
	
	if ( deck ) {
		return state.ToRValue < MOAIRubyObject* >( deck );
	}
	return mrb_nil_value ();
}

//================================================================//
// MOAIDeckPropBase
//================================================================//

//----------------------------------------------------------------//
//void MOAIDeckPropBase::GatherSurfaces ( MOAISurfaceSampler2D& sampler ) {
//	UNUSED ( sampler );

	//if ( !this->mDeck ) return;
	//
	//sampler.SetSourcePrim ( this );
	//
	//if ( this->mGrid ) {
	//	
	//	ZLRect localRect = sampler.GetLocalRect ();
	//	
	//	MOAICellCoord c0;
	//	MOAICellCoord c1;
	//	
	//	ZLRect deckBounds = this->mDeck->GetBounds ().GetRect( ZLBox::PLANE_XY );

	//	this->mGrid->GetBoundsInRect ( localRect, c0, c1, deckBounds );
	//	//this->mDeck->GatherSurfaces ( *this->mGrid, this->mRemapper, this->mGridScale, c0, c1, sampler );
	//}
	//else {
	//	//this->mDeck->GatherSurfaces ( MOAIDeckRemapper::Remap ( this->mRemapper, this->mIndex ), sampler );
	//}
//}

//----------------------------------------------------------------//
MOAIDeckPropBase::MOAIDeckPropBase () {
	
//	RTTI_BEGIN
//		RTTI_EXTEND ( MOAIPartitionHull )
//	RTTI_END
}

//----------------------------------------------------------------//
MOAIDeckPropBase::~MOAIDeckPropBase () {
	
	this->mDeck.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAIDeckPropBase::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIDeckPropBase::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	state.DefineInstanceMethod ( klass, "getDeck", _getDeck, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setDeck", _setDeck, MRB_ARGS_ARG ( 0, 1 ) );

}

//----------------------------------------------------------------//
void MOAIDeckPropBase::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	
	//this->mDeck.Set ( *this, serializer.MemberIDToObject < MOAIDeck >( state.GetFieldValue < MOAISerializerBase::ObjID >( -1, "mDeck", 0 )));
}

//----------------------------------------------------------------//
void MOAIDeckPropBase::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	
	//state.SetField ( -1, "mDeck", serializer.AffirmMemberID ( this->mDeck ));
}
