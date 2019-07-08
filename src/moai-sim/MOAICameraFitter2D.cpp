// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <moai-sim/MOAICameraAnchor2D.h>
#include <moai-sim/MOAICameraFitter2D.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAITransform.h>
#include <moai-sim/MOAIViewport.h>

//----------------------------------------------------------------//
/**	@lua	clearAnchors
	@text	Remove all camera anchors from the fitter.

	@in		MOAICameraFitter2D self
	@out	nil
*/
mrb_value MOAICameraFitter2D::_clearAnchors ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	self->Clear ();

	return context;
}

//----------------------------------------------------------------//
/**	@lua	clearFitMode
	@text	Clears bits in the fitting mask.

	@in		MOAICameraFitter2D self
	@opt	number mask		Default value is FITTING_MODE_MASK
	@out	nil
*/
mrb_value MOAICameraFitter2D::_clearFitMode( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	u32 mask = state.GetParamValue < u32 >( 1, FITTING_MODE_MASK );

	self->mFittingMode &= ~mask;

	return context;
}

//----------------------------------------------------------------//
/**	@lua	getFitDistance
	@text	Returns the distance between the camera's current x, y, scale and
			the target x, y, scale. As the camera approaches its target, the
			distance approaches 0. Check the value returned by this function
			against a small epsilon value.

	@in		MOAICameraFitter2D self
	@out	number distance
*/
mrb_value MOAICameraFitter2D::_getFitDistance ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	float distance = self->GetFitDistance ();
	return state.ToRValue ( distance );
}

//----------------------------------------------------------------//
/**	@lua	getFitLoc
	@text	Get the fitter location.

	@in		MOAICameraFitter2D self
	@out	number x
	@out	number y
*/
mrb_value MOAICameraFitter2D::_getFitLoc ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( self->mFitLoc.mX );
	ret [ 1 ] = state.ToRValue ( self->mFitLoc.mY );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getFitMode
	@text	Gets bits in the fitting mask.

	@in		MOAICameraFitter2D self
	@out	number mask
*/
mrb_value MOAICameraFitter2D::_getFitMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	return state.ToRValue ( self->mFittingMode );
}

//----------------------------------------------------------------//
/**	@lua	getFitScale
	@text	Returns the fit scale

	@in		MOAICameraFitter2D self
	@out	number scale
*/
mrb_value MOAICameraFitter2D::_getFitScale ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	return state.ToRValue ( self->mFitScale );
}

//----------------------------------------------------------------//
/**	@lua	getTargetLoc
 @text	Get the target location.

 @in	MOAICameraFitter2D self
 @out	number x
 @out	number y
 */
mrb_value MOAICameraFitter2D::_getTargetLoc ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( self->mTargetLoc.mX );
	ret [ 1 ] = state.ToRValue ( self->mTargetLoc.mY );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getTargetScale
 @text	Returns the target scale

 @in	MOAICameraFitter2D self
 @out	number scale
 */
mrb_value MOAICameraFitter2D::_getTargetScale ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	return state.ToRValue ( self->mTargetScale );
}

//----------------------------------------------------------------//
/**	@lua	insertAnchor
	@text	Add an anchor to the fitter.

	@in		MOAICameraFitter2D self
	@in		MOAICameraAnchor2D anchor
	@out	nil
*/
mrb_value MOAICameraFitter2D::_insertAnchor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "UU" )

	MOAICameraAnchor2D* anchor = state.GetRubyObject < MOAICameraAnchor2D >( 1, true );
	if ( anchor ) {
		self->AddAnchor ( *anchor );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	removeAnchor
	@text	Remove an anchor from the fitter.

	@in		MOAICameraFitter2D self
	@in		MOAICameraAnchor2D anchor
	@out	nil
*/
mrb_value MOAICameraFitter2D::_removeAnchor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "UU" )

	MOAICameraAnchor2D* anchor = state.GetRubyObject < MOAICameraAnchor2D >( 1, true );
	if ( anchor ) {
		self->RemoveAnchor ( *anchor );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setBounds
	@text	Sets or clears the world bounds of the fitter. The camera will not
			move outside of the fitter's bounds.

	@overload

		@in		MOAICameraFitter2D self
		@in		number xMin
		@in		number yMin
		@in		number xMax
		@in		number yMax
		@out	nil

	@overload

		@in		MOAICameraFitter2D self
		@out	nil
*/
mrb_value MOAICameraFitter2D::_setBounds ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	if ( state.CheckParams ( 1, "NNNN" )) {

		float x0	= state.GetParamValue < float >( 1, 0.0f );
		float y0	= state.GetParamValue < float >( 2, 0.0f );
		float x1	= state.GetParamValue < float >( 3, 0.0f );
		float y1	= state.GetParamValue < float >( 4, 0.0f );

		self->mBounds.Init ( x0, y0, x1, y1 );
		self->mFittingMode |= FITTING_MODE_APPLY_BOUNDS;
	}
	else {

		self->mFittingMode &= ~FITTING_MODE_APPLY_BOUNDS;
	}

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setCamera
	@text	Set a MOAITransform for the fitter to use as a camera.
			The fitter will dynamically change the location and
			scale of the camera to keep all of the anchors on the
			screen.

	@in		MOAICameraFitter2D self
	@opt	MOAITransform camera		Default value is nil.
	@out	nil
*/
mrb_value MOAICameraFitter2D::_setCamera ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	self->mCamera.Set ( *self, state.GetRubyObject < MOAITransform >( 1, true ));
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setDamper
	@text	Sets the fitter's damper coefficient. This is a scalar
			applied to the difference between the camera transform's
			location and the fitter's target location every frame.
			The smaller the coefficient, the tighter the fit will be.
			A value of '0' will not dampen at all; a value of '1' will
			never move the camera.

	@in		MOAICameraFitter2D self
	@opt	number damper		Default value is 0.
	@out	nil
*/
mrb_value MOAICameraFitter2D::_setDamper ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	self->mDamper = state.GetParamValue < float >( 1, 0.0f );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setFitLoc
	@text	Set the fitter's location.

	@in		MOAICameraFitter2D self
	@opt	number x		Default value is 0.
	@opt	number y		Default value is 0.
	@opt	boolean snap	Default value is false.
	@out	nil
*/
mrb_value MOAICameraFitter2D::_setFitLoc ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	self->mFitLoc.mX = state.GetParamValue < float >( 1, 0.0f );
	self->mFitLoc.mY = state.GetParamValue < float >( 2, 0.0f );

	self->mFittingMode &= ~FITTING_MODE_APPLY_ANCHORS;
	self->UpdateTarget ();

	bool snap = state.GetParamValue < bool >( 3, false );
	if ( snap && self->mCamera ) {
		self->SnapToTargetLoc ( *self->mCamera );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setFitMode
	@text	Sets bits in the fitting mask.

	@in		MOAICameraFitter2D self
	@opt	number mask		Default value is FITTING_MODE_DEFAULT
	@out	nil
*/
mrb_value MOAICameraFitter2D::_setFitMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	self->mFittingMode |= state.GetParamValue < u32 >( 1, FITTING_MODE_DEFAULT );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setFitScale
	@text	Set the fitter's scale.

	@in		MOAICameraFitter2D self
	@opt	number scale	Default value is 1.
	@opt	boolean snap	Default value is false.
	@out	nil
*/
mrb_value MOAICameraFitter2D::_setFitScale ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	self->mFitScale = state.GetParamValue < float >( 1, 1.0f );

	self->mFittingMode &= ~FITTING_MODE_APPLY_ANCHORS;
	self->UpdateTarget ();

	bool snap = state.GetParamValue < bool >( 2, false );
	if ( snap && self->mCamera ) {
		self->SnapToTargetScale ( *self->mCamera );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setMin
	@text	Set the minimum number of world units to be displayed by
			the camera along either axis.

	@in		MOAICameraFitter2D self
	@opt	number min					Default value is 0.
	@out	nil
*/
mrb_value MOAICameraFitter2D::_setMin ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	self->mMin = state.GetParamValue < float >( 1, 0.0f );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setViewport
	@text	Set the viewport to be used for fitting.

	@in		MOAICameraFitter2D self
	@opt	MOAIViewport viewport			Default value is nil.
	@out	nil
*/
mrb_value MOAICameraFitter2D::_setViewport ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	self->mViewport.Set ( *self, state.GetRubyObject < MOAIViewport >( 1, true ));
	return context;
}

//----------------------------------------------------------------//
/**	@lua	snapToTarget
	@text	Snap the camera to the target fitting.

	@overload	Snap the fitter's camera transform to the target.

		@in		MOAICameraFitter2D self
		@out	nil

	@overload	Snap a passed in transform to the target.

		@in		MOAICameraFitter2D self
		@in		MOAITransform transform
		@out	nil
*/
mrb_value MOAICameraFitter2D::_snapToTarget ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	MOAITransform* camera = state.GetRubyObject < MOAITransform >( 1, true );
	if ( camera ) {
		self->SnapToTargetLoc ( *camera );
		self->SnapToTargetScale ( *camera );
	}
	else {
		if ( self->mCamera ) {
			self->SnapToTargetLoc ( *self->mCamera );
			self->SnapToTargetScale ( *self->mCamera );
		}
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	startTrackingNode
	@text	Track a MOAITransform's position by setting the fit location.
	Works best with FITTING_MODE_APPLY_BOUNDS.

	@in		MOAICameraFitter2D self
	@in		MOAITransform node
	@out	nil
*/
mrb_value MOAICameraFitter2D::_startTrackingNode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP (MOAICameraFitter2D, "UU" )

	MOAITransform* node = state.GetRubyObject < MOAITransform >( 1, true );
	if ( node ) {
		self->StartTrackingNode ( *node );
	} else {
		self->mFittingMode &= ~FITTING_MODE_TRACK_NODE;
	}

	return context;
}

//----------------------------------------------------------------//
/**	@lua	stopTrackingNode
	@text	Stop tracking the node if one was tracked

	@in		MOAICameraFitter2D self
	@out	nil
*/
mrb_value MOAICameraFitter2D::_stopTrackingNode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAICameraFitter2D, "U" )

	self->StopTrackingNode ();

	return context;
}

//================================================================//
// MOAICameraFitter2D
//================================================================//

//----------------------------------------------------------------//
void MOAICameraFitter2D::AddAnchor ( MOAICameraAnchor2D& anchor ) {

	if ( !this->mAnchors.contains ( &anchor )) {
		this->RubyRetain ( &anchor );
		this->mAnchors.insert ( &anchor );
	}
}

//----------------------------------------------------------------//
void MOAICameraFitter2D::Clear () {

	while ( this->mAnchors.size ()) {
		AnchorIt anchorIt = this->mAnchors.begin ();
		MOAICameraAnchor2D* anchor = *anchorIt;
		this->mAnchors.erase ( anchorIt );
		this->RubyRelease ( anchor );
	}

	this->mCamera.Set ( *this, 0 );
	this->mViewport.Set ( *this, 0 );
}

//----------------------------------------------------------------//
ZLRect MOAICameraFitter2D::GetAnchorRect () {

	// expand the world rect to include all the anchors
	AnchorIt anchorIt = this->mAnchors.begin ();
	ZLRect worldRect = ( *anchorIt )->GetRect ();
	++anchorIt;

	for ( ; anchorIt != this->mAnchors.end (); ++anchorIt ) {
		MOAICameraAnchor2D* anchor = *anchorIt;
		worldRect.Grow ( anchor->GetRect ());
	}

	// clip the world rect to the bounds
	if ( this->mFittingMode & FITTING_MODE_APPLY_BOUNDS ) {
		this->mBounds.Clip ( worldRect );
	}

	// enforce the minimum
	float width = worldRect.Width ();
	float height = worldRect.Height ();
	float pad;

	if ( this->mMin > 0.0f ) {

		if ( width < this->mMin ) {
			pad = ( this->mMin - width ) * 0.5f;
			worldRect.mXMin -= pad;
			worldRect.mXMax += pad;
		}

		if ( height < this->mMin ) {
			pad = ( this->mMin - height ) * 0.5f;
			worldRect.mYMin -= pad;
			worldRect.mYMax += pad;
		}
	}

	return worldRect;
}

//----------------------------------------------------------------//
void MOAICameraFitter2D::StartTrackingNode ( MOAITransform& node) {
	this->SetNodeLink(node);
	this->mTrackingNode.Set(*this, &node);
	this->mFittingMode |= FITTING_MODE_TRACK_NODE;
}

//----------------------------------------------------------------//
void MOAICameraFitter2D::StopTrackingNode () {
	this->mFittingMode &= ~(FITTING_MODE_TRACK_NODE);
	this->ClearNodeLink(*this->mTrackingNode);
	this->mTrackingNode.Set(*this, 0);
}

//----------------------------------------------------------------//
void MOAICameraFitter2D::GetCamera ( ZLAffine3D& camera ) {

	camera.ScRoTr ( this->mTargetScale, this->mTargetScale, 1.0f, 0.0f, 0.0f, 0.0f, this->mTargetLoc.mX, this->mTargetLoc.mY, 0.0f );
}

//----------------------------------------------------------------//
float MOAICameraFitter2D::GetFitDistance () {

	if ( this->mCamera ) {

		ZLVec3D loc = this->mCamera->GetLoc ();
		float scale = this->mCamera->GetScl ().mX;

		ZLVec3D current ( loc.mX, loc.mY, scale );
		ZLVec3D target ( this->mTargetLoc.mX, this->mTargetLoc.mY, this->mTargetScale );

		return ZLDist::VecToVec ( current, target );
	}
	return 0.0f;
}

//----------------------------------------------------------------//
MOAICameraFitter2D::MOAICameraFitter2D () :
	mMin ( 0.0f ),
	mDamper ( 0.0f ),
	mFittingMode ( FITTING_MODE_DEFAULT ) {

	RTTI_BEGIN
		RTTI_EXTEND ( MOAIAction )
		RTTI_EXTEND ( MOAINode )
	RTTI_END

	this->mFitLoc.Init ( 0.0f, 0.0f, 0.0f );
	this->mFitScale = 1.0f;

	this->mTargetLoc.Init ( 0.0f, 0.0f, 0.0f );
	this->mTargetScale = 1.0f;
}

//----------------------------------------------------------------//
MOAICameraFitter2D::~MOAICameraFitter2D () {

	this->Clear ();
}

//----------------------------------------------------------------//
void MOAICameraFitter2D::RemoveAnchor ( MOAICameraAnchor2D& anchor ) {

	if ( this->mAnchors.contains ( &anchor )) {
		this->mAnchors.erase ( &anchor );
		this->RubyRelease ( &anchor );
	}
}

//----------------------------------------------------------------//
void MOAICameraFitter2D::SnapToTargetLoc ( MOAITransform& camera ) {

	camera.SetLoc ( this->mTargetLoc );

	camera.ScheduleUpdate ();
}

//----------------------------------------------------------------//
void MOAICameraFitter2D::SnapToTargetScale ( MOAITransform& camera ) {

	ZLVec3D scaleVec;
	scaleVec.Init ( this->mTargetScale, this->mTargetScale, 1.0f );
	camera.SetScl ( scaleVec );

	camera.ScheduleUpdate ();
}

//----------------------------------------------------------------//
void MOAICameraFitter2D::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIAction::RegisterRubyClass ( state, klass );
	MOAINode::RegisterRubyClass ( state, klass );

	state.DefineClassConst ( klass, "FITTING_MODE_SEEK_LOC", ( u32 )FITTING_MODE_SEEK_LOC );
	state.DefineClassConst ( klass, "FITTING_MODE_SEEK_SCALE", ( u32 )FITTING_MODE_SEEK_SCALE );
	state.DefineClassConst ( klass, "FITTING_MODE_APPLY_ANCHORS", ( u32 )FITTING_MODE_APPLY_ANCHORS );
	state.DefineClassConst ( klass, "FITTING_MODE_APPLY_BOUNDS", ( u32 )FITTING_MODE_APPLY_BOUNDS );
	state.DefineClassConst ( klass, "FITTING_MODE_DEFAULT", ( u32 )FITTING_MODE_DEFAULT );
	state.DefineClassConst ( klass, "FITTING_MODE_TRACK_NODE", ( u32 )FITTING_MODE_TRACK_NODE);
	state.DefineClassConst ( klass, "FITTING_MODE_MASK", ( u32 )FITTING_MODE_MASK );
}

//----------------------------------------------------------------//
void MOAICameraFitter2D::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIAction::RegisterRubyFuncs ( state, klass );
	MOAINode::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "clearAnchors", _clearAnchors, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "clearFitMode", _clearFitMode, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "getFitDistance", _getFitDistance, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getFitLoc", _getFitLoc, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getFitMode", _getFitMode, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getFitScale", _getFitScale, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getTargetLoc", _getTargetLoc, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getTargetScale", _getTargetScale, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "insertAnchor", _insertAnchor, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "removeAnchor", _removeAnchor, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setBounds", _setBounds, MRB_ARGS_ARG ( 0, 4 ) );
	state.DefineInstanceMethod ( klass, "setCamera", _setCamera, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setDamper", _setDamper, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setFitLoc", _setFitLoc, MRB_ARGS_ARG ( 0, 3 ) );
	state.DefineInstanceMethod ( klass, "setFitMode", _setFitMode, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setFitScale", _setFitScale, MRB_ARGS_ARG ( 0, 2 ) );
	state.DefineInstanceMethod ( klass, "setMin", _setMin, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setViewport", _setViewport, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "snapToTarget", _snapToTarget, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "startTrackingNode", _startTrackingNode, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "stopTrackingNode", _stopTrackingNode, MRB_ARGS_NONE () );

}

//----------------------------------------------------------------//
void MOAICameraFitter2D::UpdateFit () {

	if ( !( this->mFittingMode & FITTING_MODE_APPLY_ANCHORS )) return;
	if ( !this->mAnchors.size ()) return;
	if ( !this->mViewport ) return;

	// reset the fitter
	this->mFitLoc.Init ( 0.0f, 0.0f, 0.0f );
	this->mFitScale = 1.0f;

	// grab the view transform
	ZLMatrix4x4 wndToWorld = this->mViewport->GetWndToNormMtx ();
	wndToWorld.Append ( this->mViewport->GetProjMtxInv ());

	// grab the view rect in world space
	// TODO: take viewport offset into account
	ZLRect worldViewRect = this->mViewport->GetRect ();
	wndToWorld.Transform ( worldViewRect );
	worldViewRect.Bless ();

	// build the anchor rect (clipped to bounds)
	ZLRect anchorRect = this->GetAnchorRect ();

	// fit the view rect around the target rect while preserving aspect ratio
	ZLRect fitViewRect = worldViewRect;
	anchorRect.FitOutside ( fitViewRect );

	// get the fitting
	this->mFitScale = fitViewRect.Width () / worldViewRect.Width ();
	fitViewRect.GetCenter ( this->mFitLoc );
}

//----------------------------------------------------------------//
void MOAICameraFitter2D::UpdateTarget () {

	if ( !this->mViewport ) return;

	// reset the fitter
	this->mTargetLoc = this->mFitLoc;
	this->mTargetScale = this->mFitScale;

	// clamp to bounds
	if ( this->mFittingMode & FITTING_MODE_APPLY_BOUNDS ) {

		// grab the view transform
		ZLMatrix4x4 wndToWorld = this->mViewport->GetWndToNormMtx ();
		wndToWorld.Append ( this->mViewport->GetProjMtxInv ());

		// grab the view rect in world space
		// TODO: take viewport offset into account
		ZLRect worldViewRect = this->mViewport->GetRect ();
		wndToWorld.Transform ( worldViewRect );
		worldViewRect.Bless ();

		// get the camera's target position and scale
		ZLAffine3D cameraMtx;
		float rot = this->mCamera ? this->mCamera->GetRot ().mZ : 0.0f;
		cameraMtx.ScRoTr ( this->mFitScale, this->mFitScale, 1.0f, 0.0f, 0.0f, rot * ( float )D2R, this->mFitLoc.mX, this->mFitLoc.mY, 0.0f );

		// get the camera rect
		ZLRect cameraRect = worldViewRect;
		cameraMtx.Transform ( cameraRect );
		cameraRect.Bless ();

		this->mBounds.ConstrainWithAspect ( cameraRect );

		// get the fitting
		this->mTargetScale = cameraRect.Width () / worldViewRect.Width ();
		cameraRect.GetCenter ( this->mTargetLoc );
	}
}

//----------------------------------------------------------------//
void MOAICameraFitter2D::UpdateTracking () {

	if ( !( this->mFittingMode & FITTING_MODE_TRACK_NODE ) || !this->mTrackingNode ) return;
	this->mFitLoc = this->mTrackingNode->GetLoc ();
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAICameraFitter2D::MOAIAction_IsDone () {

	return false;
}

//----------------------------------------------------------------//
void MOAICameraFitter2D::MOAIAction_Update ( double step ) {
	UNUSED ( step );
	
	this->ScheduleUpdate ();
	
	// make sure all the anchors are ahead of fitter in the update schedule
	AnchorIt anchorIt = this->mAnchors.begin ();	
	for ( ; anchorIt != this->mAnchors.end (); ++anchorIt ) {
		MOAICameraAnchor2D* anchor = *anchorIt;
		anchor->Activate ( *this );
	}
}

//----------------------------------------------------------------//
void MOAICameraFitter2D::MOAINode_Update () {

	this->UpdateTracking();
	this->UpdateFit ();
	this->UpdateTarget ();
	
	if ( this->mCamera ) {
		
		float d = 1.0f - ZLFloat::Clamp ( this->mDamper, 0.0f, 1.0f );
		
		ZLVec3D loc = this->mCamera->GetLoc ();
		float scale = this->mCamera->GetScl ().mX;
		
		loc.mX += ( this->mTargetLoc.mX - loc.mX ) * d;
		loc.mY += ( this->mTargetLoc.mY - loc.mY ) * d;
		scale += ( this->mTargetScale - scale ) * d;
		
		ZLVec3D scaleVec;
		scaleVec.Init ( scale, scale, 1.0f );
		this->mCamera->SetScl ( scaleVec );
		this->mCamera->SetLoc ( loc );
		this->mCamera->ScheduleUpdate ();
	}
}
