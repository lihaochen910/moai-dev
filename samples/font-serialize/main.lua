----------------------------------------------------------------
-- Copyright (c) 2010-2017 Zipline Games, Inc. 
-- All Rights Reserved. 
-- http://getmoai.com
----------------------------------------------------------------

-- build the font
charcodes = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 .,:;!?()&/-'
text = 'The quick brown fox jumps over the lazy dog.'

font = MOAIFont.new ()
font:load ( 'times.ttf' )
font:preloadGlyphs ( charcodes, 8 )
font:preloadGlyphs ( charcodes, 12 )
font:preloadGlyphs ( charcodes, 16 )
font:preloadGlyphs ( charcodes, 24 )
font:preloadGlyphs ( charcodes, 32 )
font:preloadGlyphs ( charcodes, 42 )
font:preloadGlyphs ( charcodes, 56 )
font:preloadGlyphs ( charcodes, 64 )
font:preloadGlyphs ( charcodes, 72 )
font:preloadGlyphs ( charcodes, 76 )

-- yank out the font image
image = font:getImage ()
image:writePNG ( '../temp/font.png' )

-- serialize out to a file
file = io.open ( '../temp/font.lua', 'w' )
file:write ( MOAISerializer.serializeToString ( font ))
file:close ()

-- nothing up our sleeve...
image = nil
font = nil
file = nil

MOAISim.forceGarbageCollection ()

-- load and show the font
MOAISim.openWindow ( "test", 320, 480 )

viewport = MOAIViewport.new ()
viewport:setSize ( 320, 480 )
viewport:setScale ( 320, 480 )

layer = MOAIPartitionViewLayer.new ()
layer:setViewport ( viewport )
layer:pushRenderPass ()

-- read the font in from the file
file = io.open ( '../temp/font.lua', 'r' )
font = loadstring ( file:read ( '*all' ))()
file:close ()

-- load the font image back in
image = MOAIImage.new ()
image:load ( '../temp/font.png', 0 )

-- set the font image
font:setCache ()
font:setReader ()
font:setImage ( image )

-- font now ready for use
textbox = MOAITextBox.new ()
textbox:setString ( text )
textbox:setFont ( font )
textbox:setTextSize ( 24 )
textbox:setRect ( -150, -230, 150, 230 )
textbox:setYFlip ( true )
textbox:setPartition ( layer )
