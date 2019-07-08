// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include <moai-untz/MOAIUntzSystem.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// placeholder
mrb_value MOAIUntzSystem::_getDeviceCount ( mrb_state* M, mrb_value context ) {
	UNUSED ( M );
	
	//UInt32 deviceCount = UNTZ::System::get ()->getDeviceCount ();
	//lua_pushnumber ( L, deviceCount );
	
	return context;
}

//----------------------------------------------------------------//
// placeholder
mrb_value MOAIUntzSystem::_getDeviceInfo ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	//u32 deviceIndex = state.GetParamValue ( 1, 0 );
	//UNTZ::DeviceInfo info = UNTZ::System::get ()->getDeviceInfo ( deviceIndex );
	//
	//lua_pushstring ( L, info.name.c_str ());
	//lua_pushnumber ( L, info.numInputChannels );
	//lua_pushnumber ( L, info.numOutputChannels );
	
	return context;
}

//----------------------------------------------------------------//
// placeholder
mrb_value MOAIUntzSystem::_getOptions ( mrb_state* M, mrb_value context ) {
	UNUSED ( M );

	//UInt32 options = UNTZ::System::get ()->getOptions ();
	//lua_pushnumber ( L, options );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	getSampleRate
	@text	Return the system's current sample rate.
	
	@out	number sampleRate
*/
mrb_value MOAIUntzSystem::_getSampleRate ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	UInt32 sampleRate = UNTZ::System::get ()->getSampleRate ();
	
	return state.ToRValue ( sampleRate );
}

//----------------------------------------------------------------//
// placeholder
mrb_value MOAIUntzSystem::_getSupportedFormats ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	//RStringArray& formats = UNTZ::System::get ()->getSupportedFormats ();
	//
	//lua_newtable ( M );
	//for ( RStringArray::size_type i = 0; i < formats.size (); ++i ) {
	//	lua_pushboolean ( L, true );
	//	lua_setfield ( L, -2, formats [ i ].c_str ());
	//}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	initialize
	@text	Inititalize the sound system.
	
	@opt	number sampleRate		Default value is 44100.
	@opt	number numFrames		Default value is 8192
	@out	nil
*/
mrb_value MOAIUntzSystem::_initialize ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	u32 sampleRate = state.GetParamValue ( 1, DEFAULT_SAMPLE_RATE );
	u32 numFrames = state.GetParamValue ( 2, DEFAULT_FRAMES_PER_BUFFER );
	u32 options = state.GetParamValue ( 3, DEFAULT_OPTIONS );
	
	UNTZ::System::get ()->initialize ( sampleRate, numFrames, options );
	
	return context;
}

//----------------------------------------------------------------//
// placeholder
mrb_value MOAIUntzSystem::_setInputDevice ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	//u32 deviceIndex = state.GetParamValue ( 1, 0 );
	//UNTZ::System::get ()->setInputDevice ( deviceIndex );
	
	return context;
}

//----------------------------------------------------------------//
// placeholder
mrb_value MOAIUntzSystem::_setOptions ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	//u32 options = state.GetParamValue ( 1, DEFAULT_OPTIONS );
	//UNTZ::System::get ()->_setOptions ( options );
	
	return context;
}

//----------------------------------------------------------------//
// placeholder
mrb_value MOAIUntzSystem::_setOutputDevice ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	//u32 deviceIndex = state.GetParamValue ( 1, 0 );
	//UNTZ::System::get ()->setOutputDevice ( deviceIndex );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setSampleRate
	@text	Set the system sample rate.
	
	@opt	number sampleRate		Default value is 44100.
	@out	nil
*/
mrb_value MOAIUntzSystem::_setSampleRate ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	u32 sampleRate = state.GetParamValue ( 1, DEFAULT_SAMPLE_RATE );
	UNTZ::System::get ()->setSampleRate ( sampleRate );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setVolume
	@text	Set the system level volume.
	
	@opt	number volume		Valid Range: 0 >= x <= 1.0 (Default value is 1.0)
	@out	nil
*/
mrb_value MOAIUntzSystem::_setVolume ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	float volume = ( float )state.GetParamValue ( 1, 1.0 );
	UNTZ::System::get ()->setVolume ( volume );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	getVolume
	@text	Return the system's current volume
	
	@out	number volume
*/
mrb_value MOAIUntzSystem::_getVolume ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	float volume = UNTZ::System::get ()->getVolume ();
	
	return state.ToRValue( volume );
}

//================================================================//
// MOAIUntzSystem
//================================================================//

//----------------------------------------------------------------//
MOAIUntzSystem::MOAIUntzSystem () {

	RTTI_SINGLE ( MOAIRubyObject )
}

//----------------------------------------------------------------//
MOAIUntzSystem::~MOAIUntzSystem () {
}

//----------------------------------------------------------------//
void MOAIUntzSystem::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "RECORDABLE", ( u32 )UNTZ::RECORDABLE ); // bitwise

	state.DefineStaticMethod ( klass, "getDeviceCount", _getDeviceCount, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getDeviceInfo", _getDeviceInfo, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getOptions", _getOptions, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getSampleRate", _getSampleRate, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getSupportedFormats", _getSupportedFormats, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "initialize", _initialize, MRB_ARGS_ARG ( 0, 3 ) );
	state.DefineStaticMethod ( klass, "setInputDevice", _setInputDevice, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "setOptions", _setOptions, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "setOutputDevice", _setOutputDevice, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "setSampleRate", _setSampleRate, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "setVolume", _setVolume, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "getVolume", _getVolume, MRB_ARGS_NONE () );

}

//----------------------------------------------------------------//
void MOAIUntzSystem::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIUntzSystem::Resume () {
	
	if ( UNTZ::System::get () ) {
		UNTZ::System::get ()->resume ();
	}
}

//----------------------------------------------------------------//
void MOAIUntzSystem::Suspend () {

	if ( UNTZ::System::get () ) {
		UNTZ::System::get ()->suspend ();
	}
}

