// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include <moai-untz/MOAIUntzSound.h>
#include <moai-untz/MOAIUntzSampleBuffer.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getLength
	@text	Return the duration of the sound.
               
	@in		MOAIUntzSound self
	@out	number length
*/
mrb_value MOAIUntzSound::_getLength ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSound, "U" )
               
	if ( self->mSound ) {
		return state.ToRValue ( self->mSound->getInfo ().mLength );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	getPosition
	@text	Return the position of the cursor in the sound.
	
	@in		MOAIUntzSound self
	@out	number position
*/
mrb_value MOAIUntzSound::_getPosition ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSound, "U" )
	
	if ( self->mSound ) {
		return state.ToRValue ( self->mSound->getPosition ());
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	getVolume
	@text	Return the volume of the sound.
	
	@in		MOAIUntzSound self
	@out	number volume
*/
mrb_value MOAIUntzSound::_getVolume ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSound, "U" )
	
	if ( self->mSound ) {
		return state.ToRValue ( self->mSound->getVolume ());
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	getFilename
 @text	Return the file name of the sound.
 
 @in	MOAIUntzSound self
 @out	string filename
 */
mrb_value MOAIUntzSound::_getFilename ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSound, "U" )
	
	if ( self->mSound ) {
		return state.ToRValue ( self->mFilename.str () );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	isLooping
	@text	Return the looping status if the sound.
	
	@in		MOAIUntzSound self
	@out	boolean looping
*/
mrb_value MOAIUntzSound::_isLooping ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSound, "U" )
	
	if ( self->mSound ) {
		return state.ToRValue ( self->mSound->isLooping ());
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	isPaused
	@text	Return the pause status of the sound.
	
	@in		MOAIUntzSound self
	@out	boolean paused
*/
mrb_value MOAIUntzSound::_isPaused ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSound, "U" )
	
	if ( self->mSound ) {
		return state.ToRValue ( self->mSound->isPaused ());
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	isPlaying
	@text	Return the playing status of the sound.
	
	@in		MOAIUntzSound self
	@out	boolean playing
*/
mrb_value MOAIUntzSound::_isPlaying ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSound, "U" )
	
	if ( self->mSound ) {
		return state.ToRValue ( self->mSound->isPlaying ());
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	load
	@text	Loads a sound from disk.
	
	@in		MOAIUntzSound self
	@in		string filename
	@out	nil
*/
mrb_value MOAIUntzSound::_load ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSound, "U" )

	if ( self->mSound ) {
		UNTZ::Sound::dispose ( self->mSound );
		self->mSound = 0;
	}

	MOAIUntzSampleBuffer* data = state.GetRubyObject < MOAIUntzSampleBuffer >( 1, false );
	if ( data ) {
	
		self->mSound = UNTZ::Sound::create ( data->GetSoundInfo (), data->GetSampleBuffer ());
		self->mInMemory = true;
	}
	else if ( state.ParamIsType ( 1, MRB_TT_STRING )) {
	
		cc8* filename = state.GetParamValue < cc8* >( 1, "" );
		bool loadIntoMemory = state.GetParamValue < bool >( 2, true );	

		self->mFilename = filename;
		self->mInMemory = loadIntoMemory;
		//printf ( "creating sound: %s - %s\n", self->mFilename.str(), (loadIntoMemory) ? "in memory" : "not in memory" );
		if ( MOAILogMgr::CheckFileExists ( filename )) {
			self->mSound = UNTZ::Sound::create ( filename, loadIntoMemory );
		}
		else {
			self->mSound = NULL;
		}
	}

	return context;
}

//----------------------------------------------------------------//
/**	@lua	moveVolume
	@text	Animation helper for volume attribute,
	
	@in		MOAITransform self
	@in		number vDelta		Delta to be added to v.
	@in		number length		Length of animation in seconds.
	@opt	number mode			The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
								MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.

	@out	MOAIEaseDriver easeDriver
*/
mrb_value MOAIUntzSound::_moveVolume ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSound, "UNN" )
	
	float volume	= state.GetParamValue < float >( 1, 0.0f );
	float delay		= state.GetParamValue < float >( 2, 0.0f );
	
	if ( delay > 0.0f ) {
	
		u32 mode = state.GetParamValue < u32 >( 3, ZLInterpolate::kSmooth );
		
		MOAIEaseDriver* action = state.CreateClassInstance < MOAIEaseDriver >();
		action->ReserveLinks ( 1 );
		
		action->SetLink ( 0, self, MOAIUntzSoundAttr::Pack ( ATTR_VOLUME ), volume, mode );
		
		action->SetSpan ( delay );
		action->Start ( 0, false );
		return action->PushRubyUserdata ( state );
	}
	
	self->mSound->setVolume ( self->mSound->getVolume () + volume );
	self->ScheduleUpdate ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	pause
	@text	Pause the sound.
	
	@in		MOAIUntzSound self
	@out	nil
*/
mrb_value MOAIUntzSound::_pause ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSound, "U" )
	
	if ( self->mSound ) {
		self->mSound->pause ();
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	play
	@text	Play the sound.
	
	@in		MOAIUntzSound self
	@out	nil
*/
mrb_value MOAIUntzSound::_play ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSound, "U" )
	
	if ( self->mSound ) {
		self->mSound->play ();
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	seekVolume
	@text	Animation helper for volume attribute,
	
	@in		MOAITransform self
	@in		number vGoal		Desired resulting value for v.
	@in		number length		Length of animation in seconds.
	@opt	number mode			The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
								MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.

	@out	MOAIEaseDriver easeDriver
*/
mrb_value MOAIUntzSound::_seekVolume ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSound, "UNN" )

	float volume	= state.GetParamValue < float >( 1, 0.0f );
	float delay		= state.GetParamValue < float >( 2, 0.0f );
	
	if ( delay > 0.0f ) {
	
		u32 mode = state.GetParamValue < u32 >( 3, ZLInterpolate::kSmooth );
		
		MOAIEaseDriver* action = state.CreateClassInstance < MOAIEaseDriver >();
		action->ReserveLinks ( 1 );
		
		action->SetLink ( 0, self, MOAIUntzSoundAttr::Pack ( ATTR_VOLUME ), volume - self->mSound->getVolume (), mode );
		
		action->SetSpan ( delay );
		action->Start ( 0, false );
		return action->PushRubyUserdata ( state );
	}
	
	self->mSound->setVolume ( volume );
	self->ScheduleUpdate ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setLooping
	@text	Set or clear the looping status of the sound.
	
	@in		MOAIUntzSound self
	@opt	boolean looping		Default value is 'false.'
	@out	nil
*/
mrb_value MOAIUntzSound::_setLooping ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSound, "U" )
	
	if ( self->mSound ) {
		bool loop = state.GetParamValue < bool >( 1, false );
		self->mSound->setLooping ( loop );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setLoopPoints
	@text	Sets the start and end looping positions for the sound
	
	@in		MOAIUntzSound self
	@in		double startTime
	@in		double endTime
	
	@out	nil
*/
mrb_value MOAIUntzSound::_setLoopPoints ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSound, "U" )
	
	if ( self->mSound ) {
		double startTime = state.GetParamValue < double >( 1, 0.0 );
		double endTime = state.GetParamValue < double >( 2, 0.0 );
		self->mSound->setLoopPoints ( startTime, endTime );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setPosition
	@text	Sets the position of the sound cursor.
	
	@in		MOAIUntzSound self
	@opt	boolean position		Default value is 0.
	@out	nil
*/
mrb_value MOAIUntzSound::_setPosition ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSound, "U" )
	
	if ( self->mSound ) {
		double position = state.GetParamValue < double >( 1, 0.0 );
		self->mSound->setPosition ( position );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setVolume
	@text	Sets the volume of the sound.
	
	@in		MOAIUntzSound self
	@opt	boolean volume			Default value is 0.
	@out	nil
*/
mrb_value MOAIUntzSound::_setVolume ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSound, "U" )
	
	if ( self->mSound ) {
		float volume = state.GetParamValue < float >( 1, 0.0f );
		self->mSound->setVolume ( volume );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	stop
	@text	Stops the sound from playing.
	
	@in		MOAIUntzSound self
	@out	nil
*/
mrb_value MOAIUntzSound::_stop ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSound, "U" )
	
	if ( self->mSound ) {
		self->mSound->stop ();
	}
	return context;
}

//================================================================//
// MOAIUntzSound
//================================================================//

//----------------------------------------------------------------//
MOAIUntzSound::MOAIUntzSound () :
	mSound ( 0 ),
	mFilename ( 0 ),
	mInMemory ( false ) {

	RTTI_SINGLE ( MOAINode )
}

//----------------------------------------------------------------//
MOAIUntzSound::~MOAIUntzSound () {

	//if (mFilename != NULL)
		//printf ( "deleteing sound: %s - %s\n", mFilename.str(), (mInMemory) ? "in memory" : "not in memory" );
		
	if ( this->mSound ) {
		UNTZ::Sound::dispose ( this->mSound );
	}
}
//----------------------------------------------------------------//
void MOAIUntzSound::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAINode::RegisterRubyClass ( state, klass );

	state.DefineClassConst ( klass, "ATTR_VOLUME", MOAIUntzSoundAttr::Pack ( ATTR_VOLUME ));
}

//----------------------------------------------------------------//
void MOAIUntzSound::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAINode::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getLength", _getLength, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getPosition", _getPosition, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getVolume", _getVolume, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isLooping", _isLooping, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isPaused", _isPaused, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "isPlaying", _isPlaying, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "load", _load, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "moveVolume", _moveVolume, MRB_ARGS_ARG ( 2, 1 ) );
	state.DefineInstanceMethod ( klass, "pause", _pause, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "play", _play, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "seekVolume", _seekVolume, MRB_ARGS_ARG ( 2, 1 ) );
	state.DefineInstanceMethod ( klass, "setLooping", _setLooping, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setLoopPoints", _setLoopPoints, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setPosition", _setPosition, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setVolume", _setVolume, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "stop", _stop, MRB_ARGS_NONE () );

}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAIUntzSound::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	if ( MOAIUntzSoundAttr::Check ( attrID )) {
		attrID = UNPACK_ATTR ( attrID );

		if ( attrID == ATTR_VOLUME ) {
			this->mSound->setVolume ( attr.Apply ( this->mSound->getVolume (), op, MOAIAttribute::ATTR_READ_WRITE ));
			return true;
		}
	}
	return false;
}
