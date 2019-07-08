// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIVIEWLAYER_H
#define	MOAIVIEWLAYER_H

#include <moai-sim/MOAIFrameBuffer.h>
#include <moai-sim/MOAIGraphicsProp.h>
#include <moai-sim/MOAIPartitionHolder.h>
#include <moai-sim/MOAILayer.h>
#include <moai-sim/MOAIViewport.h>

class MOAICamera;

//================================================================//
// MOAIViewLayer
//================================================================//
// TODO: doxygen
class MOAIViewLayer :
	public virtual MOAIGraphicsProp,
	public virtual MOAILayer {
protected:

	MOAIRubySharedPtr < MOAICamera >		mCamera;
	MOAIRubySharedPtr < MOAICamera >		mDebugCamera;
	MOAIRubySharedPtr < MOAIViewport >		mViewport;

	ZLVec3D			mParallax;
	bool			mShowDebugLines;

	//----------------------------------------------------------------//
	static mrb_value		_getCamera				( mrb_state* M, mrb_value context );
	static mrb_value		_getFitting				( mrb_state* M, mrb_value context );
	static mrb_value		_getFitting3D			( mrb_state* M, mrb_value context );
	static mrb_value		_getViewport			( mrb_state* M, mrb_value context );
	static mrb_value		_setDebugCamera			( mrb_state* M, mrb_value context );
	static mrb_value		_setCamera				( mrb_state* M, mrb_value context );
	static mrb_value		_setParallax			( mrb_state* M, mrb_value context );
	static mrb_value		_setViewport			( mrb_state* M, mrb_value context );
	static mrb_value		_showDebugLines			( mrb_state* M, mrb_value context );
	static mrb_value		_wndToWorld				( mrb_state* M, mrb_value context );
	static mrb_value		_wndToWorldRay			( mrb_state* M, mrb_value context );
	static mrb_value		_worldToWnd				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void					MOAIDrawable_Draw					( int subPrimID );
	ZLBounds				MOAIPartitionHull_GetModelBounds	();
	virtual void			MOAIViewLayer_Draw					() = 0;

public:

	DECL_RUBY_ABSTRACT ( MOAILayer )

	//----------------------------------------------------------------//
	float					GetFitting				( ZLRect& worldRect, float hPad, float vPad );
	MOAIRubyClass*			GetSuperRubyClass		();
	ZLMatrix4x4				GetWndToWorldMtx		() const;
	ZLMatrix4x4				GetWorldToWndMtx		() const;
							MOAIViewLayer			();
							~MOAIViewLayer			();
	void					RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void					RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
