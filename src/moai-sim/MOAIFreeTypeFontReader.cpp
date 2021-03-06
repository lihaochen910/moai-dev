// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

SUPPRESS_EMPTY_FILE_WARNING

#if MOAI_WITH_FREETYPE

#include <moai-sim/MOAIFont.h>
#include <moai-sim/MOAIFreeTypeFontReader.h>
#include <moai-sim/MOAIGlyphCache.h>
#include <moai-sim/MOAIImageTexture.h>

#include <ft2build.h>

#include FT_FREETYPE_H
#include FT_STROKER_H

//================================================================//
// RenderParams
//================================================================//
class RenderParams {
public:

	MOAIImage*	mImage;
	
	MOAIImageBlendMode	mBlendMode;
	ZLColorVec			mPenColor;
	int					mPenX;
	int					mPenY;
};

//----------------------------------------------------------------//
static void _renderSpan ( const int y, const int count, const FT_Span* const spans, void* const user ) {

	if ( !user ) return;
	
	RenderParams* render = ( RenderParams* )user;

	int line = render->mPenY - y - 1;
	int offset = render->mPenX;

	MOAIImage* image = render->mImage;
	ZLColor::ColorFormat colorFormat = image->GetColorFormat ();
	MOAIImageBlendMode blendMode = render->mBlendMode;

	u32 penColor = render->mPenColor.PackRGBA ();

	for ( int i = 0; i < count; ++i ) {
		
		const FT_Span& span = spans [ i ];
		
		int x = offset + span.x;
		int len = span.len;
		
		u32 srcColor = ZLColor::Scale ( penColor, span.coverage );
		
		if ( blendMode.mEquation == ZLColor::BLEND_EQ_NONE ) {
			
			u32 pixel = ZLColor::ConvertFromRGBA ( srcColor, colorFormat );
			for ( int j = 0; j < len; ++j ) {
				render->mImage->SetPixel ( x + j, line, pixel );
			}
		}
		else {
			for ( int j = 0; j < len; ++j ) {
				u32 dstColor = render->mImage->GetColor ( x + j, line );
				u32 blendColor = ZLColor::Blend ( srcColor, dstColor, blendMode );
				u32 pixel = ZLColor::ConvertFromRGBA ( blendColor, colorFormat );
				render->mImage->SetPixel ( x + j, line, pixel );
			}
		}
	}
}

//================================================================//
// MOAIFreeTypeFontReader lua
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIFreeTypeFontReader::_enableAntiAliasing ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFreeTypeFontReader, "U" )
	self->mAntiAlias = state.GetParamValue < bool >( 1, false );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIFreeTypeFontReader::_extractSystemFont ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_CLASS ( "S" )

	#ifdef __APPLE__

		cc8* fontName = state.GetParamValue < cc8* >( 1, 0 );
		cc8* fileName = state.GetParamValue < cc8* >( 2, 0 );
		
		if ( fontName && fileName ) {
		
			ZLFileStream stream;
			stream.OpenWrite ( fileName );
			MOAIFreeTypeFontReader::ExtractSystemFont ( fontName, stream );
			stream.Close ();
		}
	#endif
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIFreeTypeFontReader::_setPenColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFreeTypeFontReader, "U" )
	self->mPenColor = state.GetColor ( 1, 1.0f, 1.0f, 1.0f, 1.0f );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIFreeTypeFontReader::_strokeGlyph ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIFreeTypeFontReader, "U" )

	int result = MOAIFontReader::FONT_ERROR;
	MOAIImage* image = state.GetRubyObject < MOAIImage >( 1, true );
	
	if ( image ) {
	
		float x				= state.GetParamValue < float >( 2, 0.0f );
		float y				= state.GetParamValue < float >( 3, 0.0f );
		float strokeSize	= state.GetParamValue < float >( 4, 0.0f );

		u32 capStyle		= state.GetParamValue < u32 >( 5, ( u32 )FT_STROKER_LINECAP_SQUARE );
		u32 joinStyle		= state.GetParamValue < u32 >( 6, ( u32 )FT_STROKER_LINEJOIN_MITER_VARIABLE );
		
		// PCM: this scalar is arbitrary. FT miter limit doesn't behave like I'd expect.
		// (I'd expect either a simple distance, or a ratio, like SVG uses.)
		// TODO: investigate FT's miter limit implementation and fix accordingly.
		float miterLimit	= state.GetParamValue < float >( 7, strokeSize * 100.0f );

		result = self->StrokeGlyph ( *image, x, y, strokeSize, capStyle, joinStyle, miterLimit );
	}
	return state.ToRValue ( result != MOAIFontReader::OK );
}

//================================================================//
// MOAIFreeTypeFontReader
//================================================================//

//----------------------------------------------------------------//
int MOAIFreeTypeFontReader::CloseFontFile () {

	if ( this->mFace ) {

		FT_Done_Face (( FT_Face )this->mFace );
		FT_Done_FreeType (( FT_Library )this->mLibrary );
	}
	
	this->mFace = 0;
	this->mLibrary = 0;
	
	this->mFaceSize = 0.0f;
	this->mGlyphCode = GLYPH_CODE_NULL;

	return ( int )OK;
}

#ifndef __APPLE__

	//----------------------------------------------------------------//
	void MOAIFreeTypeFontReader::ExtractSystemFont ( cc8* fontName, ZLStream& stream ) {
		UNUSED ( fontName );
		UNUSED ( stream );
	}

#endif

//----------------------------------------------------------------//
int MOAIFreeTypeFontReader::GetFaceMetrics ( MOAIFontFaceMetrics& faceMetrics ) {

	FT_Face face = ( FT_Face )this->mFace;

	if ( !face ) return FONT_ERROR;

	faceMetrics.mAscent = ( float )( face->size->metrics.ascender >> 6 ); // div 64
	faceMetrics.mHeight = ( float )( face->size->metrics.height >> 6 ); // div 64
	
	return OK;
}

//----------------------------------------------------------------//
int MOAIFreeTypeFontReader::GetGlyphMetrics ( MOAIGlyphMetrics& glyphMetrics ) {

	FT_Face face = ( FT_Face )this->mFace;

	if ( !face ) return FONT_ERROR;
	if ( this->mGlyphCode == GLYPH_CODE_NULL ) return FONT_ERROR;

	glyphMetrics.mWidth			= ( float )( face->glyph->metrics.width >> 6 ); // div 64
	glyphMetrics.mHeight		= ( float )( face->glyph->metrics.height >> 6 ); // div 64
	glyphMetrics.mAdvanceX		= ( float )( face->glyph->metrics.horiAdvance >> 6 ); // div 64
	glyphMetrics.mBearingX		= ( float )( face->glyph->metrics.horiBearingX >> 6 ); // div 64
	glyphMetrics.mBearingY		= ( float )( face->glyph->metrics.horiBearingY >> 6 ); // div 64
	
	return OK;
}

//----------------------------------------------------------------//
int MOAIFreeTypeFontReader::GetKernVec ( u32 c, MOAIKernVec& kernVec ) {

	kernVec.mX = 0.0f;
	kernVec.mY = 0.0f;

	FT_Vector  delta;
	FT_Get_Kerning (( FT_Face )this->mFace, this->mGlyphCode, c, FT_KERNING_DEFAULT, &delta );
		
	if ( delta.x || delta.y ) {
		kernVec.mName = c;
		kernVec.mX = ( float )( delta.x >> 6 );
		kernVec.mY = ( float )( delta.y >> 6 );
		return OK;
	}
	return NO_KERNING;
}

//----------------------------------------------------------------//
bool MOAIFreeTypeFontReader::HasKerning () {

	FT_Face face = ( FT_Face )this->mFace;
	return ( FT_HAS_KERNING ( face ) != 0 );
}

//----------------------------------------------------------------//
MOAIFreeTypeFontReader::MOAIFreeTypeFontReader () :
	mGlyphCode ( GLYPH_CODE_NULL ),
	mFaceSize ( 0.0f ),
	mLibrary ( 0 ),
	mFace ( 0 ),
	mFaceHeight ( 0.0f ),
	mAntiAlias ( true ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIFontReader )
	RTTI_END
	
	this->mPenColor.Set ( 1.0f, 1.0f, 1.0f, 1.0f );
}

//----------------------------------------------------------------//
MOAIFreeTypeFontReader::~MOAIFreeTypeFontReader () {
}

//----------------------------------------------------------------//
int MOAIFreeTypeFontReader::OpenFontFile ( cc8* filename ) {

	this->CloseFontFile ();

	FT_Init_FreeType ( ( FT_Library* )&this->mLibrary );

	if ( FT_New_Face (( FT_Library )this->mLibrary, filename, 0, ( FT_Face* )&this->mFace )) {
		FT_Done_FreeType (( FT_Library )this->mLibrary );

		this->mFace = 0;
		this->mLibrary = 0;

		ZLLog_ErrorF ( ZLLog::CONSOLE, "FONT_ERROR loading font: %s\n", filename );
		return FONT_ERROR;
	}
	return OK;
}

//----------------------------------------------------------------//
void MOAIFreeTypeFontReader::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAIFontReader::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "STROKER_CAP_STYLE_BUTT",				( u32 )FT_STROKER_LINECAP_BUTT );
	state.DefineClassConst ( klass, "STROKER_CAP_STYLE_ROUND",				( u32 )FT_STROKER_LINECAP_ROUND );
	state.DefineClassConst ( klass, "STROKER_CAP_STYLE_SQUARE",				( u32 )FT_STROKER_LINECAP_SQUARE );
	
	state.DefineClassConst ( klass, "STROKER_JOIN_STYLE_ROUND",				( u32 )FT_STROKER_LINEJOIN_ROUND );
	state.DefineClassConst ( klass, "STROKER_JOIN_STYLE_BEVEL",				( u32 )FT_STROKER_LINEJOIN_BEVEL );
	state.DefineClassConst ( klass, "STROKER_JOIN_STYLE_MITER_VARIABLE",	( u32 )FT_STROKER_LINEJOIN_MITER_VARIABLE );
	state.DefineClassConst ( klass, "STROKER_JOIN_STYLE_MITER",				( u32 )FT_STROKER_LINEJOIN_MITER );
	state.DefineClassConst ( klass, "STROKER_JOIN_STYLE_MITER_FIXED",		( u32 )FT_STROKER_LINEJOIN_MITER_FIXED );
	
	state.DefineStaticMethod ( klass, "extractSystemFont", _extractSystemFont, MRB_ARGS_NONE () );

}

//----------------------------------------------------------------//
void MOAIFreeTypeFontReader::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIFontReader::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "enableAntiAliasing", _enableAntiAliasing, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setPenColor", _setPenColor, MRB_ARGS_ARG ( 0, 4 ) );
	state.DefineInstanceMethod ( klass, "strokeGlyph", _strokeGlyph, MRB_ARGS_ARG ( 1, 6 ) );

}

//----------------------------------------------------------------//
int MOAIFreeTypeFontReader::RenderGlyph ( MOAIImage& image, float x, float y ) {

	if ( image.GetPixelFormat () != MOAIImage::TRUECOLOR ) return FONT_ERROR;

	if ( !this->mFace ) return FONT_ERROR;
	if ( this->mGlyphCode == GLYPH_CODE_NULL ) return FONT_ERROR;

	FT_Face face = ( FT_Face )this->mFace;
	
	// bail if glyph has no outline we can render
	if ( face->glyph->format!= FT_GLYPH_FORMAT_OUTLINE ) return FONT_ERROR;
	
	// TODO: not clear what is going on here - why are we checking mAntiAlias?
	if ( this->mAntiAlias ) {
	
		// set up the render params in case they are needed
		RenderParams render;
		FT_Raster_Params params;
		memset ( &params, 0, sizeof ( params ));
		params.flags		= FT_RASTER_FLAG_DIRECT | FT_RASTER_FLAG_AA;
		params.gray_spans	= _renderSpan;
		params.user			= &render;
		
		render.mImage		= &image;
		render.mBlendMode	= this->mBlendMode;
		render.mPenColor	= this->mPenColor;
		render.mPenX		=  ( int )x;
		render.mPenY		= ( int )y;
		
		FT_Outline_Render (( FT_Library )this->mLibrary, &face->glyph->outline, &params );
	}
	else {
		// completely different path to render monochrome
		// TODO: render monochrome glyph
		FT_Error error = FT_Render_Glyph ( face->glyph, FT_RENDER_MODE_MONO );
	}

	return OK;
}

//----------------------------------------------------------------//
int MOAIFreeTypeFontReader::SelectFace ( float size ) {

	if ( this->mFaceSize != size ) {

		FT_Face face = ( FT_Face )this->mFace;

		if ( !face ) return FONT_ERROR;

		FT_Set_Char_Size ( face, 0, ( u32 )( size * 64.0f ), DPI, DPI );

		int yMin = FT_MulFix ( face->bbox.yMin, face->size->metrics.y_scale ) >> 6; // div 64
		int yMax = FT_MulFix ( face->bbox.yMax, face->size->metrics.y_scale ) >> 6; // div 64
			
		this->mFaceHeight = ( float )( yMax - yMin );
		this->mFaceSize = size;
	}
	return OK;
}

//----------------------------------------------------------------//
int MOAIFreeTypeFontReader::SelectGlyph ( u32 c ) {

	if ( this->mGlyphCode != c ) {
	
		this->mGlyphCode = GLYPH_CODE_NULL;
	
		u32 index = FT_Get_Char_Index (( FT_Face )this->mFace, c );
		int result = FT_Load_Glyph (( FT_Face )this->mFace, index, FT_LOAD_NO_BITMAP );
		if ( result ) return FONT_ERROR;
		
		this->mGlyphCode = c;
	}
	return OK;
}

//----------------------------------------------------------------//
void MOAIFreeTypeFontReader::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	MOAIFontReader::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIFreeTypeFontReader::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	MOAIFontReader::SerializeOut ( state, serializer );
}

//----------------------------------------------------------------//
int MOAIFreeTypeFontReader::StrokeGlyph ( MOAIImage& image, float x, float y, float strokeSize, u32 capStyle, u32 joinStyle, float miterLimit ) {

	if ( image.GetPixelFormat () != MOAIImage::TRUECOLOR ) return FONT_ERROR;

	if ( !this->mFace ) return FONT_ERROR;
	if ( this->mGlyphCode == GLYPH_CODE_NULL ) return FONT_ERROR;

	FT_Face face = ( FT_Face )this->mFace;
	
	// bail if glyph has no outline we can render
	if ( face->glyph->format!= FT_GLYPH_FORMAT_OUTLINE ) return FONT_ERROR;
	
	// set up the render params in case they are needed
	RenderParams render;
	FT_Raster_Params params;
	memset ( &params, 0, sizeof ( params ));
	params.flags		= FT_RASTER_FLAG_DIRECT | ( this->mAntiAlias ? FT_RASTER_FLAG_AA : 0 );
	params.gray_spans	= _renderSpan;
	params.user			= &render;
	
	render.mImage		= &image;
	render.mBlendMode	= this->mBlendMode;
	render.mPenColor	= this->mPenColor;
	render.mPenX		= ( int )x;
	render.mPenY		= ( int )y;
	
	// Set up a stroker.
	FT_Stroker stroker;
	FT_Stroker_New (( FT_Library )this->mLibrary, &stroker );
	FT_Stroker_Set (
		stroker,
		( int )( strokeSize * 64.0f ),
		( FT_Stroker_LineCap )capStyle,
		( FT_Stroker_LineJoin )joinStyle,
		( int )( miterLimit * 64.0f )
	);

	FT_Glyph glyph;
	if ( FT_Get_Glyph ( face->glyph, &glyph ) == 0 ) {
	
		FT_Glyph_Stroke ( &glyph, stroker, true );
		
		if ( glyph->format == FT_GLYPH_FORMAT_OUTLINE ) {
		
			FT_Outline* outline = &reinterpret_cast < FT_OutlineGlyph >( glyph )->outline;
			FT_Outline_Render (( FT_Library )this->mLibrary, outline, &params );
		}

		// Clean up afterwards.
		FT_Stroker_Done ( stroker );
		FT_Done_Glyph ( glyph );
	}
	return OK;
}

#endif
