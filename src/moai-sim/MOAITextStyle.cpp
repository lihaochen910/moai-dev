// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIAnimCurve.h>
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIDebugLines.h>
#include <moai-sim/MOAIFont.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAINodeMgr.h>
#include <moai-sim/MOAIShader.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAITextStyle.h>

//================================================================//
// MOAITextStyleState
//================================================================//

//----------------------------------------------------------------//
void MOAITextStyleState::AffirmGlyph ( u32 c ) {

	assert ( this->mFont );
	this->mFont->AffirmGlyph ( this->mSize, c );
}

//----------------------------------------------------------------//
void MOAITextStyleState::Init ( MOAITextStyleState& style ) {

	*this = style;
}

//----------------------------------------------------------------//
MOAITextStyleState::MOAITextStyleState () :
	mFont ( 0 ),
	mShader ( 0 ),
	mSize ( 0.0f ),
	mScale ( 1.0f, 1.0f ),
	mColor ( 0xffffffff ) {
	
	this->mPadding.Init ( 0.0f, 0.0f, 0.0f, 0.0f );
}

//----------------------------------------------------------------//
MOAITextStyleState::~MOAITextStyleState () {
}

//----------------------------------------------------------------//
bool MOAITextStyleState::NeedsLayout ( const MOAITextStyleState& compare ) const {

	if ( this->mFont != compare.mFont ) return true;
	if ( this->mSize != compare.mSize ) return true;
	if ( !this->mScale.Equals ( compare.mScale )) return true;
	
	return false;
}

//----------------------------------------------------------------//
void MOAITextStyleState::SetFont ( MOAIFont* font ) {

	this->mFont = font;
}

//----------------------------------------------------------------//
void MOAITextStyleState::SetShader ( MOAIShader* shader ) {

	this->mShader = shader;
}

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getColor
	@text	Gets the color of the style.
	
	@in		MOAITextStyle self
	@out	number r
	@out	number g
	@out	number b
	@out	number a
*/
mrb_value MOAITextStyle::_getColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextStyle, "U" )

	ZLColorVec color = ZLColor::Set ( self->mColor );

	mrb_value ret [ 4 ];
	ret [ 0 ] = state.ToRValue ( color.mR );
	ret [ 1 ] = state.ToRValue ( color.mG );
	ret [ 2 ] = state.ToRValue ( color.mB );
	ret [ 3 ] = state.ToRValue ( color.mA );

	return mrb_ary_new_from_values ( state, 4, ret );
}

//----------------------------------------------------------------//
/**	@lua	getFont
	@text	Gets the font of the style.
	
	@in		MOAITextStyle self
	@out	MOAIFont font
*/
mrb_value MOAITextStyle::_getFont ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextStyle, "U" )

	MOAIFont* font = self->GetFont ();
	if ( font ) {
		return font->PushRubyUserdata ( state );
	}

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getScale
	@text	Gets the scale of the style.
	
	@in		MOAITextStyle self
	@out	number scale
*/
mrb_value MOAITextStyle::_getScale ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextStyle, "U" )

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( self->mScale.mX );
	ret [ 1 ] = state.ToRValue ( self->mScale.mY );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getSize
	@text	Gets the size of the style.
	
	@in		MOAITextStyle self
	@out	number size
*/
mrb_value MOAITextStyle::_getSize ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextStyle, "U" )
	return state.ToRValue ( self->mSize );
}

//----------------------------------------------------------------//
/**	@lua	setColor
	@text	Initialize the style's color.
	
	@in		MOAITextStyle self
	@in		number r	Default value is 0.
	@in		number g	Default value is 0.
	@in		number b	Default value is 0.
	@opt	number a	Default value is 1.
	@out	nil
*/
mrb_value MOAITextStyle::_setColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextStyle, "UNNN" )
	self->mColor = state.GetColor32 ( 1, 0.0f, 0.0f, 0.0f, 1.0f );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setFont
	@text	Sets or clears the style's font.
	
	@in		MOAITextStyle self
	@opt	MOAIFont font		Default value is nil.
	@out	nil
*/
mrb_value MOAITextStyle::_setFont ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextStyle, "U" )
	MOAIFont* font = state.GetRubyObject < MOAIFont >( 1, true );
	
	self->SetFont ( font );
	self->ScheduleUpdate ();

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setShader
	@text	Sets the shader for the text style to use.
	
	@in		MOAITextStyle self
	@opt	variant shader			Shader or shader preset.
	@out	MOAIShader shader		The shader that was set or created.
*/
mrb_value MOAITextStyle::_setShader ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextStyle, "U" )
	
	MOAIShader* shader = MOAIShader::AffirmShader ( state, 1 );
	self->SetShader ( shader );
	return state.ToRValue < MOAIRubyObject* >( shader );
}

//----------------------------------------------------------------//
/**	@lua	setPadding
	@text	Add padding to glyphs during layout. Padding does not affect
			placement of glyphs. Padding only causes more of the glyph
			and its underlying texture to be shown.
	
	@overload
	
		@in		MOAITextStyle self
		@in		number hPad		glyph xMin -= hPad * 0.5, glyph xMax += hPad * 0.5
		@in		number vPad		glyph yMin -= vPad * 0.5, glyph yMax += vPad * 0.5
		@out	nil

	@overload
	
		@in		MOAITextStyle self
		@in		xMinP			glyph xMin -= xMinP
		@in		yMinP			glyph yMin -= yMinP
		@in		xMaxP			glyph xMax += xMaxP
		@in		yMaxP			glyph yMax += yMaxP
		@out	nil
*/
mrb_value MOAITextStyle::_setPadding ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextStyle, "U" )
	
	// TODO: pasted from MOAIDynamicGlyphCache; find some central place for this
	
	if ( state.CheckParams ( 1, "NNNN", false )) {
	
		self->mPadding.mXMin = state.GetParamValue < float >( 1, 0.0f );
		self->mPadding.mYMin = state.GetParamValue < float >( 2, 0.0f );
		
		self->mPadding.mXMax = state.GetParamValue < float >( 3, 0.0f );
		self->mPadding.mYMax = state.GetParamValue < float >( 4, 0.0f );
	}
	else {
	
		float hPad = state.GetParamValue < float >( 1, 0.0f );
		float vPad = state.GetParamValue < float >( 2, hPad );
	
		hPad *= 0.5f;
		vPad *= 0.5f;
		
		self->mPadding.mXMin = -hPad;
		self->mPadding.mYMin = -vPad;
		
		self->mPadding.mXMax = hPad;
		self->mPadding.mYMax = vPad;
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setScale
	@text	Sets the scale of the style. The scale is applied to
			any glyphs drawn using the style after the glyph set
			has been selected by size.
	
	@in		MOAITextStyle self
	@opt	number scale		Default value is 1.
	@out	nil
*/
mrb_value MOAITextStyle::_setScale ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextStyle, "U" )
	self->mScale.mX = state.GetParamValue < float >( 1, 1.0f );
	self->mScale.mY = state.GetParamValue < float >( 2, self->mScale.mX );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setSize
	@text	Sets or clears the style's size.
	
	@in		MOAITextStyle self
	@in		number points			The point size to be used by the style.
	@opt	number dpi				The device DPI (dots per inch of device screen). Default value is 72 (points same as pixels).
	@out	nil
*/
mrb_value MOAITextStyle::_setSize ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextStyle, "UN" )
	
	float points	= state.GetParamValue < float >( 1, 0.0f );
	float dpi		= state.GetParamValue < float >( 2, DPI );
	
	self->SetSize ( POINTS_TO_PIXELS ( points, dpi ));
	self->ScheduleUpdate ();
	
	return mrb_nil_value ();
}

//================================================================//
// MOAITextStyle
//================================================================//

//----------------------------------------------------------------//
void MOAITextStyle::Init ( MOAITextStyleState& style ) {
	
	// do this first!!
	this->SetFont ( style.mFont );
	this->SetShader ( style.mShader );
	
	MOAITextStyleState::Init ( style );
}

//----------------------------------------------------------------//
MOAITextStyle::MOAITextStyle () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAINode )
	RTTI_END
}

//----------------------------------------------------------------//
MOAITextStyle::~MOAITextStyle () {

	this->SetFont ( 0 );
}

//----------------------------------------------------------------//
void MOAITextStyle::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAITextStyle::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	
	state.DefineInstanceMethod ( klass, "getColor",				_getColor, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getFont",				_getFont, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getScale",				_getScale, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getSize",				_getSize, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setColor",				_setColor, MRB_ARGS_ARG ( 3, 1 ) );
	state.DefineInstanceMethod ( klass, "setFont",				_setFont, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setPadding",			_setPadding, MRB_ARGS_ARG ( 2, 2 ) );
	state.DefineInstanceMethod ( klass, "setScale",				_setScale, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "setShader",			_setShader, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setSize",				_setSize, MRB_ARGS_ARG ( 1, 1 ) );
}

//----------------------------------------------------------------//
void MOAITextStyle::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAITextStyle::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAITextStyle::SetFont ( MOAIFont* font ) {

	if ( this->mFont != font ) {
	
		this->RubyRetain ( font );
		this->RubyRelease ( this->mFont );
		this->mFont = font;
		
		if ( font && ( this->mSize == 0.0f )) {
			this->mSize = font->GetDefaultSize ();
		}
	}
}

//----------------------------------------------------------------//
void MOAITextStyle::SetShader ( MOAIShader* shader ) {

	if ( this->mShader != shader ) {
	
		this->RubyRetain ( shader );
		this->RubyRelease ( this->mShader );
		this->mShader = shader;
	}
}
