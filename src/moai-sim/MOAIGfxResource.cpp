// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGfxResource.h>
#include <moai-sim/MOAIGfxResourceClerk.h>
#include <moai-sim/MOAIRenderMgr.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getAge
	@text	Returns the 'age' of the graphics resource. The age
			is the number of times MOAIRenderMgr has rendered a scene
			since the resource was last bound. It is part of the
			render count, not a timestamp. This may change to be
			time-based in future releases.
 
	@in		MOAIGfxResource self
	@out	number age
*/
mrb_value MOAIGfxResource::_getAge ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGfxResource, "U" )

	u32 age = MOAIRenderMgr::Get ().GetRenderCounter () - self->mLastRenderCount;

	return state.ToRValue ( age );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIGfxResource::_getResourceState ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGfxResource, "U" )

	return state.ToRValue ( self->mState );
}

//----------------------------------------------------------------//
/**	@lua	purge
	@text	Attempt to release the resource. Generally this is used when
			responding to a memory warning from the system. A resource
			will only be released if it is renewable (i.e. has a renew
			callback or contains all information needed to reload the
			resources on demand).
			
			Using soft release can save an app in extreme memory
			circumstances, but may trigger reloads of resources during
			runtime which can significantly degrade performance.
 
	@in		MOAIGfxResource self
	@opt	number age				Release only if the texture hasn't been used in X frames.
	@out	boolean released		True if the texture was actually released.
*/
mrb_value MOAIGfxResource::_purge ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGfxResource, "U" )

	int age = state.GetParamValue < int >( 1, 0 );

	return state.ToRValue ( self->Purge ( age ) );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIGfxResource::_scheduleForGPUCreate ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGfxResource, "U" )

	u32 listID = state.GetParamValue < u32 >( 1, MOAIGfxPipelineClerk::DRAWING_PIPELINE );
	self->ScheduleForGPUCreate ( listID );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setReloader
	@text	The reloaded is called prior to recreating the resource. It should
			in turn call the resources regular load or init methods.
 
	@in		MOAIGfxResource self
	@opt	function reloader
	@out	nil
*/
mrb_value MOAIGfxResource::_setReloader ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGfxResource, "U" )

	self->mReloader.SetRef ( *self, state, 1 );
	self->InvokeLoader ();
	return context;
}

//================================================================//
// MOAIGfxResource
//================================================================//

//----------------------------------------------------------------//
bool MOAIGfxResource::Affirm () {

	if ( this->mState == STATE_NEEDS_GPU_UPDATE ) {
		this->DoGPUUpdate ();
	}
	else {
		this->InvokeLoader ();
		this->DoGPUCreate ();
	}
	return this->IsReady ();
}

//----------------------------------------------------------------//
u32 MOAIGfxResource::Bind () {

//	if ( !MOAIGfxMgr::Get ().GetHasContext ()) {
//		MOAILog ( 0, MOAISTRING_MOAIGfxResource_MissingDevice );
//		return false;
//	}

	if ( this->mState != STATE_READY_TO_BIND ) {
		this->Affirm ();
	}

	// we're ready to bind, so do it
	if ( this->mState == STATE_READY_TO_BIND ) {
		this->mLastRenderCount = MOAIRenderMgr::Get ().GetRenderCounter ();
		this->OnGPUBind ();
	}
	return this->mState;
}

//----------------------------------------------------------------//
void MOAIGfxResource::Destroy () {

	this->OnGPUDeleteOrDiscard ( true );
	this->OnCPUDestroy ();
	this->mState = STATE_UNINITIALIZED;
}

//----------------------------------------------------------------//
bool MOAIGfxResource::DoCPUCreate () {

	if ( this->mState == STATE_READY_TO_BIND ) return true;
	if (( this->mState == STATE_ERROR ) || ( this->mState == STATE_UNINITIALIZED )) return false;

	// whether or not GPU is deferred, do the CPU part
	if ( this->mState == STATE_READY_FOR_CPU_CREATE ) {
		this->mState = this->OnCPUCreate () ? STATE_READY_FOR_GPU_CREATE : STATE_ERROR;
	}
	return this->mState != STATE_ERROR;
}

//----------------------------------------------------------------//
bool MOAIGfxResource::DoGPUCreate () {

	if ( this->mState == STATE_READY_TO_BIND ) return true;
	if (( this->mState == STATE_ERROR ) || ( this->mState == STATE_UNINITIALIZED )) return false;

	// if we get here, load both CPU and GPU parts

	if ( this->mState == STATE_READY_FOR_CPU_CREATE ) {
		this->mState = this->OnCPUCreate () ? STATE_READY_FOR_GPU_CREATE : STATE_ERROR;
	}

	if ( this->mState == STATE_READY_FOR_GPU_CREATE ) {
	
		// set this now; in immediate mode we'll go straight to STATE_READY_TO_BIND on success
		// otherwise we'll go there later when we get GFX_EVENT_CREATED
		// why can't we set STATE_READY_TO_BIND? becausein multi-threaded mode, it might be
		// a few more frames before we get GFX_EVENT_CREATED
		
		this->mState = STATE_PENDING;
	
		if ( this->OnGPUCreate ()) {
			MOAIGfxMgr::GetDrawingAPI ().Event ( this, GFX_EVENT_CREATED, 0 );
			this->OnCPUDestroy ();
		}
		else {
			this->mState = STATE_ERROR;
		}
	}
	return this->mState == STATE_READY_TO_BIND;
}

//----------------------------------------------------------------//
bool MOAIGfxResource::DoGPUUpdate () {

	if ( this->mState == STATE_NEEDS_GPU_UPDATE ) {
		this->mState = this->OnGPUUpdate () ? STATE_READY_TO_BIND : STATE_ERROR;
	}
	return true;
}

//----------------------------------------------------------------//
void MOAIGfxResource::FinishInit () {

	if (( this->mState == STATE_UNINITIALIZED ) || ( this->mState == STATE_ERROR )) {
		this->mState = STATE_READY_FOR_CPU_CREATE;
		this->ScheduleForGPUCreate ( MOAIGfxPipelineClerk::DRAWING_PIPELINE );
	}
	else {
		this->ScheduleForGPUUpdate ();
	}
}

//----------------------------------------------------------------//
bool MOAIGfxResource::HasReloader () {

	return ( bool )this->mReloader;
}

//----------------------------------------------------------------//
bool MOAIGfxResource::InvokeLoader () {

	if ( this->mReloader ) {
		MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
		if ( this->mReloader.PushRef ( state )) {
			state.DebugCall ( 0, 0 );
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------//
MOAIGfxResource::MOAIGfxResource () :
	mState ( STATE_UNINITIALIZED ),
	mLastRenderCount ( 0 ) {

	RTTI_SINGLE ( MOAIInstanceEventSource )

	this->mMasterLink.Data ( this );
	this->mPendingLink.Data ( this );
	
	MOAIGfxMgr::Get ().mResourceMgr.InsertGfxResource ( *this );
}

//----------------------------------------------------------------//
MOAIGfxResource::~MOAIGfxResource () {

	if ( MOAIGfxMgr::IsValid ()) {
		MOAIGfxMgr::Get ().mResourceMgr.RemoveGfxResource ( *this );
	}
	this->mReloader.Clear ();
}

//----------------------------------------------------------------//
void MOAIGfxResource::OnClearDirty () {
}

//----------------------------------------------------------------//
void MOAIGfxResource::OnGfxEvent ( u32 event, void* userdata ) {
	UNUSED ( userdata );

	if ( event == GFX_EVENT_CREATED ) {
	
		// we should only get this event if the creation sequence was successful
		this->mState = STATE_READY_TO_BIND;
		
		// let Ruby know the resource is ready for use
		this->InvokeListener ( GFX_EVENT_CREATED );
	}
}

//----------------------------------------------------------------//
bool MOAIGfxResource::Purge ( u32 age ) {

	if ( this->mState == STATE_ERROR ) return false;

	age = MOAIRenderMgr::Get ().GetRenderCounter () - age;

	if ( this->mLastRenderCount <= age ) {
		this->OnCPUDestroy ();
		this->OnGPUDeleteOrDiscard ( true );
		this->mState = STATE_READY_FOR_CPU_CREATE;
		
		this->ScheduleForGPUCreate ( MOAIGfxPipelineClerk::DRAWING_PIPELINE );
		
		return true;
	}
	return false;
}

//----------------------------------------------------------------//
void MOAIGfxResource::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIInstanceEventSource::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "STATE_UNINITIALIZED",					( u32 )STATE_UNINITIALIZED );
	state.DefineClassConst ( klass, "STATE_READY_FOR_CPU_CREATE",			( u32 )STATE_READY_FOR_CPU_CREATE );
	state.DefineClassConst ( klass, "STATE_READY_FOR_GPU_CREATE",			( u32 )STATE_READY_FOR_GPU_CREATE );
	state.DefineClassConst ( klass, "STATE_READY_TO_BIND",					( u32 )STATE_READY_TO_BIND );
	state.DefineClassConst ( klass, "STATE_ERROR",							( u32 )STATE_ERROR );
	
	state.DefineClassConst ( klass, "GFX_EVENT_CREATED",					( u32 )GFX_EVENT_CREATED );
	
	state.DefineClassConst ( klass, "DRAWING_PIPELINE",					( u32 )MOAIGfxPipelineClerk::DRAWING_PIPELINE );
	state.DefineClassConst ( klass, "LOADING_PIPELINE",					( u32 )MOAIGfxPipelineClerk::LOADING_PIPELINE );
}

//----------------------------------------------------------------//
void MOAIGfxResource::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIInstanceEventSource::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getAge", _getAge, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getResourceState", _getResourceState, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "purge", _purge, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "softRelease", _purge, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "scheduleForGPUCreate", _scheduleForGPUCreate, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setReloader", _setReloader, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAIGfxResource::Renew () {

	// any (valid) state other than error we go back to square zero
	if ( !(( this->mState == STATE_UNINITIALIZED ) || ( this->mState == STATE_ERROR ))) {
	
		this->OnGPUDeleteOrDiscard ( false ); // clear out the resource id (if any)
		this->mState = STATE_READY_FOR_CPU_CREATE;
		this->InvokeLoader ();
		this->DoGPUCreate ();
	}
}

//----------------------------------------------------------------//
bool MOAIGfxResource::ScheduleForGPUCreate ( u32 pipelineID ) {

	if ( this->mState == STATE_READY_TO_BIND ) return true;
	if (( this->mState == STATE_UNINITIALIZED ) || ( this->mState == STATE_ERROR )) return false;
	
	if ( MOAIGfxMgr::IsValid ()) {
		MOAIGfxMgr::Get ().mResourceMgr.ScheduleGPUAffirm ( *this, pipelineID );
	}
	return true;
}

//----------------------------------------------------------------//
bool MOAIGfxResource::ScheduleForGPUUpdate () {

	if ( this->mState != STATE_READY_TO_BIND ) return false;

	this->mState = STATE_NEEDS_GPU_UPDATE;

	if ( MOAIGfxMgr::IsValid ()) {
		MOAIGfxMgr::Get ().mResourceMgr.ScheduleGPUAffirm ( *this, MOAIGfxPipelineClerk::DRAWING_PIPELINE ); // always update in the drawing pipeline
	}
	return true;
}

//----------------------------------------------------------------//
void MOAIGfxResource::Unbind () {

	this->OnGPUUnbind ();
}
