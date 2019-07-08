// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAISELECTIONMESH_H
#define	MOAISELECTIONMESH_H

#include <moai-sim/MOAIDeckProxy.h>
#include <moai-sim/MOAIMesh.h>

//================================================================//
// MOAISelectionSpan
//================================================================//
class MOAISelectionSpan :
	public MOAIMeshSpan {
private:
	
	friend class MOAISelectionMesh;
	
	u32					mSetID;
	
	MOAISelectionSpan*	mPrevInMaster;
	MOAISelectionSpan*	mNextInMaster;
};

//================================================================//
// MOAISelectionMesh
//================================================================//
// TODO: doxygen
class MOAISelectionMesh :
	public MOAIDeckProxy {
protected:

	ZLLeanPool < MOAISelectionSpan, 16 >	mSpanPool;
	ZLLeanArray < MOAIMeshSpan* >			mSets;

	MOAISelectionSpan*	mSpanListHead;
	MOAIMesh*			mMesh;

	//----------------------------------------------------------------//
	static mrb_value			_addSelection				( mrb_state* M, mrb_value context );
	static mrb_value			_clearSelection				( mrb_state* M, mrb_value context );
	static mrb_value			_mergeSelection				( mrb_state* M, mrb_value context );
	static mrb_value			_printSelection				( mrb_state* M, mrb_value context );
	static mrb_value			_reserveSelections			( mrb_state* M, mrb_value context );
	static mrb_value			_setMesh					( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	ZLResult < u32 >	AffirmSpanSet				();
	MOAISelectionSpan*	AllocSpan					( u32 set, size_t base, size_t top );
//	void				BooleanAnd					( u32 set, u32 a, u32 b );
//	void				BooleanOr					( u32 set, u32 a, u32 b );
//	void				BooleanNot					( u32 set, u32 a, u32 b );
//	void				BooleanXor					( u32 set, u32 a, u32 b );
	void				ChangeSpanSet				( MOAISelectionSpan* span, u32 set );
	void				Clear						();
	void				DrawIndex					( u32 idx, MOAIMaterialBatch* materials, ZLVec3D offset, ZLVec3D scale );
	void				FixOverlaps					( MOAISelectionSpan* span );
	void				FreeSpan					( MOAISelectionSpan* span );
	MOAISelectionSpan*	InsertSpan					( MOAISelectionSpan* span, MOAISelectionSpan* prevInMaster, MOAISelectionSpan* prevInSet );

	//----------------------------------------------------------------//
	void				MOAIDeck_Draw				( u32 idx );

public:

//	enum {
//		AND,
//		OR,
//		NOT,
//		XOR,
//	};

	DECL_RUBY_FACTORY ( MOAISelectionMesh, MOAIDeckProxy )

	//----------------------------------------------------------------//
	void				AddSelection				( u32 set, size_t base, size_t top );
	void				ClearSelection				( u32 set );
	void				ClearSelection				( u32 set, size_t base, size_t top );
	void				MergeSelection				( u32 set, u32 merge );
						MOAISelectionMesh			();
						~MOAISelectionMesh			();
	void				PrintSelection				( u32 set );
	void				PrintSelections				();
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void				ReserveSelections			( u32 total );
	void				Select						( u32 set, size_t base, size_t size );
	void				SerializeIn					( MOAIRubyState& state, MOAIDeserializer& serializer );
	void				SerializeOut				( MOAIRubyState& state, MOAISerializer& serializer );
};

#endif
