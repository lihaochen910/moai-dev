
MOAIDebugLinesMgr.setStyle( MOAICollisionProp::DEBUG_DRAW_COLLISION_ACTIVE_PROP_BOUNDS, 2, 0, 0, 1 )
MOAIDebugLinesMgr.setStyle( MOAICollisionProp::DEBUG_DRAW_COLLISION_ACTIVE_OVERLAP_PROP_BOUNDS, 2, 1, 1, 1 )
MOAIDebugLinesMgr.setStyle( MOAICollisionProp::DEBUG_DRAW_COLLISION_ACTIVE_TOUCHED_PROP_BOUNDS, 2, 1, 0, 0 )
MOAIDebugLinesMgr.setStyle( MOAICollisionProp::DEBUG_DRAW_COLLISION_OVERLAP_PROP_BOUNDS, 2, 0, 1, 0 )
MOAIDebugLinesMgr.setStyle( MOAICollisionProp::DEBUG_DRAW_COLLISION_OVERLAPS, 4, 1, 0, 1 )
MOAIDebugLinesMgr.setStyle( MOAICollisionProp::DEBUG_DRAW_COLLISION_WORLD_BOUNDS, 1, 0.5, 0.5, 0.5 )

MOAISim.openWindow( "test", 320, 480 )

viewport = MOAIViewport.new
viewport.setSize( 320, 480 )
viewport.setScale( 320, -480 )

onOverlap = proc do | event, prop1, prop2 |
	
	eventName = nil
	
	eventName =( event == MOAICollisionWorld::OVERLAP_BEGIN ) and 'BEGIN' or eventName
	eventName =( event == MOAICollisionWorld::OVERLAP_END ) and 'END' or eventName
	eventName =( event == MOAICollisionWorld::OVERLAP_UPDATE ) and 'UPDATE' or eventName
	
	print( eventName, prop1, prop2 )
	
	puts() if event == MOAICollisionWorld::OVERLAP_END
end

world = MOAICollisionWorld.new
world.setCallback( onOverlap )
world.start()

layer = MOAIPartitionViewLayer.new
layer.setViewport( viewport )
layer.pushRenderPass()

debugLayer = MOAIPartitionViewLayer.new
debugLayer.setViewport( viewport )
debugLayer.setLayerPartition( world )
debugLayer.pushRenderPass()

spriteDeck = MOAISpriteDeck2D.new
spriteDeck.setTexture( 'moai.png' )
spriteDeck.setRect( -64, -64, 64, 64 )
spriteDeck.setUVRect( 0, 0, 1, 1 )

# font = MOAIFont.new
# font.loadFromTTF( 'fonts/arial-rounded.TTF' )

collDeck = MOAICollisionDeck.new
collDeck.reserveShapes( 1 )
collDeck.setQuad( 1,
	0, 64,
	-64, 0,
	0, -64,
	64, 0
)

ready = proc do | prop, x, y, group |

	coll = MOAICollisionProp.new
	coll.setParent( prop )
	coll.setDeck( collDeck )
	# coll.setOverlapFlags( MOAICollisionProp.OVERLAP_EVENTS_ON_UPDATE + MOAICollisionProp.OVERLAP_EVENTS_LIFECYCLE )
	coll.setOverlapFlags( MOAICollisionProp::OVERLAP_EVENTS_LIFECYCLE + MOAICollisionProp::OVERLAP_CALCULATE_BOUNDS )
	# coll.setGroupMask( group or MOAICollisionProp.CATEGORY_MASK_ALL )
	coll.setPartition( world )
	
	[ prop, coll ]
end

makePropWithColl = proc do | x, y, group |

	prop = MOAIGraphicsProp.new
	prop.setDeck( spriteDeck )
	prop.setPartition( layer )
	prop.setLoc( x, y )
	ready.call( prop, x, y, group )
end

=begin
makeTextWithColl = proc do | text, size, x, y, group |

	label = MOAITextLabel.new
	label.setString( text )
	label.setFont( font )
	label.setTextSize( size )
	label.setAlignment( MOAITextBox.CENTER_JUSTIFY, MOAITextBox.CENTER_JUSTIFY )
	layer.insertProp( label )

	return ready( label, x, y, group )
end
=end

prop1, coll1 = makePropWithColl.call( -70, 30 )
prop2, coll2 = makePropWithColl.call( 70, -30 )

# collDeck.setBox( 1, -64, -64, 0, 64, 64, 0 )

# coll1.setBounds( prop1.getBounds())
# coll1.setDeck( collDeck )
# coll2.setBounds( prop1.getBounds())

# prop1 = makeTextWithColl( 'TEXTY TEXT\nTEXT\nTEXTERSON', 22, -70, 0, 1 )
# prop2 = makeTextWithColl( 'TEXTY TEXT\nTEXT\nTEXTERSON', 22, 70, 0, 1 )
# prop3 = makeTextWithColl( 'TEXTY TEXT\nTEXT\nTEXTERSON', 22, 0, 0, 2 )

# prop1.moveRot( 0, 0, 360, 5 )
# prop2.moveRot( 0, 0, -360, 5 )

prop1.moveLoc( 35, 0, 0, 5 )
prop1.moveRot( 0, 0, 45, 5 )

prop2.moveLoc( -35, 0, 0, 5 )
prop2.moveRot( 0, 0, 45, 5 )

=begin
main = proc do
	
	while true do
		
		if coll1.hasOverlaps() then
			puts( coll1, 'overlaps.', coll1.getOverlaps())
		end
		
		if coll2.hasOverlaps() then
			puts( coll2, 'overlaps.', coll2.getOverlaps())
		end
		
		# coroutine.yield()
	end
end

thread = MOAICoroutine.new
thread.run( main )
=end
