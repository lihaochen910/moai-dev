// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIGfxBuffer.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGfxResourceClerk.h>
#include <moai-sim/MOAIVertexFormat.h>
#include <moai-sim/MOAIVertexFormatMgr.h>

//================================================================//
// MOAIGfxBufferBase
//================================================================//

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	copyFromStream
	@text	Copies buffer contents from a stream.
	
	@in		MOAIGfxBuffer self
	@in		MOAIStream stream
	@opt	number length
	@out	nil
*/
mrb_value MOAIGfxBuffer::_copyFromStream ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGfxBuffer, "U" )
	
	MOAIStream* stream = state.GetRubyObject < MOAIStream >( 1, true );
	if ( stream ) {
	
		size_t size = state.GetParamValue < u32 >( 2, ( u32 )( stream->GetLength () - stream->GetCursor () ));
		self->CopyFromStream ( *stream, size );
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	release
	@text	Releases any memory associated with buffer.
	
	@in		MOAIGfxBuffer self
	@out	nil
*/
mrb_value	MOAIGfxBuffer::_release ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGfxBuffer, "U" )
	
	self->Clear ();
	return context;
}

//----------------------------------------------------------------//
/**	@lua	reserve
	@text	Sets capacity of buffer in bytes.
	
	@in		MOAIGfxBuffer self
	@in		number size
	@out	nil
*/
mrb_value	MOAIGfxBuffer::_reserve ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGfxBuffer, "UN" )
	
	u32 size = state.GetParamValue < u32 >( 1, 0 );
	self->Reserve ( size );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	reserveVBOs
	@text	Reserves one or more VBO objects. Multi-buffering is
			supported via multiple VBOs.
	
	@in		MOAIGfxBuffer self
	@in		number count
	@out	nil
*/
mrb_value	MOAIGfxBuffer::_reserveVBOs ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGfxBuffer, "UN" )

	u32 vbos = state.GetParamValue < u32 >( 1, 0 );
	self->ReserveVBOs ( vbos );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	scheduleFlush
	@text	Trigger an update of the GPU-side buffer. Call this when
			the backing buffer has been altered.
	
	@in		MOAIGfxBuffer self
	@out	nil
*/
mrb_value MOAIGfxBuffer::_scheduleFlush ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIGfxBuffer, "U" )
	
	self->ScheduleForGPUUpdate ();
	return context;
}

//================================================================//
// MOAIGfxBuffer
//================================================================//

//----------------------------------------------------------------//
void MOAIGfxBuffer::Clear () {

	this->ZLCopyOnWrite::Free ();

	this->mVBOs.Clear ();
	this->mCurrentVBO = 0;
	
	this->Destroy ();
}

//----------------------------------------------------------------//
void MOAIGfxBuffer::CopyFromStream ( ZLStream& stream, size_t size ) {

	this->Reserve (( u32 )size );
	this->WriteStream ( stream );
	
	this->ScheduleForGPUUpdate ();
}

//----------------------------------------------------------------//
ZLSharedConstBuffer* MOAIGfxBuffer::GetBufferForBind ( ZLGfx& gfx ) {
	UNUSED(gfx);
	return this->mUseVBOs ? 0 : this->ZLCopyOnWrite::GetSharedConstBuffer ();

//	if ( this->mUseVBOs ) return 0;
//
//	ZLSharedConstBuffer* buffer = this->ZLCopyOnWrite::GetSharedConstBuffer ();
//	return this->mCopyOnBind ? gfx.CopyBuffer ( buffer ) : buffer;
}

//----------------------------------------------------------------//
MOAIGfxBuffer::MOAIGfxBuffer () :
	mCurrentVBO ( 0 ),
	mTarget ( ZGL_BUFFER_TARGET_ARRAY ),
	mLoader ( 0 ),
	mUseVBOs ( false ),
	mCopyOnUpdate ( false ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIGfxResource )
		RTTI_EXTEND ( MOAIStream )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIGfxBuffer::~MOAIGfxBuffer () {

	this->Clear ();
}

//----------------------------------------------------------------//
bool MOAIGfxBuffer::OnCPUCreate () {
	return true;
}

//----------------------------------------------------------------//
void MOAIGfxBuffer::OnCPUDestroy () {
}

//----------------------------------------------------------------//
void MOAIGfxBuffer::OnGPUBind () {
	
	if ( !this->mUseVBOs ) return;
	
	const ZLGfxHandle& vbo = this->mVBOs [ this->mCurrentVBO ];
	
	if ( vbo.CanBind ()) {
		MOAIGfxMgr::GetDrawingAPI ().BindBuffer ( this->mTarget, vbo );
	}
}

//----------------------------------------------------------------//
bool MOAIGfxBuffer::OnGPUCreate () {

	u32 count = 0;
	ZLGfx& gfx = MOAIGfxMgr::GetDrawingAPI ();

	this->mUseVBOs = ( this->mVBOs.Size () > 0 );
	
	if ( this->mUseVBOs ) {

		u32 hint = this->mVBOs.Size () > 1 ? ZGL_BUFFER_USAGE_STREAM_DRAW : ZGL_BUFFER_USAGE_STATIC_DRAW;

		for ( u32 i = 0; i < this->mVBOs.Size (); ++i ) {
			
			ZLGfxHandle vbo = gfx.CreateBuffer ();
			
			// TODO: error handling
			//if ( vbo ) {
			
				ZLSharedConstBuffer* buffer = this->GetCursor () ? this->GetSharedConstBuffer () : 0;
				
//				if ( this->mCopyOnUpdate ) {
//					buffer = gfx.CopyBuffer ( buffer );
//				}
			
				gfx.BindBuffer ( this->mTarget, vbo );
				gfx.BufferData ( this->mTarget, this->GetLength (), buffer, 0, hint );
				gfx.BindBuffer ( this->mTarget, ZLGfxResource::UNBIND );
				
				count++;
			//}
			this->mVBOs [ i ] = vbo;
		}
	}
	
	return count == this->mVBOs.Size ();
}

//----------------------------------------------------------------//
void MOAIGfxBuffer::OnGPUDeleteOrDiscard ( bool shouldDelete ) {

	for ( u32 i = 0; i < this->mVBOs.Size (); ++i ) {
		MOAIGfxResourceClerk::DeleteOrDiscard ( this->mVBOs [ i ], shouldDelete );
	}
}

//----------------------------------------------------------------//
void MOAIGfxBuffer::OnGPUUnbind () {

	MOAIGfxMgr::GetDrawingAPI ().BindBuffer ( this->mTarget, ZLGfxResource::UNBIND ); // OK?
}

//----------------------------------------------------------------//
bool MOAIGfxBuffer::OnGPUUpdate () {

	if ( !this->mUseVBOs ) return true;
	
	bool dirty = this->GetCursor () > 0;
	
	if ( dirty ) {
		this->mCurrentVBO = ( this->mCurrentVBO + 1 ) % this->mVBOs.Size ();
	}
	
	const ZLGfxHandle& vbo = this->mVBOs [ this->mCurrentVBO ];
	
	if ( dirty && vbo.CanBind ()) {
		
		// TODO: There are a few different ways to approach updating buffers with varying performance
		// on different platforms. The approach here is just to multi-buffer the VBO and replace its
		// contents via zglBufferSubData when they change. The TODO here is to do performance tests
		// on multiple devices, evaluate other approaches and possible expose the configuration of
		// those to the end user via Ruby.
	
		ZLGfx& gfx = MOAIGfxMgr::GetDrawingAPI ();
		
		ZLSharedConstBuffer* buffer = this->GetSharedConstBuffer ();
		
//		if ( this->mCopyOnUpdate ) {
//			buffer = gfx.CopyBuffer ( buffer );
//		}
		
		gfx.BindBuffer ( this->mTarget, vbo );
		gfx.BufferSubData ( this->mTarget, 0, this->GetCursor (), buffer, 0 );
		gfx.BindBuffer ( this->mTarget, ZLGfxResource::UNBIND );
	
		//u32 hint = this->mVBOs.Size () > 1 ? ZGL_BUFFER_USAGE_DYNAMIC_DRAW : ZGL_BUFFER_USAGE_STATIC_DRAW;
		//zglBufferData ( this->mTarget, this->GetLength (), 0, hint );
	}
	
	return true;
}

//----------------------------------------------------------------//
void MOAIGfxBuffer::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIGfxResource::RegisterRubyClass ( state, klass );
	MOAIStream::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "INDEX_BUFFER",			( u32 )ZGL_BUFFER_TARGET_ELEMENT_ARRAY );
	state.DefineClassConst ( klass, "VERTEX_BUFFER",			( u32 )ZGL_BUFFER_TARGET_ARRAY );
}

//----------------------------------------------------------------//
void MOAIGfxBuffer::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIGfxResource::RegisterRubyFuncs ( state, klass );
	MOAIStream::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "copyFromStream", _copyFromStream, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "release", _release, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "reserve", _reserve, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "reserveVBOs", _reserveVBOs, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "scheduleFlush", _scheduleFlush, MRB_ARGS_NONE () );

}

//----------------------------------------------------------------//
void MOAIGfxBuffer::Reserve ( u32 size ) {
	
	this->ZLCopyOnWrite::Free ();
	
	if ( size ) {
		this->ZLCopyOnWrite::Reserve ( size );
		this->FinishInit ();
	}
}

//----------------------------------------------------------------//
void MOAIGfxBuffer::ReserveVBOs ( u32 gpuBuffers ) {

	if ( gpuBuffers < this->mVBOs.Size ()) {
		this->mVBOs.Clear ();
	}

	if ( gpuBuffers ) {
		this->mVBOs.Resize ( gpuBuffers );
		this->mCurrentVBO = gpuBuffers - 1;
	}

	this->FinishInit ();
}

//----------------------------------------------------------------//
void MOAIGfxBuffer::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	UNUSED ( serializer );

	// TODO:
	//u32 totalVBOs		= state.GetFieldValue < u32 >( -1, "mTotalVBOs", 0 );
	//u32 size			= state.GetFieldValue < u32 >( -1, "mSize", 0 );

	//this->Reserve ( size );
	//this->ReserveVBOs ( totalVBOs );
	//
	//state.PushField ( -1, "mData" );
	//if ( state.IsType ( -1, LUA_TSTRING )) {
	//	
	//	STLString zipString = lua_tostring ( state, -1 );
	//	size_t unzipLen = zipString.zip_inflate ( this->ZLCopyOnWrite::Invalidate (), size );
	//	assert ( unzipLen == size ); // TODO: fail gracefully
	//	UNUSED ( unzipLen ); // TODO: this *should* be handled by the zl assert redefine
	//	
	//	this->Seek ( size, SEEK_SET );
	//}
	//lua_pop ( state, 1 );
	
	this->ScheduleForGPUUpdate ();
	this->FinishInit ();
}

//----------------------------------------------------------------//
void MOAIGfxBuffer::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	UNUSED ( serializer );

	// TODO:
	/*size_t size = this->GetLength ();

	state.SetField < u32 >( -1, "mTotalVBOs", ( u32 )this->mVBOs.Size ());
	state.SetField < u32 >( -1, "mSize", ( u32 )size );

	STLString zipString;
	zipString.zip_deflate ( this->ZLCopyOnWrite::GetBuffer (), size );

	lua_pushstring ( state, zipString.str ());
	lua_setfield ( state, -2, "mData" );*/
}
