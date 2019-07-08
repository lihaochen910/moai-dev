// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIDebugLines.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAILayoutFrame.h>
#include <moai-sim/MOAIRenderMgr.h>
#include <moai-sim/MOAIPartition.h>
#include <moai-sim/MOAIPartitionHolder.h>
#include <moai-sim/MOAIPartitionResultBuffer.h>
#include <moai-sim/MOAIPartitionHull.h>
#include <moai-sim/MOAIScissorRect.h>
#include <moai-sim/MOAIShader.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAISurfaceSampler2D.h>
#include <moai-sim/MOAITexture.h>
#include <moai-sim/MOAITextureBase.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getBounds
	@text	Return the prop's local bounds or 'nil' if prop bounds is
			global or missing. The bounds are in model space and will
			be overridden by the prop's bounds if it's been set (using
			setBounds ())
	
	@in		MOAIPartitionHull self
	@out	number xMin
	@out	number yMin
	@out	number zMin
	@out	number xMax
	@out	number yMax
	@out	number zMax
*/
mrb_value MOAIPartitionHull::_getBounds ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHull, "U" )
	
	ZLBounds bounds = self->GetModelBounds ();
	if ( bounds.mStatus != ZLBounds::ZL_BOUNDS_OK ) return context;

	mrb_value ret [ 6 ];
	ret [ 0 ] = state.ToRValue ( bounds.mMin.mX );
	ret [ 1 ] = state.ToRValue ( bounds.mMin.mY );
	ret [ 2 ] = state.ToRValue ( bounds.mMin.mZ );

	ret [ 3 ] = state.ToRValue ( bounds.mMax.mX );
	ret [ 4 ] = state.ToRValue ( bounds.mMax.mY );
	ret [ 5 ] = state.ToRValue ( bounds.mMax.mZ );

	return mrb_ary_new_from_values ( state, 6, ret );
}

//----------------------------------------------------------------//
/**	@lua	getDims
	@text	Return the prop's width and height or 'nil' if prop rect is global.
               
	@in		MOAIPartitionHull self
	@out	number width		X max - X min
	@out	number height		Y max - Y min
	@out	number depth		Z max - Z min
*/
mrb_value MOAIPartitionHull::_getDims ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHull, "U" )

	ZLBounds bounds = self->GetModelBounds ();
	if ( bounds.mStatus != ZLBounds::ZL_BOUNDS_OK ) return context;
 
	mrb_value ret [ 3 ];
	ret [ 0 ] = state.ToRValue ( bounds.mMax.mX - bounds.mMin.mX );
	ret [ 1 ] = state.ToRValue ( bounds.mMax.mY - bounds.mMin.mY );
	ret [ 2 ] = state.ToRValue ( bounds.mMax.mZ - bounds.mMin.mZ );

	return mrb_ary_new_from_values ( state, 3, ret );
}

//----------------------------------------------------------------//
/**	@lua	getPartition
	@text	Returns the partition prop is currently held in.
	
	@in		MOAIPartitionHull self
	@out	MOAIPartition partition
*/
mrb_value	MOAIPartitionHull::_getPartition ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHull, "U" )

	if ( self->mPartition ) {
		return self->mPartition->PushRubyUserdata ( state );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	getPriority
	@text	Returns the current priority of the node or 'nil' if the
			priority is uninitialized.
	
	@in		MOAIPartitionHull self
	@out	number priority		The node's priority or nil.
*/
mrb_value MOAIPartitionHull::_getPriority ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHull, "U" )
	
	if ( self->mPriority != UNKNOWN_PRIORITY ) {
		return state.ToRValue ( self->mPriority );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	getWorldBounds
	@text	Return the prop's world bounds or 'nil' if prop bounds is
			global or missing.
	
	@in		MOAIPartitionHull self
	@out	number xMin
	@out	number yMin
	@out	number zMin
	@out	number xMax
	@out	number yMax
	@out	number zMax
*/
mrb_value MOAIPartitionHull::_getWorldBounds ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHull, "U" )
	
	self->ForceUpdate ();
	
	if ( self->mPartition->IsGlobal ( *self )) return context;
	if ( self->mPartition->IsEmpty ( *self )) return context;
	
	ZLBox bounds = self->mWorldBounds;

	mrb_value ret [ 6 ];
	ret [ 0 ] = state.ToRValue ( bounds.mMin.mX );
	ret [ 1 ] = state.ToRValue ( bounds.mMin.mY );
	ret [ 2 ] = state.ToRValue ( bounds.mMin.mZ );

	ret [ 3 ] = state.ToRValue ( bounds.mMax.mX );
	ret [ 4 ] = state.ToRValue ( bounds.mMax.mY );
	ret [ 5 ] = state.ToRValue ( bounds.mMax.mZ );

	return mrb_ary_new_from_values ( state, 6, ret );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIPartitionHull::_getWorldBoundsCenter ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHull, "U" )
	
	self->ForceUpdate ();
	
	if ( self->mPartition->IsGlobal ( *self )) return context;
	if ( self->mPartition->IsEmpty ( *self )) return context;
	
	ZLBox bounds = self->mWorldBounds;
	
	ZLVec3D center;
	bounds.GetCenter ( center );

	mrb_value ret [ 3 ];
	ret [ 0 ] = state.ToRValue ( center.mX );
	ret [ 1 ] = state.ToRValue ( center.mY );
	ret [ 2 ] = state.ToRValue ( center.mZ );

	return mrb_ary_new_from_values ( state, 3, ret );
}

//----------------------------------------------------------------//
/**	@lua	insid
	@text	Returns true if the given world space point falls inside
			the prop's bounds.
	
	@in		MOAIPartitionHull self
	@in		number x
	@in		number y
	@in		number z
	@opt	number pad			Pad the hit bounds (in the prop's local space)
	@out	boolean isInside
*/
mrb_value MOAIPartitionHull::_inside ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHull, "U" )

	ZLVec3D vec;
	vec.mX	= state.GetParamValue < float >( 1, 0.0f );
	vec.mY	= state.GetParamValue < float >( 2, 0.0f );
	vec.mZ	= state.GetParamValue < float >( 3, 0.0f );

	float pad = state.GetParamValue < float >( 4, 0.0f );

	bool result = self->Inside ( vec, pad );
	
	return state.ToRValue ( result );
}

//----------------------------------------------------------------//
/**	@lua	setBounds
	@text	Sets or clears the partition bounds override.
	
	@overload	Clear the bounds override.
	
		@in		MOAIPartitionHull self
		@out	nil
	
	@overload	Set the bounds override.
	
		@in		MOAIPartitionHull self
		@in		number xMin
		@in		number yMin
		@in		number zMin
		@in		number xMax
		@in		number yMax
		@in		number zMax
		@out	nil
*/
mrb_value MOAIPartitionHull::_setBounds ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHull, "U" )

	if ( state.CheckParams ( 1, "NNNNNN", false )) {

		self->mBoundsOverride = state.GetBox ( 1 );
		self->mFlags |= FLAGS_OVERRIDE_BOUNDS;
	}
	else {
		self->mFlags &= ~FLAGS_OVERRIDE_BOUNDS;
	}
	
	self->ScheduleUpdate ();
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIPartitionHull::_setBoundsPad ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHull, "U" )

	self->mBoundsPad.mX	= state.GetParamValue < float >( 1, 0.0f );
	self->mBoundsPad.mY	= state.GetParamValue < float >( 2, 0.0f );
	self->mBoundsPad.mZ	= state.GetParamValue < float >( 3, 0.0f );

	bool pad = (( self->mBoundsPad.mX != 0.0f ) || ( self->mBoundsPad.mY != 0.0f ) || ( self->mBoundsPad.mZ != 0.0f ));
	self->mFlags = pad ? ( self->mFlags | FLAGS_PAD_BOUNDS ) : ( self->mFlags & ~FLAGS_PAD_BOUNDS );

	self->ScheduleUpdate ();

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setExpandForSort
	@text	Used when drawing with a layout scheme (i.e. MOAIGrid).
			Expanding for sort causes the prop to emit a sub-prim
			for each component of the layout. For example, when
			attaching a MOAIGrid to a prop, each cell of the grid
			will be added to the render queue for sorting against
			all other props and sub-prims. This is obviously less
			efficient, but still more efficient then using an
			separate prop for each cell or object.
	
	@in		MOAIPartitionHull self
	@in		boolean expandForSort	Default value is false.
	@out	nil
*/
mrb_value MOAIPartitionHull::_setExpandForSort ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHull, "U" )

	bool expandForSort = state.GetParamValue < bool >( 1, false );

	if ( expandForSort ) {
		self->mFlags |= FLAGS_EXPAND_FOR_SORT;
	}
	else {
		self->mFlags &= ~FLAGS_EXPAND_FOR_SORT;
	}

	return context;
}

//----------------------------------------------------------------//
// TODO: macro
mrb_value MOAIPartitionHull::_setFlag ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHull, "U" )

	u32 flag		= state.GetParamValue < u32 >( 1, 0 );
	bool set		= state.GetParamValue < bool >( 2, true );

	self->mFlags = set ? self->mFlags |= flag : self->mFlags &= flag;

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setHitGranularity
	@text	Specify the granularity to use when performing a hit test. This is a hint to the implementation
			as to how much processing to allocate to a given test. The default value is MOAIPartitionHull.HIT_TEST_COARSE,
			which will cause only the deck or prop bounds to be used. A setting of MOAIPartitionHull.HIT_TEST_MEDIUM or
			MOAIPartitionHull.HIT_TEST_FINE is implementation dependent on the deck, but 'medium' usually means to test
			against the geometry of the deck and 'fine' means to test against both the geometry and the pixels
			of the hit mask (if any).
	
	@in		MOAIPartitionHull self
	@opt	int granularity		One of MOAIPartitionHull.HIT_TEST_COARSE, MOAIPartitionHull.HIT_TEST_MEDIUM, MOAIPartitionHull.HIT_TEST_FINE. Default is MOAIPartitionHull.HIT_TEST_COARSE.
	@out	nil
*/
mrb_value MOAIPartitionHull::_setHitGranularity ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHull, "U" )

	self->mHitGranularity = state.GetParamValue < u32 >( 1, HIT_TEST_COARSE );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIPartitionHull::_setPartition ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHull, "U" )
	
	MOAIPartitionHolder* holder = state.GetRubyObject < MOAIPartitionHolder >( 1, false );
	MOAIPartition* partition = holder ? holder->GetPartition () : state.GetRubyObject < MOAIPartition >( 1, true );
	
	self->SetPartition ( partition );
	self->ScheduleUpdate ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setPriority
	@text	Sets or clears the node's priority. Clear the priority
			to have MOAIPartition automatically assign a priority
			to a node when it is added.
	
	@in		MOAIPartitionHull self
	@opt	number priority		Default value is nil.
	@out	nil
*/
mrb_value MOAIPartitionHull::_setPriority ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHull, "U" )
	
	if ( state.ParamIsType ( 1, MRB_TT_FIXNUM )) {
		self->mPriority = ( s32 )state.GetParamValue < int >( 1, 0 );
	}
	else {
		self->mPriority = UNKNOWN_PRIORITY;
		if ( self->mPartition ) {
			self->mPartition->AffirmPriority ( *self );
		}
	}
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIPartitionHull::_setQueryMask ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHull, "U" )

	self->mQueryMask = state.GetParamValue < u32 >( 1, 0 );
	return context;
}

//================================================================//
// MOAIPartitionHull
//================================================================//

//----------------------------------------------------------------//
void MOAIPartitionHull::AddToSortBuffer ( MOAIPartitionResultBuffer& buffer, u32 key ) {

	this->MOAIPartitionHull_AddToSortBuffer ( buffer, key );
}

//----------------------------------------------------------------//
u32 MOAIPartitionHull::AffirmInterfaceMask ( MOAIPartition& partition ) {

	return this->MOAIPartitionHull_AffirmInterfaceMask ( partition );
}

//----------------------------------------------------------------//
void MOAIPartitionHull::BoundsDidChange () {

	this->MOAIPartitionHull_BoundsDidChange ();
}

//----------------------------------------------------------------//
//void MOAIPartitionHull::GatherSurfaces ( MOAISurfaceSampler2D& sampler ) {
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
bool MOAIPartitionHull::GetCellRect ( ZLRect* cellRect, ZLRect* paddedRect ) {

	if ( !( cellRect || paddedRect )) return false;
	
	if ( this->mLevel ) {
	
		ZLVec3D center;
		this->mWorldBounds.GetCenter ( center );
		
		MOAICellCoord coord = this->mLevel->mGridSpace.GetCellCoord ( center.mX, center.mY );
		ZLRect rect = this->mLevel->mGridSpace.GetCellRect ( coord );
		
		if ( cellRect ) {
			*cellRect = rect;
		}
		
		if ( paddedRect ) {
			rect.Inflate ( this->mLevel->mCellSize * 0.5f );
			*paddedRect = rect;
		}
		return true;
	}
	
	if ( cellRect ) {
		cellRect->Init ( 0.0f, 0.0f, 0.0f, 0.0f );
	}
	
	if ( paddedRect ) {
		paddedRect->Init ( 0.0f, 0.0f, 0.0f, 0.0f );
	}
	
	return false;
}

//----------------------------------------------------------------//
ZLBounds MOAIPartitionHull::GetModelBounds () {

	if ( this->mFlags & FLAGS_PARTITION_GLOBAL ) {
		return ZLBounds::GLOBAL;
	}

	ZLBounds bounds = ZLBounds::EMPTY;

	if ( this->mFlags & FLAGS_OVERRIDE_BOUNDS ) {
		bounds.Init ( this->mBoundsOverride );
	}
	else {
		bounds = this->MOAIPartitionHull_GetModelBounds ();
	}
	
	if (( bounds.mStatus == ZLBounds::ZL_BOUNDS_OK ) && ( this->mFlags & FLAGS_PAD_BOUNDS )) {
		bounds.Pad ( this->mBoundsPad.mX, this->mBoundsPad.mY, this->mBoundsPad.mZ );
	}

	return bounds;
}

//----------------------------------------------------------------//
MOAIPartition* MOAIPartitionHull::GetPartitionTrait () {

	return this->mPartition;
}

//----------------------------------------------------------------//
bool MOAIPartitionHull::Inside ( ZLVec3D vec, float pad ) {
	
	return this->MOAIPartitionHull_Inside ( vec, pad );
}

//----------------------------------------------------------------//
bool MOAIPartitionHull::InsideModelBounds ( const ZLVec3D& vec, float pad ) {

	ZLBounds bounds = this->GetModelBounds ();
	
	if ( bounds.mStatus == ZLBounds::ZL_BOUNDS_EMPTY ) return false;
	
	bool passTrivial = false;
	
	if ( bounds.mStatus == ZLBounds::ZL_BOUNDS_GLOBAL ) {
		passTrivial = true;
	}
	else {
		bounds.Bless ();
		bounds.Inflate ( pad );
		if ( pad != 0 ) bounds.Bless ();
		passTrivial = bounds.Contains ( vec );
	}
	return passTrivial;
}

//----------------------------------------------------------------//
MOAIPartitionHull::MOAIPartitionHull () :
	mPartition ( 0 ),
	mCell ( 0 ),
	mLevel ( 0 ),
	mNextResult ( 0 ),
	mInterfaceMask ( 0 ),
	mQueryMask ( 0xffffffff ),
	mPriority ( UNKNOWN_PRIORITY ),
	mFlags ( 0 ),
	mBoundsPad ( 0.0f, 0.0f, 0.0f ),
	mHitGranularity ( HIT_TEST_COARSE ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAITransform )
	RTTI_END
	
	this->mLinkInCell.Data ( this );
	this->mWorldBounds.Init ( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
}

//----------------------------------------------------------------//
MOAIPartitionHull::~MOAIPartitionHull () {

	// MOAIPartition retains member props so it *should* be impossible
	// to destruct a prop without first removing it
	assert ( !this->mPartition );
	assert ( !this->mCell );
}

//----------------------------------------------------------------//
bool MOAIPartitionHull::PrepareForInsertion ( const MOAIPartition& partition ) {

	return this->MOAIPartitionHull_PrepareForInsertion ( partition );
}

//----------------------------------------------------------------//
void MOAIPartitionHull::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	MOAITransform::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "ATTR_PARTITION",				MOAIPartitionHullAttr::Pack ( ATTR_PARTITION ));
	
	state.DefineClassConst ( klass, "FLAGS_EXPAND_FOR_SORT",		( u32 )FLAGS_EXPAND_FOR_SORT );
	state.DefineClassConst ( klass, "FLAGS_PARTITION_GLOBAL",		( u32 )FLAGS_PARTITION_GLOBAL );
	
	state.DefineClassConst ( klass, "HIT_TEST_COARSE",				( u32 )HIT_TEST_COARSE );
	state.DefineClassConst ( klass, "HIT_TEST_MEDIUM",				( u32 )HIT_TEST_MEDIUM );
	state.DefineClassConst ( klass, "HIT_TEST_FINE",				( u32 )HIT_TEST_FINE );
}

//----------------------------------------------------------------//
void MOAIPartitionHull::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAITransform::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getBounds", _getBounds, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getDims", _getDims, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getPartition", _getPartition, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getPriority", _getPriority, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getWorldBounds", _getWorldBounds, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getWorldBoundsCenter", _getWorldBoundsCenter, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "inside", _inside, MRB_ARGS_ARG ( 3, 1 ) );
	state.DefineInstanceMethod ( klass, "setBounds", _setBounds, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "setBoundsPad", _setBoundsPad, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "setExpandForSort", _setExpandForSort, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setFlag", _setFlag, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setHitGranularity", _setHitGranularity, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setPartition", _setPartition, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setPriority", _setPriority, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setQueryMask", _setQueryMask, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAIPartitionHull::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	
	MOAITransform::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIPartitionHull::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	
	MOAITransform::SerializeOut ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIPartitionHull::SetPartition ( MOAIPartition* partition ) {

	if ( partition != this->mPartition ) {
		if ( partition ) {
			partition->InsertHull ( *this );
		}
		else if ( this->mPartition ) {
			this->mPartition->RemoveHull ( *this );
		}
	}
}

//----------------------------------------------------------------//
//void MOAIPartitionHull::UpdateWorldBounds ( u32 status ) {
//
//	ZLBox bounds;
//	bounds.Init ( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
//
//	if ( status == ZLBounds::ZL_BOUNDS_OK ) {
//		status = ZLBounds::ZL_BOUNDS_EMPTY;
//	}
//	this->UpdateWorldBounds ( bounds, status );
//}

//----------------------------------------------------------------//
void MOAIPartitionHull::UpdateWorldBounds ( const ZLBounds& bounds ) {

	MOAIPartitionCell* prevCell = this->mCell;
	ZLBox prevBounds = this->mWorldBounds;

	this->mWorldBounds = bounds;

	if ( bounds.mStatus == ZLBounds::ZL_BOUNDS_OK ) {
		this->mWorldBounds.UpdateStatus ();
	}

	if ( this->mPartition ) {
		
		this->mPartition->UpdateHull ( *this );
		
		if (( prevCell != this->mCell ) || ( !prevBounds.IsSame ( this->mWorldBounds ))) {
			this->BoundsDidChange ();
		}
	}
}

//----------------------------------------------------------------//
void MOAIPartitionHull::WasRemovedFromPartition () {

	this->MOAIPartitionHull_WasRemovedFromPartition ();
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAIPartitionHull::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	if ( MOAIPartitionHullAttr::Check ( attrID )) {
		
		switch ( UNPACK_ATTR ( attrID )) {;
			case ATTR_PARTITION:
				this->SetPartition ( attr.ApplyVariantNoAdd < MOAIPartition* >( this->GetPartition (), op, MOAIAttribute::ATTR_READ_WRITE ));
				return true;
		}
	}
	
	return MOAITransform::MOAINode_ApplyAttrOp ( attrID, attr, op );
}

//----------------------------------------------------------------//
void MOAIPartitionHull::MOAINode_Update () {
	
	MOAITransform::MOAINode_Update ();
	
	ZLBounds bounds = this->GetModelBounds ();
	
	// update the prop location in the partition
	if ( bounds.mStatus == ZLBounds::ZL_BOUNDS_OK ) {
		bounds.Transform ( this->mLocalToWorldMtx );
	}
	this->UpdateWorldBounds ( bounds );
}

//----------------------------------------------------------------//
void MOAIPartitionHull::MOAIPartitionHull_BoundsDidChange () {
}

//----------------------------------------------------------------//
bool MOAIPartitionHull::MOAIPartitionHull_Inside ( ZLVec3D vec, float pad ) {

	ZLAffine3D worldToLocal = this->GetWorldToLocalMtx ();
	worldToLocal.Transform ( vec );
	
	return this->InsideModelBounds ( vec, pad );
}

//----------------------------------------------------------------//
bool MOAIPartitionHull::MOAIPartitionHull_PrepareForInsertion ( const MOAIPartition& partition ) {
	UNUSED ( partition );
	return true;
}

//----------------------------------------------------------------//
void MOAIPartitionHull::MOAIPartitionHull_WasRemovedFromPartition () {
}
