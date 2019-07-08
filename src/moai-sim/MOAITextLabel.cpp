// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <contrib/moai_utf8.h>
#include <moai-sim/MOAIAnimCurve.h>
#include <moai-sim/MOAICamera.h>
#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIDraw.h>
#include <moai-sim/MOAIDebugLines.h>
#include <moai-sim/MOAIFont.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIMaterialMgr.h>
#include <moai-sim/MOAINodeMgr.h>
#include <moai-sim/MOAIQuadBrush.h>
#include <moai-sim/MOAIRenderMgr.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAISim.h>
#include <moai-sim/MOAITextLayoutRules.h>
#include <moai-sim/MOAITextLabel.h>
#include <moai-sim/MOAITextStyle.h>
#include <moai-sim/MOAITextStyleParser.h>

// TODO: font garbage collection
//			- ref count glyphs
//			- glyph lifecycle
//			- glyph page defragmantation
// TODO: bitmap font ripper
// TODO: hit test for characters

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	clearHighlights
	@text	Removes all highlights currently associated with the text box.

	@in		MOAITextLabel self
	@out	nil
*/
mrb_value MOAITextLabel::_clearHighlights ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )
	
	self->mLayout.ClearHighlights ();
	self->ScheduleLayout ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	getAlignment
	@text	Returns the alignment of the text

	@in		MOAITextLabel self
	@out	number hAlign			horizontal alignment
	@out	number vAlign			vertical alignment
*/
mrb_value MOAITextLabel::_getAlignment ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( self->mLayoutRules.GetHAlign () );
	ret [ 1 ] = state.ToRValue ( self->mLayoutRules.GetVAlign () );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getGlyphScale
	@text	Returns the current glyph scale.

	@in		MOAITextLabel self
	@out	number glyphScale
*/
mrb_value MOAITextLabel::_getGlyphScale ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )

	return state.ToRValue ( self->mLayoutRules.GetGlyphScale () );
}

//----------------------------------------------------------------//
/**	@lua	getLineSpacing
	@text	Returns the spacing between lines (in pixels).

	@in		MOAITextLabel self
	@out	number lineScale		The size of the spacing in pixels.
*/
mrb_value MOAITextLabel::_getLineSpacing ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )
	
	return state.ToRValue ( self->mLayoutRules.GetLineSpacing () );
}

//----------------------------------------------------------------//
/**	@lua	getOverrunRules
	@text	Returns the overrun rules.

	@in		MOAITextLabel self
	@out	number firstOverrunRule
	@out	number overrunRule
*/
mrb_value MOAITextLabel::_getOverrunRules ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( self->mLayoutRules.GetFirstOverrunRule () );
	ret [ 1 ] = state.ToRValue ( self->mLayoutRules.GetOverrunRule () );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getRect
	@text	Returns the two-dimensional boundary of the text box (if any).

	@in		MOAITextLabel self
	@out	number xMin
	@out	number yMin
	@out	number xMax
	@out	number yMax
*/
mrb_value MOAITextLabel::_getRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )

	ZLRect rect = self->mLayoutRules.GetFrame ();

	return state.Get ( rect );
}

//----------------------------------------------------------------//
/**	@lua	getSizingRules
	@text	Returns the sizing rules.

	@in		MOAITextLabel self
	@out	number hLayoutSizingRule
	@out	number vLayoutSizingRule
	@out	number lineSizingRule
*/
mrb_value MOAITextLabel::_getSizingRules ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )
	
	mrb_value ret [ 3 ];
	ret [ 0 ] = state.ToRValue ( self->mLayoutRules.GetHLayoutSizingRule () );
	ret [ 1 ] = state.ToRValue ( self->mLayoutRules.GetVLayoutSizingRule () );
	ret [ 2 ] = state.ToRValue ( self->mLayoutRules.GetLineSizingRule () );

	return mrb_ary_new_from_values ( state, 3, ret );
}

//----------------------------------------------------------------//
/**	@lua	getStyle
	@text	Returns the style associated with a name or, if no name
			is given, returns the default style.

	@overload
	
		@in		MOAITextLabel self
		@out	MOAITextStyle defaultStyle
	
	@overload
	
		@in		MOAITextLabel self
		@in		string styleName
		@out	MOAITextStyle style
*/
mrb_value MOAITextLabel::_getStyle ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )

	MOAITextStyle* style = 0;

	cc8* styleName = state.GetParamValue < cc8* >( 1, 0 );
	style = self->mStyleCache.GetStyle ( styleName );

	if ( style ) {
		return style->PushRubyUserdata ( state );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	getText
	@text	Return the text string.

	@in		MOAITextLabel self
	@out	string text				Text string.
*/
mrb_value MOAITextLabel::_getText ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )

	if ( self->mText ) {
		return state.ToRValue ( self->mText.c_str () );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	getTextBounds
	@text	Returns the bounding rectange of a given substring on a
			single line in the local space of the text box. If 'index' and
			'size' are not passed, the bounds for all visible text will be
			returned.

	@in		MOAITextLabel self
	@opt	number index		Index of the first character in the substring.
	@opt	number size			Length of the substring.
	@out	number xMin			Edge of rect or 'nil' is no match found.
	@out	number yMin			Edge of rect or 'nil' is no match found.
	@out	number xMax			Edge of rect or 'nil' is no match found.
	@out	number yMax			Edge of rect or 'nil' is no match found.
*/
mrb_value MOAITextLabel::_getTextBounds ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )
	
	ZLRect rect;
	bool hasRect = false;
	
	self->Refresh ();
	
	if ( state.CheckParams ( 1, "NN", false )) {
	
		u32 index	= state.GetParamValue < u32 >( 1, 1 ) - 1;
		u32 size	= state.GetParamValue < u32 >( 2, 0 );
		
		hasRect = self->mLayout.GetBoundsForRange ( index, size, rect );
	}
	else {
		rect = self->mLayout.GetLayoutBounds ();
		hasRect = true;
	}
	
	if ( hasRect ) {
	
		rect.Bless ();
		
		mrb_value ret [ 4 ];
		ret [ 0 ] = state.ToRValue ( rect.mXMin );
		ret [ 1 ] = state.ToRValue ( rect.mYMin );
		ret [ 2 ] = state.ToRValue ( rect.mXMax );
		ret [ 3 ] = state.ToRValue ( rect.mYMax );

		return mrb_ary_new_from_values ( state, 4, ret );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@name	hasOverrun
    @text	Returns whether a token was truncated at the end of the text layout.
 
    @in		MOAITextBox self
    @out	boolean overrun			Only true if a token is split across layout pages.
 */
mrb_value MOAITextLabel::_hasOverrun ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )

	bool overrun = self->mOverrun;
	return state.ToRValue ( overrun );
}

//----------------------------------------------------------------//
/**	@lua	more
	@text	Returns whether there are additional pages of text below the cursor position that are not visible on the screen.

	@in		MOAITextLabel self
	@out	boolean isMore				If there is additional text below the cursor that is not visible on the screen due to clipping.
*/
mrb_value MOAITextLabel::_more ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )
	
	return state.ToRValue ( self->mMore );
}

//----------------------------------------------------------------//
/**	@lua	nextPage
	@text	Advances to the next page of text (if any) or wraps to the start of the text (if at end).

	@in		MOAITextLabel self
	@opt	boolean reveal		Default is true
	@out	nil
*/
mrb_value MOAITextLabel::_nextPage ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )

	bool reveal = state.GetParamValue < bool >( 1, true );
	self->NextPage ( reveal );
	self->ScheduleLayout ();

	return context;
}

//----------------------------------------------------------------//
/**	@lua	reserveCurves
	@text	Reserves a set of IDs for animation curves to be binding to this text object.  See setCurves.

	@in		MOAITextLabel self
	@in		number nCurves
	@out	nil
*/
mrb_value MOAITextLabel::_reserveCurves ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "UN" )

	u32 total = state.GetParamValue < u32 >( 1, 0 );
	self->mLayoutRules.ReserveCurves ( total );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	revealAll
	@text	Displays as much text as will fit in the text box.

	@in		MOAITextLabel self
	@out	nil
*/
mrb_value MOAITextLabel::_revealAll ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )
	
	self->mReveal = REVEAL_ALL;
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setAlignment
	@text	Sets the horizontal and/or vertical alignment of the text in the text box.

	@in		MOAITextLabel self
	@in		number hAlignment				Can be one of LEFT_JUSTIFY, CENTER_JUSTIFY or RIGHT_JUSTIFY.
	@in		number vAlignment				Can be one of TOP_JUSTIFY, CENTER_JUSTIFY, BOTTOM_JUSTIFY or BASELINE_JUSTIFY.
	@out	nil
*/
mrb_value MOAITextLabel::_setAlignment ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "UN" )

	self->mLayoutRules.SetHAlign ( state.GetParamValue < u32 >( 1, MOAITextLayoutRules::LEFT_JUSTIFY ));
	self->mLayoutRules.SetVAlign ( state.GetParamValue < u32 >( 2, MOAITextLayoutRules::TOP_JUSTIFY ));
	self->ScheduleLayout ();

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setAutoFlip
	@text	When autoflip is enabled, the label will be evaluated in
			screen space during rendering, and flipped vertically to
			remain 'upright' in the view.

	@in		MOAITextLabel self
	@in		boolean autoflip		Default value is false.
	@out	nil
*/
mrb_value MOAITextLabel::_setAutoFlip ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )

	self->mAutoFlip = state.GetParamValue < bool >( 1, false );
	self->ScheduleLayout ();

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setBounds
	@text	Sets or removes a bounding rectangle for the text, specified
			as the XY planes of the given bounding box. Toggles
			the rect limits accordingly.

	@overload	Set a the text label's bounding rectagle and enable the limits.

		@in		MOAITextLabel self
		@in		number xMin
		@in		number yMin
		@in		number zMin
		@in		number xMax
		@in		number yMax
		@in		number zMax
		@out	nil
	
	@overload	Clear the text label's bounding rectangle and disable the limits.

		@in		MOAITextLabel self
		@out	nil
*/
mrb_value MOAITextLabel::_setBounds ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )

	if ( state.CheckParams ( 1, "NNNNNN", false )) {

		ZLBox bounds = state.GetBox ( 1 );
		bounds.Bless ();
		
		ZLRect frame;
		frame = bounds.GetRect ( ZLBox::PLANE_XY );
		
		self->mLayoutRules.SetFrame ( frame );
		self->mLayoutRules.SetLimitWidth ( true );
		self->mLayoutRules.SetLimitHeight ( true );
	}
	else {
		self->mLayoutRules.SetLimitWidth ( false );
		self->mLayoutRules.SetLimitHeight ( false );
	}
	
	self->ScheduleLayout ();
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setCurve
	@text	Binds an animation curve to the text, where the Y value of the curve indicates the text offset, or clears the curves.

	@overload

		@in		MOAITextLabel self
		@in		number curveID				The ID of the curve within this text object.
		@in		MOAIAnimCurve curve			The MOAIAnimCurve to bind to.
		@out	nil
	
	@overload
		
		@in		MOAITextLabel self
		@out	nil
*/
mrb_value MOAITextLabel::_setCurve ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )
	
	if ( state.GetParamsCount () > 1 ) {

		u32 index = state.GetParamValue < u32 >( 1, 1 ) - 1;
	
		MOAIAnimCurve* curve = state.GetRubyObject < MOAIAnimCurve >( 2, true );
		if ( !curve ) return context;

		self->mLayoutRules.SetCurve ( index, curve );
		self->ScheduleLayout ();
	}
	else {
		self->mLayoutRules.ClearCurves ();
		self->ScheduleLayout ();
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setGlyphScale
	@text	Sets the glyph scale. This is a scalar applied to glyphs
			as they are positioned in the text box.

	@in		MOAITextLabel self
	@opt	number glyphScale		Default value is 1.
	@out	number glyphScale
*/
mrb_value MOAITextLabel::_setGlyphScale ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )
	self->mLayoutRules.SetGlyphScale ( state.GetParamValue < float >( 1, 1.0f ));
	self->ScheduleLayout ();
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setHighlight
	@text	Set or clear the highlight color of a sub string in the text.
			Only affects text displayed on the current page. Highlight
			will automatically clear when layout or page changes.

	@overload

		@in		MOAITextLabel self
		@in		number index		Index of the first character in the substring.
		@in		number size			Length of the substring.
		@in		number r
		@in		number g
		@in		number b
		@opt	number a			Default value is 1.
		@out	nil
	
	@overload
		
		@in		MOAITextLabel self
		@in		number index		Index of the first character in the substring.
		@in		number size			Length of the substring.
		@out	nil
*/
mrb_value MOAITextLabel::_setHighlight ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "UNN" )
	
	u32 index	= state.GetParamValue < u32 >( 1, 1 ) - 1;
	u32 size	= state.GetParamValue < u32 >( 2, 0 );
	
	if ( size ) {
		
		if ( state.GetParamsCount () > 3 ) {
			u32 rgba = state.GetColor32 ( 3, 1.0f, 1.0f, 1.0f, 1.0f );
			self->mLayout.SetHighlight ( index, size, rgba );
		}
		else {
			self->mLayout.SetHighlight ( index, size );
		}
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setLineSnap
	@text	Sets the snapping boundary for lines of text. Only applied during layout and in the
			text label's local space.

	@in		MOAITextLabel self
	@opt	number hLineSnap
	@opt	number vLineSnap			Default value is hLineSnap.
	@out	nil
*/
mrb_value MOAITextLabel::_setLineSnap ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )
	
	float hLineSnap = state.GetParamValue < float >( 1, 0.0f );
	float vLineSnap = state.GetParamValue < float >( 2, hLineSnap );
	
	self->mLayoutRules.SetHLineSnap ( hLineSnap );
	self->mLayoutRules.SetVLineSnap ( vLineSnap );
	self->ScheduleLayout ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setLineSpacing
	@text	Sets additional space between lines in text units. '0' uses
			the default spacing.

	@in		MOAITextLabel self
	@in		number lineSpacing		Default value is 0.
	@out	nil
*/
mrb_value MOAITextLabel::_setLineSpacing ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )
	
	float lineSpacing = state.GetParamValue < float >( 1, 0.0f );
	
	self->mLayoutRules.SetLineSpacing ( lineSpacing );
	self->ScheduleLayout ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setMargins
	@text	Sets margins to be added to sides of text layout frame (if constrained). Positive
			margins will move text toward the inside of the frame. Negative margins will
			allow text to overflow the frame.

	@in		MOAITextLabel self
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@out	nil
 */
mrb_value MOAITextLabel::_setMargins ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )
	
	ZLRect margins;
	
	margins.mXMin = state.GetParamValue < float >( 1, 0.0f );
	margins.mYMin = state.GetParamValue < float >( 2, 0.0f );
	margins.mXMax = state.GetParamValue < float >( 3, 0.0f );
	margins.mYMax = state.GetParamValue < float >( 4, 0.0f );
	
	self->mLayoutRules.SetMargins ( margins );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setOverrunRule
	@text	Control behavior of text shaper when a token needs to be wrapped.
			An alternate rule may be set for the first token on a line.

			OVERRUN_MOVE_WORD will cause the entire token to be moved to the next line.
			OVERRUN_SPLIT_WORD will split the token across lines.
			OVERRUN_TRUNCATE_WORD will discard the remaining characters in the token.
			OVERRUN_ABORT_LAYOUT will back up to the start of the token and stop the layout.
			
			OVERRUN_MOVE_WORD is not permitted as the rule for the line's first token. If the
			first token doesn't fit on current line, it will not fit on the next line and so on.

	@in		MOAITextLabel self
	@in		number firstOverrunRule		One of OVERRUN_SPLIT_WORD, OVERRUN_TRUNCATE_WORD, OVERRUN_ABORT_LAYOUT. Default value is OVERRUN_SPLIT_WORD.
	@in		number overrunRule			One of OVERRUN_MOVE_WORD, OVERRUN_SPLIT_WORD, OVERRUN_TRUNCATE_WORD, OVERRUN_ABORT_LAYOUT. Default value is OVERRUN_MOVE_WORD.
	@out	nil
*/
mrb_value MOAITextLabel::_setOverrunRules ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )

	self->mLayoutRules.SetFirstOverrunRule ( state.GetParamValue < u32 >( 1, MOAITextLayoutRules::OVERRUN_SPLIT_WORD ));
	self->mLayoutRules.SetOverrunRule ( state.GetParamValue < u32 >( 2, MOAITextLayoutRules::OVERRUN_MOVE_WORD ));

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setRect
	@text	Sets the rectangular area for this text box.

	@in		MOAITextLabel self
	@in		number x1					The X coordinate of the rect's upper-left point.
	@in		number y1					The Y coordinate of the rect's upper-left point.
	@in		number x2					The X coordinate of the rect's lower-right point.
	@in		number y2					The Y coordinate of the rect's lower-right point.
	@out	nil
*/
mrb_value MOAITextLabel::_setRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "UNNNN" )

	ZLRect rect = state.GetRect < float >( 1 );
	rect.Bless ();
	self->mLayoutRules.SetFrame ( rect );

	self->mLayoutRules.SetLimitWidth ( true );
	self->mLayoutRules.SetLimitHeight ( true );

	self->ScheduleLayout ();

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setRectLimits
	@text	Toggles width/height constraints based on the rect.

	@in		MOAITextLabel self
	@opt	boolean limitWidth		Limit text to the rect's width. Default value is 'false'.
	@opt	boolean limitHeight		Limit text to the rect's height. Default value is 'false'.
	@out	nil
*/
mrb_value MOAITextLabel::_setRectLimits ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )

	self->mLayoutRules.SetLimitWidth ( state.GetParamValue < bool >( 1, false ));
	self->mLayoutRules.SetLimitHeight ( state.GetParamValue < bool >( 2, false ));

	self->ScheduleLayout ();

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setReveal
	@text	Sets the number of renderable characters to be shown. 
			Can range from 0 to any value; values greater than the
			number of renderable characters in the current text will
			be ignored.

	@in		MOAITextLabel self
	@in		number reveal				The number of renderable characters (i.e. not whitespace) to be shown.
	@out	nil
*/
mrb_value MOAITextLabel::_setReveal ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "UN" )

	self->mReveal = state.GetParamValue < u32 >( 1, self->mReveal );
	self->mSpool = ( float )self->mReveal;

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setSizingRule
	@text	The sizing rules are used to determine the boundaries of each line for
			layout and alignment purposes. The H and V layout sizing rules determine whether
			the layout is based on the tight-fitting glyph boundaries or the (possibly) looser
			fitting 'logical' bounds (i.e. the bounds specified by the type face, allowing for
			full ascent and descent). The line sizing rule determines whether lines are spaced
			from logical or glyph bounds. Note that these rules do not affect the spacing of
			glyphs as they are laid out within a line, but they do affect the spacing of lines.

	@in		MOAITextLabel self
	@in		number hLayoutSizingRule		One of LOGICAL_SIZE or GLYPH_SIZE. Default is LOGICAL_SIZE.
	@in		number vLayoutSizingRule		One of LOGICAL_SIZE or GLYPH_SIZE. Default is LOGICAL_SIZE.
	@in		number lineSizingRule			One of LOGICAL_SIZE or GLYPH_SIZE. Default is LOGICAL_SIZE.
	@out	nil
*/
mrb_value MOAITextLabel::_setSizingRules ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )

	self->mLayoutRules.SetHLayoutSizingRule ( state.GetParamValue < u32 >( 1, MOAITextLayoutRules::LOGICAL_SIZE ));
	self->mLayoutRules.SetVLayoutSizingRule ( state.GetParamValue < u32 >( 1, MOAITextLayoutRules::LOGICAL_SIZE ));
	self->mLayoutRules.SetLineSizingRule ( state.GetParamValue < u32 >( 2, MOAITextLayoutRules::LOGICAL_SIZE ));

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setSpeed
	@text	Sets the base spool speed used when creating a spooling MOAIAction with the spool() function.

	@in		MOAITextLabel self
	@in		number speed		The base spooling speed.
	@out	nil
*/
mrb_value MOAITextLabel::_setSpeed ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "UN" )
	
	self->mSpeed = state.GetParamValue < float >( 1, self->mSpeed );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setText
	@text	Sets the text string to be displayed by this textbox.

	@in		MOAITextLabel self
	@in		string newStr				The new text string to be displayed.
	@out	nil
*/
mrb_value MOAITextLabel::_setText ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "US" )

	cc8* text = state.GetParamValue < cc8* >( 1, "" );
	self->SetText ( text );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setStyle
	@text	Attaches a style to the textbox and associates a name with it.
			If no name is given, sets the default style.

	@overload
	
		@in		MOAITextLabel self
		@in		MOAITextStyle defaultStyle
		@out	nil
	
	@overload
	
		@in		MOAITextLabel self
		@in		string styleName
		@in		MOAITextStyle style
		@out	nil
*/
mrb_value MOAITextLabel::_setStyle ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )

	cc8* styleName = state.GetParamValue < cc8* >( 1, "" );
	
	if ( strlen ( styleName )) {
	
		MOAITextStyle* style = state.GetRubyObject < MOAITextStyle >( 2, true );
		self->mStyleCache.SetStyle ( styleName, style );
	}
	else {
	
		MOAITextStyle* style = state.GetRubyObject < MOAITextStyle >( 1, true );
		self->mStyleCache.SetStyle ( style );
	}
	
	self->ScheduleLayout ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setWordBreak
	@text	Sets the rule for breaking words across lines.

	@in		MOAITextLabel self
	@opt	number rule				One of MOAITextLabel.WORD_BREAK_NONE, MOAITextLabel.WORD_BREAK_CHAR.
									Default is MOAITextLabel.WORD_BREAK_NONE.
	@out	nil
*/
mrb_value MOAITextLabel::_setWordBreak ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )

	//self->mLayoutRules.SetWordBreakRule ( state.GetParamValue < u32 >( 2, MOAITextLayoutRules::WORD_BREAK_NONE ));
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setYFlip
	@text	Sets the rendering direction for the text. Default assumes
			a window style screen space (positive Y moves down the screen). Set
			to true to render text for world style coordinate systems (positive
			Y moves up the screen).

	@in		MOAITextLabel self
	@in		boolean yFlip				Whether the vertical rendering direction should be inverted.
	@out	nil
*/
mrb_value MOAITextLabel::_setYFlip ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "UB" )

	self->mLayoutRules.SetYFlip ( state.GetParamValue < bool >( 1, false ));
	self->ScheduleLayout ();

	return context;
}

//----------------------------------------------------------------//
/**	@lua	spool
	@text	Creates a new MOAIAction which when run has the effect of increasing
			the amount of characters revealed from 0 to the length of the string
			currently set.  The spool action is automatically added to the root
			of the action tree, but may be reparented or stopped by the developer.
			This function also automatically sets the current number of revealed
			characters to 0 (i.e. MOAITextLabel:setReveal(0)).

	@in		MOAITextLabel self
	@out	MOAIAction action			The new MOAIAction which spools the text when run.
*/
mrb_value MOAITextLabel::_spool ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITextLabel, "U" )

	self->mReveal = state.GetParamValue < u32 >( 1, 0 );
	self->mSpool = ( float )self->mReveal;

	self->Start ( 0, false );

	return context;
}

//================================================================//
// DOXYGEN
//================================================================//

#ifdef DOXYGEN

	//----------------------------------------------------------------//
	/**	@lua	affirmStyle
		@text	Returns the textbox's default style. If no default style
				exists, creates an empty style, sets it as the default and
				returns it.

		@in		MOAITextLabel self
		@out	MOAITextStyle style
	*/
	mrb_value MOAITextLabel::_affirmStyle ( mrb_state* M, mrb_value context ) {
	}
	
	//----------------------------------------------------------------//
	/**	@lua	setFont
		@text	Sets the font to be used by the textbox's default style.
				If no default style exists, a default style is created.

		@in		MOAITextLabel self
		@in		MOAIFont font
		@out	nil
	*/
	mrb_value MOAITextLabel::_setFont ( mrb_state* M, mrb_value context ) {
	}
	
	//----------------------------------------------------------------//
	/**	@lua	setTextSize
		@text	Sets the size to be used by the textbox's default style.
				If no default style exists, a default style is created.

		@in		MOAITextLabel self
		@in		number points			The point size to be used by the default style.
		@opt	number dpi				The device DPI (dots per inch of device screen). Default value is 72 (points same as pixels).
		@out	nil
	*/
	mrb_value MOAITextLabel::_setTextSize ( mrb_state* M, mrb_value context ) {
	}

#endif

//================================================================//
// MOAITextLabel
//================================================================//

const float MOAITextLabel::DEFAULT_SPOOL_SPEED = 24.0f;

//----------------------------------------------------------------//
MOAITextLabel::MOAITextLabel () :
	mNeedsLayout ( false ),
	mSpool ( 0.0f ),
	mSpeed ( DEFAULT_SPOOL_SPEED ),
	mReveal ( REVEAL_ALL ),
	mCurrentPageIdx ( 0 ),
	mNextPageIdx ( 0 ),
	mMore ( false ),
	mOverrun ( false ),
	mAutoFlip ( false ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIAction )
		RTTI_EXTEND ( MOAIGraphicsProp )
	RTTI_END
	
	this->mStyleCache.SetOwner ( this );
	this->mLayoutRules.SetOwner ( this );
}

//----------------------------------------------------------------//
MOAITextLabel::~MOAITextLabel () {

	this->mLayout.ClearHighlights ();
	this->ResetLayout ();
}

//----------------------------------------------------------------//
bool MOAITextLabel::More () {
	
	this->Refresh ();
	
	if ( this->mReveal < this->mLayout.CountSprites ()) {
		return true;
	}
	return this->mMore;
}

//----------------------------------------------------------------//
void MOAITextLabel::NextPage ( bool reveal ) {
	
	if ( this->mMore ) {
		this->mCurrentPageIdx = this->mNextPageIdx;
	}
	else {
		this->mCurrentPageIdx = 0;
		this->mNextPageIdx = 0;
	}
	
	this->mReveal = reveal ? REVEAL_ALL : 0;
	this->mSpool = 0.0f;
}

//----------------------------------------------------------------//
void MOAITextLabel::Refresh () {

	if ( this->mStyleCache.CheckStylesChanged ()) {
		this->mNeedsLayout = true;
		this->RefreshStyleGlyphs ();
	}

	if ( this->mNeedsLayout ) {
		this->RefreshLayout ();
		this->mNeedsLayout = false;
	}
}

//----------------------------------------------------------------//
void MOAITextLabel::RefreshLayout () {

	this->mLayout.Reset ();
	this->mStyleCache.ClearAnonymousStyles ();

	this->mStyleMap.BuildStyleMap ( this->mStyleCache, this->mText.c_str ());

	this->mLayoutRules.Layout ( this->mLayout, this->mStyleCache, this->mStyleMap, this->mText.c_str (), this->mCurrentPageIdx, &this->mMore, &this->mNextPageIdx, &this->mOverrun );
}

//----------------------------------------------------------------//
void MOAITextLabel::RefreshStyleGlyphs () {

	this->mStyleMap.RefreshStyleGlyphs ( this->mText.c_str ());
}

//----------------------------------------------------------------//
void MOAITextLabel::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIGraphicsProp::RegisterRubyClass ( state, klass );
	MOAIAction::RegisterRubyClass ( state, klass );

	MOAIDebugLinesMgr::Get ().ReserveStyleSet < MOAITextLabel >( TOTAL_DEBUG_LINE_STYLES );

	state.DefineClassConst ( klass, "DEBUG_DRAW_TEXT_LABEL_MASTER",				MOAIDebugLinesMgr::Pack < MOAITextLabel >( (u32) -1 ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_TEXT_LABEL",						MOAIDebugLinesMgr::Pack < MOAITextLabel >( DEBUG_DRAW_TEXT_LABEL ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_TEXT_LABEL_BASELINES",				MOAIDebugLinesMgr::Pack < MOAITextLabel >( DEBUG_DRAW_TEXT_LABEL_BASELINES ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_TEXT_LABEL_GLYPH_BOUNDS",			MOAIDebugLinesMgr::Pack < MOAITextLabel >( DEBUG_DRAW_TEXT_LABEL_GLYPH_BOUNDS ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_TEXT_LABEL_GLYPHS",				MOAIDebugLinesMgr::Pack < MOAITextLabel >( DEBUG_DRAW_TEXT_LABEL_GLYPHS ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_TEXT_LABEL_LAYOUT_BOUNDS",			MOAIDebugLinesMgr::Pack < MOAITextLabel >( DEBUG_DRAW_TEXT_LABEL_LAYOUT_BOUNDS ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_TEXT_LABEL_LIMITS",				MOAIDebugLinesMgr::Pack < MOAITextLabel >( DEBUG_DRAW_TEXT_LABEL_LIMITS ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_TEXT_LABEL_LINES_GLYPH_BOUNDS",	MOAIDebugLinesMgr::Pack < MOAITextLabel >( DEBUG_DRAW_TEXT_LABEL_LINES_GLYPH_BOUNDS ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_TEXT_LABEL_LINES_LAYOUT_BOUNDS",	MOAIDebugLinesMgr::Pack < MOAITextLabel >( DEBUG_DRAW_TEXT_LABEL_LINES_LAYOUT_BOUNDS ));
	state.DefineClassConst ( klass, "DEBUG_DRAW_TEXT_LABEL_MARGINS",				MOAIDebugLinesMgr::Pack < MOAITextLabel >( DEBUG_DRAW_TEXT_LABEL_MARGINS));

	state.DefineClassConst ( klass, "OVERRUN_MOVE_WORD",		( u32 )MOAITextLayoutRules::OVERRUN_MOVE_WORD );
	state.DefineClassConst ( klass, "OVERRUN_SPLIT_WORD",		( u32 )MOAITextLayoutRules::OVERRUN_SPLIT_WORD );
	state.DefineClassConst ( klass, "OVERRUN_TRUNCATE_WORD",	( u32 )MOAITextLayoutRules::OVERRUN_TRUNCATE_WORD );
	state.DefineClassConst ( klass, "OVERRUN_ABORT_LAYOUT",	( u32 )MOAITextLayoutRules::OVERRUN_ABORT_LAYOUT );

	state.DefineClassConst ( klass, "GLYPH_SIZE",				( u32 )MOAITextLayoutRules::GLYPH_SIZE );
	state.DefineClassConst ( klass, "LOGICAL_SIZE",			( u32 )MOAITextLayoutRules::LOGICAL_SIZE );

	state.DefineClassConst ( klass, "BASELINE_JUSTIFY",		( u32 )MOAITextLayoutRules::BASELINE_JUSTIFY );
	state.DefineClassConst ( klass, "BOTTOM_JUSTIFY",			( u32 )MOAITextLayoutRules::BOTTOM_JUSTIFY );
	state.DefineClassConst ( klass, "LEFT_JUSTIFY",			( u32 )MOAITextLayoutRules::LEFT_JUSTIFY );
	state.DefineClassConst ( klass, "CENTER_JUSTIFY",			( u32 )MOAITextLayoutRules::CENTER_JUSTIFY );
	state.DefineClassConst ( klass, "RIGHT_JUSTIFY",			( u32 )MOAITextLayoutRules::RIGHT_JUSTIFY );
	state.DefineClassConst ( klass, "TOP_JUSTIFY",				( u32 )MOAITextLayoutRules::TOP_JUSTIFY );
}

//----------------------------------------------------------------//
void MOAITextLabel::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIGraphicsProp::RegisterRubyFuncs ( state, klass );
	MOAIAction::RegisterRubyFuncs ( state, klass );
	
	state.DefineInstanceMethod ( klass, "clearHighlights",		_clearHighlights, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getAlignment",			_getAlignment, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getGlyphScale",		_getGlyphScale, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getLineSpacing",		_getLineSpacing, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getOverrunRules",		_getOverrunRules, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getRect",				_getRect, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getSizingRules",		_getSizingRules, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getStyle",				_getStyle, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "getText",				_getText, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getTextBounds",		_getTextBounds, MRB_ARGS_ARG ( 0, 2 ) );
	state.DefineInstanceMethod ( klass, "hasOverrun",			_hasOverrun, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "more",					_more, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "nextPage",				_nextPage, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "reserveCurves",		_reserveCurves, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "revealAll",			_revealAll, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setAlignment",			_setAlignment, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setAutoFlip",			_setAutoFlip, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setBounds",			_setBounds, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "setCurve",				_setCurve, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "setGlyphScale",		_setGlyphScale, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setLineSnap",			_setLineSnap, MRB_ARGS_ARG ( 0, 2 ) );
	state.DefineInstanceMethod ( klass, "setLineSpacing",		_setLineSpacing, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setHighlight",			_setHighlight, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "setMargins",			_setMargins, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "setOverrunRules",		_setOverrunRules, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setRect",				_setRect, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "setRectLimits",		_setRectLimits, MRB_ARGS_ARG ( 0, 2 ) );
	state.DefineInstanceMethod ( klass, "setReveal",			_setReveal, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setSizingRules",		_setSizingRules, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "setSpeed",				_setSpeed, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setStyle",				_setStyle, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "setText",				_setText, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setWordBreak",			_setWordBreak, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setYFlip",				_setYFlip, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "spool",				_spool, MRB_ARGS_NONE () );

}

//----------------------------------------------------------------//
void MOAITextLabel::ResetLayout () {

	this->mMore = false;
	this->mOverrun = false;
	this->mLayout.Reset ();
}

//----------------------------------------------------------------//
void MOAITextLabel::ScheduleLayout () {

	this->mNeedsLayout = true;
	this->ScheduleUpdate ();
}

//----------------------------------------------------------------//
void MOAITextLabel::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {

	MOAIGraphicsProp::SerializeIn ( state, serializer );
	MOAIAction::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAITextLabel::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {

	MOAIGraphicsProp::SerializeOut ( state, serializer );
	MOAIAction::SerializeOut ( state, serializer );
}

//----------------------------------------------------------------//
void MOAITextLabel::SetText ( cc8* text ) {

	this->mText = text;
	this->mMore = ( text && text [ 0 ]);
	this->mOverrun = this->mMore;
	
	this->mReveal = REVEAL_ALL;
	this->mSpool = 0.0f;
	
	this->mCurrentPageIdx = 0;
	this->mNextPageIdx = 0;
	
	this->ScheduleLayout ();
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAITextLabel::MOAIAction_IsDone () {

	if ( this->IsActive ()) {
		this->RefreshLayout ();
		return ( this->mReveal >= this->mLayout.CountSprites ());
	}
	return true;
}

//----------------------------------------------------------------//
void MOAITextLabel::MOAIAction_Update ( double step ) {
	
	this->mSpool += ( float )( this->mSpeed * step );
	this->mReveal = ( u32 )this->mSpool;
}

//----------------------------------------------------------------//
void MOAITextLabel::MOAIDrawable_Draw ( int subPrimID ) {
	UNUSED ( subPrimID );
	
	if ( !this->IsVisible ()) return;
	if ( this->IsClear ()) return;
	
	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	if ( this->mDeck ) {
	
		u32 idx = this->mIndex - 1;
	
		ZLBounds fitBounds = this->mDeck->GetBounds ( idx );
		
		if ( fitBounds.mStatus == ZLBounds::ZL_BOUNDS_OK ) {
	
			float width = fitBounds.Width ();
			float height = fitBounds.Height ();
	
			if (( width > 0.0f ) && ( height > 0.0f )) {
	
				ZLRect layoutFrame = this->mLayoutRules.GetFrame ();
	
				float xFit = layoutFrame.Width () / width;
				float yFit = layoutFrame.Height () / height;
	
				this->PushGfxState ();

				ZLMatrix4x4 fit;
				fit.Scale ( xFit, -1.0f * yFit, 1.0f );
				
				ZLMatrix4x4 worldDrawingMtx = MOAIGraphicsProp::MOAIGraphicsPropBase_GetWorldDrawingMtx ();
				worldDrawingMtx.Prepend ( fit );
			
				gfxState.SetMtx ( MOAIGfxState::MODEL_TO_WORLD_MTX, worldDrawingMtx );
			
				this->LoadUVTransform ();
				this->mDeck->Draw ( idx );
				this->PopGfxState ();
			}
		}
	}
	
	if ( this->mReveal ) {

		this->PushGfxState ();
		this->LoadVertexTransform ();
		this->LoadUVTransform ();
		
		MOAIMaterialMgr& materialStack = MOAIMaterialMgr::Get ();
		materialStack.LoadGfxState ();
	
		gfxState.SetVertexTransform ( MOAIGfxState::MODEL_TO_CLIP_MTX );
		gfxState.SetUVTransform ( MOAIGfxState::UV_TO_MODEL_MTX );

		this->mLayout.Draw ( this->mReveal );

		this->PopGfxState ();
	}
}

//----------------------------------------------------------------//
void MOAITextLabel::MOAIDrawable_DrawDebug ( int subPrimID ) {
	UNUSED ( subPrimID );

	if ( !this->IsVisible ()) return;

	MOAIGraphicsProp::MOAIDrawable_DrawDebug ( subPrimID );

	MOAIDebugLinesMgr& debugLines = MOAIDebugLinesMgr::Get ();
	if ( !( debugLines.IsVisible () && debugLines.SelectStyleSet < MOAITextLabel >())) return;

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	ZLMatrix4x4 worldDrawingMtx = this->MOAIGraphicsPropBase_GetWorldDrawingMtx ();
	
	gfxState.SetMtx ( MOAIGfxState::MODEL_TO_WORLD_MTX, worldDrawingMtx );
	gfxState.SetVertexTransform ( MOAIGfxState::MODEL_TO_CLIP_MTX );
	
	this->mLayout.DrawDebug ();
	
	MOAIDraw& draw = MOAIDraw::Get ();
	UNUSED ( draw ); // mystery warning in vs2008
	draw.Bind ();
		
	if (( this->mLayout.mLayoutBounds.Area () > 0.0f ) && debugLines.Bind ( DEBUG_DRAW_TEXT_LABEL_LAYOUT_BOUNDS )) {
		
		draw.DrawRectOutline ( this->mLayout.mLayoutBounds );
	}
	
	if (( this->mLayout.mGlyphBounds.Area () > 0.0f ) && debugLines.Bind ( DEBUG_DRAW_TEXT_LABEL_GLYPH_BOUNDS )) {
		
		draw.DrawRectOutline ( this->mLayout.mGlyphBounds );
	}
	
	ZLRect frame = this->mLayoutRules.GetFrame ();
	
	if ( frame.Area () > 0.0f ) {
	
		frame.Offset ( -this->mLayout.mXOffset, -this->mLayout.mYOffset );
		
		if ( debugLines.Bind ( DEBUG_DRAW_TEXT_LABEL_MARGINS )) {
		
			ZLRect margins = this->mLayoutRules.GetMargins ();
		
			draw.DrawLine ( frame.mXMin + margins.mXMin, frame.mYMin, frame.mXMin + margins.mXMin, frame.mYMax );
			draw.DrawLine ( frame.mXMax - margins.mXMax, frame.mYMin, frame.mXMax - margins.mXMax, frame.mYMax );
			
			draw.DrawLine ( frame.mXMin, frame.mYMin + margins.mYMin, frame.mXMax, frame.mYMin + margins.mYMin );
			draw.DrawLine ( frame.mXMin, frame.mYMax - margins.mYMax, frame.mXMax, frame.mYMax - margins.mYMax );
		}
		
		if ( debugLines.Bind ( DEBUG_DRAW_TEXT_LABEL )) {
		
			draw.DrawRectOutline ( frame );
		}
		
		if ( debugLines.Bind ( DEBUG_DRAW_TEXT_LABEL_LIMITS )) {
			
			if ( this->mLayoutRules.GetLimitHeight ()) {
				draw.DrawLine ( frame.mXMin, frame.mYMin, frame.mXMax, frame.mYMin );
				draw.DrawLine ( frame.mXMin, frame.mYMax, frame.mXMax, frame.mYMax );
			}
			
			if ( this->mLayoutRules.GetLimitWidth ()) {
				draw.DrawLine ( frame.mXMin, frame.mYMin, frame.mXMin, frame.mYMax );
				draw.DrawLine ( frame.mXMax, frame.mYMin, frame.mXMax, frame.mYMax );
			}
		}
	}
}

//----------------------------------------------------------------//
ZLMatrix4x4 MOAITextLabel::MOAIGraphicsPropBase_GetWorldDrawingMtx () {

	ZLMatrix4x4 worldDrawingMtx = MOAIGraphicsProp::MOAIGraphicsPropBase_GetWorldDrawingMtx ();
	
	if ( this->mAutoFlip ) {
		
		ZLMatrix4x4 viewProj = MOAIGfxMgr::Get ().mGfxState.GetMtx ( MOAIGfxState::WORLD_TO_CLIP_MTX );

		ZLVec3D upVec = worldDrawingMtx.GetYAxis ();

		viewProj.TransformVec ( upVec );

		// For text flipping when orbiting. Tilting should not affect this
		if ( upVec.mY > 0.0f ) {

			ZLMatrix4x4 flip;
			flip.Scale ( -1.0f, -1.0f, 1.0f );
			
			// if there's no x-axis constraint, flip inside the glyph rect
			if ( !this->mLayoutRules.GetLimitWidth ()) {
				float xOffset = this->mLayout.mGlyphBounds.mXMin + this->mLayout.mGlyphBounds.mXMax;
				flip.m [ ZLMatrix4x4::C3_R0 ] = xOffset;
			}
			
			// if there's no y-axis constraint, flip inside the glyph rect
			if ( !this->mLayoutRules.GetLimitHeight ()) {
				float yOffset = this->mLayout.mGlyphBounds.mYMin + this->mLayout.mGlyphBounds.mYMax;
				flip.m [ ZLMatrix4x4::C3_R1 ] = yOffset;
			}
			worldDrawingMtx.Prepend ( flip );
		}
	}
	
	return worldDrawingMtx;
}

//----------------------------------------------------------------//
void MOAITextLabel::MOAINode_Update () {

	this->Refresh ();
	MOAIGraphicsProp::MOAINode_Update ();
}

//----------------------------------------------------------------//
ZLBounds MOAITextLabel::MOAIPartitionHull_GetModelBounds () {
	
	this->Refresh ();

	ZLBounds bounds = ZLBounds::EMPTY;

	ZLRect textBounds; // the tight fitting bounds of the text (if any: may be empty)
	bool hasBounds = this->mLayout.GetBounds ( textBounds );
	
	ZLRect textFrame = this->mLayoutRules.GetFrame ();
	bool limitWidth = this->mLayoutRules.GetLimitWidth ();
	bool limitHeight = this->mLayoutRules.GetLimitHeight ();
	
	if ( hasBounds ) {
	
		if ( limitWidth ) {
			textBounds.mXMin = textFrame.mXMin;
			textBounds.mXMax = textFrame.mXMax;
		}
		
		if ( limitHeight ) {
			textBounds.mYMin = textFrame.mYMin;
			textBounds.mYMax = textFrame.mYMax;
		}
		
		bounds.Init ( textBounds );
	}
	else {
	
		// if the text bounds are empty, then *both* frame axis must be in use for the rect to be valid
		if ( limitWidth && limitHeight ) {
			bounds.Init ( textFrame );
		}
	}
	
	return bounds;
}

//----------------------------------------------------------------//
void MOAITextLabel::MOAITransformBase_BuildLocalToWorldMtx ( ZLAffine3D& localToWorldMtx ) {

	this->MOAITransform::MOAITransformBase_BuildLocalToWorldMtx ( localToWorldMtx );

	// do yFlip here so hit test on glyphs will work
	float yScale = this->mLayoutRules.GetYFlip () ? -1.0f : 1.0f;

	ZLAffine3D mtx;
	mtx.ScRoTr ( 1.0f, yScale, 1.0f, 0.0f, 0.0f, 0.0f, this->mLayout.mXOffset, this->mLayout.mYOffset, 0.0f );
	localToWorldMtx.Prepend ( mtx );
}
