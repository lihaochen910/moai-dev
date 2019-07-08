// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAISTREAMADAPTER_H
#define MOAISTREAMADAPTER_H

#include <moai-util/MOAIStream.h>

//================================================================//
// MOAIStreamAdapter
//================================================================//
/**	@lua	MOAIStreamAdapter
	@text	MOAIStreamAdapter may be attached to another stream for the
			purpose of encoding/decoding and/or compressing/decompressing
			bytes read in that stream using a given algorithm (such as base64
			or 'deflate'). 
*/
class MOAIStreamAdapter :
	public ZLStreamProxy,
	public virtual MOAIStream {
protected:
	
	// to save ourselves the trouble of having to create a unique class for each supported adapter
	// type, we resort to some trickery. we subclass from ZLStreamProxy, which wraps a stream. the
	// stream we wrap is the instance of ZLStreamAdapter. it, in turn, adapts the stream held in the
	// MOAIStream the user gives us. So we have ZLStreamProxy => ZLStreamAdapter => MOAIStream.
	
	ZLStreamAdapter* mAdapter; // this is the adapter that does the work
	
	MOAIRubySharedPtr < MOAIStream > mAdaptedStream; // this is the inner stream
	
	//----------------------------------------------------------------//
	static mrb_value		_close					( mrb_state* M, mrb_value context );
	static mrb_value		_openBase64Reader		( mrb_state* M, mrb_value context );
	static mrb_value		_openBase64Writer		( mrb_state* M, mrb_value context );
	static mrb_value		_openDeflateReader		( mrb_state* M, mrb_value context );
	static mrb_value		_openDeflateWriter		( mrb_state* M, mrb_value context );
	static mrb_value		_openHex				( mrb_state* M, mrb_value context );
	static mrb_value		_openRing				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	mrb_value				Open					( MOAIRubyState& state, int idx, ZLStreamAdapter* adapter );

public:
	
	DECL_RUBY_FACTORY ( MOAIStreamAdapter, MOAIRubyObject )

	//----------------------------------------------------------------//
	void			Clear					(); // closes *and* clears the adapter
	void			Close					(); // clears the stream and closes the adapter, but *doesn't* also clear the adapter
					MOAIStreamAdapter		();
					~MOAIStreamAdapter		();
	ZLResultCode	Open					( ZLStreamAdapter* adapter, MOAIStream* stream );
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
