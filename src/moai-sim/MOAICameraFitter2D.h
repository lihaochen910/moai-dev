// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAICAMERAFITTER2D_H
#define MOAICAMERAFITTER2D_H

#include <moai-sim/MOAIAction.h>
#include <moai-sim/MOAINode.h>

class MOAICameraAnchor2D;
class MOAITransform;
class MOAIViewport;

//================================================================//
// MOAICameraFitter2D
//================================================================//
/**	@lua	MOAICameraFitter2D
	@text	Action to dynamically fit a camera transform to a set of targets
			given a viewport and world space constraints.

	@const	FITTING_MODE_SEEK_LOC
	@const	FITTING_MODE_SEEK_SCALE
	@const	FITTING_MODE_APPLY_ANCHORS
	@const	FITTING_MODE_APPLY_BOUNDS
	@const	FITTING_MODE_TRACK_NODE
	@const	FITTING_MODE_DEFAULT
	@const	FITTING_MODE_MASK
*/
class MOAICameraFitter2D :
	public virtual MOAIAction,
	public virtual MOAINode {
private:

	MOAIRubySharedPtr < MOAITransform >	mCamera;
	MOAIRubySharedPtr < MOAIViewport >	mViewport;

	typedef STLSet < MOAICameraAnchor2D* >::iterator AnchorIt;
	STLSet < MOAICameraAnchor2D* > mAnchors;

	MOAIRubySharedPtr < MOAITransform > mTrackingNode;

	ZLVec3D		mFitLoc;
	float		mFitScale;

	ZLVec3D		mTargetLoc;
	float		mTargetScale;

	ZLRect		mBounds;
	float		mMin;
	float		mDamper;

	u32			mFittingMode;

	//----------------------------------------------------------------//
	static mrb_value		_clearAnchors			( mrb_state* M, mrb_value context );
	static mrb_value		_clearFitMode			( mrb_state* M, mrb_value context );
	static mrb_value		_getFitDistance			( mrb_state* M, mrb_value context );
	static mrb_value		_getFitLoc				( mrb_state* M, mrb_value context );
	static mrb_value		_getFitMode				( mrb_state* M, mrb_value context );
	static mrb_value		_getFitScale			( mrb_state* M, mrb_value context );
	static mrb_value		_getTargetLoc			( mrb_state* M, mrb_value context );
	static mrb_value		_getTargetScale			( mrb_state* M, mrb_value context );
	static mrb_value		_insertAnchor			( mrb_state* M, mrb_value context );
	static mrb_value		_removeAnchor			( mrb_state* M, mrb_value context );
	static mrb_value		_setBounds				( mrb_state* M, mrb_value context );
	static mrb_value		_setCamera				( mrb_state* M, mrb_value context );
	static mrb_value		_setDamper				( mrb_state* M, mrb_value context );
	static mrb_value		_setFitLoc				( mrb_state* M, mrb_value context );
	static mrb_value		_setFitMode				( mrb_state* M, mrb_value context );
	static mrb_value		_setFitScale			( mrb_state* M, mrb_value context );
	static mrb_value		_setMin					( mrb_state* M, mrb_value context );
	static mrb_value		_setViewport			( mrb_state* M, mrb_value context );
	static mrb_value		_snapToTarget			( mrb_state* M, mrb_value context );
	static mrb_value		_startTrackingNode		( mrb_state* M, mrb_value context );
	static mrb_value		_stopTrackingNode		( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	ZLRect			GetAnchorRect			();
	void			GetCamera				( ZLAffine3D& camera );
	void			SetTarget				( const ZLAffine3D& camera, const ZLRect& screenRect );
	void			SnapToTargetLoc			( MOAITransform& camera );
	void			SnapToTargetScale		( MOAITransform& camera );
	void			UpdateFit				();
	void			UpdateTracking			();
	void			UpdateTarget			();

	//----------------------------------------------------------------//
	bool			MOAIAction_IsDone		();
	void			MOAIAction_Update		( double step );
	void			MOAINode_Update			();

public:

	DECL_RUBY_FACTORY ( MOAICameraFitter2D, MOAIAction )

	enum {
		FITTING_MODE_SEEK_LOC		= 0x00000001,
		FITTING_MODE_SEEK_SCALE		= 0x00000002,
		FITTING_MODE_APPLY_ANCHORS	= 0x00000004,
		FITTING_MODE_APPLY_BOUNDS	= 0x00000008,
		FITTING_MODE_TRACK_NODE		= 0x00000010,

		FITTING_MODE_MASK			= 0x000000ff,
	};

	static const u32 FITTING_MODE_DEFAULT = FITTING_MODE_SEEK_LOC | FITTING_MODE_SEEK_SCALE | FITTING_MODE_APPLY_ANCHORS;

	//----------------------------------------------------------------//
	void			AddAnchor				( MOAICameraAnchor2D& anchor );
	void			Clear					();
	float			GetFitDistance			();
					MOAICameraFitter2D		();
					~MOAICameraFitter2D		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			RemoveAnchor			( MOAICameraAnchor2D& anchor );
	void			StartTrackingNode		( MOAITransform& node );
	void			StopTrackingNode		();
};

#endif
