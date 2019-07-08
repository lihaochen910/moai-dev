// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIGfxBuffer.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGfxResourceClerk.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAIShader.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAITextureBase.h>
#include <moai-sim/MOAIVertexArray.h>
#include <moai-sim/MOAIVertexBuffer.h>
#include <moai-sim/MOAIVertexFormat.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIVertexArray::_reserveVAOs ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVertexArray, "U" )
	
	self->ReserveVAOs ( state.GetParamValue < u32 >( 1, 0 ));
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIVertexArray::_reserveVertexBuffers ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVertexArray, "U" )
	
	self->ReserveVertexBuffers ( state.GetParamValue < u32 >( 1, 0 ));
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIVertexArray::_setVertexBuffer ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVertexArray, "U" )
	
	u32 baseParam = 1;
	u32 idx = 0;
	
	if ( state.ParamIsType ( baseParam, MRB_TT_FIXNUM )) {
		idx = state.GetParamValue < u32 >( baseParam++, 1 ) - 1;
	}
	
	MOAIVertexBuffer* buffer	= state.GetRubyObject < MOAIVertexBuffer >( baseParam++, false );
	MOAIVertexFormat* format	= state.GetRubyObject < MOAIVertexFormat >( baseParam++, false );
	
	self->SetVertexBuffer ( idx, buffer, format );

	return context;
}

//================================================================//
// MOAIVertexArray
//================================================================//

//----------------------------------------------------------------//
bool MOAIVertexArray::AffirmVertexBuffers ( u32 idx ) {

	if (( this->mVertexBuffers.Size () == 0 ) && ( idx == 0 )) {
		this->ReserveVertexBuffers ( 1 );
	}
	return ( idx < this->mVertexBuffers.Size ());
}

//----------------------------------------------------------------//
void MOAIVertexArray::BindVertexArrayItems () {

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	size_t totalVBOs = this->mVertexBuffers.Size ();
	for ( size_t i = 0; i < totalVBOs; ++i ) {
		gfxState.BindVertexBufferWithFormat ( this->mVertexBuffers [ i ], this->mUseVAOs );
	}
}

//----------------------------------------------------------------//
MOAIVertexBuffer* MOAIVertexArray::GetVertexBuffer ( u32 idx ) {

	return idx < this->mVertexBuffers.Size () ? this->mVertexBuffers [ idx ].mBuffer : ( MOAIVertexBuffer* )0;
}

//----------------------------------------------------------------//
MOAIVertexFormat* MOAIVertexArray::GetVertexFormat ( u32 idx ) {

	return idx < this->mVertexBuffers.Size () ? this->mVertexBuffers [ idx ].mFormat : ( MOAIVertexFormat* )0;
}

//----------------------------------------------------------------//
MOAIVertexArray::MOAIVertexArray () :
	mUseVAOs ( false ) {

	RTTI_BEGIN
		RTTI_EXTEND ( MOAIGfxResource )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIVertexArray::~MOAIVertexArray () {

	this->ReserveVAOs ( 0 );
	this->ReserveVertexBuffers ( 0 );
}

//----------------------------------------------------------------//
bool MOAIVertexArray::OnCPUCreate () {

	return true;
}

//----------------------------------------------------------------//
void MOAIVertexArray::OnCPUDestroy () {
}

//----------------------------------------------------------------//
void MOAIVertexArray::OnGPUBind () {

	if ( this->mUseVAOs && this->mVAOs.Size ()) {
		MOAIGfxMgr::GetDrawingAPI ().BindVertexArray ( this->mVAOs [ this->mCurrentVAO ]);
	}
	else {
		this->BindVertexArrayItems ();
	}
}

//----------------------------------------------------------------//
bool MOAIVertexArray::OnGPUCreate () {

	ZLGfx& gfx = MOAIGfxMgr::GetDrawingAPI ();

	this->mUseVAOs = false;
	
	size_t totalVAOs = this->mVAOs.Size ();

	if ( totalVAOs ) {
		
		for ( size_t i = 0; i < totalVAOs; ++i ) {
			ZLGfxHandle vao = gfx.CreateVertexArray (); // OK for this to return 0
			if ( vao.GetType () != ZLGfxResource::NONE ) {
				this->mVAOs [ i ] = vao;
				this->mUseVAOs = true;
			}
		}
	}
	
	this->mCurrentVAO = 0;
	this->OnGPUUpdate ();
	
	return true;
}

//----------------------------------------------------------------//
void MOAIVertexArray::OnGPUDeleteOrDiscard ( bool shouldDelete ) {

	for ( size_t i = 0; i < this->mVAOs.Size (); ++i ) {
		MOAIGfxResourceClerk::DeleteOrDiscard ( this->mVAOs [ i ], shouldDelete );
	}
}

//----------------------------------------------------------------//
void MOAIVertexArray::OnGPUUnbind () {

	if ( this->mUseVAOs ) {
		MOAIGfxMgr::GetDrawingAPI ().BindVertexArray ( ZLGfxResource::UNBIND );
	}
	else {
		this->UnbindVertexArrayItems ();
	}
}

//----------------------------------------------------------------//
bool MOAIVertexArray::OnGPUUpdate () {

	if ( !this->mUseVAOs ) return true;
	if ( !this->mVAOs.Size ()) return false;

	this->mCurrentVAO = ( this->mCurrentVAO + 1 ) % this->mVAOs.Size ();
	const ZLGfxHandle& vao = this->mVAOs [ this->mCurrentVAO ];
	
	if ( vao.CanBind ()) {
		ZLGfx& gfx = MOAIGfxMgr::GetDrawingAPI ();
		gfx.BindVertexArray ( vao );
		this->BindVertexArrayItems ();
		gfx.BindVertexArray ( ZLGfxResource::UNBIND );
		return true;
	}
	return false;
}

//----------------------------------------------------------------//
void MOAIVertexArray::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIGfxResource::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIVertexArray::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIGfxResource::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "reserveVAOs", _reserveVAOs, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "reserveVertexBuffers", _reserveVertexBuffers, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setVertexBuffer", _setVertexBuffer, MRB_ARGS_ANY () );

}

//----------------------------------------------------------------//
void MOAIVertexArray::ReserveVAOs ( u32 total ) {

	if ( MOAIGfxMgr::IsValid ()) {
		for ( size_t i = 0; i < this->mVAOs.Size (); ++i ) {
			MOAIGfxResourceClerk::DeleteOrDiscard ( this->mVAOs [ i ], false );
		}
	}
	this->mVAOs.Init ( total );
	
	this->FinishInit ();
}

//----------------------------------------------------------------//
void MOAIVertexArray::ReserveVertexBuffers ( u32 total ) {

	for ( size_t i = 0; i < this->mVertexBuffers.Size (); ++i ) {
		this->mVertexBuffers [ i ].SetBufferAndFormat ( *this, 0, 0 );
	}
	this->mVertexBuffers.Init ( total );
	
	this->FinishInit ();
}

//----------------------------------------------------------------//
void MOAIVertexArray::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {

	/*u32 totalVAOs = state.GetFieldValue < u32 >( -1, "mTotalVAOs", 0 );
	this->ReserveVAOs ( totalVAOs );
	
	u32 totalVertexBuffers = state.GetFieldValue < u32 >( -1, "mTotalVertexBuffers", 0 );
	this->ReserveVertexBuffers ( totalVertexBuffers );
	
	if ( state.PushFieldWithType ( -1, "mVertexBuffers", LUA_TTABLE )) {
		int itr = state.PushTableItr ( -1 );
		for ( u32 i = 0; state.TableItrNext ( itr ); ++i ) {
			if ( state.ParamIsType ( -1, LUA_TTABLE )) {
				MOAIVertexBuffer* buffer = serializer.MemberIDToObject < MOAIVertexBuffer >( state.GetFieldValue < MOAISerializer::ObjID >( -1, "mBuffer", 0 ));
				MOAIVertexFormat* format = serializer.MemberIDToObject < MOAIVertexFormat >( state.GetFieldValue < MOAISerializer::ObjID >( -1, "mFormat", 0 ));
				this->SetVertexBuffer ( i, buffer, format );
			}
		}
		state.Pop ();
	}
	this->FinishInit ();*/
}

//----------------------------------------------------------------//
void MOAIVertexArray::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {

	/*state.SetField < u32 >( -1, "mTotalVAOs", ( u32 )this->mVAOs.Size ());
	state.SetField < u32 >( -1, "mTotalVertexBuffers", ( u32 )this->mVertexBuffers.Size ());
	
	lua_newtable ( state );
	for ( u32 i = 0; i < this->mVertexBuffers.Size (); ++i ) {
		state.Push ( i + 1 );
		lua_newtable ( state );
		state.SetField < MOAISerializer::ObjID >( -1, "mBuffer", serializer.AffirmMemberID ( this->mVertexBuffers [ i ].mBuffer ));
		state.SetField < MOAISerializer::ObjID >( -1, "mFormat", serializer.AffirmMemberID ( this->mVertexBuffers [ i ].mFormat ));
		lua_settable ( state, -3 );
	}
	lua_setfield ( state, -2, "mVertexBuffers" );*/
}

//----------------------------------------------------------------//
void MOAIVertexArray::SetVertexBuffer ( u32 idx, MOAIVertexBuffer* vtxBuffer, MOAIVertexFormat* vtxFormat ) {

	if ( this->AffirmVertexBuffers ( idx )) {
		this->mVertexBuffers [ idx ].SetBufferAndFormat ( *this, vtxBuffer, vtxFormat );
		this->ScheduleForGPUUpdate ();
	}
}

//----------------------------------------------------------------//
void MOAIVertexArray::UnbindVertexArrayItems () {

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;

	size_t totalVBOs = this->mVertexBuffers.Size ();
	for ( size_t i = 0; i < totalVBOs; ++i ) {
		gfxState.UnbindVertexBufferWithFormat ( this->mVertexBuffers [ i ]);
	}
}
