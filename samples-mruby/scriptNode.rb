
MOAISim.openWindow( "test", 320, 480 )

viewport = MOAIViewport.new
viewport.setSize( 320, 480 )
viewport.setScale( 320, -480 )

layer = MOAIPartitionViewLayer.new
layer.setViewport( viewport )
layer.pushRenderPass()

gfxQuad = MOAISpriteDeck2D.new
gfxQuad.setTexture( "moai.png" )
gfxQuad.setRect( -128, -128, 128, 128 )
gfxQuad.setUVRect( 0, 0, 1, 1 )

prop = MOAIGraphicsProp.new
prop.setDeck( gfxQuad )
prop.setPartition( layer )

# prop.moveRot( 360, 1.5 )

nd1 = MOAIScriptNode.new
nd1.reserveAttrs( 1 )
nd1.setAttrName( 1, "something" )

function nd1.something( n )
	print("call me", n)
end

nd2 = MOAIScriptNode.new
nd2.reserveAttrs( 2 )
nd2.setAttrName( 1, "rot" )
nd2.setAttrName( 2, "func" )

prop.setAttrLink( MOAITransform.ATTR_Z_ROT, nd2, 1 )
nd2.connect( "func", nd1, "something" )

main = proc do

  for i = 1, 360 do
		nd2.rot = i
		coroutine.yield()
		nd2.func( i )
	end
end

thread = MOAICoroutine.new
thread.run( main )