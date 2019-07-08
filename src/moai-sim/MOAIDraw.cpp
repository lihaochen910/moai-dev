// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIAnimCurve.h>
#include <moai-sim/MOAIDraw.h>
#include <moai-sim/MOAIFont.h>
#include <moai-sim/MOAIGfxBuffer.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIIndexBuffer.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAIVertexFormatMgr.h>
#include <moai-sim/MOAIShader.h>
#include <moai-sim/MOAITexture.h>
#include <moai-sim/MOAITransformBase.h>
#include <moai-sim/MOAIVertexBuffer.h>
#include <moai-sim/MOAIVertexFormat.h>
#include <moai-sim/MOAIVertexFormatMgr.h>
#include <moai-sim/MOAIViewport.h>
#include <moai-sim/MOAIQuadBrush.h>

#define DEFAULT_ELLIPSE_STEPS 64

//================================================================//
// MOAIDrawVertexLineStripWriter2D
//================================================================//
class MOAIDrawVertexLineStripWriter2D :
	public ZLAbstractVertexWriter2D {
private:

	u32				mVertexCount;
	ZLVec2D			mLastVertex;
	MOAIGfxMgr&		mGfxMgr;

public:

	//----------------------------------------------------------------//
	void Begin () {
	
		this->mVertexCount = 0;
		this->mGfxMgr.mGfxState.BeginPrim ( ZGL_PRIM_LINE_STRIP, 1 );
	}
	
	//----------------------------------------------------------------//
	MOAIDrawVertexLineStripWriter2D () :
		mGfxMgr ( MOAIGfxMgr::Get ()) {
	}

	//----------------------------------------------------------------//
	void WriteVertex ( const ZLVec2D& v ) {
		
		if (( this->mVertexCount > 0 ) && ( this->mGfxMgr.mGfxState.ContinuePrim ( 1 ) == MOAIGfxState::CONTINUE_ROLLOVER )) {

			// if we roll over, repeat the last vertex to start a new line strip
			this->mGfxMgr.mGfxState.WriteVtx ( this->mLastVertex.mX, this->mLastVertex.mY );
			this->mGfxMgr.mGfxState.WritePenColor4b ();
		}
		
		this->mGfxMgr.mGfxState.WriteVtx ( v.mX, v.mY );
		this->mGfxMgr.mGfxState.WritePenColor4b ();
		
		this->mLastVertex = v;
	}
	
	//----------------------------------------------------------------//
	void End () {
	
		this->mGfxMgr.mGfxState.EndPrim ();
	}
};

//================================================================//
// text drawing stuff
//================================================================//
/*
	TODO: I want to refactor/rewrite this stuff to consolidate the glyph layout and buffering
	under a single code path - there's some redundancy here with MOAITextLabel that can probably
	be smoothed away.
*/

struct GlyphPlacement {

	MOAIGlyph* glyph;
	float x;
	float y;
};

struct TextDrawContext {
		
	// Text
	STLList < GlyphPlacement > mGlyphs;

	// Text data
	MOAIFont* mFont;
	float mScale;
	float mFontSize;
	float mShadowOffsetX;
	float mShadowOffsetY;
};

static TextDrawContext g_TextDrawContext;
static TextDrawContext* g_CurrentTextDrawContext = 0; // TODO: ugh! refactor this!

//----------------------------------------------------------------//
void MOAIDraw::BeginDrawString ( float scale, MOAIFont& font, float fontSize, float shadowOffsetX, float shadowOffsetY ) {
	
	assert ( g_CurrentTextDrawContext == 0 );
	g_CurrentTextDrawContext = &g_TextDrawContext;

	g_CurrentTextDrawContext->mFont = &font;
	g_CurrentTextDrawContext->mFontSize = fontSize;
	g_CurrentTextDrawContext->mScale = scale;
	g_CurrentTextDrawContext->mShadowOffsetX = shadowOffsetX;
	g_CurrentTextDrawContext->mShadowOffsetY = shadowOffsetY;
}

//----------------------------------------------------------------//
void MOAIDraw::DrawString ( cc8* text, float x, float y, float width, float height ) {

	// Sanity check
	size_t textLength = strlen ( text );
	if ( textLength == 0 ) return;
	
	// Get the context data
	assert ( g_CurrentTextDrawContext );

	// Transform the center into 'world' space
	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	const ZLMatrix4x4& orgWorldTransform = gfxState.GetMtx ( MOAIGfxState::MODEL_TO_WORLD_MTX );
	ZLVec2D pos ( x, y );
	orgWorldTransform.Transform ( pos );
	x = pos.mX;
	y = pos.mY;

	// Extract the 'state'
	MOAIFont& font = *g_CurrentTextDrawContext->mFont;
	float scale = g_CurrentTextDrawContext->mScale;
	float fontSize = g_CurrentTextDrawContext->mFontSize;
	
	MOAIGlyphSet* glyphSet = font.GetGlyphSet ( fontSize );
	assert ( glyphSet );

	// Let's draw the string!
	float cursorX = x;
	float cursorY = y + glyphSet->GetAscent() * scale;
	MOAIGlyph* prevGlyph = 0;
	
	// Update the glyph cache
	for ( size_t i = 0; i < textLength; i++ ) {

		cc8 c = text [ i ];
		if ( c != '\n' ) {

			font.AffirmGlyph ( fontSize, c );
		}
	}
	font.ProcessGlyphs ();

	glyphSet = font.GetGlyphSet ( fontSize );
	assert ( glyphSet );

	for ( size_t i = 0; i < textLength; i++ ) {

		cc8 c = text [ i ];
		if ( c == '\n' ) {

			// Move to the next line
			cursorX = x;
			cursorY += glyphSet->GetHeight () * scale;
			prevGlyph = 0;

			if ( height > 0 && (cursorY - y) > height ) {
				break;
			}
		}
		else {

			if ( width > 0 && (cursorX - x) > width ) {
				continue;
			}

			// Get the glyph for the current character
			MOAIGlyph* glyph = glyphSet->GetGlyph ( c );
			if ( glyph ) {

				// Draw the current glyph
				MOAITextureBase* glyphTexture = font.GetGlyphTexture ( *glyph );
				if ( glyphTexture ) {

					GlyphPlacement placement = { glyph, cursorX, cursorY };
					g_CurrentTextDrawContext->mGlyphs.push_back( placement );
				}

				// Apply kerning
				if ( prevGlyph ) {

					MOAIKernVec kern = prevGlyph->GetKerning ( glyph->GetCode () );
					cursorX += kern.mX * scale;
				}

				// Move the cursor
				cursorX += glyph->GetAdvanceX () * scale;
			}

			prevGlyph = glyph;
		}
	}
}

//----------------------------------------------------------------//
void MOAIDraw::DrawString	( cc8* text, float x, float y, float scale, MOAIFont& font, float fontSize, float shadowOffsetX, float shadowOffsetY, float width, float height ) {

	BeginDrawString ( scale, font, fontSize, shadowOffsetX, shadowOffsetY );
	DrawString ( text, x, y, width, height );
	EndDrawString ();
}

//----------------------------------------------------------------//
void MOAIDraw::EndDrawString () {

	// Setup for drawing
	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	// Get current state
	//const ZLMatrix4x4& orgWorldTransform = gfxState.GetMtx ( MOAIGfxState::MODEL_TO_WORLD_MTX );

	// TODO
	//GLint orgSrcBlend, orgDestBlend;
	//glGetIntegerv ( GL_BLEND_SRC, &orgSrcBlend );
	//glGetIntegerv ( GL_BLEND_DST, &orgDestBlend );

	// Apply render state
	if ( !gfxState.SetShader ( MOAIShaderMgr::FONT_SHADER )) return;
	
	gfxState.SetBlendMode ( ZGL_BLEND_FACTOR_ONE, ZGL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA );
	gfxState.SetVertexTransform ( MOAIGfxState::MODEL_TO_WORLD_MTX );
	MOAIQuadBrush::BindVertexFormat ();

	// Get the context data
	assert( g_CurrentTextDrawContext );
	
	//MOAIFont& font = *g_CurrentTextDrawContext->mFont;
	//float scale = g_CurrentTextDrawContext->mScale;
	float shadowOffsetX = g_CurrentTextDrawContext->mShadowOffsetX;
	float shadowOffsetY = g_CurrentTextDrawContext->mShadowOffsetY;

	bool drawDropShadows = fabsf ( shadowOffsetX ) > 0.0001 && fabsf ( shadowOffsetY ) > 0.0001;

	u32 numPasses = 1;
	//float offsetX = 0;
	//float offsetY = 0;
	ZLColorVec penColor = gfxState.GetPenColor ();
	if ( drawDropShadows ) {

		numPasses = 2;		
		gfxState.SetPenColor ( 0, 0, 0, 1 );
		//offsetX = shadowOffsetX;
		//offsetY = shadowOffsetY;
	}

	for ( u32 pass = 0; pass < numPasses; pass++ ) {

		if ( pass == 1 || numPasses == 1 ) {
			gfxState.SetPenColor ( penColor );
			//offsetX = 0;
			//offsetY = 0;
		}

		//STLList < GlyphPlacement >::const_iterator it;
		//for ( it = g_CurrentTextDrawContext->mGlyphs.begin (); it != g_CurrentTextDrawContext->mGlyphs.end (); ++it ) {

		//	const GlyphPlacement& glyphPlacement = *it;
		//	MOAIGlyph* glyph = glyphPlacement.glyph;
		//	MOAITextureBase* glyphTexture = font.GetGlyphTexture ( *glyph );
		//	glyph->Draw ( *glyphTexture, glyphPlacement.x + offsetX, glyphPlacement.y + offsetY, scale, scale );
		//}
	}

	// Restore render state
	MOAIDraw::Bind ();

	//gfxMgr.SetVertexTransform ( MOAIGfxMgr::VTX_WORLD_TRANSFORM, orgWorldTransform );
	//gfxMgr.SetBlendMode ( orgSrcBlend, orgDestBlend ); // TODO

	// Clear context
	g_CurrentTextDrawContext->mFont = 0;
	g_CurrentTextDrawContext->mGlyphs.clear();
	g_CurrentTextDrawContext = 0;
}

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_bind ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	self->Bind ();
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_bindFrameBuffer ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	MOAIGfxMgr::Get ().mGfxState.SetFrameBuffer ( state.GetRubyObject < MOAIFrameBuffer >( 1, false ));
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_bindIndexBuffer ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	MOAIGfxMgr::Get ().mGfxState.SetIndexBuffer ( state.GetRubyObject < MOAIIndexBuffer >( 1, false ));
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_bindShader ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	switch ( state.GetParamType ( 1 ) ) {
	
		case MRB_TT_DATA:
			MOAIGfxMgr::Get ().mGfxState.SetShader ( state.GetRubyObject < MOAIShader >( 1, true ));
			break;
		
		case MRB_TT_FIXNUM:
			MOAIGfxMgr::Get ().mGfxState.SetShader (( MOAIShaderMgr::Preset )state.GetParamValue < u32 >( 1, MOAIShaderMgr::LINE_SHADER ));
			break;
		
		default:
			MOAIGfxMgr::Get ().mGfxState.SetShader ();
	}
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_bindTexture ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	MOAIGfxMgr::Get ().mGfxState.SetTexture ( state.GetRubyObject < MOAITexture >( 1, false ));
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_bindVertexArray ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	MOAIGfxMgr::Get ().mGfxState.SetVertexArray ( state.GetRubyObject < MOAIVertexArray >( 1, false ));
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_bindVertexBuffer ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	MOAIGfxMgr::Get ().mGfxState.SetVertexBuffer ( state.GetRubyObject < MOAIVertexBuffer >( 1, false ));
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_bindVertexFormat ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	switch ( state.GetParamType ( 1 ) ) {
	
		case MRB_TT_DATA:
			MOAIGfxMgr::Get ().mGfxState.SetVertexFormat ( state.GetRubyObject < MOAIVertexFormat >( 1, true ));
			break;
		
		case MRB_TT_FIXNUM:
			MOAIGfxMgr::Get ().mGfxState.SetVertexFormat (( MOAIVertexFormatMgr::Preset )state.GetParamValue < u32 >( 1, MOAIVertexFormatMgr::XYZWC ));
			break;
		
		default:
			MOAIGfxMgr::Get ().mGfxState.SetVertexFormat ();
			break;
	}
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_clear ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )

	MOAIGfxMgr::Get ().mGfxState.ClearSurface ();
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_drawAnimCurve ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )

	MOAIAnimCurve* curve	= state.GetRubyObject < MOAIAnimCurve >( 1, true );
	u32 resolution			= state.GetParamValue < u32 >( 2, 1 );

	if ( curve ) {
		self->DrawAnimCurve ( *curve, resolution );
	}
	return context;
}

//----------------------------------------------------------------//
// TODO: implement
mrb_value MOAIDraw::_drawAxisGrid ( mrb_state* M, mrb_value context ) {
	UNUSED ( M );
	UNUSED ( context );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	drawBezierCurve
	@text	Draws a bezier curve.
	
	@in		number x0
	@in		number y0
	@in		number x1
	@in		number y1
	@in		number x2
	@in		number y2
	@in		number x3
	@in		number y3
	@out	nil
*/
mrb_value MOAIDraw::_drawBezierCurve ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	ZLCubicBezier2D bezier;
	
	bezier.mP0.mX = state.GetParamValue < float >( 1, 0.0f );
	bezier.mP0.mY = state.GetParamValue < float >( 2, 0.0f );
	
	bezier.mP1.mX = state.GetParamValue < float >( 3, 0.0f );
	bezier.mP1.mY = state.GetParamValue < float >( 4, 0.0f );
	
	bezier.mP2.mX = state.GetParamValue < float >( 5, 0.0f );
	bezier.mP2.mY = state.GetParamValue < float >( 6, 0.0f );
	
	bezier.mP3.mX = state.GetParamValue < float >( 7, 0.0f );
	bezier.mP3.mY = state.GetParamValue < float >( 8, 0.0f );
	
	self->DrawBezierCurve ( bezier );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	drawBoxOutline
	@text	Draw a box outline.
	
	@in		number x0
	@in		number y0
	@in		number z0
	@in		number x1
	@in		number y1
	@in		number z1
	@out	nil
*/
mrb_value MOAIDraw::_drawBoxOutline ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )

	ZLBox box;
	box.mMin.mX = state.GetParamValue < float >( 1, 0.0f );
	box.mMin.mY = state.GetParamValue < float >( 2, 0.0f );
	box.mMin.mZ = state.GetParamValue < float >( 3, 0.0f );
	box.mMax.mX = state.GetParamValue < float >( 4, box.mMin.mX );
	box.mMax.mY = state.GetParamValue < float >( 5, box.mMin.mY );
	box.mMax.mZ = state.GetParamValue < float >( 6, box.mMin.mZ );
	
	self->Get ().DrawBoxOutline ( box );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	drawCircle
	@text	Draw a circle.
	
	@in		number x
	@in		number y
	@in		number r
	@in		number steps
	@out	nil
*/
mrb_value MOAIDraw::_drawCircle ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	float x0	= state.GetParamValue < float >( 1, 0.0f );
	float y0	= state.GetParamValue < float >( 2, 0.0f );
	float r		= state.GetParamValue < float >( 3, 0.0f );
	u32 steps	= state.GetParamValue < u32 >( 4, DEFAULT_ELLIPSE_STEPS );

	self->Get ().DrawEllipseOutline ( x0, y0, r, r, steps );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	drawCircleSpokes
	@text	Draw the spokes of a circle.
	
	@in		number x
	@in		number y
	@in		number r
	@in		number steps
	@out	nil
*/
mrb_value MOAIDraw::_drawCircleSpokes ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	float x0	= state.GetParamValue < float >( 1, 0.0f );
	float y0	= state.GetParamValue < float >( 2, 0.0f );
	float r		= state.GetParamValue < float >( 3, 0.0f );
	u32 steps	= state.GetParamValue < u32 >( 4, DEFAULT_ELLIPSE_STEPS );

	self->Get ().DrawEllipseSpokes ( x0, y0, r, r, steps );
	return context;
}


//----------------------------------------------------------------//
/**	@lua	drawElements
	@text	Draw elements.
	
	@in		...     vtxBuffer	VertexBuffer
	@in		...     format		Vertex Format of vertices in the buffer
	@in		number  count       Number of indices to be rendered
	@out	nil
 */
mrb_value MOAIDraw::_drawElements ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	MOAIGfxBuffer* vtxBuffer = state.GetRubyObject < MOAIGfxBuffer >( 1, false );
	MOAIVertexFormat* format = state.GetRubyObject < MOAIVertexFormat >( 2, false );
	u32 count = state.GetParamValue < u32 >( 3, 0 );
	
	self->DrawElements ( vtxBuffer, format, count );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	drawEllipse
	@text	Draw an ellipse.
	
	@in		number x
	@in		number y
	@in		number xRad
	@in		number yRad
	@in		number steps
	@out	nil
*/
mrb_value MOAIDraw::_drawEllipse ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	float x		= state.GetParamValue < float >( 1, 0.0f );
	float y		= state.GetParamValue < float >( 2, 0.0f );
	float xRad	= state.GetParamValue < float >( 3, 0.0f );
	float yRad	= state.GetParamValue < float >( 4, 0.0f );
	
	u32 steps = state.GetParamValue < u32 >( 5, DEFAULT_ELLIPSE_STEPS );

	self->Get ().DrawEllipseOutline ( x, y, xRad, yRad, steps );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	drawCircleSpokes
	@text	Draw the spokes of an ellipse.
	
	@in		number x
	@in		number y
	@in		number xRad
	@in		number yRad
	@in		number steps
	@out	nil
*/
mrb_value MOAIDraw::_drawEllipseSpokes ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	float x		= state.GetParamValue < float >( 1, 0.0f );
	float y		= state.GetParamValue < float >( 2, 0.0f );
	float xRad	= state.GetParamValue < float >( 3, 0.0f );
	float yRad	= state.GetParamValue < float >( 4, 0.0f );

	u32 steps = state.GetParamValue < u32 >( 5, DEFAULT_ELLIPSE_STEPS );

	self->Get ().DrawEllipseSpokes ( x, y, xRad, yRad, steps );
	return context;
}

//----------------------------------------------------------------//
// TODO: implement
mrb_value MOAIDraw::_drawGrid ( mrb_state* M, mrb_value context ) {
	UNUSED ( M );
	UNUSED ( context );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	drawLine
	@text	Draw a line.
	
	@in		... vertices		List of vertices (x, y) or an array of vertices
								{ x0, y0, x1, y1, ... , xn, yn }
	@out	nil
*/
mrb_value MOAIDraw::_drawLine ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )

	if ( state.ParamIsType ( 1, MRB_TT_ARRAY ) ) {
		self->DrawRubyArray ( M, context, ZGL_PRIM_LINE_STRIP );
	}
	else {
		self->DrawRubyParams ( M, context, ZGL_PRIM_LINE_STRIP );
	}	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	drawPoints
	@text	Draw a list of points.
	
	@in		... vertices		List of vertices (x, y) or an array of vertices
								{ x0, y0, x1, y1, ... , xn, yn }
	@out	nil
*/
mrb_value MOAIDraw::_drawPoints ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	if ( state.ParamIsType ( 1, MRB_TT_ARRAY ) ) {
		self->DrawRubyArray ( M, context, ZGL_PRIM_POINTS );
	}
	else {
		self->DrawRubyParams ( M, context, ZGL_PRIM_POINTS );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	drawRay
	@text	Draw a ray.
	
	@in		number x
	@in		number y
	@in		number dx
	@in		number dy
	@out	nil
*/
mrb_value MOAIDraw::_drawRay ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	float x		= state.GetParamValue < float >( 1, 0.0f );
	float y		= state.GetParamValue < float >( 2, 0.0f );
	float dx	= state.GetParamValue < float >( 3, 0.0f );
	float dy	= state.GetParamValue < float >( 4, 0.0f );

	self->DrawRay ( x, y, dx, dy );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	drawRect
	@text	Draw a rectangle.
	
	@in		number x0
	@in		number y0
	@in		number x1
	@in		number y1
	@out	nil
*/
mrb_value MOAIDraw::_drawRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	float x0 = state.GetParamValue < float >( 1, 0.0f );
	float y0 = state.GetParamValue < float >( 2, 0.0f );
	float x1 = state.GetParamValue < float >( 3, 0.0f );
	float y1 = state.GetParamValue < float >( 4, 0.0f );

	self->Get ().DrawRectOutline ( x0, y0, x1, y1 );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	drawTexture
	@text	Draw a filled rectangle.
	
	@in		number x0
	@in		number y0
	@in		number x1
	@in		number y1
	@in		MOAITexture texture
	@out	nil
*/
mrb_value MOAIDraw::_drawTexture ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	float x0 = state.GetParamValue < float >( 1, 0.0f );
	float y0 = state.GetParamValue < float >( 2, 0.0f );
	float x1 = state.GetParamValue < float >( 3, 0.0f );
	float y1 = state.GetParamValue < float >( 4, 0.0f );
	MOAITexture* texture = (MOAITexture*)MOAITexture::AffirmTexture ( state, 5 );

	self->DrawTexture ( x0, y0, x1, y1, texture );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	drawText
	@text	Draws a string.
	
	@in		MOAIFont font
	@in		number size		Font size
	@in		string text
	@in		number x		Left position
	@in		number y		Top position
	@in		number scale
	@in		number shadowOffsetX
	@in		number shadowOffsetY
	@out	nil
*/
mrb_value MOAIDraw::_drawText ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )

	// TODO	
	//cc8* text = lua_tostring ( state, 3 );
	//if ( text ) {

	//	float x = state.GetParamValue < float >( 4, 0.0f );
	//	float y = state.GetParamValue < float >( 5, 0.0f );
	//	float scale = state.GetParamValue < float >( 6, 1.0f );

	//	float shadowOffsetX = state.GetParamValue < float >( 7, 0.0f );
	//	float shadowOffsetY = state.GetParamValue < float >( 8, 0.0f );

	//	MOAIFont* font = state.GetRubyObject < MOAIFont >( 1, true );
	//	if ( font ) {

	//		float fontSize = state.GetParamValue < float >( 2, font->GetDefaultSize () );

	//		MOAIDraw::DrawText ( text, x, y, scale, *font, fontSize, shadowOffsetX, shadowOffsetY, 0, 0 );
	//	}
	//}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	fillCircle
	@text	Draw a filled circle.
	
	@in		number x
	@in		number y
	@in		number r
	@in		number steps
	@out	nil
*/
mrb_value MOAIDraw::_fillCircle ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	float x0	= state.GetParamValue < float >( 1, 0.0f );
	float y0	= state.GetParamValue < float >( 2, 0.0f );
	float r		= state.GetParamValue < float >( 3, 0.0f );
	u32 steps	= state.GetParamValue < u32 >( 4, DEFAULT_ELLIPSE_STEPS );

	MOAIDraw::Get ().DrawEllipseFill ( x0, y0, r, r, steps );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	fillEllipse
	@text	Draw a filled ellipse.
	
	@in		number x
	@in		number y
	@in		number xRad
	@in		number yRad
	@in		number steps
	@out	nil
*/
mrb_value MOAIDraw::_fillEllipse ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	float x		= state.GetParamValue < float >( 1, 0.0f );
	float y		= state.GetParamValue < float >( 2, 0.0f );
	float xRad	= state.GetParamValue < float >( 3, 0.0f );
	float yRad	= state.GetParamValue < float >( 4, 0.0f );
	
	u32 steps = state.GetParamValue < u32 >( 5, DEFAULT_ELLIPSE_STEPS );

	MOAIDraw::Get ().DrawEllipseFill ( x, y, xRad, yRad, steps );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	fillFan
	@text	Draw a filled fan.
	
	@in		... vertices		List of vertices (x, y) or an array of vertices
								{ x0, y0, x1, y1, ... , xn, yn }
	@out	nil
*/
mrb_value MOAIDraw::_fillFan ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )

	if ( state.ParamIsType ( 1, MRB_TT_ARRAY ) ) {
		self->DrawRubyArray( M, context, ZGL_PRIM_TRIANGLE_FAN );
	}
	else {
		self->DrawRubyParams( M, context, ZGL_PRIM_TRIANGLE_FAN );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	fillRect
	@text	Draw a filled rectangle.
	
	@in		number x0
	@in		number y0
	@in		number x1
	@in		number y1
	@out	nil
*/
mrb_value MOAIDraw::_fillRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	float x0 = state.GetParamValue < float >( 1, 0.0f );
	float y0 = state.GetParamValue < float >( 2, 0.0f );
	float x1 = state.GetParamValue < float >( 3, 0.0f );
	float y1 = state.GetParamValue < float >( 4, 0.0f );

	self->Get ().DrawRectFill ( x0, y0, x1, y1 );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_fillRoundedRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	float x0 = state.GetParamValue < float >( 1, 0.0f );
	float y0 = state.GetParamValue < float >( 2, 0.0f );
	float x1 = state.GetParamValue < float >( 3, 0.0f );
	float y1 = state.GetParamValue < float >( 4, 0.0f );
	
	float xRad		= state.GetParamValue < float >( 5, 0.0f );
	float yRad		= state.GetParamValue < float >( 6, xRad );
	u32 steps		= state.GetParamValue < u32 >( 7, DEFAULT_ELLIPSE_STEPS );

	self->Get ().DrawRoundedRectFill ( x0, y0, x1, y1, xRad, yRad, steps );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_getPenColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )

	ZLColorVec color = MOAIGfxMgr::Get ().mGfxState.GetPenColor ();
	
	mrb_value ret [ 4 ];
	ret [ 0 ] = state.ToRValue ( color.mR );
	ret [ 1 ] = state.ToRValue ( color.mG );
	ret [ 2 ] = state.ToRValue ( color.mB );
	ret [ 3 ] = state.ToRValue ( color.mA );

	return mrb_ary_new_from_values ( M, 4, ret );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_popGfxState ( mrb_state* M, mrb_value context ) {

	MOAIGfxMgr::Get ().mGfxState.PopState ();
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_pushGfxState ( mrb_state* M, mrb_value context ) {

	MOAIGfxMgr::Get ().mGfxState.PushState ();
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setBlendMode
	@text	Set blend mode using OpenGL source and dest factors. OpenGl blend factor constants are exposed as members of MOAIGraphicsProp.
			See the OpenGL documentation for an explanation of blending constants.

	@in		number srcFactor
	@in		number dstFactor
	@in		number equation
	@out	nil
*/
mrb_value MOAIDraw::_setBlendMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	if ( state.ParamIsType ( 1, MRB_TT_FIXNUM )) {
		if ( state.ParamIsType ( 2, MRB_TT_FIXNUM )) {

			u32 srcFactor = state.GetParamValue < u32 >( 1, 0 );
			u32 dstFactor = state.GetParamValue < u32 >( 2, 0 );
			u32 equation = state.GetParamValue < u32 >( 3, 0 );
			gfxState.SetBlendMode ( srcFactor, dstFactor, equation );
		}
	}
	else {
		gfxState.SetBlendMode ();
	}
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_setClearColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	u32 clearFlags = gfxState.GetClearFlags () & ~ZGL_CLEAR_COLOR_BUFFER_BIT;
	ZLColorVec clearColor ( 0.0f, 0.0f, 0.0f, 1.0f );

	MOAIColor* color = state.GetRubyObject < MOAIColor >( 1, true );
	if ( color ) {
		clearColor = *color;
		clearFlags |= ZGL_CLEAR_COLOR_BUFFER_BIT;
	}

	if ( state.GetParamsCount () > 1 ) {
	
		float r = state.GetParamValue < float >( 1, 0.0f );
		float g = state.GetParamValue < float >( 2, 0.0f );
		float b = state.GetParamValue < float >( 3, 0.0f );
		float a = state.GetParamValue < float >( 4, 1.0f );
		
		clearColor.Set ( r, g, b, a );
		clearFlags |= ZGL_CLEAR_COLOR_BUFFER_BIT;
	}
	
	gfxState.SetClearColor ( clearColor );
	gfxState.SetClearFlags ( clearFlags );
	
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_setClearDepth ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	u32 clearFlags = gfxState.GetClearFlags () & ~ZGL_CLEAR_DEPTH_BUFFER_BIT;
	double clearDepth = 0.0f;
	
	if ( state.ParamIsType ( 1, MRB_TT_FIXNUM )) {
	
		clearDepth = state.GetParamValue < double >( 1, 0.0 );
		clearFlags |= ZGL_CLEAR_DEPTH_BUFFER_BIT;
	}
	
	gfxState.SetClearDepth ( clearDepth );
	gfxState.SetClearFlags ( clearFlags );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setCullMode
 @text	Set cull mode
 See the OpenGL documentation for an explanation of culling modes.
 
 @in	number
 @out	nil
 */
mrb_value MOAIDraw::_setCullMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	if ( state.ParamIsType ( 1, MRB_TT_FIXNUM )) {
			
		u32 cullMode = state.GetParamValue < u32 >( 1, 0 );
		gfxState.SetCullFunc ( cullMode );
	}
	else {
		gfxState.SetCullFunc ();
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setDefaultTexture
	@text	Specify a fallback texture to use when textures are
			unavailable (pending load, missing or in error state).
	
	@in		MOAITexture texture
	@out	MOAITexture texture		Texture that was passed in or created.
*/
mrb_value MOAIDraw::_setDefaultTexture ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	MOAITexture* texture = state.GetRubyObject < MOAITexture >( 1, false );
	
	if ( !texture ) {
		texture = state.CreateClassInstance < MOAITexture >();
		if ( !texture->Init ( state, 1 )) {
			// TODO: report error
			delete texture;
			texture = 0;
		}
	}

	gfxState.SetDefaultTexture ( texture );

	if ( texture ) {
		return state.ToRValue < MOAIRubyObject* >( texture );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setDepthFunc
 @text	Set depth function.
 
 @in	number
 @out	nil
 */
mrb_value MOAIDraw::_setDepthFunc ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	if ( state.ParamIsType ( 1, MRB_TT_FIXNUM )) {
		
		u32 depthFunc = state.GetParamValue < u32 >( 1, 0 );
		gfxState.SetDepthFunc ( depthFunc );
	}
	else {
		gfxState.SetDepthFunc ();
	}
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_setMatrix ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	u32 matrixID = state.GetParamValue < u32 >( 1, MOAIGfxState::MODEL_TO_WORLD_MTX );

	if ( gfxState.IsInputMtx( matrixID )) {
	
		if ( state.ParamIsType ( 2, MRB_TT_DATA )) {
		
			MOAITransformBase* transform = state.GetRubyObject < MOAITransformBase >( 2, true );
			if ( transform ) {
				gfxState.SetMtx ( matrixID, transform->GetLocalToWorldMtx ());
			}
		}
		else {
			int size = state.GetParamsCount () - 2;
			ZLMatrix4x4 mtx = state.GetParamMatrix ( 2, size );
			gfxState.SetMtx ( matrixID, mtx );
		}
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setPenColor

	@in		number r
	@in		number g
	@in		number b
	@opt	number a	Default value is 1.
	@out	nil
*/
mrb_value MOAIDraw::_setPenColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )

	float r = state.GetParamValue < float >( 1, 1.0f );
	float g = state.GetParamValue < float >( 2, 1.0f );
	float b = state.GetParamValue < float >( 3, 1.0f );
	float a = state.GetParamValue < float >( 4, 1.0f );

	MOAIGfxMgr::Get ().mGfxState.SetPenColor ( r, g, b, a );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setPenWidth

	@in		number width
	@out	nil
*/
mrb_value MOAIDraw::_setPenWidth ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )

	float width = state.GetParamValue < float >( 1, 1.0f );
	MOAIGfxMgr::Get ().mGfxState.SetPenWidth ( width );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_setScissorRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )

	// TODO: fix this code duplication from _setViewRect
	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	if ( state.ParamIsType ( 1, MRB_TT_DATA )) {
		MOAIViewport* viewport = state.GetRubyObject < MOAIViewport >( 1, true );
		if ( viewport ) {
			gfxState.SetScissorRect ( *viewport );
		}
	}
	else {
		ZLRect rect;
		rect.Init ( 0.0, 0.0, (float) gfxState.GetBufferWidth (), (float) gfxState.GetBufferHeight ());
		rect = state.GetParamValue < ZLRect >( 1, rect );
		gfxState.SetScissorRect ( rect );
	}
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_setViewProj ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	MOAIViewport* viewport = state.GetRubyObject < MOAIViewport >( 1, false );
	MOAICamera* camera = state.GetRubyObject < MOAICamera >( 2, false );
	
	MOAIGfxMgr::Get ().mGfxState.SetViewProj ( viewport, camera );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_setViewRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )

	// TODO: fix this code duplication from _setScissorRect
	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	if ( state.ParamIsType ( 1, MRB_TT_DATA )) {
		MOAIViewport* viewport = state.GetRubyObject < MOAIViewport >( 1, true );
		if ( viewport ) {
			gfxState.SetViewRect ( *viewport );
		}
	}
	else {
		ZLRect rect;
		rect.Init ( 0.0, 0.0, (float) gfxState.GetBufferWidth (), (float) gfxState.GetBufferHeight ());
		rect = state.GetParamValue < ZLRect >( 1, rect );
		gfxState.SetViewRect ( rect );
	}
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIDraw::_strokeRoundedRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIDraw, "" )
	
	float x0 = state.GetParamValue < float >( 1, 0.0f );
	float y0 = state.GetParamValue < float >( 2, 0.0f );
	float x1 = state.GetParamValue < float >( 3, 0.0f );
	float y1 = state.GetParamValue < float >( 4, 0.0f );
	
	float stroke	= state.GetParamValue < float >( 5, 0.0f );
	float offset	= state.GetParamValue < float >( 6, 0.0f );
	
	float xRad		= state.GetParamValue < float >( 7, 0.0f );
	float yRad		= state.GetParamValue < float >( 8, xRad );
	u32 steps		= state.GetParamValue < u32 >( 9, DEFAULT_ELLIPSE_STEPS );

	self->Get ().DrawRoundedRectStroke ( x0, y0, x1, y1, xRad, yRad, steps, stroke, offset );
	return context;
}

//================================================================//
// MOAIDraw
//================================================================//

//----------------------------------------------------------------//
bool MOAIDraw::Bind () {

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	if ( !gfxState.SetTexture ()) return false;
	if ( !gfxState.SetShader ( MOAIShaderMgr::LINE_SHADER )) return false;
	gfxState.SetVertexFormat ( MOAIVertexFormatMgr::XYZWC );
	
	gfxState.SetVertexTransform ( MOAIGfxState::MODEL_TO_CLIP_MTX );
	gfxState.SetUVTransform ( MOAIGfxState::UV_TO_MODEL_MTX );

	gfxState.SetCullFunc (); // no culling in immediate mode drawing

	return true;
}

//----------------------------------------------------------------//
void MOAIDraw::DrawAnimCurve ( const MOAIAnimCurve& curve, u32 resolution ) {

	curve.Draw ( resolution );
}

//----------------------------------------------------------------//
void MOAIDraw::DrawAxisGrid ( ZLVec2D loc, ZLVec2D vec, float size ) {

	ZLMatrix4x4 mtx = MOAIGfxMgr::Get ().mGfxState.GetMtx ( MOAIGfxState::WORLD_TO_CLIP_MTX );
	
	ZLMatrix4x4 invMtx;
	invMtx.Inverse ( mtx );
	
	// Set the axis to the grid length so we can get the length back post-transform
	vec.SetLength ( size );
	
	mtx.Transform ( loc );
	mtx.TransformVec ( vec );

	// Get the axis unit vector
	ZLVec2D norm = vec;
	size = norm.NormSafe ();
	
	// Get the axis normal
	ZLVec2D perpNorm ( norm.mY, -norm.mX );
	
	// Project the corners of the viewport onto the axis to get the mix/max bounds
	float dot;
	float min;
	float max;
	
	ZLVec2D corner;
	
	// left, top
	corner.Init ( -1.0f, 1.0f );
	corner.Sub ( loc );
	dot = norm.Dot ( corner );
	
	min = dot;
	max = dot;
	
	// right, top
	corner.Init ( 1.0f, 1.0f );
	corner.Sub ( loc );
	dot = norm.Dot ( corner );
	
	min = ( dot < min ) ? dot : min;
	max = ( dot > max ) ? dot : max;
	
	// right, bottom
	corner.Init ( 1.0f, -1.0f );
	corner.Sub ( loc );
	dot = norm.Dot ( corner );
	
	min = ( dot < min ) ? dot : min;
	max = ( dot > max ) ? dot : max;
	
	// left, bottom
	corner.Init ( -1.0f, -1.0f );
	corner.Sub ( loc );
	dot = norm.Dot ( corner );
	
	min = ( dot < min ) ? dot : min;
	max = ( dot > max ) ? dot : max;
	
	// Get the start andstop grids
	s32 start = ( s32 )( min / size ) - 1;
	s32 stop = ( s32 )( max / size ) + 1;
	
	// Set the pen to the first...
	ZLVec2D pen = norm;
	pen.Scale (( float )start * size );
	pen.Add ( loc );
	
	// Step along the axis to draw perpendicular grid lines
	ZLRect viewRect;
	viewRect.Init ( -1.0f, -1.0f, 1.0f, 1.0f );
	
	for ( ; start < stop; ++start ) {
		
		ZLVec2D p0;
		ZLVec2D p1;
		
		if ( viewRect.GetIntersection ( pen, perpNorm, p0, p1 )) {
			
			invMtx.Transform ( p0 );
			invMtx.Transform ( p1 );
			
			this->DrawLine ( p0, p1 );
		}
		
		pen.Add ( vec );
	}
}

//----------------------------------------------------------------//
void MOAIDraw::DrawBezierCurve ( const ZLCubicBezier2D& bezier ) {

	MOAIDrawVertexLineStripWriter2D writer;
	
	writer.Begin ();
		bezier.Flatten ( writer );
	writer.End ();
}

//----------------------------------------------------------------//
void MOAIDraw::DrawElements ( MOAIGfxBuffer* vtxBuffer, MOAIVertexFormat* vtxFormat, u32 count ) {

	// TODO: fix this?

	UNUSED ( vtxBuffer );
	UNUSED ( vtxFormat );
	UNUSED ( count );
	
//	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
//	
//	MOAIGfxMgr::Get ().UnbindBufferedDrawing ();
//	
//	vtxBuffer->Bind ();
//	vtxFormat->Bind ( 0 );
//	
//	gfxMgr.SetVertexMtxMode ( MOAIGfxMgr::VTX_STAGE_MODEL, MOAIGfxMgr::VTX_STAGE_MODEL );
//	
//	gfxMgr.UpdateShaderGlobals ();
//		
//	// TODO: use gfxMgr to cache buffers
//
//	zglDrawArrays ( ZGL_PRIM_TRIANGLES, 0, count );
//	vtxBuffer->Unbind ();
//	vtxFormat->Unbind ();
}

//----------------------------------------------------------------//
void MOAIDraw::DrawGrid ( const ZLRect& rect, u32 xCells, u32 yCells ) {

	if ( xCells > 1 ) {
		float xStep = rect.Width () / ( float )xCells;
		for ( u32 i = 1; i < xCells; ++i ) {
			float x = rect.mXMin + (( float )i * xStep );
			ZLVec2D v0 ( x, rect.mYMin );
			ZLVec2D v1 ( x, rect.mYMax );
			
			this->DrawLine ( v0, v1 );
		}
	}

	if ( yCells > 1 ) {
		float yStep = rect.Height () / ( float )yCells;
		for ( u32 i = 1; i < yCells; ++i ) {
			float y = rect.mYMin + (( float )i * yStep );
			ZLVec2D v0 ( rect.mXMin, y );
			ZLVec2D v1 ( rect.mXMax, y );
			
			this->DrawLine ( v0, v1 );
		}
	}

	this->DrawRectOutline ( rect );
}

//----------------------------------------------------------------//
void MOAIDraw::DrawRubyParams ( mrb_state* M, mrb_value context, u32 primType ) {

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	MOAIRubyState state ( M );

	u32 total = state.GetParamsCount () >> 1;
	
	gfxState.BeginPrim ( primType, total );
	
	for ( u32 i = 0; i < total; ++i ) {
		
		u32 idx = ( i << 1 ) + 1;
		
		float x = state.GetParamValue < float >( idx, 0.0f );
		float y = state.GetParamValue < float >( idx + 1, 0.0f );
		
		gfxState.WriteVtx ( x, y, 0.0f );
		gfxState.WritePenColor4b ();
	}
	
	gfxState.EndPrim ();
}


//----------------------------------------------------------------//
void MOAIDraw::DrawRubyArray ( mrb_state* M, mrb_value context, u32 primType ) {

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	MOAIRubyState state ( M );

	float x = 0.0f;
	float y = 0.0f;
	
	mrb_value ary = state.GetParamValue ( 1 );
	size_t size = state.GetArraySize ( ary ) >> 2;
	
	gfxState.BeginPrim ( primType, size );

	u32 counter = 0;
    while ( !state.IsNil ( mrb_ary_ref ( M, ary, counter ))) {
		// Assuming odd-numbered array entries to be x-coordinate (abscissa),
		// even-numbered array entries to be y-coordinate (oordinate).
		if ( counter % 2 == 0 ) {
			//x = state.GetParamValue < float >( -1, 0.0f );
			x = state.ToCValue < float >( mrb_ary_ref ( M, ary, counter ) );
		} else {
			y = state.ToCValue < float >( mrb_ary_ref ( M, ary, counter ) );
			gfxState.WriteVtx ( x, y );
			gfxState.WritePenColor4b ();
		}
		++counter;
	}

	gfxState.EndPrim ();
}

//----------------------------------------------------------------//
void MOAIDraw::DrawRay ( float x, float y, float dx, float dy ) {
	
	ZLVec2D loc ( x, y );
	ZLVec2D vec ( dx, dy );
	
	ZLMatrix4x4 mtx = MOAIGfxMgr::Get ().mGfxState.GetMtx ( MOAIGfxState::WORLD_TO_CLIP_MTX );
	
	ZLMatrix4x4 invMtx;
	invMtx.Inverse ( mtx );
	
	mtx.Transform ( loc );
	mtx.TransformVec ( vec );
	
	ZLRect viewRect;
	viewRect.Init ( -1.0f, -1.0f, 1.0f, 1.0f );
	
	ZLVec2D p0;
	ZLVec2D p1;
	
	if ( viewRect.GetIntersection ( loc, vec, p0, p1 )) {
		
		invMtx.Transform ( p0 );
		invMtx.Transform ( p1 );
		
		MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
		
		gfxState.BeginPrim ( ZGL_PRIM_LINES, 2 );
		
			gfxState.WriteVtx ( p0.mX, p0.mY, 0.0f );
			gfxState.WritePenColor4b ();
			
			gfxState.WriteVtx ( p1.mX, p1.mY, 0.0f );
			gfxState.WritePenColor4b ();

		gfxState.EndPrim ();
	}
}

//----------------------------------------------------------------//
void MOAIDraw::DrawTexture ( float left, float top, float right, float bottom, MOAITexture* texture ) {
	
	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	if ( texture ) {

//		gfxMgr.SetBlendMode ( ZGL_BLEND_FACTOR_ONE, ZGL_BLEND_FACTOR_ZERO );
		if ( !gfxState.SetTexture ( texture )) return;
		if ( !gfxState.SetShader ( MOAIShaderMgr::DECK2D_SHADER )) return;

		const ZLColorVec& orgColor = gfxState.GetPenColor ();
		gfxState.SetPenColor ( 1, 1, 1, 1 );
		
		MOAIQuadBrush::BindVertexFormat ();

		MOAIQuadBrush quad;
		quad.SetVerts ( left, top, right, bottom );
		quad.SetUVs ( 0, 0, 1, 1 );		
		quad.Draw ();
		
//		gfxMgr.SetBlendMode ();
		gfxState.SetPenColor ( orgColor );
		
		MOAIDraw::Bind ();
	}
}

//----------------------------------------------------------------//
void MOAIDraw::DrawVertexArray ( const ZLVec3D* verts, u32 count, u32 color, u32 primType ) {

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	gfxState.SetPenColor ( color );
	gfxState.BeginPrim ( primType, count );
	
	for ( u32 i = 0; i < count; ++i ) {
		const ZLVec3D& vtx = verts [ i ];
		gfxState.WriteVtx ( vtx );
		gfxState.WritePenColor4b ();
	}

	gfxState.EndPrim ();
}

//----------------------------------------------------------------//
void MOAIDraw::DrawVertexArray2D ( const float* verts, u32 count, u32 color, u32 primType ) {

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	gfxState.SetPenColor ( color );
	gfxState.BeginPrim ( primType, count );
	
	for ( u32 i = 0; i < count; ++i ) {
		u32 v = i << 1;
		gfxState.WriteVtx ( verts [ v ], verts [ v + 1 ], 0.0f );
		gfxState.WritePenColor4b ();
	}

	gfxState.EndPrim ();
}

//----------------------------------------------------------------//
MOAIDraw::MOAIDraw () {

	RTTI_SINGLE ( MOAIRubyObject )
}

//----------------------------------------------------------------//
MOAIDraw::~MOAIDraw () {
}

//----------------------------------------------------------------//
void MOAIDraw::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "PROJ_MATRIX",					( u32 )MOAIGfxState::VIEW_TO_CLIP_MTX );
	state.DefineClassConst ( klass, "UV_MATRIX",					( u32 )MOAIGfxState::UV_TO_MODEL_MTX );
	state.DefineClassConst ( klass, "VIEW_MATRIX",					( u32 )MOAIGfxState::WORLD_TO_VIEW_MTX );
	state.DefineClassConst ( klass, "WORLD_MATRIX",				( u32 )MOAIGfxState::MODEL_TO_WORLD_MTX );

	state.DefineStaticMethod ( klass, "bind", _bind, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "bindFrameBuffer", _bindFrameBuffer, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "bindIndexBuffer", _bindIndexBuffer, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "bindShader", _bindShader, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "bindTexture", _bindTexture, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "bindVertexArray", _bindVertexArray, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "bindVertexBuffer", _bindVertexBuffer, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "bindVertexFormat", _bindVertexFormat, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "clear", _clear, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "drawAnimCurve", _drawAnimCurve, MRB_ARGS_ARG ( 0, 2 ) );
	// state.DefineStaticMethod ( klass, "drawAxisGrid", _drawAxisGrid, MRB_ARGS_NONE () ); // TODO
	state.DefineStaticMethod ( klass, "drawBezierCurve", _drawBezierCurve, MRB_ARGS_ARG ( 0, 8 ) );
	state.DefineStaticMethod ( klass, "drawBoxOutline", _drawBoxOutline, MRB_ARGS_REQ ( 6 ) );
	state.DefineStaticMethod ( klass, "drawCircle", _drawCircle, MRB_ARGS_ARG ( 3, 1 ) );
	state.DefineStaticMethod ( klass, "drawCircleSpokes", _drawCircleSpokes, MRB_ARGS_ARG ( 3, 1 ) );
	state.DefineStaticMethod ( klass, "drawElements", _drawElements, MRB_ARGS_REQ ( 3 ) );
	state.DefineStaticMethod ( klass, "drawEllipse", _drawEllipse, MRB_ARGS_ARG ( 4, 1 ) );
	state.DefineStaticMethod ( klass, "drawEllipseSpokes", _drawEllipseSpokes, MRB_ARGS_ARG ( 4, 1 ) );
	// state.DefineStaticMethod ( klass, "drawGrid", _drawGrid, MRB_ARGS_NONE () ); // TODO
	state.DefineStaticMethod ( klass, "drawLine", _drawLine, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "drawPoints", _drawPoints, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "drawRay", _drawRay, MRB_ARGS_REQ ( 4 ) );
	state.DefineStaticMethod ( klass, "drawRect", _drawRect, MRB_ARGS_REQ ( 4 ) );
	state.DefineStaticMethod ( klass, "drawText", _drawText, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "drawTexture", _drawTexture, MRB_ARGS_REQ ( 5 ) );
	state.DefineStaticMethod ( klass, "fillCircle", _fillCircle, MRB_ARGS_ARG ( 3, 1 ) );
	state.DefineStaticMethod ( klass, "fillEllipse", _fillEllipse, MRB_ARGS_ARG ( 4, 1 ) );
	state.DefineStaticMethod ( klass, "fillFan", _fillFan, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "fillRect", _fillRect, MRB_ARGS_REQ ( 4 ) );
	state.DefineStaticMethod ( klass, "fillRoundedRect", _fillRoundedRect, MRB_ARGS_ARG ( 6, 1 ) );
	state.DefineStaticMethod ( klass, "getPenColor", _getPenColor, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "popGfxState", _popGfxState, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "pushGfxState", _pushGfxState, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "setBlendMode", _setBlendMode, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "setClearColor", _setClearColor, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "setClearDepth", _setClearDepth, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "setCullMode", _setCullMode, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "setDefaultTexture", _setDefaultTexture, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "setDepthFunc", _setDepthFunc, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "setMatrix", _setMatrix, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "setPenColor", _setPenColor, MRB_ARGS_ARG ( 3, 1 ) );
	state.DefineStaticMethod ( klass, "setPenWidth", _setPenWidth, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "setScissorRect", _setScissorRect, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "setViewProj", _setViewProj, MRB_ARGS_ARG ( 0, 2 ) );
	state.DefineStaticMethod ( klass, "setViewRect", _setViewRect, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "strokeRoundedRect", _strokeRoundedRect, MRB_ARGS_ARG ( 8, 1 ) );

}
