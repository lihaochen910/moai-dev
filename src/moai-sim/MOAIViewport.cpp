// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIViewport.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIViewport::_getFrame ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewport, "U" )
	
	mrb_value ret [ 4 ];
	ret [ 0 ] = state.ToRValue ( self->mXMin );
	ret [ 1 ] = state.ToRValue ( self->mYMin );
	ret [ 2 ] = state.ToRValue ( self->mXMax );
	ret [ 3 ] = state.ToRValue ( self->mYMax );

	return mrb_ary_new_from_values ( state, 4, ret );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIViewport::_getSize ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewport, "U" )
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( self->Width () );
	ret [ 1 ] = state.ToRValue ( self->Height () );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	setOffset
	@text	Sets the viewport offset in normalized view space (size of
			viewport is -1 to 1 in both directions).
	
	@in		MOAIViewport self
	@in		number xOff
	@in		number yOff
	@out	nil
*/
mrb_value MOAIViewport::_setOffset ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewport, "UNN" )
	
	float xOffset = state.GetParamValue < float >( 1, 0.0f );
	float yOffset = state.GetParamValue < float >( 2, 0.0f );

	self->SetOffset ( xOffset, yOffset );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setRotation
	@text	Sets global rotation to be added to camera transform.
	
	@in		MOAIViewport self
	@in		number rotation
	@out	nil
*/
mrb_value MOAIViewport::_setRotation ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewport, "U" )
	
	float rotation = state.GetParamValue < float >( 1, 0.0f );
	self->SetRotation ( rotation );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setScale
	@text	Sets the number of world units visible of the viewport for one or both
			dimensions. Set 0 for one of the dimensions to use a derived value based on
			the other dimension and the aspect ratio. Negative values are also OK.
			
			It is typical to set the scale to the number of pixels visible in the
			this-> This practice is neither endorsed nor condemned.
			
			Note that the while the contents of the viewport will appear to stretch
			or shrink to match the dimensions of the viewport given by setSize, the
			number of world units visible will remain constant.
	
	@in		MOAIViewport self
	@in		number xScale
	@in		number yScale
	@out	nil
*/
mrb_value MOAIViewport::_setScale ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewport, "U" )

	if ( !state.CheckParams ( 1, "NN" )) return context;
	
	float xScale = state.GetParamValue < float >( 1, 0.0f );
	float yScale = state.GetParamValue < float >( 2, 0.0f );
	
	self->SetScale ( xScale, yScale );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setSize
	@text	Sets the dimensions of the this->
	
	
	@overload
	
		@in		MOAIViewport self
		@in		number width
		@in		number height
		@out	nil
		
	@overload
	
		@in		MOAIViewport self
		@in		number left
		@in		number top
		@in		number right
		@in		number bottom
		@out	nil
*/
mrb_value MOAIViewport::_setSize ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIViewport, "U" )

	if ( !state.CheckParams ( 1, "NN" )) return context;
	
	float x0 = state.GetParamValue < float >( 1, 0.0f );
	float y0 = state.GetParamValue < float >( 2, 0.0f );

	if ( state.CheckParams ( 3, "NN", false )) {
	
		float x1 = state.GetParamValue < float >( 3, 0.0f );
		float y1 = state.GetParamValue < float >( 4, 0.0f );
		
		self->Init ( x0, y0, x1, y1 );
	}
	else {
		self->Init ( 0.0f, 0.0f, x0, y0 );
	}

	return context;
}

//================================================================//
// MOAIViewport
//================================================================//

//----------------------------------------------------------------//
float MOAIViewport::GetAspect () const {
	return this->Width () / this->Height ();
}

//----------------------------------------------------------------//
float MOAIViewport::GetInvAspect () const {
	return this->Height () / this->Width ();
}

//----------------------------------------------------------------//
ZLMatrix4x4 MOAIViewport::GetNormToWndMtx () const {

	ZLMatrix4x4 normToWnd;

	ZLMatrix4x4 mtx;
	ZLRect rect = this->GetRect ();

	float hWidth = rect.Width () * 0.5f;
	float hHeight = rect.Height () * 0.5f;

	// Wnd
	normToWnd.Scale ( hWidth, -hHeight, 1.0f );
	
	mtx.Translate ( hWidth + rect.mXMin, hHeight + rect.mYMin, 0.0f );
	normToWnd.Append ( mtx );
	
	return normToWnd;
}

//----------------------------------------------------------------//
ZLMatrix4x4 MOAIViewport::GetProjMtx () const {

	ZLMatrix4x4 proj;

	ZLMatrix4x4 mtx;
	ZLRect rect = this->GetRect ();

	// rotate
	proj.RotateZ ( -this->mRotation * ( float )D2R );

	// project
	ZLVec2D viewScale = this->GetScale ();
	float xScale = ( 2.0f / rect.Width ()) * viewScale.mX;
	float yScale = ( 2.0f / rect.Height ()) * viewScale.mY;
	
	mtx.Scale ( xScale, yScale, -1.0f ); // Z must be non-zero to produce invertible projection matrix
	proj.Append ( mtx );
	
	// offset
	mtx.Translate ( this->mOffset.mX, this->mOffset.mY, 0.0f );
	proj.Append ( mtx );
	
	return proj;
}

//----------------------------------------------------------------//
ZLMatrix4x4 MOAIViewport::GetProjMtxInv () const {

	ZLMatrix4x4 projInv;

	ZLMatrix4x4 mtx;
	ZLRect rect = this->GetRect ();

	// Inv Project
	
	// offset
	projInv.Translate ( -this->mOffset.mX, -this->mOffset.mY, 0.0f );
	
	// project
	ZLVec2D viewScale = this->GetScale ();
	float invXScale = 1.0f / (( 2.0f / rect.Width () * viewScale.mX ));
	float invYScale = 1.0f / (( 2.0f / rect.Height () * viewScale.mY ));
	
	mtx.Scale ( invXScale, invYScale, -1.0f );
	projInv.Append ( mtx );
	
	// rotate
	mtx.RotateZ ( this->mRotation * ( float )D2R );
	projInv.Append ( mtx );
	
	return projInv;
}

//----------------------------------------------------------------//
ZLRect MOAIViewport::GetRect () const {

	return *this;
}

//----------------------------------------------------------------//
ZLVec2D MOAIViewport::GetScale () const {

	ZLVec2D scale ( 1.0f, 1.0f );
	
	if ( this->mXScale && this->mYScale ) {
		scale.mX = this->Width () / this->mScale.mX;
		scale.mY = this->Height () / this->mScale.mY;
	}
	else {
		
		float result = 1.0f;
		
		if ( this->mXScale ) {
			result = (( float )this->Width ()) / this->mScale.mX;
		}
		
		if ( this->mYScale ) {
			result = (( float )this->Height ()) / this->mScale.mY;
		}
		
		scale.mX = result;
		scale.mY = result;
	}
	
	return scale;
}

//----------------------------------------------------------------//
ZLVec2D MOAIViewport::GetUnits () const {

	ZLVec2D scale ( 1.0f, 1.0f );
	
	if ( this->mXScale && this->mYScale ) {
		scale.mX = this->mScale.mX;
		scale.mY = this->mScale.mY;
	}
	else {
		
		float result = 1.0f;
		
		if ( this->mXScale ) {
			result = this->mScale.mX;
		}
		
		if ( this->mYScale ) {
			result = this->mScale.mY;
		}
		
		scale.mX = result;
		scale.mY = result;
	}
	
	return scale;
}

//----------------------------------------------------------------//
ZLMatrix4x4 MOAIViewport::GetWndToNormMtx () const {

	ZLMatrix4x4 wndToNorm;

	ZLMatrix4x4 mtx;
	ZLRect rect = this->GetRect ();

	float hWidth = rect.Width () * 0.5f;
	float hHeight = rect.Height () * 0.5f;

	// Inv Wnd
	wndToNorm.Translate ( -hWidth - rect.mXMin, -hHeight - rect.mYMin, 0.0f );
	
	mtx.Scale (( 1.0f / hWidth ), -( 1.0f / hHeight ), 1.0f );
	wndToNorm.Append ( mtx );
	
	return wndToNorm;
}

//----------------------------------------------------------------//
//ZLMatrix4x4 MOAIViewport::GetWndToWorldMtx ( const ZLMatrix4x4& view ) const {
//
//	ZLMatrix4x4 wndToWorld;
//
//	ZLMatrix4x4 mtx;
//	ZLRect rect = this->GetRect ();
//
//	float hWidth = rect.Width () * 0.5f;
//	float hHeight = rect.Height () * 0.5f;
//
//	// Inv Wnd
//	wndToWorld.Translate ( -hWidth - rect.mXMin, -hHeight - rect.mYMin, 0.0f );
//	
//	mtx.Scale (( 1.0f / hWidth ), -( 1.0f / hHeight ), 1.0f );
//	wndToWorld.Append ( mtx );
//
//	mtx = this->GetProjMtxInv ();
//	wndToWorld.Append ( mtx );
//	
//	mtx.Inverse ( view );
//	wndToWorld.Append ( mtx );
//	
//	return wndToWorld;
//}

//----------------------------------------------------------------//
//ZLMatrix4x4 MOAIViewport::GetWorldToWndMtx ( const ZLMatrix4x4& view ) const {
//
//	ZLMatrix4x4 worldToWnd;
//
//	ZLMatrix4x4 mtx;
//	ZLRect rect		= this->GetRect ();
//	
//	float hWidth	= this->Width () * 0.5f;
//	float hHeight	= this->Height () * 0.5f;
//
//	worldToWnd = view;
//
//	mtx = this->GetProjMtx ();
//	worldToWnd.Append ( mtx );
//
//	// Wnd
//	mtx.Scale ( hWidth, -hHeight, 1.0f );
//	worldToWnd.Append ( mtx );
//	
//	mtx.Translate ( hWidth + rect.mXMin, hHeight + rect.mYMin, 0.0f );
//	worldToWnd.Append ( mtx );
//	
//	return worldToWnd;
//}

//----------------------------------------------------------------//
MOAIViewport::MOAIViewport () :
	mXScale ( true ),
	mYScale ( true ),
	mScale ( 1.0f, 1.0f ),
	mOffset ( 0.0f, 0.0f ),
	mRotation ( 0.0f ) {
	
	RTTI_SINGLE ( MOAIRubyObject )
	
	this->Init ( 0.0f, 0.0f, 1.0f, 1.0f );
}

//----------------------------------------------------------------//
MOAIViewport::~MOAIViewport () {
}

//----------------------------------------------------------------//
void MOAIViewport::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIViewport::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "getFrame", _getFrame, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getSize", _getSize, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setOffset", _setOffset, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setRotation", _setRotation, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setScale", _setScale, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setSize", _setSize, MRB_ARGS_ANY () );

}

//----------------------------------------------------------------//
void MOAIViewport::SetOffset ( float xOffset, float yOffset ) {

	this->mOffset.Init ( xOffset, yOffset );
}

//----------------------------------------------------------------//
void MOAIViewport::SetRotation ( float degrees ) {

	this->mRotation = degrees;
}

//----------------------------------------------------------------//
void MOAIViewport::SetScale ( float xScale, float yScale ) {

	this->mXScale = ( xScale != 0.0f );
	this->mScale.mX = xScale;
	
	this->mYScale = ( yScale != 0.0f );
	this->mScale.mY = yScale;
}
