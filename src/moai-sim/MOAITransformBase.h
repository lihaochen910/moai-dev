// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAITRANSFORMBASE_H
#define	MOAITRANSFORMBASE_H

#include <moai-sim/MOAINode.h>

//================================================================//
// MOAITransformBase
//================================================================//
/**	@lua	MOAITransformBase
	@text	Base class for 2D affine transforms.
	
	@attr	ATTR_WORLD_X_LOC
	@attr	ATTR_WORLD_Y_LOC
	@attr	ATTR_WORLD_Z_LOC
	@attr	ATTR_WORLD_Z_ROT
	@attr	ATTR_WORLD_X_SCL
	@attr	ATTR_WORLD_Y_SCL
	@attr	ATTR_WORLD_Z_SCL
	@attr	TRANSFORM_TRAIT
*/
class MOAITransformBase :
	public virtual MOAINode {
protected:
	
	ZLAffine3D		mLocalToWorldMtx;
	ZLAffine3D		mWorldToLocalMtx;

	//----------------------------------------------------------------//
	static mrb_value	_getWorldDir		( mrb_state* M, mrb_value context );
	static mrb_value	_getWorldLoc		( mrb_state* M, mrb_value context );
	static mrb_value	_getWorldRot		( mrb_state* M, mrb_value context );
	static mrb_value	_getWorldScl		( mrb_state* M, mrb_value context );
	static mrb_value	_getWorldXAxis		( mrb_state* M, mrb_value context );
	static mrb_value	_getWorldYAxis		( mrb_state* M, mrb_value context );
	static mrb_value	_getWorldZAxis		( mrb_state* M, mrb_value context );
	static mrb_value	_getWorldXNormal	( mrb_state* M, mrb_value context );
	static mrb_value	_getWorldYNormal	( mrb_state* M, mrb_value context );
	static mrb_value	_getWorldZNormal	( mrb_state* M, mrb_value context );
	static mrb_value	_modelToWorld		( mrb_state* M, mrb_value context );
	static mrb_value	_setParent			( mrb_state* M, mrb_value context );
	static mrb_value	_worldToModel		( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	bool				MOAINode_ApplyAttrOp						( u32 attrID, MOAIAttribute& attr, u32 op );
	void				MOAINode_Update								();
	virtual void		MOAITransformBase_BuildLocalToWorldMtx		( ZLAffine3D& localToWorldMtx ) = 0;

public:
	
	DECL_ATTR_HELPER ( MOAITransformBase )
	
	enum {
		ATTR_WORLD_X_LOC,
		ATTR_WORLD_Y_LOC,
		ATTR_WORLD_Z_LOC,
		
		ATTR_WORLD_Z_ROT,
		
		ATTR_WORLD_X_SCL,
		ATTR_WORLD_Y_SCL,
		ATTR_WORLD_Z_SCL,
		
		TRANSFORM_TRAIT,
		
		INHERIT_LOC,
		INHERIT_TRANSFORM,
		
		TOTAL_ATTR,
	};
	
	GET ( ZLVec3D, WorldLoc, mLocalToWorldMtx.GetTranslation ())
	
	//----------------------------------------------------------------//
	const ZLAffine3D&		GetLocalToWorldMtx			() const;
	const ZLAffine3D*		GetLocTrait					() const;
	const ZLAffine3D*		GetTransformTrait			() const;
	const ZLAffine3D&		GetWorldToLocalMtx			() const;
							MOAITransformBase			();
							~MOAITransformBase			();
	void					RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void					RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
};

#endif
