
MOAISim.openWindow( "test", 480, 320 )

viewport = MOAIViewport.new
viewport.setSize( 480, 320 )
viewport.setScale( 480, 320 )

camera = MOAICamera.new
camera.moveLoc( 240, 0, 4 )

layer1 = MOAIPartitionViewLayer.new
layer1.setViewport( viewport )
layer1.setCamera( camera )
layer1.setParallax( 0.5, 1 )
layer1.pushRenderPass()
# MOAISim.pushRenderPass( layer1 )

layer2 = MOAIPartitionViewLayer.new
layer2.setViewport( viewport )
layer2.setCamera( camera )
layer2.pushRenderPass()
# MOAISim.pushRenderPass( layer2 )

gfxQuad = MOAISpriteDeck2D.new
gfxQuad.setTexture( "moai.png" )
gfxQuad.setRect( -64, -64, 64, 64 )

prop = MOAIGraphicsProp.new
prop.setDeck( gfxQuad )
prop.setScl( 0.75, 0.75 )
prop.setPartition( layer1 )

prop = MOAIGraphicsProp.new
prop.setDeck( gfxQuad )
prop.setPartition( layer2 )
