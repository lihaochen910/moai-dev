// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIUNTZSYSTEM_H
#define	MOAIUNTZSYSTEM_H

#include <moai-sim/headers.h>
#include <UntzSound.h>

//================================================================//
// MOAIUntzSystem
//================================================================//
/**	@lua	MOAIUntzSystem
	@text	Untz system singleton.
*/
class MOAIUntzSystem :
	public ZLContextClass < MOAIUntzSystem, MOAIRubyObject > {
private:

	static const u32 DEFAULT_SAMPLE_RATE		= 44100;
	static const u32 DEFAULT_FRAMES_PER_BUFFER	= 8192;
	static const u32 DEFAULT_OPTIONS			= 0;

	//----------------------------------------------------------------//
	static mrb_value		_getDeviceCount			( mrb_state* M, mrb_value context );
	static mrb_value		_getDeviceInfo			( mrb_state* M, mrb_value context );
	static mrb_value		_getOptions				( mrb_state* M, mrb_value context );
	static mrb_value		_getSampleRate			( mrb_state* M, mrb_value context );
	static mrb_value		_getSupportedFormats	( mrb_state* M, mrb_value context );
	static mrb_value		_initialize				( mrb_state* M, mrb_value context );
	static mrb_value		_setInputDevice			( mrb_state* M, mrb_value context );
	static mrb_value		_setOptions				( mrb_state* M, mrb_value context );
	static mrb_value		_setOutputDevice		( mrb_state* M, mrb_value context );
	static mrb_value		_setSampleRate			( mrb_state* M, mrb_value context );
	static mrb_value		_setVolume				( mrb_state* M, mrb_value context );
	static mrb_value		_getVolume				( mrb_state* M, mrb_value context );
public:

	DECL_RUBY_SINGLETON ( MOAIUntzSystem )

	//----------------------------------------------------------------//
					MOAIUntzSystem		();
					~MOAIUntzSystem		();
	void			RegisterRubyClass	( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs	( MOAIRubyState& state, RClass* klass );
	void			Resume				();
	void			Suspend				();
};

#endif
