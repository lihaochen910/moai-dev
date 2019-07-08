// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAICamera.h>
#include <moai-sim/MOAICollisionProp.h>
#include <moai-sim/MOAICollisionShape.h>
#include <moai-sim/MOAICollisionWorld.h>
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIDebugLines.h>
#include <moai-sim/MOAIDraw.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAILayoutFrame.h>
#include <moai-sim/MOAIMoveConstraint2D.h>
#include <moai-sim/MOAIOverlap.h>
#include <moai-sim/MOAIOverlapResolver.h>
#include <moai-sim/MOAIPartition.h>
#include <moai-sim/MOAIPartitionResultMgr.h>
#include <moai-sim/MOAIPartitionResultBuffer.h>
#include <moai-sim/MOAIRenderMgr.h>
#include <moai-sim/MOAIScissorRect.h>
#include <moai-sim/MOAIShader.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAISurfaceSampler2D.h>
#include <moai-sim/MOAITexture.h>
#include <moai-sim/MOAITextureBase.h>
#include <moai-sim/MOAIViewport.h>

// uncomment me to debug log
//#define MOAICOLLISIONPROP_DEBUG

#ifdef MOAICOLLISIONPROP_DEBUG
	#define DEBUG_LOG printf
#else
	#define DEBUG_LOG(...)
#endif

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAICollisionProp::_collisionMove ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICollisionProp, "U" )
	
	ZLVec3D move	= state.GetParamValue < ZLVec3D >( 1, ZLVec3D::ORIGIN );
	u32 detach		= state.GetParamValue < u32 >( 4, SURFACE_MOVE_DETACH );
	u32 maxSteps	= state.GetParamValue < u32 >( 5, DEFAULT_MAX_MOVE_STEPS );
	
	self->Move ( move, detach, maxSteps );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAICollisionProp::_getOverlaps ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICollisionProp, "U" )

	MOAIPropOverlapLink* link = self->mOverlapLinks;
	mrb_value ary = mrb_ary_new ( M );
	for ( ; link; link = link->mNext ) {
		mrb_ary_push ( M, ary, state.ToRValue < MOAIRubyObject* >( link->mOtherLink->mProp ) );
	}
	return ary;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAICollisionProp::_hasOverlaps ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICollisionProp, "U" )

	return state.ToRValue ( self->mOverlapLinks != 0 );
}

//----------------------------------------------------------------//
// TODO: doxygen
//mrb_value MOAICollisionProp::_setGroupMask ( mrb_state* M, mrb_value context ) {
//	MOAI_RUBY_SETUP ( MOAICollisionProp, "U" )
//	
//	self->mGroupMask = state.GetParamValue < u32 >( 2, 0 );
//	return mrb_nil_value ();
//}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAICollisionProp::_setOverlapFlags ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICollisionProp, "U" )
	
	self->mOverlapFlags = state.GetParamValue < u32 >( 1, 0 );
	return mrb_nil_value ();
}

//================================================================//
// MOAICollisionProp
//================================================================//

//----------------------------------------------------------------//
void MOAICollisionProp::ClearOverlapLink ( MOAIPropOverlap& overlap ) {

	MOAIPropOverlapLink* cursor = this->mOverlapLinks;
	this->mOverlapLinks = 0;
	
	while ( cursor ) {
		MOAIPropOverlapLink* overlapLink = cursor;
		cursor = cursor->mNext;
		
		if ( overlapLink->mOverlap != &overlap ) {
			overlapLink->mNext = this->mOverlapLinks;
			this->mOverlapLinks = overlapLink;
		}
	}
}

//----------------------------------------------------------------//
void MOAICollisionProp::DrawContactPoints ( MOAIAbstractDrawShape& draw, const MOAIMoveConstraint2D* contacts, u32 nContacts ) {

	MOAIDebugLinesMgr& debugLines = MOAIDebugLinesMgr::Get ();
	if ( !( debugLines.IsVisible () && debugLines.SelectStyleSet < MOAICollisionProp >())) return;

	draw.SetPenWidth ( 1.0f );

	for ( u32 i = 0; i < nContacts; ++i ) {
	
		const MOAIMoveConstraint2D& contact = contacts [ i ];
	
		ZLVec3D point ( contact.mPoint.mX, contact.mPoint.mY, 0.0f );
		ZLVec3D normal ( contact.mNormal.mX, contact.mNormal.mY, 0.0f );
		const ZLVec2D cornerTangent = contact.mCornerTangent;
		const ZLVec2D edgeNormal = contact.mEdgeNormal;
		
		
		if ( debugLines.Bind ( DEBUG_DRAW_COLLISION_CONTACT_NORMAL, draw )) {
			draw.DrawRay ( point.mX, point.mY, normal.mX, normal.mY, 32.0f );
		}
		
		bool drawPoint = false;
		
		switch ( contact.mType ) {
				
			case MOAIMoveConstraint2D::CORNER:
				
				if ( debugLines.Bind ( DEBUG_DRAW_COLLISION_CONTACT_POINT_CORNER_EDGE_NORMAL, draw )) {
					draw.DrawRay ( point.mX, point.mY, edgeNormal.mX, edgeNormal.mY, 32.0f );
				}
				
				if ( debugLines.Bind ( DEBUG_DRAW_COLLISION_CONTACT_POINT_CORNER_TANGENT, draw )) {
					draw.DrawRay ( point.mX, point.mY, cornerTangent.mX, cornerTangent.mY, 48.0f );
				}
			
				drawPoint = debugLines.Bind ( DEBUG_DRAW_COLLISION_CONTACT_POINT_CORNER, draw );
				break;
				
			case MOAIMoveConstraint2D::CROSSING:

				drawPoint = debugLines.Bind ( DEBUG_DRAW_COLLISION_CONTACT_POINT_CROSSING, draw );
				break;
				
			case MOAIMoveConstraint2D::LEAVING:

				drawPoint = debugLines.Bind ( DEBUG_DRAW_COLLISION_CONTACT_POINT_LEAVING, draw );
				break;
			
			case MOAIMoveConstraint2D::PARALLEL:

				drawPoint = debugLines.Bind ( DEBUG_DRAW_COLLISION_CONTACT_POINT_PARALLEL, draw );
				break;
		}
		
		if ( drawPoint ) {
			draw.DrawRectFill ( point.mX - 4.0f, point.mY + 4.0f, point.mX + 4.0f, point.mY - 4.0f );
		}
	}
}

//----------------------------------------------------------------//
void MOAICollisionProp::GatherAndProcess ( MOAICollisionPrimVisitor& visitor, const ZLBox& worldBounds ) {

	MOAICollisionWorld& world = *this->mCollisionWorld;

	MOAIScopedPartitionResultBufferHandle scopedBufferHandle = MOAIPartitionResultMgr::Get ().GetBufferHandle ();
	MOAIPartitionResultBuffer& buffer = scopedBufferHandle;
	
	u32 interfaceMask = world.GetInterfaceMask < MOAICollisionProp >();
	u32 totalResults = world.GatherHulls ( buffer, this, worldBounds, interfaceMask );
	
	for ( u32 i = 0; i < totalResults; ++i ) {
		MOAIPartitionResult* result = buffer.GetResultUnsafe ( i );
		MOAICollisionProp* other = result->AsType < MOAICollisionProp >();
		if ( !other ) continue;
		
		visitor.Process ( *this, *other );
	}
}

//----------------------------------------------------------------//
MOAICollisionShape* MOAICollisionProp::GetCollisionShape () {

	return this->mDeck ? this->mDeck->GetCollisionShape ( this->mIndex - 1 ) : 0;
}

//----------------------------------------------------------------//
bool MOAICollisionProp::IsActive () {

	return this->mActiveListLink.List () != 0;
}

//----------------------------------------------------------------//
MOAICollisionProp::MOAICollisionProp () :
	mCategory ( CATEGORY_MASK_ALL ),
	mMask ( CATEGORY_MASK_ALL ),
	mOverlapFlags ( DEFAULT_OVERLAP_FLAGS ),
	mOverlapPass ( MOAICollisionWorld::OVERLAP_PASS_INIT ),
	mOverlapLinks ( 0 ),
	mStayActive ( false ),
	mTouched ( MOAICollisionWorld::OVERLAP_PASS_INIT ),
	mCollisionWorld ( 0 ) {

	RTTI_BEGIN
		RTTI_EXTEND ( MOAIPartitionHull )
		RTTI_EXTEND ( MOAIDrawable )
		RTTI_EXTEND ( MOAIIndexedPropBase )
	RTTI_END
	
	this->mActiveListLink.Data ( this );
}

//----------------------------------------------------------------//
MOAICollisionProp::~MOAICollisionProp () {
}

//----------------------------------------------------------------//
void MOAICollisionProp::Move ( ZLVec3D move, u32 detach, u32 maxSteps ) {

	MOAIDebugLinesMgr& debugLines = MOAIDebugLinesMgr::Get ();
	bool drawDebug = ( debugLines.IsVisible () && debugLines.SelectStyleSet < MOAICollisionProp >());

	// just cram everything in here for now.
	// possible to break into steps for collision world later *if*
	// more traditional physics approach is desired.
	
	// purpose here is just a proof of concept for an
	// edge-following algorithm. will make all this user-configurable
	// later.
	
	MOAIMoveConstraint2D* contacts = ( MOAIMoveConstraint2D* )alloca ( 128 * sizeof ( MOAIMoveConstraint2D ));
	MOAIMoveConstraintAccumulator2D contactAccumulator ( contacts, 128 );
	
	ZLVec2D moveNorm ( move.mX, move.mY );
	float moveLength = moveNorm.NormSafe ();
	
	if ( moveLength > EPSILON ) {
	
		for ( u32 i = 0; i < maxSteps; ++i ) {
		
			// find best contact points
			
			float bestPushDot = 2.0f;
			const MOAIMoveConstraint2D* bestPushContact = 0;
			
			float bestPullDot = 2.0f;
			const MOAIMoveConstraint2D* bestPullContact = 0;
		
			// find contacts
			ZLBox worldBounds = this->GetWorldBounds ();
			worldBounds.Inflate ( moveLength * 1.5f ); // TODO: epsilon
		
			contactAccumulator.Reset ();
			this->GatherAndProcess ( contactAccumulator, worldBounds );
			u32 nContacts = contactAccumulator.Top ();
			u32 nPushContacts = 0;
			
			for ( u32 j = 0; j < nContacts; ++j ) {
			
				const MOAIMoveConstraint2D& contact = contacts [ j ];
				
				// ignore corner contacts if they are behind the move
				if ( contact.mType == MOAIMoveConstraint2D::CORNER ) {
					if ( !(( moveNorm.Dot ( contact.mCornerTangent ) > -EPSILON ) && ( moveNorm.Dot ( contact.mEdgeNormal ) > -EPSILON ))) continue;
				}
				
				float d = moveNorm.Dot ( contact.mNormal );
				
				if ( d <= EPSILON ) {

					nPushContacts++;

					// heading into - push
					if ( d < bestPushDot ) {
						bestPushContact = &contact;
						bestPushDot = d;
					}
				}
				else if ( detach != SURFACE_MOVE_DETACH ){

					// heading out of - pull
					if ( d < bestPullDot ) {
						bestPullContact = &contact;
						bestPullDot = d;
					}
				}
			}

			const MOAIMoveConstraint2D* bestContact = 0;

			if ( bestPushContact ) {

				// straight into a wall
				if ( bestPushDot <= ( EPSILON - 1.0f )) {
					break;
				}
				bestContact = bestPushContact;
			}
			else if ( bestPullContact ) {
			
				// straight out of a wall
				if ( bestPullDot >= ( 1.0f - EPSILON )) {
					if ( detach != SURFACE_MOVE_SLIDE ) {
						break;
					}
				}
				else {
					bestContact = bestPullContact;
				}
			}
			else if (( nContacts ) && ( detach == SURFACE_MOVE_LOCK )) {
			
				break;
			}

			ZLVec2D stepMoveNorm;
			float stepMoveLength;

			if ( bestContact ) {
				
				stepMoveNorm.Init ( move.mX, move.mY );
				stepMoveNorm.PerpProject ( bestContact->mNormal );
				stepMoveNorm.Norm ();
				
				float maxMove = stepMoveNorm.Dot ( bestContact->mTangent ) > 0.0f ? bestContact->mPosD : bestContact->mNegD;
				stepMoveLength = moveLength < maxMove ? moveLength : maxMove;
				
				DEBUG_LOG ( "----> STEP: (%g, %g) EDGE (%g, %g) MAX MOVE: %g\n",
					move.mX, move.mY,
					bestContact->mTangent.mX, bestContact->mTangent.mY,
					maxMove
				);
				DEBUG_LOG ( "\n" );
			}
			else {
			
				stepMoveNorm.Init ( moveNorm.mX, moveNorm.mY );
				stepMoveLength = moveLength;
			}
			
			// TODO: final check that we aren't moving into a wall

			// move (and force a dep node update to recompute the transform and bounds)
			// TODO: this needs to be *way* more efficient!
			this->mLoc.mX += stepMoveNorm.mX * stepMoveLength;
			this->mLoc.mY += stepMoveNorm.mY * stepMoveLength;
			this->MOAITransformBase::MOAINode_Update ();
			this->MOAIPartitionHull::MOAINode_Update ();

			// prepare the next step or break

			moveLength = moveLength - stepMoveLength;
			if ( moveLength < EPSILON ) break;
		}
	}
	
	MOAICollisionProp::DrawContactPoints ( *this->mCollisionWorld, contacts, contactAccumulator.Top ());
	
	// resolve overlaps
	MOAIOverlapResolver overlapResolver;
	this->GatherAndProcess ( overlapResolver, this->GetWorldBounds ());
	
	ZLVec3D resolveOverlaps = overlapResolver.GetResult ();
	
	this->mLoc.Add ( resolveOverlaps );
	this->ScheduleUpdate ();

	MOAIDrawShapeRetained& draw = *this->mCollisionWorld;

	if ( drawDebug && debugLines.Bind ( DEBUG_DRAW_COLLISION_MOVE_RETICLE, draw )) {

		if ( move.LengthSqrd () && this->mCollisionWorld ) {
			
			ZLRect worldRect = this->GetWorldBounds ().GetRect ();
			float width = worldRect.Width ();
			float height = worldRect.Height ();
			
			float radius = ( width < height ? width : height ) * 0.25f;
			
			draw.DrawCircleOutline ( this->mLoc.mX, this->mLoc.mY, radius, 32 );
			
			move.Norm ();
			move.Scale ( radius );
			draw.DrawLine ( this->mLoc.mX, this->mLoc.mY, this->mLoc.mX + move.mX, this->mLoc.mY + move.mY );
		}
	}
}

//----------------------------------------------------------------//
void MOAICollisionProp::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIPartitionHull::RegisterRubyClass ( state, klass );
	MOAIIndexedPropBase::RegisterRubyClass ( state, klass );
	
	MOAIDebugLinesMgr::Get ().ReserveStyleSet < MOAICollisionProp >( TOTAL_DEBUG_LINE_STYLES );
	
	state.DefineClassConst ( klass, "DEBUG_DRAW_COLLISION_PROP_MASTER",							MOAIDebugLinesMgr::Pack < MOAICollisionProp >( (u32) -1 ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_COLLISION_ACTIVE_PROP_BOUNDS",					MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_ACTIVE_PROP_BOUNDS ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_COLLISION_ACTIVE_OVERLAP_PROP_BOUNDS",			MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_ACTIVE_OVERLAP_PROP_BOUNDS ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_COLLISION_ACTIVE_TOUCHED_PROP_BOUNDS",			MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_ACTIVE_TOUCHED_PROP_BOUNDS ));
	
	state.DefineClassConst ( klass, "DEBUG_DRAW_COLLISION_CONTACT_NORMAL",						MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_CONTACT_NORMAL ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_COLLISION_CONTACT_POINT_CORNER",				MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_CONTACT_POINT_CORNER ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_COLLISION_CONTACT_POINT_CORNER_EDGE_NORMAL",	MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_CONTACT_POINT_CORNER_EDGE_NORMAL ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_COLLISION_CONTACT_POINT_CORNER_TANGENT",		MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_CONTACT_POINT_CORNER_TANGENT ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_COLLISION_CONTACT_POINT_CROSSING",				MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_CONTACT_POINT_CROSSING ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_COLLISION_CONTACT_POINT_LEAVING",				MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_CONTACT_POINT_LEAVING ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_COLLISION_CONTACT_POINT_PARALLEL",				MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_CONTACT_POINT_PARALLEL ));
	
	state.DefineClassConst ( klass, "DEBUG_DRAW_COLLISION_MOVE_RETICLE",						MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_MOVE_RETICLE ));
	
	state.DefineClassConst ( klass, "DEBUG_DRAW_COLLISION_OVERLAP_PROP_BOUNDS",					MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_OVERLAP_PROP_BOUNDS ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_COLLISION_OVERLAPS",							MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_OVERLAPS ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_COLLISION_WORLD_BOUNDS",						MOAIDebugLinesMgr::Pack < MOAICollisionProp >( DEBUG_DRAW_COLLISION_WORLD_BOUNDS ));
	
	state.DefineClassConst ( klass, "OVERLAP_EVENTS_ON_UPDATE",		( u32 )OVERLAP_EVENTS_ON_UPDATE );
	state.DefineClassConst ( klass, "OVERLAP_EVENTS_CONTINUOUS",	( u32 )OVERLAP_EVENTS_CONTINUOUS );
	state.DefineClassConst ( klass, "OVERLAP_EVENTS_LIFECYCLE",		( u32 )OVERLAP_EVENTS_LIFECYCLE );
	state.DefineClassConst ( klass, "OVERLAP_GRANULARITY_FINE",		( u32 )OVERLAP_GRANULARITY_FINE );
	state.DefineClassConst ( klass, "OVERLAP_CALCULATE_BOUNDS",		( u32 )OVERLAP_CALCULATE_BOUNDS );
	
	state.DefineClassConst ( klass, "SURFACE_MOVE_DETACH",			( u32 )SURFACE_MOVE_DETACH );
	state.DefineClassConst ( klass, "SURFACE_MOVE_SLIDE",			( u32 )SURFACE_MOVE_SLIDE );
	state.DefineClassConst ( klass, "SURFACE_MOVE_LOCK",			( u32 )SURFACE_MOVE_LOCK );
	
	state.DefineClassConst ( klass, "CATEGORY_MASK_ALL",			( u32 )CATEGORY_MASK_ALL );
}

//----------------------------------------------------------------//
void MOAICollisionProp::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIPartitionHull::RegisterRubyFuncs ( state, klass );
	MOAIIndexedPropBase::RegisterRubyFuncs ( state, klass );
	
	state.DefineInstanceMethod ( klass, "collisionMove",	_collisionMove, MRB_ARGS_ARG ( 3, 2 ) );
	state.DefineInstanceMethod ( klass, "getOverlaps",		_getOverlaps, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "hasOverlaps",		_hasOverlaps, MRB_ARGS_NONE () );
	//state.DefineInstanceMethod ( klass, "setGroupMask",		_setGroupMask, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setOverlapFlags",	_setOverlapFlags, MRB_ARGS_ARG ( 0, 1 ) );

}

//----------------------------------------------------------------//
void MOAICollisionProp::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	
	MOAIPartitionHull::SerializeIn ( state, serializer );
	MOAIIndexedPropBase::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAICollisionProp::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	
	MOAIPartitionHull::SerializeOut ( state, serializer );
	MOAIIndexedPropBase::SerializeOut ( state, serializer );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAICollisionProp::MOAIDrawable_DrawDebug ( int subPrimID ) {
	UNUSED ( subPrimID );

	MOAIDebugLinesMgr& debugLines = MOAIDebugLinesMgr::Get ();
	if ( !( debugLines.IsVisible () && debugLines.SelectStyleSet < MOAICollisionProp >())) return;

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	MOAIDraw& draw = MOAIDraw::Get ();
	UNUSED ( draw ); // mystery warning in vs2008
	
	draw.Bind ();

	if ( debugLines.Bind ( MOAICollisionProp::DEBUG_DRAW_COLLISION_WORLD_BOUNDS )) {
		gfxState.SetVertexTransform ( MOAIGfxState::WORLD_TO_CLIP_MTX );
		draw.DrawBoxOutline ( this->GetWorldBounds ());
	}
	
	MOAICollisionShape* shape = this->GetCollisionShape ();
		
	if ( shape ) {
		const ZLAffine3D& localToWorldMtx = this->GetLocalToWorldMtx ();
		gfxState.SetMtx ( MOAIGfxState::MODEL_TO_WORLD_MTX, localToWorldMtx );
		shape->Draw ( localToWorldMtx );
	}

	bool visible = false;
	
	if ( this->IsActive ()) {
		
		if ( this->mOverlapLinks ) {
			visible = debugLines.Bind ( DEBUG_DRAW_COLLISION_ACTIVE_OVERLAP_PROP_BOUNDS );
		}
		
		if ( this->mTouched == this->mOverlapPass && !visible ) {
			visible = debugLines.Bind ( DEBUG_DRAW_COLLISION_ACTIVE_TOUCHED_PROP_BOUNDS );
		}
		
		if ( !visible ) {
			visible = debugLines.Bind ( DEBUG_DRAW_COLLISION_ACTIVE_PROP_BOUNDS );
		}
	}
	
	if ( this->mOverlapLinks && !visible ) {
		visible = debugLines.Bind ( DEBUG_DRAW_COLLISION_OVERLAP_PROP_BOUNDS );
	}
	
	if ( visible ) {
		
		shape = this->GetCollisionShape ();
		
		if ( shape ) {
			const ZLAffine3D& localToWorldMtx = this->GetLocalToWorldMtx ();
			gfxState.SetMtx ( MOAIGfxState::MODEL_TO_WORLD_MTX, localToWorldMtx );
			shape->Draw ( localToWorldMtx );
		}
		else {
			gfxState.SetVertexTransform ( MOAIGfxState::WORLD_TO_CLIP_MTX );
			draw.DrawBoxOutline ( this->GetWorldBounds ());
		}
	}
	
	if ( debugLines.Bind ( MOAICollisionProp::DEBUG_DRAW_COLLISION_OVERLAPS )) {
	
		gfxState.SetVertexTransform ( MOAIGfxState::WORLD_TO_CLIP_MTX );

		MOAIPropOverlapLink* overlapLinkIt = this->mOverlapLinks;
		for ( ; overlapLinkIt; overlapLinkIt = overlapLinkIt->mNext ) {
			const ZLBounds& bounds = overlapLinkIt->mOverlap->mBounds;
			if ( bounds.mStatus == ZLBounds::ZL_BOUNDS_OK ) {
				draw.DrawBoxOutline ( bounds );
			}
		}
	}
}

//----------------------------------------------------------------//
bool MOAICollisionProp::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	if ( MOAIIndexedPropBase::MOAINode_ApplyAttrOp ( attrID, attr, op )) return true;
	if ( MOAIPartitionHull::MOAINode_ApplyAttrOp ( attrID, attr, op )) return true;
	return false;
}

//----------------------------------------------------------------//
void MOAICollisionProp::MOAINode_Update () {
	
	MOAIPartitionHull::MOAINode_Update ();
	
	if ( this->mCollisionWorld && this->mOverlapFlags ) {
		this->mCollisionWorld->MakeActive ( *this );
	}
}

//----------------------------------------------------------------//
void MOAICollisionProp::MOAIPartitionHull_AddToSortBuffer ( MOAIPartitionResultBuffer& buffer, u32 key ) {

	buffer.PushResult ( *this, key, NO_SUBPRIM_ID, this->GetPriority (), this->GetWorldLoc (), this->GetWorldBounds ());
}

//----------------------------------------------------------------//
u32 MOAICollisionProp::MOAIPartitionHull_AffirmInterfaceMask ( MOAIPartition& partition ) {

	return (
		partition.AffirmInterfaceMask < MOAICollisionProp >() |
		partition.AffirmInterfaceMask < MOAIDrawable >()
	);
}

//----------------------------------------------------------------//
ZLBounds MOAICollisionProp::MOAIPartitionHull_GetModelBounds () {

	MOAICollisionShape* shape = this->GetCollisionShape ();
	if ( shape ) {
		ZLBounds bounds;
		bounds.Init ( shape->GetBounds ());
		return bounds;
	}
	return ZLBounds::EMPTY;
}

//----------------------------------------------------------------//
bool MOAICollisionProp::MOAIPartitionHull_PrepareForInsertion ( const MOAIPartition& partition ) {
	UNUSED ( partition );
	return true;
}

//----------------------------------------------------------------//
void MOAICollisionProp::MOAIPartitionHull_WasRemovedFromPartition () {

	this->mCollisionWorld = 0;
}
