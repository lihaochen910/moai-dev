// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIUNTZSAMPLEBUFFER_H
#define	MOAIUNTZSAMPLEBUFFER_H

#include <moai-sim/headers.h>
#include <UntzSound.h>

//================================================================//
// MOAIUntzSampleBuffer
//================================================================//
/**	@lua	MOAIUntzSampleBuffer
	@text	Uncompressed WAV data held in memory. May be shared between
			multiple MOAIUntzSound objects.
*/
class MOAIUntzSampleBuffer :
	public virtual MOAIRubyObject {
private:

	float*				mBuffer;
	UNTZ::SoundInfo		mInfo;

	//----------------------------------------------------------------//
	static mrb_value		_getData					( mrb_state* M, mrb_value context );
	static mrb_value		_getInfo					( mrb_state* M, mrb_value context );
	static mrb_value		_load						( mrb_state* M, mrb_value context );
	static mrb_value		_prepareBuffer				( mrb_state* M, mrb_value context );
	static mrb_value		_setData					( mrb_state* M, mrb_value context );
	static mrb_value		_setRawData					( mrb_state* M, mrb_value context );

public:

	DECL_RUBY_FACTORY ( MOAIUntzSampleBuffer, MOAIRubyObject )

	GET ( UNTZ::SoundInfo, SoundInfo, mInfo )
	GET ( float*, SampleBuffer, mBuffer )

	//----------------------------------------------------------------//
						MOAIUntzSampleBuffer		();
						~MOAIUntzSampleBuffer		();
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
};

#endif
