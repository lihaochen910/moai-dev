// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <moai-sim/MOAINode.h>
#include <moai-sim/MOAINodeMgr.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAINodeMgr::_reset ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAINodeMgr, "" )
	self->Reset ();
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAINodeMgr::_setMaxIterations ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAINodeMgr, "" )
	self->mMaxIterations = state.GetParamValue < u32 >( 1, DEFAULT_MAX_ITERATIONS );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAINodeMgr::_update ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAINodeMgr, "" )
	self->Update ();
	return context;
}

//================================================================//
// MOAINodeMgr
//================================================================//

//----------------------------------------------------------------//
void MOAINodeMgr::InsertAfter ( MOAINode& cursor, MOAINode& node ) {

	if ( cursor.mNext ) {
	
		node.mPrev = &cursor;
		node.mNext = cursor.mNext;
		
		node.mPrev->mNext = &node;
		node.mNext->mPrev = &node;
	}
	else {
		this->PushBack ( node );
	}
}

//----------------------------------------------------------------//
void MOAINodeMgr::InsertBefore ( MOAINode& cursor, MOAINode& node ) {

	if ( cursor.mPrev ) {
		node.mPrev = cursor.mPrev;
		node.mNext = &cursor;
		
		node.mPrev->mNext = &node;
		node.mNext->mPrev = &node;
	}
	else {
		this->PushFront ( node );
	}
}

//----------------------------------------------------------------//
MOAINodeMgr::MOAINodeMgr () :
	mUpdateListHead ( 0 ),
	mUpdateListTail ( 0 ),
	mScheduled ( false ),
	mMaxIterations ( DEFAULT_MAX_ITERATIONS ) {
	
	RTTI_SINGLE ( MOAIGlobalEventSource )
}

//----------------------------------------------------------------//
MOAINodeMgr::~MOAINodeMgr () {

	MOAINode* cursor = this->mUpdateListHead;
	while ( cursor ) {
		MOAINode* node = cursor;
		cursor = cursor->mNext;
		
		node->mState = MOAINode::STATE_IDLE;
	}
}

//----------------------------------------------------------------//
void MOAINodeMgr::PushBack ( MOAINode& node ) {

	node.mNext = 0;
	node.mPrev = 0;

	if ( !this->mUpdateListHead ) {
		this->mUpdateListHead = &node;
		this->mUpdateListTail = &node;
	}
	else {
		node.mPrev = this->mUpdateListTail;
		this->mUpdateListTail->mNext = &node;
		this->mUpdateListTail = &node;
	}
}

//----------------------------------------------------------------//
void MOAINodeMgr::PushFront ( MOAINode& node ) {

	node.mNext = 0;
	node.mPrev = 0;

	if ( !this->mUpdateListHead ) {
		this->mUpdateListHead = &node;
		this->mUpdateListTail = &node;
	}
	else {
		node.mNext = this->mUpdateListHead;
		this->mUpdateListHead->mPrev = &node;
		this->mUpdateListHead = &node;
	}
}

//----------------------------------------------------------------//
void MOAINodeMgr::Remove ( MOAINode& node ) {

	if ( node.mNext ) {
		node.mNext->mPrev = node.mPrev;
	}
	else {
		this->mUpdateListTail = node.mPrev;
	}

	if ( node.mPrev ) {
		node.mPrev->mNext = node.mNext;
	}
	else {
		this->mUpdateListHead = node.mNext;
	}
}

//----------------------------------------------------------------//
void MOAINodeMgr::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineStaticMethod ( klass, "reset",				_reset, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "setMaxIterations",	_setMaxIterations, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "update",				_update, MRB_ARGS_NONE () );
}

//----------------------------------------------------------------//
void MOAINodeMgr::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAINodeMgr::Reset () {
	
	// TODO: fix this up later
	for ( MOAINode* node = this->mUpdateListHead; node; node = node->mNext ) {
		node->mState = MOAINode::STATE_IDLE;
	}
	
	this->mUpdateListHead = 0;
	this->mUpdateListTail = 0;
}

//----------------------------------------------------------------//
void MOAINodeMgr::Update () {

	for ( u32 iterations = 0; this->mScheduled && ( iterations < this->mMaxIterations ); ++iterations ) {

		this->mScheduled = false;

		MOAINode* node = this->mUpdateListHead;
		for ( ; node ; node = node->mNext ) {
			node->DepNodeUpdate ();
		}
	}
	
	if ( !this->mScheduled ) {
		this->Reset ();
	}
}
