// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIPartition.h>
#include <moai-sim/MOAIPartitionCell.h>
#include <moai-sim/MOAIPartitionLevel.h>
#include <moai-sim/MOAIPartitionResultBuffer.h>
#include <moai-sim/MOAIPartitionResultMgr.h>
#include <moai-sim/MOAIPartitionHull.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	clear
	@text	Remove all props from the partition.
	
	@in		MOAIPartition self
	@out	nil
*/
mrb_value MOAIPartition::_clear ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartition, "U" )

	self->Clear ();
	
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIPartition::_getInterfaceMask ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartition, "U" )

	u32 interfaceMask = MASK_ANY;
	u32 typeID = state.GetParamValue < u32 >( 1, 0 );

	if ( typeID ) {
		interfaceMask = self->GetInterfaceMask ( typeID );
	}
	return state.ToRValue ( interfaceMask );
}

//----------------------------------------------------------------//
/**	@lua	hullForPoint
	@text	Returns the hull with the highest priority that contains
			the given world space point.
	
	@in		MOAIPartition self
	@in		number x
	@in		number y
	@in		number z
	@opt	number sortMode			One of the MOAIPartitionViewLayer sort modes. Default value is SORT_PRIORITY_ASCENDING.
	@opt	number xScale			X scale for vector sort. Default value is 0.
	@opt	number yScale			Y scale for vector sort. Default value is 0.
	@opt	number zScale			Z scale for vector sort. Default value is 0.
	@opt	number priorityScale	Priority scale for vector sort. Default value is 1.
	@opt	number interfaceMask
	@opt	number queryMask
	@out	MOAIPartitionHull hull			The hull under the point or nil if no hull found.
*/
mrb_value MOAIPartition::_hullForPoint ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartition, "UNN" )

	ZLVec3D vec;
	vec.mX = state.GetParamValue < float >( 1, 0.0f );
	vec.mY = state.GetParamValue < float >( 2, 0.0f );
	vec.mZ = state.GetParamValue < float >( 3, 0.0f );

	u32 sortMode = state.GetParamValue < u32 >( 4, MOAIPartitionResultBuffer::SORT_PRIORITY_ASCENDING );
	float xScale = state.GetParamValue < float >( 5, 0.0f );
	float yScale = state.GetParamValue < float >( 6, 0.0f );
	float zScale = state.GetParamValue < float >( 7, 0.0f );
	float priorityScale = state.GetParamValue < float >( 8, 1.0f );

	u32 interfaceMask	= state.GetParamValue < u32 >( 9, MASK_ANY );
	u32 queryMask		= state.GetParamValue < u32 >( 10, MASK_ANY );

	MOAIScopedPartitionResultBufferHandle scopedBufferHandle = MOAIPartitionResultMgr::Get ().GetBufferHandle ();
	MOAIPartitionResultBuffer& buffer = scopedBufferHandle;
	
	u32 total = self->GatherHulls ( buffer, 0, vec, interfaceMask, queryMask );
	if ( total ) {
		
		// this just swaps the internal buffer pointers so we can access the results
		buffer.Sort ( MOAIPartitionResultBuffer::SORT_NONE );
		
		// generate the sort keys, but don't actually sort
		buffer.GenerateKeys ( sortMode, xScale, yScale, zScale, priorityScale );
		
		// since we're just looking for one hull, do a one-pass traversal to find the best result
		MOAIPartitionHull* hull = buffer.FindBest ();
		if ( hull ) {
			return hull->PushRubyUserdata ( state );
		}
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	hullForRay
	@text	Returns the first hull that intersects the given ray
	 
	@in		MOAIPartition self
	@in		number x
	@in		number y
	@in		number z
	@in		number xdirection
	@in		number ydirection
	@in		number zdirection
	@opt	number interfaceMask
	@opt	number queryMask
	@out	MOAIPartitionHull hull		The hull under the point in order of depth or nil if no hull found.
*/
mrb_value MOAIPartition::_hullForRay ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartition, "UNN" )
	
	ZLVec3D vec;
	vec.mX = state.GetParamValue < float >( 1, 0.0f );
	vec.mY = state.GetParamValue < float >( 2, 0.0f );
	vec.mZ = state.GetParamValue < float >( 3, 0.0f );
	
	ZLVec3D direction;
	direction.mX = state.GetParamValue < float >( 4, 0.0f );
	direction.mY = state.GetParamValue < float >( 5, 0.0f );
	direction.mZ = state.GetParamValue < float >( 6, 0.0f );
	direction.Norm ();
	
	u32 interfaceMask	= state.GetParamValue < u32 >( 7, MASK_ANY );
	u32 queryMask		= state.GetParamValue < u32 >( 8, MASK_ANY );
	
	MOAIScopedPartitionResultBufferHandle scopedBufferHandle = MOAIPartitionResultMgr::Get ().GetBufferHandle ();
	MOAIPartitionResultBuffer& buffer = scopedBufferHandle;
	
	u32 total = self->GatherHulls ( buffer, 0, vec, direction, interfaceMask, queryMask );

	if ( total ) {
		
		MOAIPartitionHull* hull = buffer.FindBest ();
		if ( hull ) {
			return hull->PushRubyUserdata ( state );
		}
	}

	return context;
}

//----------------------------------------------------------------//
/**	@lua	hullList
	@text	Returns all props.
	
	@in		MOAIPartition self
	@opt	number sortMode			One of the MOAIPartitionViewLayer sort modes. Default value is SORT_NONE.
	@opt	number xScale			X scale for vector sort. Default value is 0.
	@opt	number yScale			Y scale for vector sort. Default value is 0.
	@opt	number zScale			Z scale for vector sort. Default value is 0.
	@opt	number priorityScale	Priority scale for vector sort. Default value is 1.
	@opt	number interfaceMask
	@opt	number queryMask
	@out	... props				The props pushed onto the stack.
*/
mrb_value MOAIPartition::_hullList ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartition, "U" )

	u32 sortMode = state.GetParamValue < u32 >( 1, MOAIPartitionResultBuffer::SORT_NONE );
	float xScale = state.GetParamValue < float >( 2, 0.0f );
	float yScale = state.GetParamValue < float >( 3, 0.0f );
	float zScale = state.GetParamValue < float >( 4, 0.0f );
	float priorityScale = state.GetParamValue < float >( 5, 1.0f );		
	u32 interfaceMask	= state.GetParamValue < u32 >( 6, MASK_ANY );
	u32 queryMask		= state.GetParamValue < u32 >( 7, MASK_ANY );

	MOAIScopedPartitionResultBufferHandle scopedBufferHandle = MOAIPartitionResultMgr::Get ().GetBufferHandle ();
	MOAIPartitionResultBuffer& buffer = scopedBufferHandle;

	u32 total = self->GatherHulls ( buffer, 0, interfaceMask, queryMask );
	if ( total ) {
	
		buffer.GenerateKeys ( sortMode, xScale, yScale, zScale, priorityScale );
		buffer.Sort ( sortMode );
		return buffer.PushHulls ( M );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	hullListForPoint
	@text	Returns all props under a given world space point.
	
	@in		MOAIPartition self
	@in		number x
	@in		number y
	@in		number z
	@opt	number sortMode			One of the MOAIPartitionViewLayer sort modes. Default value is SORT_NONE.
	@opt	number xScale			X scale for vector sort. Default value is 0.
	@opt	number yScale			Y scale for vector sort. Default value is 0.
	@opt	number zScale			Z scale for vector sort. Default value is 0.
	@opt	number priorityScale	Priority scale for vector sort. Default value is 1.
	@opt	number interfaceMask
	@opt	number queryMask
	@out	... props				The props under the point, all pushed onto the stack.
*/
mrb_value MOAIPartition::_hullListForPoint ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartition, "UNN" )

	ZLVec3D vec;
	vec.mX = state.GetParamValue < float >( 1, 0.0f );
	vec.mY = state.GetParamValue < float >( 2, 0.0f );
	vec.mZ = state.GetParamValue < float >( 3, 0.0f );

	u32 sortMode = state.GetParamValue < u32 >( 4, MOAIPartitionResultBuffer::SORT_NONE );
	float xScale = state.GetParamValue < float >( 5, 0.0f );
	float yScale = state.GetParamValue < float >( 6, 0.0f );
	float zScale = state.GetParamValue < float >( 7, 0.0f );
	float priorityScale = state.GetParamValue < float >( 8, 1.0f );		
	u32 interfaceMask	= state.GetParamValue < u32 >( 9, MASK_ANY );
	u32 queryMask		= state.GetParamValue < u32 >( 10, MASK_ANY );

	MOAIScopedPartitionResultBufferHandle scopedBufferHandle = MOAIPartitionResultMgr::Get ().GetBufferHandle ();
	MOAIPartitionResultBuffer& buffer = scopedBufferHandle;

	u32 total = self->GatherHulls ( buffer, 0, vec, interfaceMask, queryMask );
	if ( total ) {
	
		buffer.GenerateKeys ( sortMode, xScale, yScale, zScale, priorityScale );
		buffer.Sort ( sortMode );
		return buffer.PushHulls ( M );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	hullListForRay
	@text	Returns all props that intersect the given ray.
	
	@in		MOAIPartition self
	@in		number x
	@in		number y
	@in		number z
	@in		number xdirection
	@in		number ydirection
	@in		number zdirection
	@opt	number sortMode			One of the MOAIPartitionViewLayer sort modes. Default value is SORT_KEY_ASCENDING.
	@opt	number xScale			X scale for vector sort. Default value is 0.
	@opt	number yScale			Y scale for vector sort. Default value is 0.
	@opt	number zScale			Z scale for vector sort. Default value is 0.
	@opt	number priorityScale	Priority scale for vector sort. Default value is 1.
	@opt	number interfaceMask
	@opt	number queryMask
	@out	... props				The props under the point in order of depth, all pushed onto the stack.
*/
mrb_value MOAIPartition::_hullListForRay ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartition, "UNN" )

	ZLVec3D vec;
	vec.mX = state.GetParamValue < float >( 1, 0.0f );
	vec.mY = state.GetParamValue < float >( 2, 0.0f );
	vec.mZ = state.GetParamValue < float >( 3, 0.0f );
	
	ZLVec3D direction;
	direction.mX = state.GetParamValue < float >( 4, 0.0f );
	direction.mY = state.GetParamValue < float >( 5, 0.0f );
	direction.mZ = state.GetParamValue < float >( 6, 0.0f );
	direction.Norm ();
	
	u32 sortMode = state.GetParamValue < u32 >( 7, MOAIPartitionResultBuffer::SORT_KEY_ASCENDING );
	float xScale = state.GetParamValue < float >( 8, 0.0f );
	float yScale = state.GetParamValue < float >( 9, 0.0f );
	float zScale = state.GetParamValue < float >( 10, 0.0f );
	float priorityScale = state.GetParamValue < float >( 11, 1.0f );		
	u32 interfaceMask	= state.GetParamValue < u32 >( 12, MASK_ANY );
	u32 queryMask		= state.GetParamValue < u32 >( 13, MASK_ANY );
	
	MOAIScopedPartitionResultBufferHandle scopedBufferHandle = MOAIPartitionResultMgr::Get ().GetBufferHandle ();
	MOAIPartitionResultBuffer& buffer = scopedBufferHandle;
	
	u32 total = self->GatherHulls ( buffer, 0, vec, direction, interfaceMask, queryMask );

	if ( total ) {
	
		buffer.GenerateKeys ( sortMode, xScale, yScale, zScale, priorityScale );
		buffer.Sort ( sortMode );
		return buffer.PushHulls ( M );
	}
	return context;	
}

//----------------------------------------------------------------//
/**	@lua	hullListForRect
	@text	Returns all props under a given world space rect.
	
	@in		MOAIPartition self
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@opt	number sortMode			One of the MOAIPartitionViewLayer sort modes. Default value is SORT_NONE.
	@opt	number xScale			X scale for vector sort. Default value is 0.
	@opt	number yScale			Y scale for vector sort. Default value is 0.
	@opt	number zScale			Z scale for vector sort. Default value is 0.
	@opt	number priorityScale	Priority scale for vector sort. Default value is 1.
	@opt	number interfaceMask
	@opt	number queryMask
	@out	... props				The props under the rect, all pushed onto the stack.
*/
mrb_value MOAIPartition::_hullListForRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartition, "UNNNN" )
	
	ZLRect rect;
	
	rect.mXMin = state.GetParamValue < float >( 1, 0.0f );
	rect.mYMin = state.GetParamValue < float >( 2, 0.0f );
	
	rect.mXMax = state.GetParamValue < float >( 3, 0.0f );
	rect.mYMax = state.GetParamValue < float >( 4, 0.0f );
	
	u32 sortMode = state.GetParamValue < u32 >( 5, MOAIPartitionResultBuffer::SORT_NONE );
	float xScale = state.GetParamValue < float >( 6, 0.0f );
	float yScale = state.GetParamValue < float >( 7, 0.0f );
	float zScale = state.GetParamValue < float >( 8, 0.0f );
	float priorityScale = state.GetParamValue < float >( 9, 1.0f );	
	u32 interfaceMask	= state.GetParamValue < u32 >( 10, MASK_ANY );
	u32 queryMask		= state.GetParamValue < u32 >( 11, MASK_ANY );
	
	MOAIScopedPartitionResultBufferHandle scopedBufferHandle = MOAIPartitionResultMgr::Get ().GetBufferHandle ();
	MOAIPartitionResultBuffer& buffer = scopedBufferHandle;
	
	u32 total = self->GatherHulls ( buffer, 0, rect, interfaceMask, queryMask );
	if ( total ) {
	
		buffer.GenerateKeys ( sortMode, xScale, yScale, zScale, priorityScale );
		buffer.Sort ( sortMode );
		return buffer.PushHulls ( M );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	insertHull
	@text	Inserts a hull into the partition. A hull can only be
			in one partition at a time.
	
	@in		MOAIPartition self
	@in		MOAIPartitionHull hull
	@out	nil
*/
//mrb_value MOAIPartition::_insertHull ( mrb_state* M, mrb_value context ) {
//	MOAI_RUBY_SETUP ( MOAIPartition, "UU" )
//
//	MOAIPartitionHull* hull = state.GetRubyObject < MOAIPartitionHull >( 2, true );
//	if ( !hull ) return context;
//
//	self->InsertHull ( *hull );
//	hull->ScheduleUpdate ();
//
//	return context;
//}

//----------------------------------------------------------------//
/**	@lua	removeHull
	@text	Removes a hull from the partition.
	
	@in		MOAIPartition self
	@in		MOAIPartitionHull hull
	@out	nil
*/
//mrb_value MOAIPartition::_removeHull ( mrb_state* M, mrb_value context ) {
//	MOAI_RUBY_SETUP ( MOAIPartition, "UU" )
//
//	MOAIPartitionHull* hull = state.GetRubyObject < MOAIPartitionHull >( 2, true );
//	if ( !hull ) return context;
//
//	self->RemoveHull ( *hull );
//	hull->ScheduleUpdate ();
//
//	return context;
//}

//----------------------------------------------------------------//
/**	@lua	reserveLevels
	@text	Reserves a stack of levels in the partition. Levels must be
			initialized with setLevel (). This will trigger a full rebuild
			of the partition if it contains any props.
	
	@in		MOAIPartition self
	@in		number nLevels
	@out	nil
*/
mrb_value MOAIPartition::_reserveLevels ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartition, "UN" )

	u32 totalLevels = state.GetParamValue < u32 >( 1, 0 );
	
	self->ReserveLevels ( totalLevels );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setLevel
	@text	Initializes a level previously created by reserveLevels ().
			This will trigger a full rebuild of the partition if it contains any props.
			Each level is a loose grid. Props of a given size may be placed by
			the system into any level with cells large enough to accommodate them.
			The dimensions of a level control how many cells the level contains.
			If an object goes off of the edge of a level, it will wrap around
			to the other side. It is possible to model a quad tree by initializing
			levels correctly, but for some simulations better structures
			may be possible.
	
	@in		MOAIPartition self
	@in		number levelID
	@in		number cellSize		Dimensions of the level's cells.
	@in		number xCells		Width of level in cells.
	@in		number yCells		Height of level in cells.
	@out	nil
*/
mrb_value MOAIPartition::_setLevel ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartition, "UNNNN" )

	u32 levelID		= state.GetParamValue < u32 >( 1, 1 ) - 1;
	float cellSize	= state.GetParamValue < float >( 2, 1.0f );
	u32 width		= state.GetParamValue < u32 >( 3, 0 );
	u32 height		= state.GetParamValue < u32 >( 4, 0 );

	self->SetLevel ( levelID, cellSize, width, height );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setPlane
	@text	Selects the plane the partition will use. If this is different
			from the current plane then all non-global props will be redistributed.
			Redistribution works by moving all props to the 'empties' cell and then
			scheduling them all for a dep node update (which refreshes the prop's
			bounds and may also flag it as global).
	
	@in		MOAIPartition self
	@in		number planeID		One of MOAIPartition::PLANE_XY, MOAIPartition::PLANE_XZ, MOAIPartition::PLANE_YZ. Default value is MOAIPartition::PLANE_XY.
	@out	nil
*/
mrb_value MOAIPartition::_setPlane ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartition, "U" )

	u32 planeID	= state.GetParamValue < u32 >( 1, ZLBox::PLANE_XY );
	self->SetPlane ( planeID );

	return context;
}

//================================================================//
// MOAIPartition
//================================================================//

//----------------------------------------------------------------//
u32 MOAIPartition::AffirmInterfaceMask ( u32 typeID ) {

	u32 mask = this->GetInterfaceMask ( typeID );
	if ( mask == 0 ) {
		size_t top = this->mInterfaceIDs.Size ();
		if ( top < INTERFACE_MASK_BITS ) {
			this->mInterfaceIDs.Grow ( top + 1 );
			this->mInterfaceIDs [ top ] = typeID;
			return 1 << top;
		}
	}
	return mask;
}

//----------------------------------------------------------------//
void MOAIPartition::AffirmPriority ( MOAIPartitionHull& hull ) {

	if ( hull.mPriority == MOAIPartitionHull::UNKNOWN_PRIORITY ) {
		hull.mPriority = this->mPriorityCounter++;
		this->mPriorityCounter = this->mPriorityCounter & PRIORITY_MASK;
	}
}

//----------------------------------------------------------------//
void MOAIPartition::Clear () {

	size_t totalLevels = this->mLevels.Size ();
	for ( size_t i = 0; i < totalLevels; ++i ) {
		this->mLevels [ i ].Clear ();
	}
	this->mBiggies.Clear ();
	this->mGlobals.Clear ();
	this->mEmpties.Clear ();
}

//----------------------------------------------------------------//
void MOAIPartition::DrawDebugBack () {

	this->MOAIPartition_DrawDebugBack ();
}

//----------------------------------------------------------------//
void MOAIPartition::DrawDebugFront () {

	this->MOAIPartition_DrawDebugFront ();
}


//----------------------------------------------------------------//
u32 MOAIPartition::GatherHulls ( MOAIPartitionResultBuffer& results, MOAIPartitionHull* ignoreProp, u32 interfaceMask, u32 queryMask ) {
	
	results.Reset ();
	
	size_t totalLevels = this->mLevels.Size ();
	for ( size_t i = 0; i < totalLevels; ++i ) {
		this->mLevels [ i ].GatherHulls ( results, ignoreProp, interfaceMask, queryMask );
	}
	this->mBiggies.GatherHulls ( results, ignoreProp, interfaceMask, queryMask );
	this->mGlobals.GatherHulls ( results, ignoreProp, interfaceMask, queryMask );
	this->mEmpties.GatherHulls ( results, ignoreProp, interfaceMask, queryMask );
	
	return results.Sort ( MOAIPartitionResultBuffer::SORT_NONE );
}

//----------------------------------------------------------------//
u32 MOAIPartition::GatherHulls ( MOAIPartitionResultBuffer& results, MOAIPartitionHull* ignoreProp, const ZLVec3D& point, u32 interfaceMask, u32 queryMask ) {
	
	results.Reset ();
	
	size_t totalLevels = this->mLevels.Size ();
	for ( size_t i = 0; i < totalLevels; ++i ) {
		this->mLevels [ i ].GatherHulls ( results, ignoreProp, point, this->mPlaneID, interfaceMask, queryMask );
	}
	this->mBiggies.GatherHulls ( results, ignoreProp, point, interfaceMask, queryMask );
	this->mGlobals.GatherHulls ( results, ignoreProp, interfaceMask, queryMask );
	
	return results.Sort ( MOAIPartitionResultBuffer::SORT_NONE );
}

//----------------------------------------------------------------//
u32 MOAIPartition::GatherHulls ( MOAIPartitionResultBuffer& results, MOAIPartitionHull* ignoreProp, const ZLVec3D& point, const ZLVec3D& orientation, u32 interfaceMask, u32 queryMask ) {
	
	results.Reset ();
	
	size_t totalLevels = this->mLevels.Size ();
	for ( size_t i = 0; i < totalLevels; ++i ) {
		this->mLevels [ i ].GatherHulls ( results, ignoreProp, point, orientation, interfaceMask, queryMask );
	}
	this->mBiggies.GatherHulls ( results, ignoreProp, point, orientation, interfaceMask, queryMask );
	this->mGlobals.GatherHulls ( results, ignoreProp, point, orientation, interfaceMask, queryMask );
	
	return results.Sort ( MOAIPartitionResultBuffer::SORT_NONE );
}

//----------------------------------------------------------------//
u32 MOAIPartition::GatherHulls ( MOAIPartitionResultBuffer& results, MOAIPartitionHull* ignoreProp, ZLRect rect, u32 interfaceMask, u32 queryMask ) {
	
	results.Reset ();
	rect.Bless ();
	
	size_t totalLevels = this->mLevels.Size ();
	for ( size_t i = 0; i < totalLevels; ++i ) {
		this->mLevels [ i ].GatherHulls ( results, ignoreProp, rect, interfaceMask, queryMask );
	}
	this->mBiggies.GatherHulls ( results, ignoreProp, rect, interfaceMask, queryMask );
	this->mGlobals.GatherHulls ( results, ignoreProp, interfaceMask, queryMask );
	
	return results.Sort ( MOAIPartitionResultBuffer::SORT_NONE );
}

//----------------------------------------------------------------//
u32 MOAIPartition::GatherHulls ( MOAIPartitionResultBuffer& results, MOAIPartitionHull* ignoreProp, ZLBox box, u32 interfaceMask, u32 queryMask ) {
	
	results.Reset ();
	box.Bless ();
	
	size_t totalLevels = this->mLevels.Size ();
	for ( size_t i = 0; i < totalLevels; ++i ) {
		this->mLevels [ i ].GatherHulls ( results, ignoreProp, box, this->mPlaneID, interfaceMask, queryMask );
	}
	this->mBiggies.GatherHulls ( results, ignoreProp, box, interfaceMask, queryMask );
	this->mGlobals.GatherHulls ( results, ignoreProp, interfaceMask, queryMask );
	
	return results.Sort ( MOAIPartitionResultBuffer::SORT_NONE );
}

//----------------------------------------------------------------//
u32 MOAIPartition::GatherHulls ( MOAIPartitionResultBuffer& results, MOAIPartitionHull* ignoreProp, const ZLFrustum& frustum, u32 interfaceMask, u32 queryMask ) {
	
	results.Reset ();
	
	size_t totalLevels = this->mLevels.Size ();
	for ( size_t i = 0; i < totalLevels; ++i ) {
		this->mLevels [ i ].GatherHulls ( results, ignoreProp, frustum, this->mPlaneID, interfaceMask, queryMask );
	}
	this->mBiggies.GatherHulls ( results, ignoreProp, frustum, interfaceMask, queryMask );
	this->mGlobals.GatherHulls ( results, ignoreProp, interfaceMask, queryMask );
	
	return results.Sort ( MOAIPartitionResultBuffer::SORT_NONE );
}

//----------------------------------------------------------------//
u32 MOAIPartition::GetInterfaceMask ( u32 typeID ) const {

	size_t total = this->mInterfaceIDs.Size ();
	for ( size_t i = 0; i < total; ++i ) {
		if ( this->mInterfaceIDs [ i ] == typeID ) {
			return 1 << i;
		}
	}
	return 0;
}

//----------------------------------------------------------------//
void MOAIPartition::InsertHull ( MOAIPartitionHull& hull ) {
	
	if ( hull.mPartition == this ) return;
	
	this->RubyRetain ( &hull );
	
	if ( hull.mPartition ) {
		hull.mPartition->RemoveHull ( hull );
	}
	
	u32 interfaceMask = hull.AffirmInterfaceMask ( *this );
	if ( interfaceMask ) {
	
		if ( hull.mPriority == MOAIPartitionHull::UNKNOWN_PRIORITY ) {
			hull.mPriority = this->mPriorityCounter++;
			this->mPriorityCounter = this->mPriorityCounter & PRIORITY_MASK;
		}
		
		this->mEmpties.InsertHull ( hull );
		this->AffirmPriority ( hull );
		
		hull.mPartition = this;
		hull.mInterfaceMask = interfaceMask;
		hull.ScheduleUpdate ();
		
		this->MOAIPartition_OnInsertHull ( hull );
	}
}

//----------------------------------------------------------------//
bool MOAIPartition::IsEmpty ( MOAIPartitionHull& hull ) {
	return hull.mCell == &this->mEmpties;
}

//----------------------------------------------------------------//
bool MOAIPartition::IsGlobal ( MOAIPartitionHull& hull ) {
	return hull.mCell == &this->mGlobals;
}

//----------------------------------------------------------------//
MOAIPartition::MOAIPartition () :
	mPriorityCounter ( 0 ),
	mPlaneID ( ZLBox::PLANE_XY ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIPartition::~MOAIPartition () {
	this->Clear ();
}

//----------------------------------------------------------------//
// This moves all props to the 'empties' cell
void MOAIPartition::PrepareRebuild () {

	size_t totalLevels = this->mLevels.Size ();
	for ( size_t i = 0; i < totalLevels; ++i ) {
		this->mLevels [ i ].ExtractProps ( this->mEmpties, 0 );
	}
	this->mBiggies.ExtractProps ( this->mEmpties, 0 );
	this->mGlobals.ExtractProps ( this->mEmpties, 0 );
}

//----------------------------------------------------------------//
// This schedules all props in the 'empties' cell for an update
void MOAIPartition::Rebuild () {

	this->mEmpties.ScheduleProps ();
}

//----------------------------------------------------------------//
void MOAIPartition::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	state.DefineClassConst ( klass, "PLANE_XY",	( u32 )ZLBox::PLANE_XY );
	state.DefineClassConst ( klass, "PLANE_XZ",	( u32 )ZLBox::PLANE_XZ );
	state.DefineClassConst ( klass, "PLANE_YZ",	( u32 )ZLBox::PLANE_YZ );
}

//----------------------------------------------------------------//
void MOAIPartition::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "clear", _clear, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getInterfaceMask", _getInterfaceMask, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "hullForPoint", _hullForPoint, MRB_ARGS_ARG ( 3, 7 ) );
	state.DefineInstanceMethod ( klass, "hullForRay", _hullForRay, MRB_ARGS_ARG ( 6, 2 ) );
	state.DefineInstanceMethod ( klass, "hullList", _hullList, MRB_ARGS_ARG ( 0, 7 ) );
	state.DefineInstanceMethod ( klass, "hullListForPoint", _hullListForPoint, MRB_ARGS_ARG ( 3, 7 ) );
	state.DefineInstanceMethod ( klass, "hullListForRay", _hullListForRay, MRB_ARGS_ARG ( 6, 7 ) );
	state.DefineInstanceMethod ( klass, "hullListForRect", _hullListForRect, MRB_ARGS_ARG ( 4, 7 ) );
	//state.DefineInstanceMethod ( klass, "insertHull", _insertHull, MRB_ARGS_NONE () );
	//state.DefineInstanceMethod ( klass, "removeHull", _removeHull, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "reserveLevels", _reserveLevels, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setLevel", _setLevel, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "setPlane", _setPlane, MRB_ARGS_REQ ( 1 ) );
	
}

//----------------------------------------------------------------//
void MOAIPartition::RemoveHull ( MOAIPartitionHull& hull ) {

	if ( hull.mPartition != this ) return;
	
	if ( hull.mCell ) {
		hull.mCell->RemoveHull ( hull );
	}

	hull.mPartition = 0;
	hull.mInterfaceMask = 0;
	this->MOAIPartition_OnRemoveHull ( hull );

	this->RubyRelease ( &hull );
}

//----------------------------------------------------------------//
void MOAIPartition::ReserveLevels ( int totalLevels ) {

	this->PrepareRebuild ();
	this->mLevels.Init ( totalLevels );
	this->Rebuild ();
}

//----------------------------------------------------------------//
void MOAIPartition::SetLevel ( int levelID, float cellSize, int width, int height ) {

	this->PrepareRebuild ();
	this->mLevels [ levelID ].Init ( cellSize, width, height );
	this->Rebuild ();
}

//----------------------------------------------------------------//
void MOAIPartition::UpdateHull ( MOAIPartitionHull& hull ) {

	// clear out the level; level will be re-calculated below
	// also: hull.mLevel is *only* for debug drawing 
	hull.mLevel = 0;
	
	u32 status = hull.mWorldBounds.mStatus;

	// status is not 'OK' so hull is either global or empty
	if ( status == ZLBounds::ZL_BOUNDS_GLOBAL ) {
		this->mGlobals.InsertHull ( hull );
		return;
	}
	
	if ( status == ZLBounds::ZL_BOUNDS_EMPTY ) {
		this->mEmpties.InsertHull ( hull );
		return;
	}

	ZLRect rect = hull.mWorldBounds.GetRect ( this->mPlaneID );

	float width = rect.Width ();
	float height = rect.Height ();
	float cellSize = ( width > height ) ? width : height;
	
	if ( cellSize > 0.0f ) {
		
		MOAIPartitionLevel* level = 0;
		
		size_t totalLevels = this->mLevels.Size ();
		for ( size_t i = 0; i < totalLevels; ++i ) {
			
			MOAIPartitionLevel* testLevel = &this->mLevels [ i ];
			
			if ( cellSize <= testLevel->mCellSize ) {
				if (( level == 0 ) || ( testLevel->mCellSize  < level->mCellSize )) {
					level = testLevel;
				}
			}
		}
		
		if ( level ) {
			// level hull
			level->PlaceHull ( hull );
			hull.mLevel = level;
		}
		else {
			// biggie hull - has dimension but too big to fit in any level
			this->mBiggies.InsertHull ( hull );
		}
	}
	else {
		// empty hull
		this->mEmpties.InsertHull ( hull );
	}
}

//----------------------------------------------------------------//
void MOAIPartition::SetPlane ( u32 planeID ) {

	if ( this->mPlaneID == planeID ) return;
	this->PrepareRebuild ();
	this->mPlaneID = planeID;
	this->Rebuild ();
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAIPartition::MOAIPartition_DrawDebugBack () {
}

//----------------------------------------------------------------//
void MOAIPartition::MOAIPartition_DrawDebugFront () {
}

//----------------------------------------------------------------//
void MOAIPartition::MOAIPartition_OnInsertHull ( MOAIPartitionHull& hull ) {
	UNUSED ( hull );
}

//----------------------------------------------------------------//	
void MOAIPartition::MOAIPartition_OnRemoveHull ( MOAIPartitionHull& hull ) {
	UNUSED ( hull );
}

//----------------------------------------------------------------//
void MOAIPartition::MOAIPartition_OnUpdateHull ( MOAIPartitionHull& hull ) {
	UNUSED ( hull );
}
