
MOAISim.openWindow( "test", 320, 480 )

viewport = MOAIViewport.new
viewport.setSize( 320, 480 )
viewport.setScale( 320, 480 )

layer = MOAIPartitionViewLayer.new
layer.setViewport( viewport )
layer.pushRenderPass()

grid = MOAIGrid.new
grid.setSize( 2, 2, 64, 64 )

grid.setRow( 1, 	0x00000001, 0x20000002 )
grid.setRow( 2,	0x40000003, 0x60000004 )

tileDeck = MOAITileDeck2D.new
tileDeck.setTexture( 'test.png' )
tileDeck.setSize( 2, 2 )

prop = MOAIGraphicsGridProp.new
prop.setDeck( tileDeck )
prop.setGrid( grid )
prop.setLoc( -64, -64, 0 )
prop.setPartition( layer )