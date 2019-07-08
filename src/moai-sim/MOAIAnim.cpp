// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIAnim.h>
#include <moai-sim/MOAIAnimCurveBase.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	apply
	@text	Apply the animation at a given time or time step.

	@overload	Apply the animation at time t0.

		@in		MOAIAnim self
		@opt	number t0		Default value is 0.
		@out	nil
	
	@overload	Apply the animation for the step t0 to t1.

		@in		MOAIAnim self
		@in		number t0
		@in		number t1
		@out	nil
*/
mrb_value MOAIAnim::_apply ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAnim, "U" );

	float t0 = state.GetParamValue < float >( 1, 0.0f );
	float t1 = state.GetParamValue < float >( 2, t0 );

	self->Apply ( t0, t1 );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	getLength
	@text	Return the length of the animation.
	
	@in		MOAIAnim self
	@out	number length
*/
mrb_value MOAIAnim::_getLength ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAnim, "U" )
	
	return state.ToRValue ( self->GetLength () );
}

//----------------------------------------------------------------//
/**	@lua	reserveLinks
	@text	Reserves a specified number of links for the animation.
	
	@in		MOAIAnim self
	@in		number nLinks
	@out	nil
*/
mrb_value MOAIAnim::_reserveLinks ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAnim, "UN" );
	
	u32 totalLinks = state.GetParamValue < u32 >( 1, 0 );
	self->ReserveLinks ( totalLinks );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setLink
	@text	Connect a curve to a given node attribute.
	
	@in		MOAIAnim self
	@in		number linkID
	@in		MOAIAnimCurveBase curve
	@in		MOAINode target				Target node.
	@in		number attrID				Attribute of the target node to be driven by the curve.
	@opt	boolean asDelta				'true' to apply the curve as a delta instead of an absolute. Default value is false.
	@out	nil
*/
mrb_value MOAIAnim::_setLink ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIAnim, "UNUUN" );
	
	MOAINode* target = state.GetRubyObject < MOAINode >( 3, true );
	if ( !target ) return context;
	
	u32 linkID					= state.GetParamValue < u32 >( 1, 1 ) - 1;
	MOAIAnimCurveBase* curve	= state.GetRubyObject < MOAIAnimCurveBase >( 2, true );
	u32 attrID					= state.GetParamValue < u32 >( 4, 0 );
	bool relative				= state.GetParamValue < bool >( 5, false );
	
	self->SetLink ( linkID, curve, target, attrID, relative );
	
	return context;
}

//================================================================//
// MOAIAnim
//================================================================//

//----------------------------------------------------------------//
void MOAIAnim::Apply ( float t ) {
	
	MOAIAttribute attr;
	
	u32 total = ( u32 )this->mLinks.Size ();
	for ( u32 i = 0; i < total; ++i ) {
		
		MOAIAnimLink& link = this->mLinks [ i ];
		MOAIAnimCurveBase* curve = link.mCurve;
		MOAINode* target = link.mTarget;
		
		if ( curve && target ) {
			
			if ( !link.mRelative ) {
				curve->GetValue ( attr, t );
				target->ApplyAttrOp ( link.mAttrID, attr, MOAIAttribute::SET );
			}
			target->ScheduleUpdate ();
		}
	}
}

//----------------------------------------------------------------//
void MOAIAnim::Apply ( float t0, float t1 ) {
	
	if ( t0 == t1 ) {
		this->Apply ( t0 );
		return;
	}
	
	MOAIAttribute attr;
	
	u32 total = ( u32 )this->mLinks.Size ();
	for ( u32 i = 0; i < total; ++i ) {
		
		MOAIAnimLink& link = this->mLinks [ i ];
		MOAIAnimCurveBase* curve = link.mCurve;
		MOAINode* target = link.mTarget;
		
		if ( curve && target ) {
			
			if ( link.mRelative ) {
				curve->GetDelta ( attr, t0, t1 );
				target->ApplyAttrOp ( link.mAttrID, attr, MOAIAttribute::ADD );
			}
			else {
				curve->GetValue ( attr, t1 );
				target->ApplyAttrOp ( link.mAttrID, attr, MOAIAttribute::SET );
			}
			target->ScheduleUpdate ();
		}
	}
}

//----------------------------------------------------------------//
void MOAIAnim::Clear () {

	this->ClearLinks ();
	this->mLength = 0.0f;
}

//----------------------------------------------------------------//
void MOAIAnim::ClearLinks () {

	for ( u32 i = 0; i < this->mLinks.Size (); ++i ) {
		MOAIAnimLink& link = this->mLinks [ i ];
		link.mCurve.Set ( *this, 0 );
		link.mTarget.Set ( *this, 0 );
	}
	this->mLinks.Clear ();
}

//----------------------------------------------------------------//
MOAIAnim::MOAIAnim () :
	mLength ( 0.0f ) {
	
	RTTI_SINGLE ( MOAITimer )
}

//----------------------------------------------------------------//
MOAIAnim::~MOAIAnim () {

	this->Clear ();
}

//----------------------------------------------------------------//
void MOAIAnim::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
}

//----------------------------------------------------------------//
void MOAIAnim::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "apply", _apply, MRB_ARGS_ARG ( 0, 1 ) | MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "getLength", _getLength, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "reserveLinks", _reserveLinks, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setLink", _setLink, MRB_ARGS_ARG ( 4, 1 ) );
}

//----------------------------------------------------------------//
void MOAIAnim::ReserveLinks ( u32 totalLinks ) {

	this->ClearLinks ();
	this->mLinks.Init ( totalLinks );
}

//----------------------------------------------------------------//
void MOAIAnim::SetLink ( u32 linkID, MOAIAnimCurveBase* curve, MOAINode* target, u32 attrID, bool relative ) {

	if ( linkID >= this->mLinks.Size ()) return;
	if ( !target ) return;
	if ( !target->CheckAttrExists ( attrID )) return;

	MOAIAnimLink& link = this->mLinks [ linkID ];
	link.mCurve.Set ( *this, curve );
	link.mTarget.Set ( *this, target );
	link.mAttrID	= attrID;
	link.mRelative	= relative;
	
	float length = curve->GetLength ();

	if ( this->mLength < length ) {
		this->mLength = length;
	}
	this->mEndTime = this->mLength;
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAIAnim::MOAIAction_Update ( double step ) {

	float t0 = this->GetTime ();
	this->DoStep (( float )step );
	float t1 = this->GetTime ();
	
	this->Apply ( t0, t1 );
}

