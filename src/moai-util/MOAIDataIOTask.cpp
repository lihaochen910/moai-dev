// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-util/MOAIDataBuffer.h>
#include <moai-util/MOAIDataIOTask.h>

//================================================================//
// MOAIDataIOTask
//================================================================//

//----------------------------------------------------------------//
void MOAIDataIOTask::Execute () {

	if ( this->mAction == LOAD_ACTION ) { 
		this->mData->Load ( this->mFilename );
		
		if ( this->mInflateOnTaskThread ) {
			this->mData->Inflate ( this->mWindowBits );
		}
	}
	else if ( this->mAction == SAVE_ACTION ) {
		this->mData->Save ( this->mFilename );
	}
}

//----------------------------------------------------------------//
void MOAIDataIOTask::Init ( cc8* filename, MOAIDataBuffer& target, u32 action ) {

	this->mFilename = filename;
	this->mAction = action;
	this->mData = &target;
	
	this->mData->RubyRetain ( this->mData );
}

//----------------------------------------------------------------//
MOAIDataIOTask::MOAIDataIOTask () :
	mData ( 0 ),
	mAction ( NONE ),
	mInflateOnLoad ( false ),
	mInflateOnTaskThread ( false ),
	mWindowBits ( 0 ) {
}

//----------------------------------------------------------------//
MOAIDataIOTask::~MOAIDataIOTask () {

	this->mData->RubyRelease ( this->mData );
	this->mData = 0;
}

//----------------------------------------------------------------//
void MOAIDataIOTask::Publish () {

	if ( this->mInflateOnLoad && ( !this->mInflateOnTaskThread )) {
		this->mData->Inflate ( this->mWindowBits );
	}

	if ( this->mOnFinish ) {
		MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
		/*if ( this->mOnFinish.PushRef ( state ) ) {
			this->mData->PushRubyUserdata ( state );
			state.DebugCall ( 1, 0 );
		}*/
	}
}

//----------------------------------------------------------------//
void MOAIDataIOTask::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAITask::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIDataIOTask::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAITask::RegisterRubyFuncs ( state, klass );
}

//----------------------------------------------------------------//
void MOAIDataIOTask::SetCallback ( mrb_state* M, mrb_value callback ) {

	MOAIRubyState state ( M );
	//this->mOnFinish.SetRef ( state, idx );
}

//----------------------------------------------------------------//
void MOAIDataIOTask::SetInflateOnLoad ( bool inflateOnLoad, bool inflateOnTaskThread, int windowBits ) {

	this->mInflateOnLoad = inflateOnLoad;
	this->mInflateOnTaskThread = inflateOnTaskThread;
	this->mWindowBits = windowBits;
}
