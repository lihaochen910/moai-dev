
MOAISim.openWindow( "test", 512, 512 )

viewport = MOAIViewport.new
viewport.setSize( 512, 512 )
viewport.setScale( 512, -512 )

layer = MOAIPartitionViewLayer.new
layer.setViewport( viewport )
layer.pushRenderPass()

grid = MOAIGrid.new
grid.setSize( 8, 8, 32, 32 )

grid.setRow( 1, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03 )
grid.setRow( 2,	0x03, 0x03, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03 )
grid.setRow( 3,	0x03, 0x03, 0x00, 0x03, 0x03, 0x00, 0x03, 0x03 )
grid.setRow( 4,	0x03, 0x03, 0x00, 0x03, 0x03, 0x00, 0x03, 0x03 )
grid.setRow( 5,	0x03, 0x03, 0x00, 0x03, 0x03, 0x00, 0x03, 0x03 )
grid.setRow( 6,	0x03, 0x03, 0x00, 0x03, 0x03, 0x00, 0x03, 0x03 )
grid.setRow( 7,	0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0x03 )
grid.setRow( 8,	0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0x03 )

tileDeck = MOAITileDeck2D.new
tileDeck.setTexture( "test.png" )
tileDeck.setSize( 2, 2 )

prop = MOAIGraphicsGridProp.new
prop.setDeck( tileDeck )
prop.setGrid( grid )
prop.setLoc( -128, -128 )
prop.setPartition( layer )

startNode = grid.getCellAddr( 1, 1 )
endNode = grid.getCellAddr( 8, 8 )

pathFinder = MOAIPathFinder.new
pathFinder.setGraph( grid )
pathFinder.init( startNode, endNode )

while pathFinder.findPath( 3 ) do
	puts 'finding...'
end

pathSize = pathFinder.getPathSize
for i in 1..pathSize do
	entry = pathFinder.getPathEntry( i )
	x, y = grid.cellAddrToCoord( entry )
	grid.setTile( x, y, 0x02 )
end

grid.setTile( 1, 1, 0x01 )
grid.setTile( 8, 8, 0x04 )
