
MOAISim.openWindow( "test", 320, 480 )

MOAIDebugLinesMgr.setStyle( MOAITextLabel::DEBUG_DRAW_TEXT_LABEL, 1, 0, 1, 1, 1 )
MOAIDebugLinesMgr.setStyle( MOAITextLabel::DEBUG_DRAW_TEXT_LABEL_LIMITS, 2, 1, 1, 1, 1 )
MOAIDebugLinesMgr.setStyle( MOAITextLabel::DEBUG_DRAW_TEXT_LABEL_BASELINES, 1, 1, 0, 0, 1 )
# MOAIDebugLinesMgr.setStyle( MOAITextLabel::DEBUG_DRAW_TEXT_LABEL_GLYPHS, 1, 0.5, 0, 0, 1 )
# MOAIDebugLinesMgr.setStyle( MOAITextLabel::DEBUG_DRAW_TEXT_LABEL_GLYPH_BOUNDS, 2, 1, 1, 0, 1 )
# MOAIDebugLinesMgr.setStyle( MOAITextLabel::DEBUG_DRAW_TEXT_LABEL_LINES_GLYPH_BOUNDS, 1, 1, 0, 1, 1 )
MOAIDebugLinesMgr.setStyle( MOAITextLabel::DEBUG_DRAW_TEXT_LABEL_LINES_LAYOUT_BOUNDS, 1, 1, 0, 0, 1 )


newStyle = proc do | font, size, r, g, b, a |
	style = MOAITextStyle.new
	style.setFont( font )
  style.setSize( size )
  r = r or 1
  g = g or 1
  b = b or 1
  a = a or 1
	style.setColor( r, g, b, a )
	style
end

viewport = MOAIViewport.new
viewport.setSize( 320, 480 )
viewport.setScale( 320, -480 )

layer = MOAIPartitionViewLayer.new
layer.setViewport( viewport )
layer.pushRenderPass()

text = 'abcdefg hijklmnop qrstuv wxyz 0123456789 aaaaaa bbbbbb cccccc dddddd eeeeee'

font = MOAIFont.new
font.load( 'HyliaSerifBeta-Regular.ttf' )

label = MOAITextLabel.new

label.setStyle( 'foo', newStyle.call( font, 24 ))
label.setStyle( 'bar', newStyle.call( font, 32 ))
label.setStyle( 'baz', newStyle.call( font, 42 ))

label.setText( text )
# label.setFont( font )
# label.setTextSize( 32 )
label.setLineSpacing( 0 )
# label.setYFlip( true )
label.setRect( -64, -128, 64, 128 )
label.setRectLimits( true, true )
label.setAlignment( MOAITextLabel::CENTER_JUSTIFY, MOAITextLabel::TOP_JUSTIFY )
label.setOverrunRules( MOAITextLabel::OVERRUN_SPLIT_WORD, MOAITextLabel::OVERRUN_MOVE_WORD )
label.setPartition( layer )

# gfxQuad = MOAISpriteDeck2D.new
# gfxQuad.setTexture( 'tiny-x.png' )
# gfxQuad.setRect( -4, -4, 4, 4 )
# gfxQuad.setUVRect( 0, 0, 1, 1 )

# local prop = MOAIGraphicsProp.new
# prop.setDeck( gfxQuad )
# prop.setPartition( layer )
