puts "Hello! MOAI Ruby!"

MOAISim.setStep( 1.0 / 60.0 )
MOAISim.openWindow( 'MOAI MRuby', 640, 480 )

viewport = MOAIViewport.new
viewport.setSize( 640, 480 )
viewport.setScale( 640, 480 )

layer = MOAIPartitionViewLayer.new
layer.setViewport( viewport )
layer.pushRenderPass()

gfxQuad = MOAISpriteDeck2D.new
gfxQuad.setTexture( "moai.png" )
gfxQuad.setRect( -128, -128, 128, 128 )
gfxQuad.setUVRect( 0, 1, 1, 0 )

# gfxQuad.setHitMask( "moai.png" )
# gfxQuad.setHitMaskScalar( 1, 1, 1, 1 )
# gfxQuad.setHitMaskThreshold( 1, 1, 1, 1 ) # just hit test the white pixels

prop = MOAIGraphicsProp.new
prop.setDeck( gfxQuad )
prop.setHitGranularity( MOAIGraphicsProp::HIT_TEST_FINE )
prop.setPartition( layer )

render = MOAIParticleScript.new
render.sprite()
render.set( MOAIParticleScript::SPRITE_RED, MOAIParticleScript.packConst( 0.0 ))
render.rand( MOAIParticleScript::SPRITE_GREEN, MOAIParticleScript.packConst( 0.5 ), MOAIParticleScript.packConst( 1 ))
render.rand( MOAIParticleScript::SPRITE_BLUE, MOAIParticleScript.packConst( 0.5 ), MOAIParticleScript.packConst( 1 ))

texture = MOAISpriteDeck2D.new
texture.setTexture( "moai.png" )
texture.setRect( -2, -2, 2, 2 )

system = MOAIParticleSystem.new
system.reserveParticles( 1024, 2 )
system.reserveSprites( 1024 )
system.reserveStates( 1 )
system.setDeck( texture )
system.start()
system.setPartition( layer )

# state 1
state1 = MOAIParticleState.new
state1.setTerm( 0.75, 1.25 )
state1.setRenderScript( render )
system.setState( 1, state1 )

emitter = MOAIParticleTimedEmitter.new
emitter.setLoc( 0, 0 )
emitter.setSystem( system )
emitter.setFrequency( 1 / 60 )
emitter.setEmission( 12 )
emitter.setRect( -128, -128, 128, 128 )
emitter.start()

emitter.setMask( prop )

clickCallback = proc do | down |

	if down then
		
		x, y = MOAIInputMgr.device.pointer.getLoc()
		x, y = layer.wndToWorld( x, y )
		if prop.inside( x, y ) then
			prop.addRot( 0, 0, 5 )
		end
	end
end

MOAIInputMgr.device.pointer.setCallback( clickCallback )
