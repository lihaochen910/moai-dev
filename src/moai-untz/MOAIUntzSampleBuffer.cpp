// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include <moai-untz/MOAIUntzSampleBuffer.h>

//extern "C" {
//	extern void __cdecl lua_pushnumber (lua_State *L, lua_Number n);
//}

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getData
	@text	Retrieve every sample data in buffer

	@in		MOAIUntzSampleBuffer self
	@out	table data					array of sample data number ( -1 ~ 1 as sample level)
*/
mrb_value MOAIUntzSampleBuffer::_getData ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSampleBuffer, "U" )
	
	u32 arynum = self->mInfo.mTotalFrames * self->mInfo.mChannels;
	mrb_value ary = mrb_ary_new ( M );
	for ( u32 i = 0; i < arynum; i++ ) {
		mrb_ary_push ( M, ary, state.ToRValue ( self->mBuffer [ i ] ) );
	}
	return ary;
}

//----------------------------------------------------------------//
/** @lua	getInfo
	@text	Returns attributes of sample buffer.

	@in		MOAIUntzSampleBuffer self
	@out	number bps					bits per sample
	@out	number channels				number of channels (mono=1, stereo=2)
	@out	number frames				num of total frames contained
	@out	number sampleRate			sample rate (44100, 22050, etc.)
	@out	number length				sound length in seconds
*/
mrb_value MOAIUntzSampleBuffer::_getInfo ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSampleBuffer, "U" )

	mrb_value ret [ 5 ];
	ret [ 0 ] = state.ToRValue ( self->mInfo.mBitsPerSample );
	ret [ 1 ] = state.ToRValue ( self->mInfo.mChannels );
	ret [ 2 ] = state.ToRValue ( self->mInfo.mTotalFrames );
	ret [ 3 ] = state.ToRValue ( self->mInfo.mSampleRate );
	ret [ 4 ] = state.ToRValue ( self->mInfo.mLength );

	return mrb_ary_new_from_values ( state, 5, ret );
}

//----------------------------------------------------------------//
/**	@lua	load
	@text	Loads a sound from disk.
	
	@in		MOAIUntzSampleBuffer self
	@in		string filename
	@out	nil
*/
mrb_value MOAIUntzSampleBuffer::_load ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSampleBuffer, "US" )
	
	cc8* filename = state.GetParamValue < cc8* >( 1, "" );

	if ( !UNTZ::Sound::decode ( filename, self->mInfo, (float** )& self->mBuffer ) ) {
		printf ( "error creating sample buffer\n" );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	prepareBuffer
	@text	Allocate internal memory for sample buffer

	@in		MOAIUntzSampleBuffer self
	@in		number channels				number of channels (mono=1, stereo=2)
	@in		number frames				number of total frames of sample
	@in		number sampleRate			sample rate in Hz (44100 or else)
	@out	nil
*/
mrb_value MOAIUntzSampleBuffer::_prepareBuffer( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSampleBuffer, "UNNN" )
	
	u32 bitsPerSample	= 32;
	u32 channels		= state.GetParamValue < u32 >( 1, 1 );
	u32 totalFrames		= state.GetParamValue < u32 >( 2, 0 );
	u32 sampleRate		= state.GetParamValue < u32 >( 3, 44100 );
	
	double length		= ( double )totalFrames / sampleRate;
	size_t memSize		= totalFrames * channels * 4;
	
	// Allocate space and copy the buffer
	self->mBuffer = ( float* )new char [ memSize ];
	memset ( self->mBuffer, 0, memSize );

	self->mInfo.mBitsPerSample	= bitsPerSample;
	self->mInfo.mChannels		= channels;
	self->mInfo.mTotalFrames	= totalFrames;
	self->mInfo.mSampleRate		= sampleRate;
	self->mInfo.mLength			= length;
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setData
	@text	Write sample data into buffer

	@in		MOAIUntzSampleBuffer self
	@in		table data					array of sample data number ( -1 ~ 1 as sample level )
	@in		number startIndex			index of sample buffer to start copying from (1 for the first sample)
	@out	nil
*/
mrb_value MOAIUntzSampleBuffer::_setData ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIUntzSampleBuffer, "UTN" )
	
//	u32 startDataIndex = state.GetParamValue<u32>( 2,1 );
//	int itr = state.PushTableItr(2);
//	int idx = 0;
//	//u32 total = 0;
//	int maxindex = self->mInfo.mChannels * self->mInfo.mTotalFrames;
//
//	idx = startDataIndex - 1;
//	
//	for ( ; state.TableItrNext ( itr ) && idx < maxindex; ++idx ) {
//		float val = state.GetParamValue < float> ( -1, 0 );
//		self->mBuffer[idx] = val;		
////		fprintf(stderr, "value %d: %f\n", idx, val );
//	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setRawData
	@text	Write raw sample data (array of 16bit short value) into buffer
	
	@in		MOAIUntzSampleBuffer self
	@in		string raw binary data that contains array of network byte ordered 16bit short value
	@in		number of bytes to read
	@in		number index of sample buffer start copying from (1 for the first sample)
	@out	nil
*/
mrb_value MOAIUntzSampleBuffer::_setRawData ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP( MOAIUntzSampleBuffer, "USNN" )
	
	cc8* s					= state.GetParamValue < cc8* >( 1, "" );
	short* buf				= ( short* )s;
	u32 l					= state.GetParamValue < u32 >( 2, 0 );
	
	u32 elemnum				= l >> 1;
	int maxindex			= self->mInfo.mChannels * self->mInfo.mTotalFrames;
	
	u32 startDataIndex		= state.GetParamValue < u32 >( 3 , 1 ) - 1;
	
	for ( int idx = 0; ( idx + ( int )startDataIndex ) < maxindex && idx < ( int )elemnum; ++idx ) {
	
		float val = buf [ idx ];
		val = val < 0 ? ( val / 32768.0f ) : ( val / 32767.0f );
//		printf ( "bloop: %f\n", val );
		
		self->mBuffer [ idx ] = val;
	}	
	return context;
}

//================================================================//
// MOAIUntzSampleBuffer
//================================================================//

//----------------------------------------------------------------//
MOAIUntzSampleBuffer::MOAIUntzSampleBuffer () 
	: mBuffer ( 0 ) {

	RTTI_SINGLE ( MOAINode )
}

//----------------------------------------------------------------//
MOAIUntzSampleBuffer::~MOAIUntzSampleBuffer () {
	if ( mBuffer )
		delete [] mBuffer;
}
//----------------------------------------------------------------//
void MOAIUntzSampleBuffer::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIUntzSampleBuffer::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "getData", _getData, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getInfo", _getInfo, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "load", _load, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "prepareBuffer", _prepareBuffer, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "setData", _setData, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setRawData", _setRawData, MRB_ARGS_REQ ( 3 ) );

}
