// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIFontReader.h>

//================================================================//
// MOAIGlyphMetrics
//================================================================//

//----------------------------------------------------------------//
/**
 * Get the rect of the glyph which includes the bearing + the size of the bounding box of the glyph.
 * 
 * @param x The x pen position when drawing this glyph
 * @param y The y pen position when drawing this glyph
 * @param scale The scale at which the glyph would be drawn
 */
ZLRect MOAIGlyphMetrics::GetGlyphRect ( float x, float y, float xScale, float yScale ) const {

	ZLRect rect;

	x += ( this->mBearingX * xScale );
	y -= ( this->mBearingY * yScale );

	rect.Init (
		x,
		y,
		x + ( this->mWidth * xScale ),
		y + ( this->mHeight * yScale )
	);

	return rect;
}

//----------------------------------------------------------------//
ZLRect MOAIGlyphMetrics::GetLogicalRect ( float x, float y, float ascent, float descent, float xScale, float yScale ) const {

	ZLRect rect;

	x += ( this->mBearingX * xScale );

	rect.Init (
		x,
		y - ( ascent * yScale ),
		x + ( this->mWidth * xScale ),
		y + ( descent * yScale )
	);

	return rect;
}

//----------------------------------------------------------------//
MOAIGlyphMetrics::MOAIGlyphMetrics () :
	mAdvanceX ( 0.0f ),
	mBearingX ( 0.0f ),
	mBearingY ( 0.0f ),
	mWidth ( 0.0f ),
	mHeight ( 0.0f ) {
};

//================================================================//
// MOAIFontFaceMetrics
//================================================================//

//----------------------------------------------------------------//
MOAIFontFaceMetrics::MOAIFontFaceMetrics () :
	mHeight ( 0.0f ),
	mAscent ( 0.0f ) {
};

//================================================================//
// MOAIFontReader lua
//================================================================//

//----------------------------------------------------------------//
mrb_value MOAIFontReader::_close ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFontReader, "U" )
	u32 result = self->CloseFontFile ();
	return state.ToRValue ( result != MOAIFontReader::OK );
}

//----------------------------------------------------------------//
mrb_value MOAIFontReader::_getFaceMetrics ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFontReader, "U" )
	
	MOAIFontFaceMetrics faceMetrics;
	int result = self->GetFaceMetrics ( faceMetrics );
	
	if ( result == MOAIFontReader::OK ) {
		mrb_value ret [ 2 ];
		ret [ 0 ] = state.ToRValue ( faceMetrics.mHeight );
		ret [ 1 ] = state.ToRValue ( faceMetrics.mAscent );

		return mrb_ary_new_from_values ( state, 2, ret );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIFontReader::_getGlyphMetrics ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFontReader, "U" )
	
	MOAIGlyphMetrics glyphMetrics;
	int result = self->GetGlyphMetrics ( glyphMetrics );
	
	if ( result == MOAIFontReader::OK ) {
		mrb_value ret [ 5 ];
		ret [ 0 ] = state.ToRValue ( glyphMetrics.mWidth );
		ret [ 1 ] = state.ToRValue ( glyphMetrics.mHeight );
		ret [ 2 ] = state.ToRValue ( glyphMetrics.mAdvanceX );
		ret [ 3 ] = state.ToRValue ( glyphMetrics.mBearingX );
		ret [ 4 ] = state.ToRValue ( glyphMetrics.mBearingY );

		return mrb_ary_new_from_values ( state, 5, ret );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIFontReader::_getKernVec ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFontReader, "UN" )
	
	u32 c = state.GetParamValue < u32 >( 1, MOAIFontReader::GLYPH_CODE_NULL );
	
	MOAIKernVec kernVec;
	int result = self->GetKernVec ( c, kernVec );
	
	if ( result == MOAIFontReader::OK ) {
		mrb_value ret [ 2 ];
		ret [ 0 ] = state.ToRValue ( kernVec.mX );
		ret [ 1 ] = state.ToRValue ( kernVec.mY );

		return mrb_ary_new_from_values ( state, 2, ret );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIFontReader::_hasKerning ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFontReader, "U" )
	return state.ToRValue ( self->HasKerning () );
}

//----------------------------------------------------------------//
mrb_value MOAIFontReader::_open ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFontReader, "US" )
	u32 result = self->OpenFontFile ( state.GetParamValue < cc8* >( 1, NULL ));
	return state.ToRValue ( result != MOAIFontReader::OK );
}

//----------------------------------------------------------------//
mrb_value MOAIFontReader::_renderGlyph ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFontReader, "UU" )
	
	int result = MOAIFontReader::FONT_ERROR;
	MOAIImage* image = state.GetRubyObject < MOAIImage >( 1, true );
	
	if ( image ) {
	
		float x = state.GetParamValue < float >( 2, 0.0f );
		float y = state.GetParamValue < float >( 3, 0.0f );

		result = self->RenderGlyph ( *image, x, y );
	}
	return state.ToRValue ( result != MOAIFontReader::OK );
}

//----------------------------------------------------------------//
mrb_value MOAIFontReader::_selectFace ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFontReader, "U" )
	u32 result = self->SelectFace ( state.GetParamValue < float >( 1, 0.0f ));
	return state.ToRValue ( result != MOAIFontReader::OK );
}

//----------------------------------------------------------------//
mrb_value MOAIFontReader::_selectGlyph ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFontReader, "UN" )
	u32 result = self->SelectGlyph ( state.GetParamValue < u32 >( 1, MOAIFontReader::GLYPH_CODE_NULL ));
	return state.ToRValue ( result != MOAIFontReader::OK );
}

//----------------------------------------------------------------//
mrb_value MOAIFontReader::_setBlendMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFontReader, "U" )
	
	self->mBlendMode.Init ( state, 1 );
	return context;
}

//================================================================//
// MOAIFontReader
//================================================================//

//----------------------------------------------------------------//
int MOAIFontReader::CloseFontFile () {

	return OK;
}

//----------------------------------------------------------------//
bool MOAIFontReader::HasKerning () {

	return false;
}

//----------------------------------------------------------------//
int MOAIFontReader::GetKernVec ( u32 c, MOAIKernVec& kernVec ) {
	UNUSED ( c );
	UNUSED ( kernVec );

	kernVec.mX = 0.0f;
	kernVec.mY = 0.0f;

	return NO_KERNING;
}

//----------------------------------------------------------------//
MOAIFontReader::MOAIFontReader () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIFontReader::~MOAIFontReader () {

	this->CloseFontFile ();
}

//----------------------------------------------------------------//
int MOAIFontReader::OpenFontFile ( cc8* filename ) {
	UNUSED ( filename );
	return OK;
}

//----------------------------------------------------------------//
void MOAIFontReader::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIFontReader::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "close", _close, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getFaceMetrics", _getFaceMetrics, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getGlyphMetrics", _getGlyphMetrics, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getKernVec", _getKernVec, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "hasKerning", _hasKerning, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "open", _open, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "renderGlyph", _renderGlyph, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "selectFace", _selectFace, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "selectGlyph", _selectGlyph, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setBlendMode", _setBlendMode, MRB_ARGS_NONE () );
	
}

//----------------------------------------------------------------//
void MOAIFontReader::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAIFontReader::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}
