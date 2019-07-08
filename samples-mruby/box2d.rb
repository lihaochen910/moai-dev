
width = 640
height = 480
MOAISim.openWindow( "test", width, height  )

viewport = MOAIViewport.new
viewport.setSize( width, height )
viewport.setScale( width /( width / 640 ), height /( height / 480 ))

onCollide = proc do | event |

	if event == MOAIBox2DArbiter::BEGIN then
		print( 'begin!' )
	end
	
	if event == MOAIBox2DArbiter::END then
		print( 'end!' )
	end
	
	if event == MOAIBox2DArbiter::PRE_SOLVE then
		print( 'pre!' )
	end
	
	if event == MOAIBox2DArbiter::POST_SOLVE then
		print( 'post!' )
	end
end

#  set up the world and start its simulation
world = MOAIBox2DWorld.new
world.setGravity( 0, -10 )
world.setUnitsToMeters( 0.05 )
world.start()

debugLayer = MOAITableLayer.new
# debugLayer.setViewport( viewport )
debugLayer.pushRenderPass()
debugLayer.setRenderTable( world )

worldBody = world.addBody( MOAIBox2DBody::STATIC )
fixture2 = worldBody.addRect( -( 300 / 2 ), -200, 300 / 2, -300 )
fixture2.setFilter( 0x02 )
fixture2.setCollisionHandler( onCollide, MOAIBox2DArbiter::BEGIN + MOAIBox2DArbiter::END, 0x00 )

layer = MOAIPartitionViewLayer.new
layer.setViewport( viewport )
layer.pushRenderPass()

deck = MOAISpriteDeck2D.new
deck.setTexture( 'moai.png' )
deck.setQuad( 0, -25, 25, 0, 0, 25, -25, 0 )
deck.setUVRect( 0, 0, 1, 1 )

addSprite = proc do | x, y |

	x = x or 0
	y = y or 0

	body = world.addBody( MOAIBox2DBody::DYNAMIC )
	body.setTransform( x, y )

	poly = [
		0, -25,
		25, 0,
		0, 25,
		-25, 0,
  ]

	# fixture = body.addPolygon( poly )
	# fixture.setDensity( 1 )
	# fixture.setFriction( 0.3 )
	# fixture.setFilter( 0x01 )
	# fixture.setCollisionHandler( onCollide, MOAIBox2DArbiter::BEGIN + MOAIBox2DArbiter::END, 0x02 )

	body.resetMassData()
	body.applyAngularImpulse( 80 )

	sprite = MOAIGraphicsProp.new
	sprite.setDeck( deck )
	# sprite.body = body
	sprite.setParent( body )
	sprite.setPartition( layer )
end

pick = nil
worldX, worldY = nil
pointerCallback = proc do | x, y |

	worldX, worldY = layer.wndToWorld( x, y )

	if pick then
		mouseJoint.setTarget( worldX, worldY )
	end
end

clickCallback = proc do | down |
	if down then
		pick = layer.getLayerPartition().hullForPoint( worldX, worldY )
		if pick then

			mouseBody = world.addBody( MOAIBox2DBody::DYNAMIC )
			mouseBody.setTransform( worldX, worldY )

			mouseJoint = world.addMouseJoint( mouseBody, pick.body, worldX, worldY,  10000.0 * pick.body.getMass());
			mouseJoint.setDampingRatio( 2 ) ;
		else
			addSprite.call( worldX, worldY )
		end
	else
		if pick then
			# also destroys joint
			mouseBody.destroy()
			mouseBody = nil
			pick = nil
		end
	end
end

rightclickCallback = proc do | down |
	if down then
		addSprite.call( worldX, worldY )
	end
end

MOAIInputMgr.device.pointer.setCallback( pointerCallback )
MOAIInputMgr.device.mouseLeft.setCallback( clickCallback )
MOAIInputMgr.device.mouseRight.setCallback( rightclickCallback )
