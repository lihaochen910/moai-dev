// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <moai-sim/MOAIFrameBuffer.h>
#include <moai-sim/MOAIFrameBufferTexture.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGfxResource.h>
#include <moai-sim/MOAIGfxResourceClerk.h>
#include <moai-sim/MOAIShader.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAIShaderProgram.h>
#include <moai-sim/MOAISim.h>
#include <moai-sim/MOAITexture.h>
#include <moai-sim/MOAIVertexFormat.h>
#include <moai-sim/MOAIVertexFormatMgr.h>
#include <moai-sim/MOAIViewport.h>
#include <moai-sim/strings.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIGfxMgr::_enablePipelineLogging ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIGfxMgr, "" )

	MOAIGfxMgr::Get ().mPipelineMgr.EnablePipelineLogging ( state.GetParamValue < bool >( 1, false ));

	ZLFileSys::DeleteDirectory ( GFX_PIPELINE_LOGGING_FOLDER, true, true );
	ZLFileSys::AffirmPath ( GFX_PIPELINE_LOGGING_FOLDER );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	getFrameBuffer
	@text	Returns the frame buffer associated with the device.

	@out	MOAIFrameBuffer frameBuffer
*/
mrb_value MOAIGfxMgr::_getFrameBuffer ( mrb_state* M, mrb_value context ) {

	MOAIRubyState state ( M );

	return state.ToRValue < MOAIRubyObject* >( MOAIGfxMgr::Get ().mGfxState.GetDefaultFrameBuffer ());
}

//----------------------------------------------------------------//
/**	@lua	getMaxTextureSize
	@text	Returns the maximum texture size supported by device
 
	@out	number maxTextureSize
*/
mrb_value MOAIGfxMgr::_getMaxTextureSize ( mrb_state* M, mrb_value context ) {
	
	MOAIRubyState state ( M );
	
	return state.ToRValue ( MOAIGfxMgr::Get ().mMaxTextureSize );
}

//----------------------------------------------------------------//
/**	@lua	getMaxTextureUnits
	@text	Returns the total number of texture units available on the device.

	@out	number maxTextureUnits
*/
mrb_value MOAIGfxMgr::_getMaxTextureUnits ( mrb_state* M, mrb_value context ) {

	MOAIRubyState state ( M );

	return state.ToRValue ( ( double )MOAIGfxMgr::Get ().mGfxState.CountTextureUnits () );
}

//----------------------------------------------------------------//
/**	@lua	getViewSize
	@text	Returns the width and height of the view
	
	@out	number width
	@out	number height
*/
mrb_value MOAIGfxMgr::_getViewSize ( mrb_state* M, mrb_value context  ) {

	MOAIFrameBuffer* frameBuffer = MOAIGfxMgr::Get ().mGfxState.GetCurrentFrameBuffer ();
	
	MOAIRubyState state ( M );
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( frameBuffer->GetBufferWidth () );
	ret [ 1 ] = state.ToRValue ( frameBuffer->GetBufferHeight () );

	return mrb_ary_new_from_values ( M, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	purgeResources
	@text	Purges all resources older that a given age (in render cycles).
			If age is 0 then all resources are purged.
 
	@opt	number age		Default value is 0.
	@out	nil
*/
mrb_value MOAIGfxMgr::_purgeResources ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	u32 age = state.GetParamValue < u32 >( 1, 0 );

	MOAIGfxMgr::Get ().mResourceMgr.PurgeResources ( age );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	renewResources
	@text	Renews all resources.
 
	@out	nil
*/
mrb_value MOAIGfxMgr::_renewResources ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	MOAIGfxMgr::Get ().mResourceMgr.RenewResources ();
	
	return context;
}

//================================================================//
// MOAIGfxMgr
//================================================================//

//----------------------------------------------------------------//
void MOAIGfxMgr::ClearErrors () {

	#ifndef MOAI_OS_NACL
		if ( this->mHasContext ) {
			while ( ZLGfxDevice::GetError () != ZGL_ERROR_NONE );
		}
	#endif
}

//----------------------------------------------------------------//
void MOAIGfxMgr::DetectContext () {

	this->mHasContext = true;
	
	ZLGfxDevice::Initialize ();
	
	u32 maxTextureUnits = ZLGfxDevice::GetCap ( ZGL_CAPS_MAX_TEXTURE_UNITS );
	this->mGfxState.InitTextureUnits ( maxTextureUnits );
	
	this->mMaxTextureSize = ZLGfxDevice::GetCap ( ZGL_CAPS_MAX_TEXTURE_SIZE );

	// renew resources in immediate mode
	this->mPipelineMgr.SelectDrawingAPI ();
	
	this->mGfxState.GetDefaultFrameBuffer ()->DetectGLFrameBufferID ();
	
	MOAIShaderMgr::Get ().AffirmAll ();
	
	mResourceMgr.RenewResources ();
}

//----------------------------------------------------------------//
void MOAIGfxMgr::DetectFramebuffer () {
	
	this->mGfxState.GetDefaultFrameBuffer ()->DetectGLFrameBufferID ();
	this->mGfxState.SetFrameBuffer ();
}

//----------------------------------------------------------------//
u32 MOAIGfxMgr::LogErrors () {

	u32 count = 0;
	#ifndef MOAI_OS_NACL
		if ( this->mHasContext ) {
			for ( u32 error = ZLGfxDevice::GetError (); error != ZGL_ERROR_NONE; error = ZLGfxDevice::GetError (), ++count ) {
				MOAILogF ( 0, ZLLog::LOG_ERROR, MOAISTRING_MOAIGfxDevice_OpenGLError_S, ZLGfxDevice::GetErrorString ( error ));
			}
		}
	#endif
	return count;
}

//----------------------------------------------------------------//
MOAIGfxMgr::MOAIGfxMgr () :
	mHasContext ( false ),
	mIsFramebufferSupported ( 0 ),
	#if defined ( MOAI_OS_NACL ) || defined ( MOAI_OS_IPHONE ) || defined ( MOAI_OS_ANDROID ) || defined ( EMSCRIPTEN )
		mIsOpenGLES ( true ),
	#else
		mIsOpenGLES ( false ),
	#endif
	mMajorVersion ( 0 ),
	mMinorVersion ( 0 ),
	mTextureMemoryUsage ( 0 ),
	mMaxTextureSize ( 0 ) {
	
	RTTI_BEGIN
		RTTI_SINGLE ( MOAIGfxStateGPUCache )
		RTTI_SINGLE ( MOAIGlobalEventSource )
	RTTI_END

	this->mGfxState.SetDefaultFrameBuffer ( MOAIRubyRuntime::Get ().State ().CreateClassInstance < MOAIFrameBuffer >() );
}

//----------------------------------------------------------------//
MOAIGfxMgr::~MOAIGfxMgr () {

	this->mGfxState.SetDefaultFrameBuffer ( 0 );
	this->mGfxState.SetDefaultTexture ( 0 );
}

//----------------------------------------------------------------//
void MOAIGfxMgr::OnGlobalsFinalize () {

	this->mGfxState.SetDefaultFrameBuffer ( 0 );
	this->mGfxState.SetDefaultTexture ( 0 );
	
	mResourceMgr.ProcessDeleters ();
}

//----------------------------------------------------------------//
void MOAIGfxMgr::OnGlobalsInitialize () {

	//this->mGfxState.InitBuffers ();
}

//----------------------------------------------------------------//
void MOAIGfxMgr::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "EVENT_RESIZE",	( u32 )EVENT_RESIZE );
	
	state.DefineClassConst ( klass, "DRAWING_PIPELINE",	( u32 )MOAIGfxPipelineClerk::DRAWING_PIPELINE );
	state.DefineClassConst ( klass, "LOADING_PIPELINE",	( u32 )MOAIGfxPipelineClerk::LOADING_PIPELINE );

	state.DefineStaticMethod ( klass, "enablePipelineLogging",	_enablePipelineLogging, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getFrameBuffer",			_getFrameBuffer, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getListener",			&MOAIGlobalEventSource::_getListener < MOAIGfxMgr >, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getMaxTextureSize",		_getMaxTextureSize, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getMaxTextureUnits",		_getMaxTextureUnits, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getViewSize",			_getViewSize, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "purgeResources",			_purgeResources, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "renewResources",			_renewResources, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "setListener",			&MOAIGlobalEventSource::_setListener < MOAIGfxMgr >, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAIGfxMgr::ReportTextureAlloc ( cc8* name, size_t size ) {

	this->mTextureMemoryUsage += size;
	float mb = ( float )this->mTextureMemoryUsage / 1024.0f / 1024.0f;
	MOAILogF ( 0, ZLLog::LOG_STATUS, MOAISTRING_MOAITexture_MemoryUse_SDFS, "+", size / 1024, mb, name );
}

//----------------------------------------------------------------//
void MOAIGfxMgr::ReportTextureFree ( cc8* name, size_t size ) {

	this->mTextureMemoryUsage -= size;
	float mb = ( float )this->mTextureMemoryUsage / 1024.0f / 1024.0f;
	MOAILogF ( 0, ZLLog::LOG_STATUS, MOAISTRING_MOAITexture_MemoryUse_SDFS, "-", size / 1024, mb, name );
}

//----------------------------------------------------------------//
void MOAIGfxMgr::ResetDrawCount () {
	//this->mDrawCount = 0;
}

//----------------------------------------------------------------//
void MOAIGfxMgr::SetBufferScale ( float scale ) {

	this->mGfxState.GetDefaultFrameBuffer ()->SetBufferScale ( scale );
}

//----------------------------------------------------------------//
void MOAIGfxMgr::SetBufferSize ( u32 width, u32 height ) {

	this->mGfxState.GetDefaultFrameBuffer ()->SetBufferSize ( width, height );
}
