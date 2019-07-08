// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAICamera.h>
#include <moai-sim/MOAIDebugLines.h>
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIFrameBufferTexture.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIViewLayer.h>
#include <moai-sim/MOAIMaterialMgr.h>
#include <moai-sim/MOAIPartition.h>
#include <moai-sim/MOAIPartitionResultBuffer.h>
#include <moai-sim/MOAIPartitionResultMgr.h>
#include <moai-sim/MOAIRenderMgr.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAITextureBase.h>
#include <moai-sim/MOAITransform.h>
#include <moai-sim/MOAIVertexFormatMgr.h>
#include <moai-sim/MOAIViewProj.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getCamera
	@text	Get the camera associated with the layer.
	
	@in		MOAIViewLayer self
	@out	MOAICamera camera
*/
mrb_value MOAIViewLayer::_getCamera ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewLayer, "U" )
	return state.ToRValue < MOAIRubyObject* >( ( MOAIRubyObject* )self->mCamera );
}

//----------------------------------------------------------------//
/**	@lua	getFitting
	@text	Computes a camera fitting for a given world rect along with
			an optional screen space padding. To do a fitting, compute
			the world rect based on whatever you are fitting to, use
			this method to get the fitting, then animate the camera
			to match.
	
	@in		MOAIViewLayer self
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@opt	number xPad
	@opt	number yPad
	@out	number x		X center of fitting (use for camera location).
	@out	number y		Y center of fitting (use for camera location).
	@out	number s		Scale of fitting (use for camera scale).
*/
mrb_value MOAIViewLayer::_getFitting ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewLayer, "UNNNN" )

	ZLRect worldRect;
	worldRect.mXMin = state.GetParamValue < float >( 1, 0.0f );
	worldRect.mYMin = state.GetParamValue < float >( 2, 0.0f );
	worldRect.mXMax = state.GetParamValue < float >( 3, 0.0f );
	worldRect.mYMax = state.GetParamValue < float >( 4, 0.0f );

	worldRect.Bless ();

	float hPad = state.GetParamValue < float >( 5, 0.0f );
	float vPad = state.GetParamValue < float >( 6, 0.0f );

	float x = worldRect.mXMin + (( worldRect.mXMax - worldRect.mXMin ) * 0.5f );
	float y = worldRect.mYMin + (( worldRect.mYMax - worldRect.mYMin ) * 0.5f );

	mrb_value ret [ 3 ];
	ret [ 0 ] = state.ToRValue ( x );
	ret [ 1 ] = state.ToRValue ( y );

	float fitting = self->GetFitting ( worldRect, hPad, vPad );
	ret [ 2 ] = state.ToRValue ( fitting );

	return mrb_ary_new_from_values ( state, 3, ret );
}

//----------------------------------------------------------------//
/**	@lua	getFitting3D
	@text	Find a position for the camera where all given locations or
			props will be visible without changing the camera's orientation
			(i.e. orient the camera first, then call this to derive the
			correct position).
	
	@in		MOAIViewLayer self
	@in		table targets		A table of either props or locations. Locations are tables containing {x, y, z, r}.
	@out	number x
	@out	number y
	@out	number z
*/
mrb_value MOAIViewLayer::_getFitting3D ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewLayer, "UT" )

	/*if (( !self->mViewport ) || ( !self->mCamera ) || ( self->mCamera->GetType () != MOAICamera::CAMERA_TYPE_3D )) return mrb_nil_value ();
	
	ZLRect fitRect = state.GetParamValue < ZLRect >( 2, *self->mViewport );
	
	self->mCamera->ForceUpdate ();
	
	ZLFrustumFitter fitter;
	
	fitter.Init (
		*self->mViewport,
		fitRect,
		self->mCamera->GetFieldOfView (),
		self->mCamera->GetLocalToWorldMtx ()
	);

	u32 itr = state.PushTableItr ( 1 );
	while ( state.TableItrNext ( itr )) {
	
		int type = lua_type ( state, -1 );
		
		switch ( type ) {
		
			case LUA_TTABLE: {
			
				ZLVec3D loc;
				
				loc.mX = state.GetFieldValue < float >( -1, "x", 0.0f );
				loc.mY = state.GetFieldValue < float >( -1, "y", 0.0f );
				loc.mZ = state.GetFieldValue < float >( -1, "z", 0.0f );
				
				float r = state.GetFieldValue < float >( -1, "r", 0.0f );
				
				fitter.FitPoint( loc, r );
				
				break;
			}
			
			case LUA_TUSERDATA: {
			
				MOAIPartitionHull* hull = state.GetRubyObject < MOAIPartitionHull >( -1, true );
		
				if ( hull ) {
					ZLBox bounds = hull->GetWorldBounds ();
					
					ZLVec3D center;
					bounds.GetCenter ( center );
					fitter.FitBox ( bounds, 0.0f );
				}
				break;
			}
		}
	}
	
	ZLVec3D position = fitter.GetPosition ();
	
	state.Push ( position.mX );
	state.Push ( position.mY );
	state.Push ( position.mZ );*/

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getViewport
	@text	Return the viewport currently associated with the layer.
	
	@in		MOAIViewLayer self
	@out	MOAIRubyObject viewport
*/
mrb_value MOAIViewLayer::_getViewport ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewLayer, "U" )
	return state.ToRValue < MOAIRubyObject* >( ( MOAIRubyObject* )self->mViewport );
}

//----------------------------------------------------------------//
/**	@lua	setCamera
	@text	Sets a camera for the layer. If no camera is supplied,
			layer will render using the identity matrix as view/proj.
	
	@overload
	
		@in		MOAIViewLayer self
		@opt	MOAICamera camera		Default value is nil.
		@out	nil
	
	@overload
	
		@in		MOAIViewLayer self
		@opt	MOAICamera2D camera		Default value is nil.
		@out	nil
*/
mrb_value MOAIViewLayer::_setCamera ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewLayer, "U" )

	self->mCamera.Set ( *self, state.GetRubyObject < MOAICamera >( 1, true ));

	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIViewLayer::_setDebugCamera ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewLayer, "U" )

	self->mDebugCamera.Set ( *self, state.GetRubyObject < MOAICamera >( 1, true ));

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setParallax
	@text	Sets the parallax scale for this layer. This is simply a
			scalar applied to the view transform before rendering.
	
	@in		MOAIViewLayer self
	@opt	number xParallax	Default value is 1.
	@opt	number yParallax	Default value is 1.
	@opt	number zParallax	Default value is 1.
	@out	nil
*/
mrb_value MOAIViewLayer::_setParallax ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewLayer, "U" )

	self->mParallax.mX = state.GetParamValue < float >( 1, 1.0f );
	self->mParallax.mY = state.GetParamValue < float >( 2, 1.0f );
	self->mParallax.mZ = state.GetParamValue < float >( 3, 1.0f );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setViewport
	@text	Set the layer's viewport.
	
	@in		MOAIViewLayer self
	@in		MOAIViewport viewport
	@out	nil
*/
mrb_value MOAIViewLayer::_setViewport ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewLayer, "UU" )

	self->mViewport.Set ( *self, state.GetRubyObject < MOAIViewport >( 1, true ));

	return context;
}

//----------------------------------------------------------------//
/**	@lua	showDebugLines
	@text	Display debug lines for props in this layer.
	
	@in		MOAIViewLayer self
	@opt	boolean showDebugLines		Default value is 'true'.
	@out	nil
*/
mrb_value	MOAIViewLayer::_showDebugLines ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewLayer, "U" )
	
	self->mShowDebugLines = state.GetParamValue < bool >( 1, true );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	wndToWorld
	@text	Project a point from window space into world space.
	
	@in		MOAIViewLayer self
	@in		number x
	@in		number y
	@out	number x
	@out	number y
	@out	number z
*/
mrb_value MOAIViewLayer::_wndToWorld ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewLayer, "UNN" )

	ZLMatrix4x4 worldToWnd = self->GetWorldToWndMtx ();

	ZLMatrix4x4 wndToWorld = worldToWnd;
	wndToWorld.Inverse ();

	ZLVec4D loc;
	loc.mX = state.GetParamValue < float >( 1, 0.0f );
	loc.mY = state.GetParamValue < float >( 2, 0.0f );
	loc.mZ = worldToWnd.m [ ZLMatrix4x4::C3_R2 ] / worldToWnd.m [ ZLMatrix4x4::C3_R3 ];
	loc.mW = 1.0f;

	if ( self->mCamera && ( self->mCamera->GetType () == MOAICamera::CAMERA_TYPE_3D )) {
		wndToWorld.Project ( loc );
	}
	else {
		wndToWorld.Transform ( loc );
	}

	mrb_value ret [ 3 ];
	ret [ 0 ] = state.ToRValue ( loc.mX );
	ret [ 1 ] = state.ToRValue ( loc.mY );
	ret [ 2 ] = state.ToRValue ( loc.mZ );

	return mrb_ary_new_from_values ( state, 3, ret );
}

//----------------------------------------------------------------//
/**	@lua	wndToWorldRay
	@text	Project a point from window space into world space and return
			a normal vector representing a ray cast from the point into
			the world away from the camera (suitable for 3D picking).
	
	@in		MOAIViewLayer self
	@in		number x
	@in		number y
	@in		number d	If non-zero, scale normal by dist to plane d units away from camera. Default is zero.
	@out	number x
	@out	number y
	@out	number z
	@out	number xn
	@out	number yn
	@out	number zn
*/
mrb_value MOAIViewLayer::_wndToWorldRay ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewLayer, "UNN" )

	if ( self->mCamera ) {
		self->mCamera->ForceUpdate ();
	}

	ZLMatrix4x4 wndToWorld = self->GetWndToWorldMtx ();

	ZLVec4D loc;
	loc.mX = state.GetParamValue < float >( 1, 0.0f );
	loc.mY = state.GetParamValue < float >( 2, 0.0f );
	loc.mZ = 0.0f;
	loc.mW = 1.0f;

	float d = state.GetParamValue < float >( 3, 0.0f );

	ZLVec4D origin;

	if ( self->mCamera  && ( self->mCamera->GetType () == MOAICamera::CAMERA_TYPE_3D )) {
		const ZLAffine3D& localToWorldMtx = self->mCamera->GetLocalToWorldMtx ();
		ZLVec3D cameraLoc = localToWorldMtx.GetTranslation ();
		origin.mX = cameraLoc.mX;
		origin.mY = cameraLoc.mY;
		origin.mZ = cameraLoc.mZ;
	}
	else {
		origin = loc;
		wndToWorld.Project ( origin );
	}

	mrb_value ret [ 6 ];
	ret [ 0 ] = state.ToRValue ( origin.mX );
	ret [ 1 ] = state.ToRValue ( origin.mY );
	ret [ 2 ] = state.ToRValue ( origin.mZ );

	ZLVec3D norm;

	if ( self->mCamera  && ( self->mCamera->GetType () == MOAICamera::CAMERA_TYPE_3D )) {
	
		wndToWorld.Project ( loc );
	
		norm.mX = loc.mX - origin.mX;
		norm.mY = loc.mY - origin.mY;
		norm.mZ = loc.mZ - origin.mZ;
		norm.Norm ();
	}
	else {
		
		norm.mX = 0.0f;
		norm.mY = 0.0f;
		norm.mZ = -1.0f;
	}

	float ns = 1.0f;
	
	if ( d != 0.0f ) {
	
		if ( self->mCamera  && ( self->mCamera->GetType () == MOAICamera::CAMERA_TYPE_3D )) {
			const ZLAffine3D& localToWorldMtx = self->mCamera->GetLocalToWorldMtx ();
			ZLVec3D zAxis = localToWorldMtx.GetZAxis ();
			ns = -( d * zAxis.Dot ( norm ));
		}
		else {
			ns = d;
		}
	}
	
	ret [ 3 ] = state.ToRValue ( norm.mX * ns );
	ret [ 4 ] = state.ToRValue ( norm.mY * ns );
	ret [ 5 ] = state.ToRValue ( norm.mZ * ns );

	return mrb_ary_new_from_values ( state, 6, ret );
}

//----------------------------------------------------------------//
/**	@lua	worldToWnd
	@text	Transform a point from world space to window space.
	
	@in		MOAIViewLayer self
	@in		number x
	@in		number y
	@in		number Z
	@out	number x
	@out	number y
	@out	number z
*/
mrb_value MOAIViewLayer::_worldToWnd ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewLayer, "UNN" )

	ZLVec4D loc;
	loc.mX = state.GetParamValue < float >( 1, 0.0f );
	loc.mY = state.GetParamValue < float >( 2, 0.0f );
	loc.mZ = state.GetParamValue < float >( 3, 0.0f );
	loc.mW = 1.0f;

	ZLMatrix4x4 worldToWnd = self->GetWorldToWndMtx ();
	worldToWnd.Project ( loc );

	mrb_value ret [ 3 ];
	ret [ 0 ] = state.ToRValue ( loc.mX );
	ret [ 1 ] = state.ToRValue ( loc.mY );
	ret [ 2 ] = state.ToRValue ( loc.mZ );

	return mrb_ary_new_from_values ( state, 3, ret );
}

//================================================================//
// MOAIViewLayer
//================================================================//

//----------------------------------------------------------------//
float MOAIViewLayer::GetFitting ( ZLRect& worldRect, float hPad, float vPad ) {

	if ( !( this->mCamera && this->mViewport )) return 1.0f;

	ZLRect viewRect = this->mViewport->GetRect ();
	
	float hFit = ( viewRect.Width () - ( hPad * 2 )) / worldRect.Width ();
	float vFit = ( viewRect.Height () - ( vPad * 2 )) / worldRect.Height ();
	
	return ( hFit < vFit ) ? hFit : vFit;
}

//----------------------------------------------------------------//
MOAIRubyClass* MOAIViewLayer::GetSuperRubyClass () {

	return MOAIRubyObject::GetRubyClass ();
}

//----------------------------------------------------------------//
ZLMatrix4x4 MOAIViewLayer::GetWndToWorldMtx () const {

	return MOAIViewProj::GetWndToWorldMtx ( this->mViewport, this->mCamera, this->mLocalToWorldMtx, this->mParallax );
}

//----------------------------------------------------------------//
ZLMatrix4x4 MOAIViewLayer::GetWorldToWndMtx () const {

	return MOAIViewProj::GetWorldToWndMtx ( this->mViewport, this->mCamera, this->mLocalToWorldMtx, this->mParallax );
}

//----------------------------------------------------------------//
MOAIViewLayer::MOAIViewLayer () :
	mParallax ( 1.0f, 1.0f, 1.0f ),
	mShowDebugLines ( true ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIGraphicsProp )
		RTTI_EXTEND ( MOAILayer )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIViewLayer::~MOAIViewLayer () {

	this->mCamera.Set ( *this, 0 );
	this->mDebugCamera.Set ( *this, 0 );
	this->mViewport.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAIViewLayer::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIGraphicsProp::RegisterRubyClass ( state, klass );
	MOAILayer::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIViewLayer::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIGraphicsProp::RegisterRubyFuncs ( state, klass );
	MOAILayer::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getCamera", _getCamera, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getFitting", _getFitting, MRB_ARGS_ARG ( 4, 2 ) );
	state.DefineInstanceMethod ( klass, "getFitting3D", _getFitting3D, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getViewport", _getViewport, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setDebugCamera", _setDebugCamera, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setCamera", _setCamera, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setParallax", _setParallax, MRB_ARGS_ARG ( 0, 3 ) );
	state.DefineInstanceMethod ( klass, "setViewport", _setViewport, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "showDebugLines", _showDebugLines, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "wndToWorld", _wndToWorld, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "wndToWorldRay", _wndToWorldRay, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "worldToWnd", _worldToWnd, MRB_ARGS_REQ ( 3 ) );
	
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAIViewLayer::MOAIDrawable_Draw ( int subPrimID ) {
	UNUSED ( subPrimID );
    
   	if ( !this->IsVisible ()) return;
	if ( !this->mViewport ) return;
	if ( this->IsClear ()) return;
	
	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	gfxState.SetFrameBuffer ( this->GetFrameBuffer ());
	
	MOAIViewport& viewport = *this->mViewport;
	ZLRect viewportRect = viewport;

	ZLMatrix4x4 mtx ( this->mLocalToWorldMtx );
	mtx.Transform ( viewportRect );

	gfxState.SetViewRect ( viewportRect );
	gfxState.SetScissorRect ( viewportRect );
	
	this->ClearSurface ();
	
	gfxState.SetViewProj ( this->mViewport, this->mCamera, this->mDebugCamera, this->mParallax );
	gfxState.SetMtx ( MOAIGfxState::MODEL_TO_WORLD_MTX );
	
	// set up the ambient color
	gfxState.SetAmbientColor ( this->mColor );
	
	this->MOAIViewLayer_Draw ();
	
	if ( MOAIDebugLinesMgr::Get ().IsVisible () && this->mShowDebugLines ) {
		if ( this->mCamera ) {
			this->mCamera->DrawDebug ();
		}
	}
	gfxState.SetFrameBuffer ( this->GetFrameBuffer ());
}

//----------------------------------------------------------------//
ZLBounds MOAIViewLayer::MOAIPartitionHull_GetModelBounds () {
	
	if ( this->mViewport ) {
		ZLBounds bounds;
		bounds.Init ( this->mViewport->GetRect ());
		return bounds;
	}
	return ZLBounds::EMPTY;
}
