// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAITEXTLABEL_H
#define	MOAITEXTLABEL_H

#include <moai-sim/MOAIAction.h>
#include <moai-sim/MOAIGraphicsPropBase.h>
#include <moai-sim/MOAITextLayoutRules.h>
#include <moai-sim/MOAITextLayout.h>
#include <moai-sim/MOAITextStyle.h>
#include <moai-sim/MOAITextStyleCache.h>
#include <moai-sim/MOAITextStyleMap.h>

class MOAIAnimCurve;
class MOAIFont;

//================================================================//
// MOAITextLabel
//================================================================//
/**	@lua	MOAITextLabel
	@text	<p>The text box manages styling, laying out and displaying text.
			You can attach named styles to the text box to be applied to
			the text using style escapes. You can also inline style
			escapes to control color. Style escapes may be nested.</p>
			
			<p>To attach a style to a text box use setStyle (). If you
			provide a name for the style then the style may be applied
			by name using a style escape. If you do not provide a name
			then the style will be used as the default style for
			the text box. The default style is the style that will be
			used when no style escapes are in effect.</p>
			
			<p>The setFont () and setSize () methods are helpers that
			operate on the text box's default style. If no default
			style exists when these methods are called, one will be
			created.</p>
			
			<p>There are three kinds of text escapes. The first takes
			the form of <styleName> where 'styleName' is the name
			of the style you provided via setStyle (). If there is no
			match for the name then the default style will be used.</p>
			
			<p>The second form of style escape sets text color. It
			takes the form of <c:XXX> where 'XXX' is a hexadecimal
			number representing a color value. The hexadecimal number
			may be have from one up to eight digits, excluding five
			digit numbers. One and two digit numbers correspond to grayscale
			values of 4 and 8 bits of precision (16 or 256 levels) respectively.
			Three and four digit numbers represent RGB and RGBA colors at
			4 bit precision. Six digits is RGB at 8 bits of precision.
			Seven digits is RGBA with 8 bits for RGB and 4 bits for A.
			Eight digits is RGBA with 8 bits for each component.</p>
			
			<p>The final text escapes ends the current escape. It takes the
			form of </>. Including any additional text in this kind of escape
			is an error.</p>
			
			<p>You may escape the '<' symbol itself by using an additional '<'.
			For example, '<<' will output '<'. '<<test>' will short circuit
			the style escape and output '<test>' in the displayed text.</p>
			
			<p>When using MOAITextLabel with MOAIFont it's important to
			understand how and when glyphs are rendered. When you call
			setText () the text box's style goes to work. The entire
			string you provide is scanned and a 'style span' is created
			for each uniquely styled block of text. If you do not use
			any styles then there will be only one style span.</p>
			
			<p>Once the text style has created style spans for your text,
			the spans themselves are scanned. Each span must specify
			a font to be used. All of the characters in the span are 'affirmed'
			by the font: if the glyphs for the characters have already been
			ripped then nothing happens. If not, the characters are enqueued
			by the font to have their glyphs ripped.</p>
			
			<p>Finally, we iterate through all of the fonts used by the text
			and instruct them to load and render any pending glyphs. If
			the font is dynamic and has a valid implementation of MOAIFontReader
			and MOAIDynamicGlyphCache attached to it then the glyphs will be rendered
			and placed in the cache.</p>
			
			<p>Once the glyphs have been rendered, we know their metrics and
			(hopefully) have valid textures for them. We can now lay out
			an actual page of text. This is done by a separate subsystem known
			as the text designer. The text designer reads the style spans and
			uses the associated font, color and size information to place
			the glyphs into a layout.</p>
			
			<p>If the text associated with the textbox doesn't fit, then the
			textbox will have multiple pages. The only method that deals with
			pages at this time is nextPage (). Additional methods giving
			finer control over multi-page text boxes will be provided in a
			future release.</p>
			
			<p>There are some additional ways you can use the text box to style
			your text. The current implementation supports left, center and right
			positioning as well as top, center and bottom positioning. A future
			implementation will include justification in which words and lines
			of text will be spaced out to align with the edges of the text box.</p>
			
			<p>You can also attach MOAIAnimCurves to the text box. The animation curves
			may be used to offset characters in lines of text. Each curve may
			have any number of keyframes, but only the span between t0 and t1
			is used by the text box, regardless of its width. Curves correspond
			to lines of text. If there are more lines of text than curves,
			the curves will simply repeat.</p>
			
			<p>Once you've loaded text into the text box you can apply highlight colors.
			These colors will override any colors specified by style escapes.
			Highlight spans may be set or cleared using setHighlight ().
			clearHighlights () will remove all highlights from the text.
			Highlights will persists from page to page of text, but will be
			lost if new text is loaded by calling setText ().</p>
	
	@const	BASELINE_JUSTIFY
	@const	BOTTOM_JUSTIFY
	@const	LEFT_JUSTIFY
	@const	CENTER_JUSTIFY
	@const	RIGHT_JUSTIFY
	@const	TOP_JUSTIFY
	
	@const	WORD_BREAK_NONE
	@const	WORD_BREAK_CHAR
*/
class MOAITextLabel :
	public MOAIGraphicsProp,
	public MOAIAction {
private:

	static const u32 REVEAL_ALL = 0xffffffff;
	static const float DEFAULT_SPOOL_SPEED;
	
	bool					mNeedsLayout;
	
	float					mSpool;
	float					mSpeed;
	float					mThrottle;
	u32						mReveal;
	
	u32						mCurrentPageIdx;
	u32						mNextPageIdx;
	bool					mMore;
	bool					mOverrun;

	MOAITextLayoutRules		mLayoutRules;	// design params for laying out text
	MOAITextStyleCache		mStyleCache;	// anonymous styles are created ad hoc
	MOAITextStyleMap		mStyleMap;		// preprocessed text broken up by style changes
	MOAITextLayout			mLayout;		// cached glyph layout for currently visible text
	
	STLString				mText;
	
	bool					mAutoFlip;
	
	//----------------------------------------------------------------//
	static mrb_value			_clearHighlights		( mrb_state* M, mrb_value context );
	static mrb_value			_getAlignment			( mrb_state* M, mrb_value context );
	static mrb_value			_getGlyphScale			( mrb_state* M, mrb_value context );
	static mrb_value			_getLineSpacing			( mrb_state* M, mrb_value context );
	static mrb_value			_getOverrunRules		( mrb_state* M, mrb_value context );
	static mrb_value			_getRect				( mrb_state* M, mrb_value context );
	static mrb_value			_getSizingRules			( mrb_state* M, mrb_value context );
	static mrb_value			_getStyle				( mrb_state* M, mrb_value context );
	static mrb_value			_getText				( mrb_state* M, mrb_value context );
	static mrb_value			_getTextBounds			( mrb_state* M, mrb_value context );
	static mrb_value			_hasOverrun				( mrb_state* M, mrb_value context );
	static mrb_value			_more					( mrb_state* M, mrb_value context );
	static mrb_value			_nextPage				( mrb_state* M, mrb_value context );
	static mrb_value			_revealAll				( mrb_state* M, mrb_value context );
	static mrb_value			_reserveCurves			( mrb_state* M, mrb_value context );
	static mrb_value			_setAlignment			( mrb_state* M, mrb_value context );
	static mrb_value			_setAutoFlip			( mrb_state* M, mrb_value context );
	static mrb_value			_setBounds				( mrb_state* M, mrb_value context );
	static mrb_value			_setCurve				( mrb_state* M, mrb_value context );
	static mrb_value			_setGlyphScale			( mrb_state* M, mrb_value context );
	static mrb_value			_setHighlight			( mrb_state* M, mrb_value context );
	static mrb_value			_setLineSnap			( mrb_state* M, mrb_value context );
	static mrb_value			_setLineSpacing			( mrb_state* M, mrb_value context );
	static mrb_value			_setMargins				( mrb_state* M, mrb_value context );
	static mrb_value			_setOverrunRules		( mrb_state* M, mrb_value context );
	static mrb_value			_setRect				( mrb_state* M, mrb_value context );
	static mrb_value			_setRectLimits			( mrb_state* M, mrb_value context );
	static mrb_value			_setReveal				( mrb_state* M, mrb_value context );
	static mrb_value			_setSpeed				( mrb_state* M, mrb_value context );
	static mrb_value			_setSizingRules			( mrb_state* M, mrb_value context );
	static mrb_value			_setStyle				( mrb_state* M, mrb_value context );
	static mrb_value			_setText				( mrb_state* M, mrb_value context );
	static mrb_value			_setWordBreak			( mrb_state* M, mrb_value context );
	static mrb_value			_setYFlip				( mrb_state* M, mrb_value context );
	static mrb_value			_spool					( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	#ifdef DOXYGEN
		static mrb_value		_affirmStyle			( mrb_state* M, mrb_value context );
		static mrb_value		_setFont				( mrb_state* M, mrb_value context );
		static mrb_value		_setTextSize			( mrb_state* M, mrb_value context );
	#endif
	
	//----------------------------------------------------------------//
	void				ResetLayout				();
	void				ScheduleLayout			();
	void				Refresh					();
	virtual void		RefreshLayout			();
	virtual void		RefreshStyleGlyphs		();

	//----------------------------------------------------------------//
	bool				MOAIAction_IsDone							();
	void				MOAIAction_Update							( double step );
	void				MOAIDrawable_Draw							( int subPrimID );
	void				MOAIDrawable_DrawDebug						( int subPrimID );
	ZLMatrix4x4			MOAIGraphicsPropBase_GetWorldDrawingMtx		();
	void				MOAINode_Update								();
	ZLBounds			MOAIPartitionHull_GetModelBounds			();
	void				MOAITransformBase_BuildLocalToWorldMtx		( ZLAffine3D& localToWorldMtx );

public:
	
	DECL_RUBY_FACTORY ( MOAITextLabel, MOAIGraphicsProp )
	
	enum {
		DEBUG_DRAW_TEXT_LABEL,
		DEBUG_DRAW_TEXT_LABEL_BASELINES,
		DEBUG_DRAW_TEXT_LABEL_GLYPH_BOUNDS,
		DEBUG_DRAW_TEXT_LABEL_GLYPHS,
		DEBUG_DRAW_TEXT_LABEL_LAYOUT_BOUNDS,
		DEBUG_DRAW_TEXT_LABEL_LIMITS,
		DEBUG_DRAW_TEXT_LABEL_LINES_GLYPH_BOUNDS,
		DEBUG_DRAW_TEXT_LABEL_LINES_LAYOUT_BOUNDS,
		DEBUG_DRAW_TEXT_LABEL_MARGINS,
		TOTAL_DEBUG_LINE_STYLES,
	};
	
	//----------------------------------------------------------------//
						MOAITextLabel			();
						~MOAITextLabel			();
	bool				More					();
	void				NextPage				( bool reveal );
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void				SerializeIn				( MOAIRubyState& state, MOAIDeserializer& serializer );
	void				SerializeOut			( MOAIRubyState& state, MOAISerializer& serializer );
	void				SetText					( cc8* text );
};

#endif
