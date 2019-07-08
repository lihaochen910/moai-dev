// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPARTITION_H
#define	MOAIPARTITION_H

#include <moai-sim/MOAIPartitionCell.h>
#include <moai-sim/MOAIPartitionLevel.h>

//================================================================//
// MOAIPartition
//================================================================//
/**	@lua	MOAIPartition
	@text	Class for optimizing spatial queries against sets of primitives.
			Configure for performance; default behavior is a simple list.
	
	@const PLANE_XY
	@const PLANE_XZ
	@const PLANE_YZ
*/
class MOAIPartition :
	public virtual MOAIRubyObject {
private:

	static const u32 INTERFACE_MASK_BITS = 32;
	static const u32 MASK_ANY = 0xffffffff;

	friend class MOAIPartitionCell;
	friend class MOAIPartitionLevel;
	friend class MOAIPartitionHull;

	ZLLeanArray < MOAIPartitionLevel >	mLevels;
	MOAIPartitionCell					mEmpties;
	MOAIPartitionCell					mGlobals;
	MOAIPartitionCell					mBiggies;

	ZLLeanArray < u32 >					mInterfaceIDs; // array if ZLTypeIDs for supported interfaces

	s32					mPriorityCounter;
	static const s32	PRIORITY_MASK = 0x7fffffff;

	u32 mPlaneID; // One of ZLBox::PLANE_XY, ZLBox::PLANE_XZ, ZLBox::PLANE_YZ

	//----------------------------------------------------------------//
	static mrb_value		_clear					( mrb_state* M, mrb_value context );
	static mrb_value		_getInterfaceMask		( mrb_state* M, mrb_value context );
	static mrb_value		_hullForPoint			( mrb_state* M, mrb_value context );
	static mrb_value		_hullForRay				( mrb_state* M, mrb_value context );
	static mrb_value		_hullList				( mrb_state* M, mrb_value context );
	static mrb_value		_hullListForPoint		( mrb_state* M, mrb_value context );
	static mrb_value		_hullListForRay			( mrb_state* M, mrb_value context );
	static mrb_value		_hullListForRect		( mrb_state* M, mrb_value context );
	//static mrb_value		_insertHull				( mrb_state* M, mrb_value context );
	//static mrb_value		_removeHull				( mrb_state* M, mrb_value context );
	static mrb_value		_reserveLevels			( mrb_state* M, mrb_value context );
	static mrb_value		_setLevel				( mrb_state* M, mrb_value context );
	static mrb_value		_setPlane				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	u32				AffirmInterfaceMask		( u32 typeID );
	void			AffirmPriority			( MOAIPartitionHull& hull );
	u32				GetInterfaceMask		( u32 typeID ) const;
	void			PrepareRebuild			();
	void			Rebuild					();
	void			UpdateHull				( MOAIPartitionHull& hull );

	//----------------------------------------------------------------//
	virtual void	MOAIPartition_DrawDebugBack		();
	virtual void	MOAIPartition_DrawDebugFront	();
	virtual void	MOAIPartition_OnInsertHull		( MOAIPartitionHull& hull );
	virtual void	MOAIPartition_OnRemoveHull		( MOAIPartitionHull& hull );
	virtual void	MOAIPartition_OnUpdateHull		( MOAIPartitionHull& hull );

public:
	
	DECL_RUBY_FACTORY ( MOAIPartition, MOAIRubyObject )
	
	//----------------------------------------------------------------//
	void			Clear					();
	void			DrawDebugBack			();
	void			DrawDebugFront			();
	u32				GatherHulls				( MOAIPartitionResultBuffer& results, MOAIPartitionHull* ignoreProp, u32 interfaceMask = MASK_ANY, u32 mask = MASK_ANY );
	u32				GatherHulls				( MOAIPartitionResultBuffer& results, MOAIPartitionHull* ignoreProp, const ZLVec3D& point, u32 interfaceMask = MASK_ANY, u32 mask = MASK_ANY );
	u32				GatherHulls				( MOAIPartitionResultBuffer& results, MOAIPartitionHull* ignoreProp, const ZLVec3D& point, const ZLVec3D& orientation, u32 interfaceMask = MASK_ANY, u32 mask = MASK_ANY );
	u32				GatherHulls				( MOAIPartitionResultBuffer& results, MOAIPartitionHull* ignoreProp, ZLRect rect, u32 interfaceMask = MASK_ANY, u32 mask = MASK_ANY );
	u32				GatherHulls				( MOAIPartitionResultBuffer& results, MOAIPartitionHull* ignoreProp, ZLBox box, u32 interfaceMask = MASK_ANY, u32 mask = MASK_ANY );
	u32				GatherHulls				( MOAIPartitionResultBuffer& results, MOAIPartitionHull* ignoreProp, const ZLFrustum& frustum, u32 interfaceMask = MASK_ANY, u32 mask = MASK_ANY );
	void			InsertHull				( MOAIPartitionHull& hull );
	bool			IsEmpty					( MOAIPartitionHull& hull );
	bool			IsGlobal				( MOAIPartitionHull& hull );
					MOAIPartition			();
					~MOAIPartition			();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			RemoveHull				( MOAIPartitionHull& hull );
	void			ReserveLevels			( int totalLevels );
	void			SetLevel				( int levelID, float cellSize, int width, int height );
	void			SetPlane				( u32 planeID );
	
	//----------------------------------------------------------------//
	template < typename TYPE >
	u32 AffirmInterfaceMask () {
		return this->AffirmInterfaceMask ( ZLTypeID < TYPE >::GetID ());
	}
	
	//----------------------------------------------------------------//
	template < typename TYPE >
	u32 GetInterfaceMask () const {
		return this->GetInterfaceMask ( ZLTypeID < TYPE >::GetID ());
	}
};

#endif
