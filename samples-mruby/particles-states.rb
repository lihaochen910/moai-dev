viewport = MOAIViewport.new
viewport.setSize( 640, 480 )
viewport.setScale( 640, 480 )

layer = MOAIPartitionViewLayer.new
layer.setViewport( viewport )
layer.pushRenderPass()

MOAISim.openWindow( "cathead", 640, 480 )

r1 = MOAIParticleScript.packReg( 1 )
r2 = MOAIParticleScript.packReg( 2 )


init = MOAIParticleScript.new
init.randVec( r1, r2, MOAIParticleScript.packConst( 64 ), MOAIParticleScript.packConst( 128 ))

render = MOAIParticleScript.new
render.easeDelta( MOAIParticleScript::PARTICLE_X, 0, r1, MOAIEaseType::EASE_IN )
render.easeDelta( MOAIParticleScript::PARTICLE_Y, 0, r2, MOAIEaseType::EASE_IN )

render.sprite()
render.set( MOAIParticleScript::SPRITE_RED, MOAIParticleScript.packConst( 0.0 ))
render.rand( MOAIParticleScript::SPRITE_GREEN, MOAIParticleScript.packConst( 0.5 ), MOAIParticleScript.packConst( 1 ))
render.rand( MOAIParticleScript::SPRITE_BLUE, MOAIParticleScript.packConst( 0.5 ), MOAIParticleScript.packConst( 1 ))


init2 = MOAIParticleScript.new
init2.rand( r1, MOAIParticleScript.packConst( -360 ), MOAIParticleScript.packConst( 360 ))

render2 = MOAIParticleScript.new

render2.sprite
render2.ease( MOAIParticleScript::SPRITE_ROT, 0, r1, MOAIEaseType::EASE_IN )
render2.rand( MOAIParticleScript::SPRITE_RED, MOAIParticleScript.packConst( 0.5 ), MOAIParticleScript.packConst( 1 ))
render2.rand( MOAIParticleScript::SPRITE_GREEN, MOAIParticleScript.packConst( 0.5 ), MOAIParticleScript.packConst( 1 ))
render2.set( MOAIParticleScript::SPRITE_BLUE, 0 )

texture = MOAISpriteDeck2D.new
texture.setTexture( "moai.png" )
texture.setRect( -16, -16, 16, 16 )

system = MOAIParticleSystem.new
system.reserveParticles( 256, 2 )
system.reserveSprites( 256 )
system.reserveStates( 2 )
system.setDeck( texture )
system.start()
system.setPartition( layer )

magnet = MOAIParticleForce.new
magnet.initBasin( 256, 64 )
magnet.setType( MOAIParticleForce::OFFSET )

# state 1
state1 = MOAIParticleState.new
state1.setTerm( 0.75, 1.25 )
state1.setInitScript( init )
state1.setRenderScript( render )
system.setState( 1, state1 )

state1.pushForce( magnet )

# state 2
state2 = MOAIParticleState.new
state2.setTerm( 0.5 )
state2.setInitScript( init2 )
state2.setRenderScript( render2 )
system.setState( 2, state2 )

state2.pushForce( magnet )

state1.setNext( state2 )
state2.setNext( state1 )

emitter = MOAIParticleTimedEmitter.new
emitter.setLoc( 0, 0 )
emitter.setSystem( system )
emitter.setMagnitude( 50, 100 )
emitter.setFrequency( 0.2, 1 )
emitter.setRadius( 8, 8 )
emitter.start()