// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIPartition.h>
#include <moai-sim/MOAIPartitionResultBuffer.h>
#include <moai-sim/MOAIIndexedPropBase.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getIndex
	@text	Gets the value of the deck indexer.
	
	@in		MOAIIndexedPropBase self
	@out	number index
*/
mrb_value MOAIIndexedPropBase::_getIndex ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIIndexedPropBase, "U" )

	return state.ToRValue ( self->mIndex );
}

//----------------------------------------------------------------//
/**	@lua	setIndex
	@text	Set the prop's index into its deck.
	
	@in		MOAIIndexedPropBase self
	@opt	number index		Default value is 1.
	@out	nil
*/
mrb_value MOAIIndexedPropBase::_setIndex ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIIndexedPropBase, "U" )

	self->mIndex = state.GetParamValue < u32 >( 1, 1 );
	self->ScheduleUpdate ();

	return context;
}

//================================================================//
// MOAIIndexedPropBase
//================================================================//

//----------------------------------------------------------------//
MOAIIndexedPropBase::MOAIIndexedPropBase () :
	mIndex ( 1 ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIDeckPropBase )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIIndexedPropBase::~MOAIIndexedPropBase () {
}

//----------------------------------------------------------------//
void MOAIIndexedPropBase::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIDeckPropBase::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "ATTR_INDEX", MOAIIndexedPropBaseAttr::Pack ( ATTR_INDEX ));
}

//----------------------------------------------------------------//
void MOAIIndexedPropBase::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIDeckPropBase::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getIndex", _getIndex, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setIndex", _setIndex, MRB_ARGS_ARG ( 0, 1 ) );

}

//----------------------------------------------------------------//
void MOAIIndexedPropBase::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	
	MOAIDeckPropBase::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIIndexedPropBase::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {

	MOAIDeckPropBase::SerializeOut ( state, serializer );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAIIndexedPropBase::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	if ( MOAIIndexedPropBaseAttr::Check ( attrID )) {
		
		switch ( UNPACK_ATTR ( attrID )) {
			case ATTR_INDEX:
				this->mIndex = ZLFloat::ToIndex ((float) attr.Apply (( s32 )this->mIndex, op, MOAIAttribute::ATTR_READ_WRITE ));
				return true;
		}
	}
	return false;
}
