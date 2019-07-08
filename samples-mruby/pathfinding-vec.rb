
MOAISim.openWindow( "test", 512, 512 )

graph = MOAIVecPathGraph.new
graph.reserveNodes( 8 )
graph.setNode( 1, 1, 1 )
graph.setNode( 2, 5, 2 )
graph.setNode( 3, 9, 1 )
graph.setNode( 4, 9, 6 )
graph.setNode( 5, 5, 7 )
graph.setNode( 6, 9, 9 )
graph.setNode( 7, 4, 8 )
graph.setNode( 8, 2, 9 )
graph.setNeighbors( 1, 2 )
graph.setNeighbors( 2, 3 )
graph.setNeighbors( 3, 5 )
graph.setNeighbors( 5, 4 )
graph.setNeighbors( 5, 6 )
graph.setNeighbors( 6, 7 )
graph.setNeighbors( 7, 8 )
graph.setNeighbors( 7, 1 )

startNode = 1
endNode = 6

pathFinder = MOAIPathFinder.new
pathFinder.setGraph( graph )
pathFinder.init( startNode, endNode )

while pathFinder.findPath( 3 ) do
	print( 'finding...' )
end

onDraw = proc do | index, xOff, yOff, xFlip, yFlip |

	# draw graph
	MOAIDraw.setPenColor( 1, 1, 1, 1 )

	for i in 1..graph.getNodeCount() do
		x, y = graph.getNode( i )
		MOAIDraw.fillCircle( x, y, 0.2, 32 )

		for j in 1..graph.getNodeCount() do
			if graph.areNeighbors( i, j ) then
				x2, y2 = graph.getNode( j )
				MOAIDraw.drawLine( x, y, x2, y2 )
			end
		end
	end

	# draw path
	MOAIDraw.setPenColor( 1, 0, 0, 1 )

	lastX, lastY = nil
	for i in 1..pathFinder.getPathSize() do
		entry = pathFinder.getPathEntry( i )
		x, y = graph.getNode( entry )
		MOAIDraw.fillCircle( x, y, 0.2, 32 )

		if lastX and lastY then
			MOAIDraw.drawLine( lastX, lastY, x, y )
		end

		lastX, lastY = x, y
	end
end

viewport = MOAIViewport.new
viewport.setSize( 512, 512 )
viewport.setOffset( -1, -1 )
viewport.setScale( 10, 10 )

layer = MOAIPartitionViewLayer.new
layer.setViewport( viewport )
layer.pushRenderPass()

scriptDeck = MOAIDrawDeck.new
scriptDeck.setBounds( 0, 0, 10, 10 )
scriptDeck.setDrawCallback( onDraw )

prop = MOAIGraphicsProp.new
prop.setDeck( scriptDeck )
prop.setPartition( layer )
