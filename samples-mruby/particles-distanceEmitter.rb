viewport = MOAIViewport.new
viewport.setSize( 640, 480 )
viewport.setScale( 640, 480 )

layer = MOAIPartitionViewLayer.new
layer.setViewport( viewport )
layer.pushRenderPass()

MOAISim.openWindow( "cathead", 640, 480 )

r1 = MOAIParticleScript.packReg( 1 )

init = MOAIParticleScript.new

init.vecAngle( r1, MOAIParticleScript::PARTICLE_DX, MOAIParticleScript::PARTICLE_DY )
init.sub( r1, MOAIParticleScript.packConst( 180.0 ), r1 )

render = MOAIParticleScript.new

render.add( MOAIParticleScript::PARTICLE_X, MOAIParticleScript::PARTICLE_X, MOAIParticleScript::PARTICLE_DX )
render.add( MOAIParticleScript::PARTICLE_Y, MOAIParticleScript::PARTICLE_Y, MOAIParticleScript::PARTICLE_DY )

render.sprite
render.set( MOAIParticleScript::SPRITE_ROT, r1 )
render.ease( MOAIParticleScript::SPRITE_X_SCL, MOAIParticleScript.packConst( 0.5 ), MOAIParticleScript.packConst( 3 ), MOAIEaseType::SHARP_EASE_IN )
render.ease( MOAIParticleScript::SPRITE_OPACITY, MOAIParticleScript.packConst( 1 ), MOAIParticleScript.packConst( 0 ), MOAIEaseType::EASE_OUT )

texture = MOAISpriteDeck2D.new
texture.setTexture( "moai.png" )
texture.setRect( -16, -16, 16, 16 )

system = MOAIParticleSystem.new
system.reserveParticles( 256, 1 )
system.reserveSprites( 256 )
system.reserveStates( 2 )
system.setDeck( texture )
system.start()
system.setPartition( layer )

state = MOAIParticleState.new
state.setTerm( 3, 3 )
state.setInitScript( init )
state.setRenderScript( render )
system.setState( 1, state )

emitter = MOAIParticleDistanceEmitter.new
emitter.setLoc( 0, 0 )
emitter.setSystem( system )
emitter.setMagnitude( 0.125 )
emitter.setAngle( 260, 280 )
emitter.setDistance( 16 )
emitter.start()

pointerCallback = proc do | x, y |
	
	mouseX, mouseY = layer.wndToWorld( x, y )
	
	emitter.setLoc( mouseX, mouseY )
	
	if MOAIInputMgr.device.mouseLeft.isUp then
		emitter.reset
	end
end

MOAIInputMgr.device.pointer.setCallback( pointerCallback )