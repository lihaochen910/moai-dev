// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIVertexFormat.h>
#include <moai-sim/MOAIVertexFormatMgr.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIVertexFormatMgr::_getFormat ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	u32 formatID = state.GetParamValue < u32 >( 1, 0xffffffff );
	
	return state.ToRValue < MOAIRubyObject* >( MOAIVertexFormatMgr::Get ().GetFormat ( formatID ) );
}

//================================================================//
// MOAIVertexFormatMgr
//================================================================//

//----------------------------------------------------------------//
MOAIVertexFormat* MOAIVertexFormatMgr::GetFormat ( u32 formatID ) {
	MOAIRubyState& state = MOAIRubyRuntime::Get ().GetMainState ();
	
	MOAIVertexFormat* format = 0;
	
	if ( formatID < TOTAL_FORMATS ) {
	
		format = this->mFormats [ formatID ];
		
		if ( !format ) {

			format = state.CreateClassInstance < MOAIVertexFormat >();
			this->RubyRetain ( format );
			
			switch ( formatID ) {
				
				case XYZC:
					format->DeclareAttribute ( XYZC_POSITION, ZGL_TYPE_FLOAT, 3, MOAIVertexFormat::ATTRIBUTE_COORD, false );
					format->DeclareAttribute ( XYZC_COLOR, ZGL_TYPE_UNSIGNED_BYTE, 4, MOAIVertexFormat::ATTRIBUTE_COLOR, true );
					break;
				
				case XYZWC:
					format->DeclareAttribute ( XYZWC_POSITION, ZGL_TYPE_FLOAT, 4, MOAIVertexFormat::ATTRIBUTE_COORD, false );
					format->DeclareAttribute ( XYZWC_COLOR, ZGL_TYPE_UNSIGNED_BYTE, 4, MOAIVertexFormat::ATTRIBUTE_COLOR, true );
					break;
				
				case XYZWUVC:
					format->DeclareAttribute ( XYZWUVC_POSITION, ZGL_TYPE_FLOAT, 4, MOAIVertexFormat::ATTRIBUTE_COORD, false );
					format->DeclareAttribute ( XYZWUVC_TEXCOORD, ZGL_TYPE_FLOAT, 2, MOAIVertexFormat::ATTRIBUTE_TEX_COORD, false );
					format->DeclareAttribute ( XYZWUVC_COLOR, ZGL_TYPE_UNSIGNED_BYTE, 4, MOAIVertexFormat::ATTRIBUTE_COLOR, true );
					break;
				
				case XYZWNNNC:
					format->DeclareAttribute ( XYZWNNNC_POSITION, ZGL_TYPE_FLOAT, 4, MOAIVertexFormat::ATTRIBUTE_COORD, false );
					format->DeclareAttribute ( XYZWNNNC_NORMAL, ZGL_TYPE_FLOAT, 3, MOAIVertexFormat::ATTRIBUTE_NORMAL, false );
					format->DeclareAttribute ( XYZWNNNC_COLOR, ZGL_TYPE_UNSIGNED_BYTE, 4, MOAIVertexFormat::ATTRIBUTE_COLOR, true );
					break;
				
				case XYZWNNNUVC:
					format->DeclareAttribute ( XYZWNNNUVC_POSITION, ZGL_TYPE_FLOAT, 4, MOAIVertexFormat::ATTRIBUTE_COORD, false );
					format->DeclareAttribute ( XYZWNNNUVC_NORMAL, ZGL_TYPE_FLOAT, 3, MOAIVertexFormat::ATTRIBUTE_NORMAL, false );
					format->DeclareAttribute ( XYZWNNNUVC_TEXCOORD, ZGL_TYPE_FLOAT, 2, MOAIVertexFormat::ATTRIBUTE_TEX_COORD, false );
					format->DeclareAttribute ( XYZWNNNUVC_COLOR, ZGL_TYPE_UNSIGNED_BYTE, 4, MOAIVertexFormat::ATTRIBUTE_COLOR, true );
					break;
			}
			
			this->mFormats [ formatID ] = format;
		}
	}
	return format;
}

//----------------------------------------------------------------//
u32 MOAIVertexFormatMgr::GetVertexSize ( u32 formatID ) {

	const MOAIVertexFormat* format = this->GetFormat ( formatID );
	return format ? format->GetVertexSize () : 0;
}

//----------------------------------------------------------------//
MOAIVertexFormatMgr::MOAIVertexFormatMgr () {
	
	RTTI_SINGLE ( MOAIRubyObject )
	
	for ( u32 i = 0; i < TOTAL_FORMATS; ++i ) {
		this->mFormats [ i ] = 0;
	}
}

//----------------------------------------------------------------//
MOAIVertexFormatMgr::~MOAIVertexFormatMgr () {

	for ( u32 i = 0; i < TOTAL_FORMATS; ++i ) {
		if ( this->mFormats [ i ]) {
			this->RubyRelease ( this->mFormats [ i ]);
		}
	}
}

//----------------------------------------------------------------//
void MOAIVertexFormatMgr::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "XYZC",			( u32 )XYZC );
	state.DefineClassConst ( klass, "XYZWC",		( u32 )XYZWC );
	state.DefineClassConst ( klass, "XYZWUVC",		( u32 )XYZWUVC );
	state.DefineClassConst ( klass, "XYZWNNNC",		( u32 )XYZWNNNC );
	state.DefineClassConst ( klass, "XYZWNNNUVC",	( u32 )XYZWNNNUVC );

	state.DefineStaticMethod ( klass, "getFormat", _getFormat, MRB_ARGS_REQ ( 1 ) );
	
}

//----------------------------------------------------------------//
void MOAIVertexFormatMgr::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}
