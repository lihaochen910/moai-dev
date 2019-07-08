// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIEaseDriver.h>
#include <moai-sim/MOAINode.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	reserveLinks
	@text	Reserve links.

	@in		MOAIEaseDriver self
	@in		number nLinks
	@out	nil
*/
mrb_value MOAIEaseDriver::_reserveLinks ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIEaseDriver, "UN" );
	
	u32 total = state.GetParamValue < u32 >( 1, 0 );
	self->ReserveLinks ( total );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setLink
	@text	Set the ease for a target node attribute.

	@overload	Target is a number.

		@in		MOAIEaseDriver self
		@in		number idx				Index of the link;
		@in		MOAINode target			Target node.
		@in		number attrID			Index of the attribute to be driven.
		@opt	number value			Value for attribute at the end of the ease. Default is 0.
		@opt	number mode				The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
										MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.
		@out	nil
	
	@overload	Target is a node.

		@in		MOAIEaseDriver self
		@in		number idx				Index of the link;
		@in		MOAINode target			Target node.
		@in		number attrID			Index of the attribute to be driven.
		@in		MOAINode source			Node that you are linking to target.
		@in		number sourceAttrID		Index of the attribute being linked.
		@opt	number mode				The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
										MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.
		@out	nil

	
*/
mrb_value MOAIEaseDriver::_setLink ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIEaseDriver, "UNUN" );
	
	MOAINode* dest = state.GetRubyObject < MOAINode >( 2, true );
	if ( !dest ) return context;
	
	u32 idx				= state.GetParamValue < u32 >( 1, 1 ) - 1;
	u32 destAttrID		= state.GetParamValue < u32 >( 3, 0 );
	
	MOAINode* source = state.GetRubyObject < MOAINode >( 4, true );
	
	if ( source ) {
	
		u32 sourceAttrID	= state.GetParamValue < u32 >( 5, MOAIAttribute::NULL_ATTR );
		u32 mode			= state.GetParamValue < u32 >( 6, ZLInterpolate::kSmooth );
		
		self->SetLink ( idx, dest, destAttrID, source, sourceAttrID, mode );
	}
	else {
	
		float v1			= state.GetParamValue < float >( 5, 0.0f );
		u32 mode			= state.GetParamValue < u32 >( 6, ZLInterpolate::kSmooth );
		
		self->SetLink ( idx, dest, destAttrID, v1, mode );
	}
	return context;
}

//================================================================//
// MOAIEaseDriver
//================================================================//

//----------------------------------------------------------------//
MOAIEaseDriver::MOAIEaseDriver () {
	
	RTTI_SINGLE ( MOAITimer )
}

//----------------------------------------------------------------//
MOAIEaseDriver::~MOAIEaseDriver () {

	size_t total = this->mLinks.Size ();
	for ( size_t i = 0; i < total; ++i ) {	
		MOAIEaseDriverLink& link = this->mLinks [ i ];
		link.mSource.Set ( *this, 0 );
		link.mDest.Set ( *this, 0 );
	}
}

//----------------------------------------------------------------//
u32 MOAIEaseDriver::ParseForMove ( MOAIRubyState& state, int idx, MOAINode* dest, u32 total, int mode, ... ) {

	float* params = ( float* )alloca ( total * sizeof ( float ));
	u32* destAttrIDs = ( u32* )alloca ( total * sizeof ( u32 ));
	
	va_list args;
	va_start ( args, mode );
	
	u32 count = 0;
	for ( u32 i = 0; i < total; ++i ) {
	
		destAttrIDs [ i ] = ( u32 )va_arg ( args, int );
		float defaultValue = ( float )va_arg ( args, double );
	
		params [ i ] = state.GetParamValue < float >( idx + i, defaultValue );
		if ( params [ i ] != 0.0f ) {
			count++;
		}
	}
	va_end ( args );

	if ( count ) {
		
		this->ReserveLinks ( count );
		
		u32 linkID = 0;
		for ( u32 i = 0; i < total; ++i ) {
		
			u32 destAttrID = destAttrIDs [ i ];
			float v1 = params [ i ];
			
			if ( v1 != 0.0f ) {
				this->SetLink ( linkID++, dest, destAttrID, v1, ( u32 )mode );
			}
		}
	}
	return count;
}

//----------------------------------------------------------------//
u32 MOAIEaseDriver::ParseForSeek ( MOAIRubyState& state, int idx, MOAINode* dest, u32 total, int mode, ... ) {

	float* params = ( float* )alloca ( total * sizeof ( float ));
	u32* destAttrIDs = ( u32* )alloca ( total * sizeof ( u32 ));
	
	va_list args;
	va_start ( args, mode );
	
	u32 count = 0;
	for ( u32 i = 0; i < total; ++i ) {
	
		destAttrIDs [ i ] = ( u32 )va_arg ( args, int );
		float attrValue =  ( float )va_arg ( args, double );
		float defaultValue =  ( float )va_arg ( args, double );
		
		params [ i ] = state.GetParamValue < float >( idx + i, defaultValue ) - attrValue;
		if ( params [ i ] != 0.0f ) {
			count++;
		}
	}
	va_end ( args );

	if ( count ) {
		
		this->ReserveLinks ( count );
		
		u32 linkID = 0;
		for ( u32 i = 0; i < total; ++i ) {
		
			u32 destAttrID = destAttrIDs [ i ];
			float v1 = params [ i ];
			
			if ( v1 != 0.0f ) {
				this->SetLink ( linkID++, dest, destAttrID, v1, ( u32 )mode );
			}
		}
	}
	return count;
}

//----------------------------------------------------------------//
void MOAIEaseDriver::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIEaseDriver::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	state.DefineInstanceMethod ( klass, "reserveLinks", _reserveLinks, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setLink", _setLink, MRB_ARGS_ARG ( 5, 1 ) );

}

//----------------------------------------------------------------//
void MOAIEaseDriver::ReserveLinks ( u32 total ) {

	this->mLinks.Init ( total );
}

//----------------------------------------------------------------//
void MOAIEaseDriver::SetLink ( u32 idx, MOAINode* dest, u32 destAttrID, float v1, u32 mode  ) {

	if ( idx < this->mLinks.Size ()) {
		
		MOAIEaseDriverLink& link = this->mLinks [ idx ];

		link.mSource.Set ( *this, 0 );
		link.mSourceAttrID	= MOAIAttribute::NULL_ATTR;

		link.mDest.Set ( *this, dest );
		link.mDestAttrID	= destAttrID;
		
		link.mV0			= 0.0f;
		link.mV1			= v1;
		link.mMode			= mode;
	}
}

//----------------------------------------------------------------//
void MOAIEaseDriver::SetLink ( u32 idx, MOAINode* dest, u32 destAttrID, MOAINode* source, u32 sourceAttrID, u32 mode ) {

	if ( idx < this->mLinks.Size ()) {
		
		MOAIEaseDriverLink& link = this->mLinks [ idx ];

		link.mSource.Set ( *this, source );
		link.mSourceAttrID	= sourceAttrID;

		link.mDest.Set ( *this, dest );
		link.mDestAttrID	= destAttrID;
		
		link.mV0			= dest->GetAttributeValue ( destAttrID, 0.0f );
		link.mV1			= source->GetAttributeValue ( sourceAttrID, 0.0f );
		link.mMode			= mode;
	}
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAIEaseDriver::MOAIAction_Update ( double step ) {
	
	float c0 = this->GetCycle ();
	float t0 = ZLFloat::Clamp ( this->GetNormalizedTime () - c0, 0.0f, 1.0f );
	
	MOAITimer::MOAIAction_Update ( step );
	if ( step == 0.0f ) return;
	
	float c1 = this->GetCycle ();
	float t1 = ZLFloat::Clamp ( this->GetNormalizedTime () - c1, 0.0f, 1.0f );

	MOAIAttribute adder;

	size_t total = this->mLinks.Size ();
	for ( size_t i = 0; i < total; ++i ) {
		
		MOAIEaseDriverLink& link = this->mLinks [ i ];
		if ( link.mDest ) {
			
			float delta = 0.0f;
			
			if ( link.mSource ) {
				if ( this->mDirection > 0.0f ) {
				
					t0 = c0 >= 1.0f ? 1.0f : t0;
					t1 = c1 >= 1.0f ? 1.0f : t1;
					
					float v0 = ZLInterpolate::Interpolate ( link.mMode, link.mV0, link.mV1, t0 );
					
					link.mV1 = link.mSource->GetAttributeValue ( link.mSourceAttrID, link.mV1 );
					
					float v1 = ZLInterpolate::Interpolate ( link.mMode, link.mV0, link.mV1, t1 );
					
					delta = v1 - v0;
				}
			}
			else {
				
				float magnitude = ( link.mV1 - link.mV0 );
				if ( magnitude == 0.0f ) continue;
				
				float v0 = link.mV0 + ( magnitude * c0 ) + ZLInterpolate::Interpolate ( link.mMode, 0.0f, magnitude, t0 );
				float v1 = link.mV0 + ( magnitude * c1 ) + ZLInterpolate::Interpolate ( link.mMode, 0.0f, magnitude, t1 );
				
				delta = v1 - v0;
			}
			
			if ( delta != 0.0f ) {
				adder.SetValue ( delta );
				link.mDest->ApplyAttrOp ( link.mDestAttrID, adder, MOAIAttribute::ADD );
				link.mDest->ScheduleUpdate ();
			}
		}
	}
}
