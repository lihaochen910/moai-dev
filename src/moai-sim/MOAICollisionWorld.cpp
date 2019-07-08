// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAICollisionPrim.h>
#include <moai-sim/MOAICollisionProp.h>
#include <moai-sim/MOAICollisionShape.h>
#include <moai-sim/MOAICollisionWorld.h>
#include <moai-sim/MOAIDraw.h>
#include <moai-sim/MOAIDebugLines.h>
#include <moai-sim/MOAIGraphicsProp.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIPartitionResultBuffer.h>
#include <moai-sim/MOAIPartitionResultMgr.h>

// uncomment me to debug log
//#define MOAICOLLISIONWORLD_DEBUG

#ifdef MOAICOLLISIONWORLD_DEBUG
	#define DEBUG_LOG printf
#else
	#define DEBUG_LOG(...)
#endif

//================================================================//
// MOAIOverlapHandler
//================================================================//
class MOAIOverlapHandler :
	public MOAIOverlap {
private:

	friend class MOAICollisionWorld;

	//----------------------------------------------------------------//
	void MOAIOverlap_OnOverlap	( const ZLBounds& bounds ) {
		if ( this->mProp0 && this->mProp1 ) {
			this->mProp0->mCollisionWorld->HandleOverlap ( *this->mProp0, 0, *this->mProp1, 0, bounds );
		}
	}
};


//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAICollisionWorld::_processOverlaps ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICollisionWorld, "U" )

	self->ProcessOverlaps ();
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAICollisionWorld::_setCallback ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICollisionWorld, "U" )
	
	self->mCallback.SetRef ( state.GetParamValue ( 1 ) );
	return mrb_nil_value ();
}

//================================================================//
// MOAICollisionWorld
//================================================================//

//----------------------------------------------------------------//
void MOAICollisionWorld::AffirmOverlap ( MOAICollisionProp& prop0, u32 type0, MOAICollisionProp& prop1, u32 type1, const ZLBounds& bounds ) {
	
	MOAIPropOverlapLink* overlapLink = prop0.mOverlapLinks;
	for ( ; overlapLink; overlapLink = overlapLink->mNext ) {
	
		if (( overlapLink->mType == type0 ) && ( overlapLink->mOtherLink->mProp == &prop1 ) && ( overlapLink->mOtherLink->mType == type1 )) {
			MOAIPropOverlap& overlap = *overlapLink->mOverlap;
			overlap.mIsValid = true;
			overlap.mBounds = bounds;
			return;
		}
	}
	
	MOAIPropOverlap* overlap = this->mOverlapPool.Alloc ();
	assert ( overlap );
	
	overlap->mIsValid			= true; // latch is set
	overlap->mBounds			= bounds;
	
	overlap->mLeft.mType		= type0;
	overlap->mLeft.mProp		= &prop0;
	overlap->mLeft.mOtherLink	= &overlap->mRight;
	overlap->mLeft.mOverlap		= overlap;
	overlap->mLeft.mNext		= prop0.mOverlapLinks;
	prop0.mOverlapLinks			= &overlap->mLeft;
	
	overlap->mRight.mType		= type1;
	overlap->mRight.mProp		= &prop1;
	overlap->mRight.mOtherLink	= &overlap->mLeft;
	overlap->mRight.mOverlap	= overlap;
	overlap->mRight.mNext		= prop1.mOverlapLinks;
	prop1.mOverlapLinks			= &overlap->mRight;
	
	overlap->mOverlapListLink.Data ( overlap );
	this->mOverlapList.PushBack ( overlap->mOverlapListLink );
	
	this->DoCallback ( OVERLAP_BEGIN, prop0, prop1, bounds );
}

//----------------------------------------------------------------//
//void MOAICollisionWorld::ClearOverlap ( MOAICollisionProp& prop0, MOAICollisionProp& prop1 ) {
//
//	if ( !( prop0.mOverlapLinks && prop1.mOverlapLinks )) return;
//
//	MOAIPropOverlapLink* cursor = prop0.mOverlapLinks;
//	prop0.mOverlapLinks = 0;
//	
//	while ( cursor ) {
//	
//		MOAIPropOverlapLink* overlapLink = cursor;
//		cursor = cursor->mNext;
//		
//		const MOAICollisionProp* otherProp = overlapLink->mOtherLink->mProp;
//		
//		if ( otherProp == &prop1 ) {
//			
//			if (( prop0.mOverlapFlags | prop1.mOverlapFlags ) & MOAICollisionProp::OVERLAP_EVENTS_LIFECYCLE ) {
//				this->DoCallback ( OVERLAP_END, prop0, prop1 );
//			}
//		
//			prop1.ClearOverlapLink ( prop0 );
//			this->mOverlapList.Remove ( overlapLink->mOverlap->mOverlapListLink );
//			this->mOverlapPool.Free ( overlapLink->mOverlap );
//		}
//		else {
//		
//			overlapLink->mNext = prop0.mOverlapLinks;
//			prop0.mOverlapLinks = overlapLink;
//		}
//	}
//}

//----------------------------------------------------------------//
void MOAICollisionWorld::ClearOverlaps ( MOAICollisionProp& prop ) {
	
	MOAIPropOverlapLink* cursor = prop.mOverlapLinks;
	prop.mOverlapLinks = 0;
	
	while ( cursor ) {
		
		MOAIPropOverlapLink* overlapLink = cursor;
		cursor = cursor->mNext;
		
		MOAICollisionProp& other = *overlapLink->mOtherLink->mProp;
		
		// see if we should broadcast finish events
		u32 flags = prop.mOverlapFlags | other.mOverlapFlags;
		if ( flags & MOAICollisionProp::OVERLAP_EVENTS_LIFECYCLE ) {
			this->DoCallback ( OVERLAP_END, prop, other );
		}
	
		// clear out the linkbacks
		other.ClearOverlapLink ( *overlapLink->mOverlap );
		
		// remove from the list
		this->mOverlapList.Remove ( overlapLink->mOverlap->mOverlapListLink );
		
		// done with the overlap and links
		this->mOverlapPool.Free ( overlapLink->mOverlap );
	}
}

//----------------------------------------------------------------//
void MOAICollisionWorld::DoCallback ( u32 eventID, MOAICollisionProp& prop0, MOAICollisionProp& prop1 ) {
	
	if ( this->mCallback ) {
		
		MOAIRubyState state = MOAIRubyRuntime::Get ().State ();

		mrb_value ret [ 3 ];
		ret [ 0 ] = state.ToRValue ( eventID );
		ret [ 1 ] = state.ToRValue < MOAIRubyObject* >( &prop0 );
		ret [ 2 ] = state.ToRValue < MOAIRubyObject* >( &prop1 );

		state.FuncCall ( this->mCallback, "call", 3, ret );
	}
}

//----------------------------------------------------------------//
void MOAICollisionWorld::DoCallback ( u32 eventID, MOAICollisionProp& prop0, MOAICollisionProp& prop1, const ZLBounds& bounds ) {
	
	if ( this->mCallback ) {
	
		MOAIRubyState state = MOAIRubyRuntime::Get ().State ();

		mrb_value ret [ 4 ];
		ret [ 0 ] = state.ToRValue ( eventID );
		ret [ 1 ] = state.ToRValue < MOAIRubyObject* >( &prop0 );
		ret [ 2 ] = state.ToRValue < MOAIRubyObject* >( &prop1 );
		
		if ( bounds.mStatus == ZLBounds::ZL_BOUNDS_OK ) {

			mrb_value ary = mrb_ary_new ( state );

			mrb_ary_push ( state, ary, state.ToRValue ( bounds.mMin.mX ));
			mrb_ary_push ( state, ary, state.ToRValue ( bounds.mMin.mY ));
			mrb_ary_push ( state, ary, state.ToRValue ( bounds.mMin.mZ ));
			mrb_ary_push ( state, ary, state.ToRValue ( bounds.mMax.mX ));
			mrb_ary_push ( state, ary, state.ToRValue ( bounds.mMax.mY ));
			mrb_ary_push ( state, ary, state.ToRValue ( bounds.mMax.mZ ));
			ret [ 3 ] = ary;
		
			state.FuncCall ( this->mCallback, "call", 4, ret );
		}
		else {
			ret [ 3 ] = mrb_nil_value ();
			state.FuncCall ( this->mCallback, "call", 4, ret );
		}
	}
}

//----------------------------------------------------------------//
void MOAICollisionWorld::HandleOverlap ( MOAICollisionProp& prop0, u32 type0, MOAICollisionProp& prop1, u32 type1, const ZLBounds& bounds ) {

	u32 flags = prop0.mOverlapFlags | prop1.mOverlapFlags;
		
	// create an overlap if needed
	if ( flags & MOAICollisionProp::OVERLAP_EVENTS_LIFECYCLE ) {
		this->AffirmOverlap ( prop0, type0, prop1, type1, bounds );
	}
	
	// send update event and keep both active
	if ( flags & ( MOAICollisionProp::OVERLAP_EVENTS_ON_UPDATE | MOAICollisionProp::OVERLAP_EVENTS_CONTINUOUS )) {
	
		u32 nextPass = this->GetNextPass ();
	
		// TODO: this is only for debug drawing
		prop0.mTouched = nextPass;
		prop1.mTouched = nextPass;
		
		this->DoCallback ( OVERLAP_UPDATE, prop0, prop1, bounds );
	
		if ( flags & MOAICollisionProp::OVERLAP_EVENTS_CONTINUOUS ) {
			this->MakeActive ( prop1 );
			prop0.mStayActive = true;
			prop1.mStayActive = true;
		}
	}

}

//----------------------------------------------------------------//
void MOAICollisionWorld::InvalidateOverlaps ( MOAICollisionProp& prop, u32 nextPass ) {

	MOAIPropOverlapLink* cursor = prop.mOverlapLinks;
	for ( ; cursor; cursor = cursor->mNext ) {
		cursor->mOverlap->mIsValid = cursor->mOtherLink->mProp->mOverlapPass == nextPass;
	}
}

//----------------------------------------------------------------//
void MOAICollisionWorld::MakeActive ( MOAICollisionProp& prop ) {

	if ( !prop.mActiveListLink.List ()) {
		this->mActiveList.PushBack ( prop.mActiveListLink );
	}
}

//----------------------------------------------------------------//
void MOAICollisionWorld::MakeInactive ( MOAICollisionProp& prop ) {

	this->mActiveList.Remove ( prop.mActiveListLink );
}

//----------------------------------------------------------------//
MOAICollisionWorld::MOAICollisionWorld () :
	mUpdated ( false ),
	mOverlapPass ( OVERLAP_PASS_INIT ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIAction )
		RTTI_EXTEND ( MOAIDrawable )
		RTTI_EXTEND ( MOAIPartition )
	RTTI_END
}

//----------------------------------------------------------------//
MOAICollisionWorld::~MOAICollisionWorld () {

	while ( this->mOverlapList.Count ()) {
		MOAIPropOverlap* overlap = this->mOverlapList.Front ();
		this->mOverlapList.Remove ( overlap->mOverlapListLink );
		this->mOverlapPool.Free ( overlap );
	}
}

//----------------------------------------------------------------//
void MOAICollisionWorld::ProcessOverlaps () {

	u32 thisPass = this->mOverlapPass;
	u32 nextPass = thisPass + 1;

	// any prop becomes active when it is updated but only if
	// flags are set

	// a prop's status is given by the combination of its overlap
	// flags and all others
	// OVERLAP_EVENTS_CONTINUOUS - both stay 'active' after overlap
	// OVERLAP_EVENTS_LIFECYCLE - 'active' when updated; does not stay in overlap list

	// cycle through the active props
	ActiveListIt activeIt = this->mActiveList.Head ();
	while ( activeIt ) {
		
		MOAICollisionProp& prop = *activeIt->Data ();
		this->InvalidateOverlaps ( prop, nextPass );
		
		u32 interfaceMask = this->GetInterfaceMask < MOAICollisionProp >();
		
		// this gives us the coarse filter based on world space bounds
		// TODO: find a way to utilize overlap flags?
		MOAIScopedPartitionResultBufferHandle scopedBufferHandle = MOAIPartitionResultMgr::Get ().GetBufferHandle ();
		MOAIPartitionResultBuffer& buffer = scopedBufferHandle;

		u32 totalResults = this->GatherHulls ( buffer, &prop, prop.GetWorldBounds (), interfaceMask );
		
		for ( u32 i = 0; i < totalResults; ++i ) {
		
			MOAIPartitionResult* result = buffer.GetResultUnsafe ( i );
			MOAICollisionProp* otherProp = result->AsType < MOAICollisionProp >();
			
			if ( !otherProp ) continue;
			if ( !(( prop.mCategory & otherProp->mMask ) || ( prop.mMask & otherProp->mCategory ))) continue;
			if ( otherProp->mOverlapPass == nextPass ) continue; // has been processed
			
			// this calculates the detailed overlap, updates the links and sends overlap events
			MOAIOverlapHandler overlapHandler;
			overlapHandler.SetCalculateBounds ((( prop.mOverlapFlags | otherProp->mOverlapFlags ) & MOAICollisionProp::OVERLAP_CALCULATE_BOUNDS ) != 0 );
			overlapHandler.Process ( prop, *otherProp );
		}
		
		this->PruneOverlaps ( prop );
		
		prop.mOverlapPass = nextPass;
		activeIt = activeIt->Next ();

		if ( !prop.mStayActive ) {
			this->MakeInactive ( prop );
		}
		prop.mStayActive = false;
	}
	
	this->mOverlapPass = nextPass;
}

//----------------------------------------------------------------//
void MOAICollisionWorld::PruneOverlaps ( MOAICollisionProp& prop ) {

	MOAIPropOverlapLink* cursor = prop.mOverlapLinks;
	prop.mOverlapLinks = 0;
	
	while ( cursor ) {
	
		MOAIPropOverlapLink* overlapLink = cursor;
		cursor = cursor->mNext;
		
		MOAICollisionProp& other = *overlapLink->mOtherLink->mProp;
		MOAIPropOverlap& overlap = *overlapLink->mOverlap;
		
		if ( !overlap.mIsValid ) {
			
			if (( prop.mOverlapFlags | other.mOverlapFlags ) & MOAICollisionProp::OVERLAP_EVENTS_LIFECYCLE ) {
				this->DoCallback ( OVERLAP_END, prop, other );
			}
		
			other.ClearOverlapLink ( overlap );
			this->mOverlapList.Remove ( overlapLink->mOverlap->mOverlapListLink );
			this->mOverlapPool.Free ( overlapLink->mOverlap );
		}
		else {
		
			overlapLink->mNext = prop.mOverlapLinks;
			prop.mOverlapLinks = overlapLink;
		}
	}
}

//----------------------------------------------------------------//
void MOAICollisionWorld::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIAction::RegisterRubyClass ( state, klass );
	MOAIPartition::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "OVERLAP_BEGIN",		( u32 )OVERLAP_BEGIN );
	state.DefineClassConst ( klass, "OVERLAP_END",			( u32 )OVERLAP_END );
	state.DefineClassConst ( klass, "OVERLAP_UPDATE",		( u32 )OVERLAP_UPDATE );
}

//----------------------------------------------------------------//
void MOAICollisionWorld::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIAction::RegisterRubyFuncs ( state, klass );
	MOAIPartition::RegisterRubyFuncs ( state, klass );
	
	state.DefineInstanceMethod ( klass, "processOverlaps",	_processOverlaps, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setCallback",		_setCallback, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAICollisionWorld::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	MOAIAction::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAICollisionWorld::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	MOAIAction::SerializeOut ( state, serializer );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAICollisionWorld::MOAIAction_IsDone () {

	return false;
}

//----------------------------------------------------------------//
void MOAICollisionWorld::MOAIAction_Update ( double step ) {
	UNUSED ( step );

	this->ResetShapeStream ();
	this->ProcessOverlaps ();
}

//----------------------------------------------------------------//
void MOAICollisionWorld::MOAIPartition_DrawDebugFront () {
	
	if ( this->GetShapeStreamSize ()) {
	
		MOAIGfxMgr::Get ().mGfxState.SetMtx ( MOAIGfxState::MODEL_TO_WORLD_MTX );
		this->DrawShapeStream ();
	}
}

//----------------------------------------------------------------//
void MOAICollisionWorld::MOAIPartition_OnInsertHull ( MOAIPartitionHull& hull ) {

	MOAICollisionProp* prop = hull.AsType < MOAICollisionProp >();

	if ( prop && ( prop->mCollisionWorld != this )) {
	
		// must be set before calling set partition
		prop->mCollisionWorld = this;
	
		// now activate the prop
		if ( prop->mOverlapFlags ) {
			this->MakeActive ( *prop );
		}
	}
}

//----------------------------------------------------------------//
void MOAICollisionWorld::MOAIPartition_OnRemoveHull ( MOAIPartitionHull& hull ) {

	MOAICollisionProp* prop = hull.AsType < MOAICollisionProp >();

	if ( prop ) {
		assert ( prop->mCollisionWorld == this );
		
		this->ClearOverlaps ( *prop );
		this->MakeInactive ( *prop );
		prop->mCollisionWorld = 0;
	}
}

//----------------------------------------------------------------//
void MOAICollisionWorld::MOAIPartition_OnUpdateHull ( MOAIPartitionHull& hull ) {
	UNUSED ( hull );
}
