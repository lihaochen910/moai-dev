
MOAISim.openWindow( "test", 320, 480 )

MOAIUntzSystem.initialize

sound = MOAIUntzSound.new()
sound.load( 'cat_vo_damage0.mca.wav' )
sound.setVolume( 1 )
sound.setLooping( false )
sound.play()