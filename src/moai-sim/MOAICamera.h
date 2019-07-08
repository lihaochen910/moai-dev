// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAICAMERA_H
#define	MOAICAMERA_H

#include <moai-sim/MOAITransform.h>

class MOAIViewport;

//================================================================//
// MOAICamera
//================================================================//
/**	@lua	MOAICamera
	@text	Perspective or orthographic camera.
*/
class MOAICamera :
	public virtual MOAITransform {
private:

	ZLMatrix4x4		mProjectionMtx;
	bool			mUseProjectionMtx;

	float			mFieldOfView;
	float			mNearPlane;
	float			mFarPlane;

	u32				mType;

	//----------------------------------------------------------------//
	static mrb_value		_getFarPlane			( mrb_state* M, mrb_value context );
	static mrb_value		_getFieldOfView			( mrb_state* M, mrb_value context );
	static mrb_value		_getFloorMove			( mrb_state* M, mrb_value context );
	static mrb_value		_getFocalLength			( mrb_state* M, mrb_value context );
	static mrb_value		_getNearPlane			( mrb_state* M, mrb_value context );
	static mrb_value		_getViewVector			( mrb_state* M, mrb_value context );
	static mrb_value		_lookAt					( mrb_state* M, mrb_value context );
	static mrb_value		_moveFieldOfView		( mrb_state* M, mrb_value context );
	static mrb_value		_seekFieldOfView		( mrb_state* M, mrb_value context );
	static mrb_value		_setFarPlane			( mrb_state* M, mrb_value context );
	static mrb_value		_setFieldOfView			( mrb_state* M, mrb_value context );
	static mrb_value		_setNearPlane			( mrb_state* M, mrb_value context );
	static mrb_value		_setOrtho				( mrb_state* M, mrb_value context );
	static mrb_value		_setType				( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	void			LookAt					( float x, float y, float z );
	
	//----------------------------------------------------------------//
	bool			MOAINode_ApplyAttrOp	( u32 attrID, MOAIAttribute& attr, u32 op );
	
public:
	
	enum {
		DEBUG_DRAW_FRAME,
		DEBUG_DRAW_RETICLE,
		TOTAL_DEBUG_LINE_STYLES,
	};
	
	enum {
		CAMERA_TYPE_3D,
		CAMERA_TYPE_ORTHO,
		CAMERA_TYPE_WINDOW,
	};
	
	DECL_RUBY_FACTORY ( MOAICamera, MOAITransform )
	DECL_ATTR_HELPER ( MOAICamera )

	enum {
		ATTR_FOV,		
		TOTAL_ATTR,
	};
	
	GET_SET ( float, FieldOfView, mFieldOfView )
	GET_SET ( float, NearPlane, mNearPlane )
	GET_SET ( float, FarPlane, mFarPlane )
	
	GET_SET ( u32, Type, mType )
	
	//----------------------------------------------------------------//
	void			DrawDebug				();
	ZLMatrix4x4		GetBillboardMtx			() const;
	float			GetFocalLength			( float width ) const;
	ZLMatrix4x4		GetProjMtx				( const MOAIViewport& viewport ) const;
	ZLMatrix4x4		GetViewMtx				() const;
	ZLVec3D			GetViewVector			() const;
					MOAICamera				();
					~MOAICamera				();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			SetProjMtx				();
	void			SetProjMtx				( const ZLMatrix4x4& mtx );
};

#endif
