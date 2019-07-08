// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIImage.h>
#include <moai-sim/MOAIImageLoadTask.h>

//================================================================//
// MOAIImageLoadTask
//================================================================//

//----------------------------------------------------------------//
void MOAIImageLoadTask::Execute () {

	if ( this->mFilename.size() ) {
		this->mImage.Load ( this->mFilename, this->mTransform );
	}
	else if ( this->mDataBuffer ) {

		void* bytes = 0;
		size_t size = 0;
		ZLByteStream stream;

		this->mDataBuffer->Lock ( &bytes, &size );
		
		stream.SetBuffer ( bytes, size );
		stream.SetLength ( size );

		this->mImage.Load ( stream, this->mTransform );

		this->mDataBuffer->Unlock();
	}
}

//----------------------------------------------------------------//
void MOAIImageLoadTask::Init ( cc8* filename, MOAIImage& target, u32 transform ) {

	this->mTarget = &target;

	this->mFilename = filename;
	this->mTransform = transform;
	this->mTarget->RubyRetain ();
}

//----------------------------------------------------------------//
void MOAIImageLoadTask::Init ( MOAIDataBuffer& data, MOAIImage& target, u32 transform ) {

	this->mDataBuffer = &data;
	this->mTarget = &target;

	this->mTransform = transform;
	this->mDataBuffer->RubyRetain ();
	this->mTarget->RubyRetain ();
}

//----------------------------------------------------------------//
MOAIImageLoadTask::MOAIImageLoadTask () :
	mDataBuffer ( 0 ),
	mTarget ( 0 ) {
}

//----------------------------------------------------------------//
MOAIImageLoadTask::~MOAIImageLoadTask () {

	if ( this->mDataBuffer ) {
		this->mDataBuffer->RubyRelease ();
	}
	
	if ( this->mTarget ) {
		this->mTarget->RubyRelease ();
	}
}

//----------------------------------------------------------------//
void MOAIImageLoadTask::Publish () {

	// This provides some degree of thread-safety
	this->mTarget->Take ( this->mImage );

	if ( this->mOnFinish ) {
		MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
		if ( this->mOnFinish ) {

			mrb_value argv [ 1 ];
			argv [ 0 ] = state.ToRValue < MOAIRubyObject* >( this->mTarget );

			state.FuncCall ( this->mOnFinish, "call", 1, argv );
		}
	}
}

//----------------------------------------------------------------//
void MOAIImageLoadTask::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAITask::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIImageLoadTask::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAITask::RegisterRubyFuncs ( state, klass );
}

//----------------------------------------------------------------//
void MOAIImageLoadTask::SetCallback ( mrb_state* M, int idx ) {

	MOAIRubyState state ( M );
	this->mOnFinish.SetRef ( state.GetParamValue ( idx ) );
}

