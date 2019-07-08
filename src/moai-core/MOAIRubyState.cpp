// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <moai-core/MOAIRubyObject.h>
#include <moai-core/MOAIRubyRef.h>
#include <moai-core/MOAIRubyRuntime.h>
#include <moai-core/MOAIRubyState.h>
#include <moai-core/MOAIRubyState-impl.h>
#include <moai-core/MOAILogMgr.h>

#define LEVELS1	12	// size of the first part of the stack
#define LEVELS2	10	// size of the second part of the stack

int _rubyStreamWriter ( mrb_state* M, const void* p, size_t sz, void* ud );

//----------------------------------------------------------------//
int _rubyStreamWriter ( mrb_state* M, const void* p, size_t sz, void* ud ) {
	UNUSED ( M );

	ZLStream* stream = ( ZLStream* )ud;
	return ( stream->WriteBytes ( p, sz ) == sz ) ? 0 : -1;
}

//================================================================//
// MOAIRubyState
//================================================================//

//----------------------------------------------------------------//
int MOAIRubyState::AbsIndex ( int idx ) {

	if ( idx > 0 ) {
		return idx - 1;
	}
	return 0;
}

//----------------------------------------------------------------//
bool MOAIRubyState::Base64Decode ( int idx ) {

	ZLBase64Reader base64;
	return this->Decode ( idx, base64 );
}

//----------------------------------------------------------------//
bool MOAIRubyState::Base64Encode ( int idx ) {

	ZLBase64Writer base64;
	return this->Encode ( idx, base64 );
}

//----------------------------------------------------------------//
bool MOAIRubyState::CheckParams ( int idx, cc8* format, bool verbose ) {

	idx = this->AbsIndex ( idx );

	mrb_int argc = mrb_get_argc ( this->mState );
	mrb_value* array_argv = mrb_get_argv ( this->mState );
#define ARGV \
  ( array_argv ? array_argv : ( this->mState->c->stack + 1 ) )

	for ( int i = 0; format [ i ]; ++i ) {
	
		int pos = idx + i;
		mrb_vtype type, expected = MRB_TT_UNDEF;
		
		if ( pos <= argc ) {
			type = mrb_type ( ARGV [ pos ] );
		}
		
		switch ( format [ i ] ) {

			// {Array}
			case 'a':
			case 'A':
				if ( type != MRB_TT_ARRAY ) expected = MRB_TT_ARRAY;
				break;
		
			// boolean
			case 'b':
			case 'B':
				if ( type != MRB_TT_FALSE && type != MRB_TT_TRUE ) expected = MRB_TT_TRUE;
				break;
		
			// {Class}/{Module}
			case 'C':
				if ( type != MRB_TT_FIBER ) expected = MRB_TT_FIBER;
				break;

			// wrapped C pointers
			case 'D':
				if ( type != MRB_TT_DATA ) expected = MRB_TT_DATA;
				break;
		
			// function / proc
			case 'f':
			case 'F':
				if ( type != MRB_TT_PROC ) expected = MRB_TT_PROC;
				break;

			// {Hash}
			case 'H':
				if ( type != MRB_TT_HASH ) expected = MRB_TT_HASH;
				break;

			// {Integer}
			case 'i':
			case 'N':
				if ( type != MRB_TT_FIXNUM && type != MRB_TT_FLOAT ) expected = MRB_TT_FIXNUM;
				break;

			// {Symbol}
			case 'n':
				if ( type != MRB_TT_SYMBOL ) expected = MRB_TT_SYMBOL;
				break;

			// object
			case 'o':
			case 'O':
				if ( type != MRB_TT_OBJECT ) expected = MRB_TT_OBJECT;
				break;

			// proc / lambda
			case 'P':
			case '&':
				if ( type != MRB_TT_PROC ) expected = MRB_TT_PROC;
				break;

			// {String}
			case 's':
			case 'S':
				if ( type != MRB_TT_STRING ) expected = MRB_TT_STRING;
				break;
			
			// userdata
			case 'U':
				if ( type != MRB_TT_DATA ) expected = MRB_TT_DATA;
				break;

			// any type
			case '*':
			case '?':
			case '.':
				break;
			
			// any non-nil type
			case '@':
				if ( type == MRB_TT_UNDEF ) return false; // TODO: log a message
				break;
			
			// nil
			case '-':
				if ( type != MRB_TT_UNDEF ) expected = MRB_TT_UNDEF;
				break;
		}
		
		if ( expected != MRB_TT_UNDEF ) {
		
			if ( verbose ) {

				cc8* expectedName = MOAIRubyState::GetRubyTypeName ( expected );
				cc8* gotName = MOAIRubyState::GetRubyTypeName ( type );

				MOAILogF ( *this, ZLLog::LOG_ERROR, MOAISTRING_ParamTypeMismatch_DSS, pos, expectedName, gotName );

				mrb_print_backtrace ( this->mState );
			}
			return false;
		}
	}
	
	return true;
}

//----------------------------------------------------------------//
bool MOAIRubyState::CheckVector ( int idx, u32 n, mrb_float match, mrb_float fallback ) {

	return this->CheckVector ( idx, n, match, match, fallback );
}

//----------------------------------------------------------------//
bool MOAIRubyState::CheckVector ( int idx, u32 n, mrb_float min, mrb_float max, mrb_float fallback ) {

	for ( u32 i = 0; i < n; ++i ) {
		mrb_float element = this->ParamIsType ( idx + i, MRB_TT_FLOAT ) ? this->ToCValue < mrb_float > ( this->GetParamValue ( idx + i ) ) : fallback;
		if ( ( element < min ) || ( element > max ) ) return false;
	}
	return true;
}

//----------------------------------------------------------------//
void MOAIRubyState::ClearField ( mrb_value obj, cc8* key ) {

	mrb_iv_remove ( this->mState, obj, mrb_intern_cstr (  this->mState, key ) );
}

//----------------------------------------------------------------//
void MOAIRubyState::DefineClassConst ( struct RClass* klass, cc8* name, mrb_value value ) {

	mrb_define_const ( this->mState, klass, name, value );
}

//----------------------------------------------------------------//
void MOAIRubyState::DefineInstanceMethod ( struct RClass* klass, cc8* name, mrb_func_t func, mrb_aspec aspec ) {

	mrb_define_method ( this->mState, klass, name, func, aspec );
}

//----------------------------------------------------------------//
void MOAIRubyState::DefineSingletonMethod ( struct RObject* obj, cc8* name, mrb_func_t func, mrb_aspec aspec ) {

	mrb_define_singleton_method ( this->mState, obj, name, func, aspec );
}

//----------------------------------------------------------------//
void MOAIRubyState::DefineStaticMethod ( struct RClass* klass, cc8* name, mrb_func_t func, mrb_aspec aspec ) {

	mrb_define_class_method ( this->mState, klass, name, func, aspec );
}

//----------------------------------------------------------------//
int MOAIRubyState::DebugCall ( int nArgs, int nResults ) {
	UNUSED ( nArgs );
	UNUSED ( nResults );
	
	/*int errIdx = this->AbsIndex ( -( nArgs + 1 ));

	MOAILuaRuntime::Get ().PushTraceback ( *this );
	lua_insert ( this->mState, errIdx );

	int status = lua_pcall ( this->mState, nArgs, nResults, errIdx );

	if ( status ) {
		this->LogErrors ( ZLLog::LOG_ERROR, ZLLog::CONSOLE, status );
	}
	else {
		lua_remove ( this->mState, errIdx );
	}
	return status;*/
	return 0;
}

//----------------------------------------------------------------//
bool MOAIRubyState::Decode ( int idx, ZLStreamAdapter& reader ) {

	/*if ( !this->IsType ( idx, LUA_TSTRING )) return false;

	size_t len;
	void* buffer = ( void* )lua_tolstring ( this->mState, idx, &len );

	if ( !len ) {
		lua_pushstring ( this->mState, "" );
		return true;
	}

	ZLByteStream cryptStream;
	cryptStream.SetBuffer ( buffer, len );
	cryptStream.SetLength ( len );

	ZLMemStream plainStream;

	reader.Open ( &cryptStream );
	plainStream.WriteStream ( reader );
	reader.Close ();

	len = plainStream.GetLength ();
	buffer = malloc ( len );

	plainStream.Seek ( 0, SEEK_SET );
	plainStream.ReadBytes ( buffer, len );

	lua_pushlstring ( this->mState, ( cc8* )buffer, len );

	free ( buffer );*/
	
	return true;
}

//----------------------------------------------------------------//
bool MOAIRubyState::Deflate ( int idx, int level, int windowBits ) {

	ZLDeflateWriter deflater;
	deflater.SetCompressionLevel ( level );
	deflater.SetWindowBits ( windowBits );

	return this->Encode ( idx, deflater );
}

//----------------------------------------------------------------//
//bool MOAIRubyState::DumpChunk ( int idx, ZLStream& stream ) {
//
//	lua_pushvalue ( this->mState, idx );
//	int result = lua_dump ( this->mState, _luaStreamWriter, ( void* )&stream );
//	lua_pop ( this->mState, 1 );
//	
//	return ( result == 0 );
//}

//----------------------------------------------------------------//
bool MOAIRubyState::Encode ( int idx, ZLStreamAdapter& writer ) {

	/*if ( !this->IsType ( idx, LUA_TSTRING )) return false;

	size_t len;
	cc8* buffer = lua_tolstring ( this->mState, idx, &len );

	if ( !len ) {
		lua_pushstring ( this->mState, "" );
		return true;
	}

	ZLMemStream stream;

	writer.Open ( &stream );
	writer.WriteBytes ( buffer, len );
	writer.Close ();

	len = stream.GetLength ();
	void* temp = malloc ( len );

	stream.Seek ( 0, SEEK_SET );
	stream.ReadBytes (( void* )temp, len );

	lua_pushlstring ( this->mState, ( cc8* )temp, len );

	free ( temp );*/
	
	return true;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::Get ( const ZLBox& value ) {

	mrb_value ret [ 6 ];
	ret [ 0 ] = this->ToRValue ( value.mMin.mX );
	ret [ 1 ] = this->ToRValue ( value.mMin.mY );
	ret [ 2 ] = this->ToRValue ( value.mMin.mZ );

	ret [ 3 ] = this->ToRValue ( value.mMax.mX );
	ret [ 4 ] = this->ToRValue ( value.mMax.mY );
	ret [ 5 ] = this->ToRValue ( value.mMax.mZ );

	return mrb_ary_new_from_values ( this->mState, 6, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::Get ( const ZLColorVec& value ) {

	mrb_value ret [ 4 ];
	ret [ 0 ] = this->ToRValue ( value.mR );
	ret [ 1 ] = this->ToRValue ( value.mG );
	ret [ 2 ] = this->ToRValue ( value.mB );
	ret [ 3 ] = this->ToRValue ( value.mA );

	return mrb_ary_new_from_values ( this->mState, 4, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::Get ( const ZLAffine2D& value ) {

	mrb_value ret [ 6 ];
	ret [ 0 ] = this->ToRValue ( value.m [ ZLAffine2D::C0_R0 ] );
	ret [ 1 ] = this->ToRValue ( value.m [ ZLAffine2D::C1_R0 ] );
	ret [ 2 ] = this->ToRValue ( value.m [ ZLAffine2D::C2_R0 ] );

	ret [ 3 ] = this->ToRValue ( value.m [ ZLAffine2D::C0_R1 ] );
	ret [ 4 ] = this->ToRValue ( value.m [ ZLAffine2D::C1_R1 ] );
	ret [ 5 ] = this->ToRValue ( value.m [ ZLAffine2D::C2_R1 ] );

	return mrb_ary_new_from_values ( this->mState, 6, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::Get ( const ZLAffine3D& value ) {

	mrb_value ret [ 12 ];
	ret [ 0 ] = this->ToRValue ( value.m [ ZLAffine3D::C0_R0 ] );
	ret [ 1 ] = this->ToRValue ( value.m [ ZLAffine3D::C1_R0 ] );
	ret [ 2 ] = this->ToRValue ( value.m [ ZLAffine3D::C2_R0 ] );
	ret [ 3 ] = this->ToRValue ( value.m [ ZLAffine3D::C3_R0 ] );

	ret [ 4 ] = this->ToRValue ( value.m [ ZLAffine3D::C0_R1 ] );
	ret [ 5 ] = this->ToRValue ( value.m [ ZLAffine3D::C1_R1 ] );
	ret [ 6 ] = this->ToRValue ( value.m [ ZLAffine3D::C2_R1 ] );
	ret [ 7 ] = this->ToRValue ( value.m [ ZLAffine3D::C3_R1 ] );

	ret [ 8 ] = this->ToRValue ( value.m [ ZLAffine3D::C0_R2 ] );
	ret [ 9 ] = this->ToRValue ( value.m [ ZLAffine3D::C1_R2 ] );
	ret [ 10 ] = this->ToRValue ( value.m [ ZLAffine3D::C2_R2 ] );
	ret [ 11 ] = this->ToRValue ( value.m [ ZLAffine3D::C3_R2 ] );

	return mrb_ary_new_from_values ( this->mState, 12, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::Get ( const ZLMatrix3x3& value ) {

	mrb_value ret [ 9 ];
	ret [ 0 ] = this->ToRValue ( value.m [ ZLMatrix3x3::C0_R0 ] );
	ret [ 1 ] = this->ToRValue ( value.m [ ZLMatrix3x3::C1_R0 ] );
	ret [ 2 ] = this->ToRValue ( value.m [ ZLMatrix3x3::C2_R0 ] );

	ret [ 3 ] = this->ToRValue ( value.m [ ZLMatrix3x3::C0_R1 ] );
	ret [ 4 ] = this->ToRValue ( value.m [ ZLMatrix3x3::C1_R1 ] );
	ret [ 5 ] = this->ToRValue ( value.m [ ZLMatrix3x3::C2_R1 ] );

	ret [ 6 ] = this->ToRValue ( value.m [ ZLMatrix3x3::C0_R2 ] );
	ret [ 7 ] = this->ToRValue ( value.m [ ZLMatrix3x3::C1_R2 ] );
	ret [ 8 ] = this->ToRValue ( value.m [ ZLMatrix3x3::C2_R2 ] );

	return mrb_ary_new_from_values ( this->mState, 9, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::Get ( const ZLMatrix4x4& value ) {

	mrb_value ret [ 16 ];
	ret [ 0 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C0_R0 ] );
	ret [ 1 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C1_R0 ] );
	ret [ 2 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C2_R0 ] );
	ret [ 3 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C3_R0 ] );

	ret [ 4 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C0_R1 ] );
	ret [ 5 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C1_R1 ] );
	ret [ 6 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C2_R1 ] );
	ret [ 7 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C3_R1 ] );

	ret [ 8 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C0_R2 ] );
	ret [ 9 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C1_R2 ] );
	ret [ 10 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C2_R2 ] );
	ret [ 11 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C3_R2 ] );

	ret [ 12 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C0_R3 ] );
	ret [ 13 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C1_R3 ] );
	ret [ 14 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C2_R3 ] );
	ret [ 15 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C3_R3 ] );

	return mrb_ary_new_from_values ( this->mState, 16, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::Get ( const ZLQuad& value ) {

	mrb_value ret [ 8 ];
	ret [ 0 ] = this->ToRValue ( value.mV [ 0 ].mX );
	ret [ 1 ] = this->ToRValue ( value.mV [ 0 ].mY );
	ret [ 2 ] = this->ToRValue ( value.mV [ 1 ].mX );
	ret [ 3 ] = this->ToRValue ( value.mV [ 1 ].mY );
	ret [ 4 ] = this->ToRValue ( value.mV [ 2 ].mX );
	ret [ 5 ] = this->ToRValue ( value.mV [ 2 ].mY );
	ret [ 6 ] = this->ToRValue ( value.mV [ 3 ].mX );
	ret [ 7 ] = this->ToRValue ( value.mV [ 3 ].mY );

	return mrb_ary_new_from_values ( this->mState, 8, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::Get ( const ZLRect& value ) {

	mrb_value ret [ 4 ];
	ret [ 0 ] = this->ToRValue ( value.mXMin );
	ret [ 1 ] = this->ToRValue ( value.mYMin );
	ret [ 2 ] = this->ToRValue ( value.mXMax );
	ret [ 3 ] = this->ToRValue ( value.mYMax );

	return mrb_ary_new_from_values ( this->mState, 4, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::Get ( const ZLVec2D& value ) {

	mrb_value ret [ 2 ];
	ret [ 0 ] = this->ToRValue ( value.mX );
	ret [ 1 ] = this->ToRValue ( value.mY );

	return mrb_ary_new_from_values ( this->mState, 2, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::Get ( const ZLVec3D& value ) {

	mrb_value ret [ 3 ];
	ret [ 0 ] = this->ToRValue ( value.mX );
	ret [ 1 ] = this->ToRValue ( value.mY );
	ret [ 2 ] = this->ToRValue ( value.mZ );

	return mrb_ary_new_from_values ( this->mState, 3, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::Get ( const ZLVec4D& value ) {

	mrb_value ret [ 4 ];
	ret [ 0 ] = this->ToRValue ( value.mX );
	ret [ 1 ] = this->ToRValue ( value.mY );
	ret [ 2 ] = this->ToRValue ( value.mZ );
	ret [ 3 ] = this->ToRValue ( value.mW );

	return mrb_ary_new_from_values ( this->mState, 4, ret );
}

//----------------------------------------------------------------//
ZLBox MOAIRubyState::GetBox ( int idx ) {

	ZLBox box;

	box.mMin.mX = this->GetParamValue < float >( idx++, 0.0f );
	box.mMin.mY = this->GetParamValue < float >( idx++, 0.0f );
	box.mMin.mZ = this->GetParamValue < float >( idx++, 0.0f );

	box.mMax.mX = this->GetParamValue < float >( idx++, 0.0f );
	box.mMax.mY = this->GetParamValue < float >( idx++, 0.0f );
	box.mMax.mZ = this->GetParamValue < float >( idx++, 0.0f );

	return box;
}

//----------------------------------------------------------------//
ZLColorVec MOAIRubyState::GetColor ( int idx, float r, float g, float b, float a ) {

	ZLColorVec color;
	color.mR = this->GetParamValue < float >( idx++, r );
	color.mG = this->GetParamValue < float >( idx++, g );
	color.mB = this->GetParamValue < float >( idx++, b );
	color.mA = this->GetParamValue < float >( idx++, a );

	return color;
}

//----------------------------------------------------------------//
u32 MOAIRubyState::GetColor32 ( int idx, float r, float g, float b, float a ) {

	ZLColorVec color = this->GetColor ( idx, r, g, b, a );
	return color.PackRGBA ();
}

//----------------------------------------------------------------//
bool MOAIRubyState::FiberAlive ( mrb_value fib ) {

	return ToCValue < bool >( mrb_fiber_alive_p ( this->mState, fib ) );
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::FiberResume ( mrb_value fib, mrb_int argc, const mrb_value* args ) {

	return mrb_fiber_resume ( this->mState, fib, argc, args );
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::FiberYield ( mrb_int argc, const mrb_value* args ) {

	return mrb_fiber_yield ( this->mState, argc, args );
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::FuncCall ( mrb_value obj, cc8* funcName, mrb_int argc, const mrb_value* args ) {

	//mrbc_context* context = mrbc_context_new ( this->mState );

	mrb_sym const sym = mrb_intern_cstr ( this->mState, funcName );

	mrb_value ret = mrb_funcall_argv ( this->mState, obj, sym, argc, args );

	if ( this->mState->exc ) {

		//mrb_value exc = mrb_obj_value ( M->exc );

		//// エラー内容を取得
		//mrb_value backtrace = mrb_get_backtrace ( M );
		//puts ( mrb_str_to_cstr ( M, mrb_inspect ( M, backtrace ) ) );

		//// バックトレースを取得
		//mrb_value inspect = mrb_inspect ( M, exc );
		//puts ( mrb_str_to_cstr ( M, inspect ) );

		if ( mrb_undef_p ( ret ) ) {
			//ZLLog_DebugF ( ZLLog::CONSOLE, "mrb_undef_p(v) == true! \n" );
			mrb_p ( this->mState, mrb_obj_value ( this->mState->exc ) );
		}
		else {
			mrb_print_error ( this->mState );
		}
		printf ( "\n" );
	}

	//mrbc_context_free ( this->mState, context );

	return ret;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::FuncCallWithBlock ( mrb_value obj, cc8* funcName, mrb_int argc, const mrb_value* args, mrb_value block ) {

	mrb_sym const sym = mrb_intern_cstr ( this->mState, funcName );

	return mrb_funcall_with_block ( this->mState, obj, sym, argc, args, block );
}

////----------------------------------------------------------------//
//STLString MOAIRubyState::GetField ( int idx, cc8* key, cc8* value ) {
//
//	STLString str;
//	if ( this->PushFieldWithType ( idx, key, LUA_TSTRING )) {
//		str = lua_tostring ( this->mState, -1 );
//		lua_pop ( this->mState, 1 );
//	}
//	else {
//		str = value;
//	}
//	return str;
//}
//
////----------------------------------------------------------------//
//STLString MOAIRubyState::GetField ( int idx, int key, cc8* value ) {
//
//	STLString str;
//	if ( this->PushFieldWithType ( idx, key, LUA_TSTRING )) {
//		str = lua_tostring ( this->mState, -1 );
//		lua_pop ( this->mState, 1 );
//	}
//	else {
//		str = value;
//	}
//	return str;
//}

////----------------------------------------------------------------//
//STLString MOAIRubyState::GetField ( int idx, cc8* key, const STLString& value ) {
//
//	STLString str;
//	if ( this->PushFieldWithType ( idx, key, LUA_TSTRING )) {
//		str = lua_tostring ( this->mState, -1 );
//		lua_pop ( this->mState, 1 );
//	}
//	else {
//		str = value;
//	}
//	return str;
//}
//
////----------------------------------------------------------------//
//STLString MOAIRubyState::GetField ( int idx, int key, const STLString& value ) {
//
//	STLString str;
//	if ( this->PushFieldWithType ( idx, key, LUA_TSTRING )) {
//		str = lua_tostring ( this->mState, -1 );
//		lua_pop ( this->mState, 1 );
//	}
//	else {
//		str = value;
//	}
//	return str;
//}

//----------------------------------------------------------------//
mrb_int MOAIRubyState::GetArraySize ( mrb_value ary ) {
	return RARRAY_LEN ( ary );
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::GetField ( mrb_value obj, cc8* name ) {

	return mrb_iv_get ( this->mState, obj, mrb_intern_cstr ( this->mState, name ) );
}

//----------------------------------------------------------------//
mrb_int MOAIRubyState::GetHashSize ( mrb_value hash ) {
	return mrb_hash_size ( this->mState, hash );
}

//----------------------------------------------------------------//
cc8* MOAIRubyState::GetRubyTypeName ( mrb_vtype type ) {

	switch ( type ) {
	
		case MRB_TT_FALSE:			return "false";
		case MRB_TT_TRUE:			return "true";
		case MRB_TT_FIXNUM:			return "number";
		case MRB_TT_SYMBOL:			return "symbol";
		case MRB_TT_UNDEF:			return "undef";
		case MRB_TT_FLOAT:			return "float";
		case MRB_TT_CPTR:			return "cptr";
		case MRB_TT_OBJECT:			return "object";
		case MRB_TT_CLASS:			return "class";
		case MRB_TT_MODULE:			return "module";
		case MRB_TT_STRING:			return "string";
		case MRB_TT_RANGE:			return "range";
		case MRB_TT_EXCEPTION:		return "exception";
		case MRB_TT_ARRAY:			return "array";
		case MRB_TT_HASH:			return "hash";
		case MRB_TT_PROC:			return "function";
		case MRB_TT_DATA:			return "userdata";
		case MRB_TT_FIBER:			return "thread";
	}
	return "unknown";
}

//----------------------------------------------------------------//
mrb_int MOAIRubyState::GetParamsCount () {
	return mrb_get_argc ( this->mState );
}

//----------------------------------------------------------------//
ZLMatrix4x4 MOAIRubyState::GetParamMatrix ( int idx, size_t size ) {

	switch ( size ) {

	case 6:
		return  ZLMatrix4x4 ( this->GetParamValue < ZLAffine2D >( idx, ZLAffine2D::IDENT ) );

	case 9:
		return  ZLMatrix4x4 ( this->GetParamValue < ZLMatrix3x3 >( idx, ZLMatrix3x3::IDENT ) );

	case 12:
		return  ZLMatrix4x4 ( this->GetParamValue < ZLAffine3D >( idx, ZLAffine3D::IDENT ) );

	case 16:
		return this->GetParamValue < ZLMatrix4x4 >( idx, ZLMatrix4x4::IDENT );
	}

	return ZLMatrix4x4::IDENT;
}

//----------------------------------------------------------------//
mrb_vtype MOAIRubyState::GetParamType ( int idx ) {
	return mrb_type ( this->GetParamValue ( this->RealParamIndex ( idx ) ) );
}

//----------------------------------------------------------------//
ZLQuad MOAIRubyState::GetParamQuad ( int idx ) {

	ZLQuad quad;

	quad.mV [ 0 ].mX = this->GetParamValue < float >( idx + 0, 0.0f );
	quad.mV [ 0 ].mY = this->GetParamValue < float >( idx + 1, 0.0f );
	quad.mV [ 1 ].mX = this->GetParamValue < float >( idx + 2, 0.0f );
	quad.mV [ 1 ].mY = this->GetParamValue < float >( idx + 3, 0.0f );
	quad.mV [ 2 ].mX = this->GetParamValue < float >( idx + 4, 0.0f );
	quad.mV [ 2 ].mY = this->GetParamValue < float >( idx + 5, 0.0f );
	quad.mV [ 3 ].mX = this->GetParamValue < float >( idx + 6, 0.0f );
	quad.mV [ 3 ].mY = this->GetParamValue < float >( idx + 7, 0.0f );

	return quad;
}

//----------------------------------------------------------------//
void* MOAIRubyState::GetPtrUserData ( int idx ) {
	UNUSED ( idx );

	void* ptr = 0;

	/*if ( this->IsType ( idx, LUA_TUSERDATA )) {
		ptr = *( void** )lua_touserdata ( this->mState, idx );
	}*/
	return ptr;
}

//----------------------------------------------------------------//
STLString MOAIRubyState::GetStackDump () {

	STLString out;
	//int top = GetTop ();
	//out.write ( "Lua stack: %d element(s)", top );

	//for ( int index = top; index >= 1; index-- ) {
	//
	//	int type = lua_type ( this->mState, index );

	//	// Print index and type
	//	int relativeIndex = index - top - 1;
	//	out.write ( "\n[ %d | %d ] = %s", index, relativeIndex, GetLuaTypeName ( type ) );

	//	// Print value, if possible
	//	switch ( type ) {
	//		case LUA_TBOOLEAN:
	//			out.write ( ": %s", lua_toboolean ( this->mState, index ) ? "true" : "false" );
	//			break;
	//		case LUA_TNUMBER:
	//			out.write ( ": %g", lua_tonumber ( this->mState, index ) );
	//			break;
	//		case LUA_TSTRING:
	//			out.write ( ": \"%s\"", lua_tostring ( this->mState, index ) );
	//			break;
	//		case LUA_TUSERDATA: {
	//			// Moai uses userdata exclusively for pointers to MOAILuaObject instances.
	//			// This code will most likely crash if it encounters userdata that is used differently.
	//			MOAILuaObject* luaObject = ( MOAILuaObject* )this->GetPtrUserData ( index );
	//			if ( luaObject ) {
	//				out.write ( ": %s at %p", luaObject->TypeName (), luaObject );
	//			}
	//			break;
	//		}
	//		case LUA_TLIGHTUSERDATA:
	//		case LUA_TTABLE:
	//		case LUA_TFUNCTION:
	//		case LUA_TTHREAD:
	//			// anything with an address
	//			out.write ( " at %p", lua_topointer ( this->mState, index ) );
	//			break;
	//	}
	//}

	//out.write("\n");
	return out;
}

//----------------------------------------------------------------//
STLString MOAIRubyState::GetStackTrace ( cc8* title, int level ) {

	int firstpart = 1;  /* still before eventual `...' */
	//lua_Debug ar;
	
	mrb_state* M = this->mState;

	STLString out;
	
	//out.append ( title ? title : "stack traceback:" );
	//
	//while ( lua_getstack ( L, level++, &ar )) {
	//	
	//	if ( level > LEVELS1 && firstpart ) {
	//		
	//		if ( !lua_getstack ( L, level + LEVELS2, &ar )) {
	//			level--;
	//		}
	//		else {
	//			// too many levels
	//			out.append ( "\n\t..." );  /* too many levels */
	//			
	//			// find last levels */
	//			while ( lua_getstack ( L, level + LEVELS2, &ar ))  
	//				level++;
	//		}
	//		firstpart = 0;
	//		continue;
	//	}
	//	
	//	out.append ( "\n\t" );
	//	
	//	lua_getinfo ( L, "Snl", &ar );
	//	
	//	out.append ( ar.short_src );
	//	
	//	if ( ar.currentline > 0 ) {
	//		out.write ( ":%d", ar.currentline );
	//	}
	//	
	//	if ( *ar.namewhat != '\0' ) {
	//		out.write ( " in function '%s'", ar.name );
	//	}
	//	else {
	//		if ( *ar.what == 'm' ) {
	//			out.write ( " in main chunk" );
	//		}
	//		else if ( *ar.what == 'C' || *ar.what == 't' ) {
	//			out.write ( " ?" );
	//		}
	//		else {
	//			out.write ( " in function <%s:%d>", ar.short_src, ar.linedefined );
	//		}
	//	}
	//}
	//
	//out.append ( "\n" );
	return out;
}

//----------------------------------------------------------------//
void* MOAIRubyState::GetUserData ( int idx, void* value ) {
	UNUSED ( idx );
	UNUSED ( value );

	/*if ( lua_type ( this->mState, idx ) == LUA_TLIGHTUSERDATA ) {
		return lua_touserdata ( this->mState, idx );
	}*/
	return value;
}

//----------------------------------------------------------------//
void* MOAIRubyState::GetUserData ( int idx, cc8* name, void* value ) {
	UNUSED ( idx );
	UNUSED ( name );

	/*if ( this->PushFieldWithType ( idx, name, LUA_TLIGHTUSERDATA )) {
		value = lua_touserdata ( this->mState, -1 );
		lua_pop ( this->mState, 1 );
	}*/
	return value;
}

//----------------------------------------------------------------//
template <>
bool MOAIRubyState::GetParamValue < bool >( int idx, const bool value ) {

	if ( this->ParamIsType ( idx, MRB_TT_FALSE ) || this->ParamIsType ( idx, MRB_TT_TRUE ) ) {
		return this->ToCValue < bool >( this->GetParamValue ( idx ) );
	}
	return value;
}

//----------------------------------------------------------------//
template <>
cc8* MOAIRubyState::GetParamValue < cc8* >( int idx, const cc8* value ) {

	if ( this->ParamIsType ( idx, MRB_TT_STRING ) ) {
		return this->ToCValue < cc8* >( this->GetParamValue ( idx ) );
	}
	return value;
}

//----------------------------------------------------------------//
STLString MOAIRubyState::GetValue ( int idx, const cc8* value ) {

	STLString str;
	if ( this->ParamIsType ( idx, MRB_TT_STRING ) ) {
		str = this->ToCValue < cc8* >( this->GetParamValue ( idx ) );
	}
	else {
		str = value;
	}
	return str;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::GetParamValue ( int idx ) {

	int real_idx = this->RealParamIndex ( idx );

	mrb_value* argv;
	mrb_int argc;
	mrb_get_args ( this->mState, "*", &argv, &argc );

	if ( real_idx < argc ) {

		// mrb_value* array_argv = mrb_get_argv ( this->mState );

		//MOAILogF ( this->mState, ZLLog::LOG_DEBUG, "MOAIRubyState::GetParamValue(%d) %d (argc = %d)", idx, real_idx, argc );

// #define ARGV \
//   ( array_argv ? array_argv : ( this->mState->c->stack + 1 ) )

// 		return ARGV [ real_idx ];
		return argv [ real_idx ];
	}
	else {
		//MOAILogF ( this->mState, ZLLog::LOG_ERROR, "MOAIRubyState::GetParamValue(%d) index %d out of bounds. argc = %d", idx, real_idx, mrb_get_argc ( this->mState ) );
		return mrb_nil_value ();
	}
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::GetRObjectField ( RObject* obj, cc8* key ) {

	mrb_sym const sym = mrb_intern_cstr ( this->mState, key );

	if ( mrb_obj_iv_defined ( this->mState, obj, sym ) ) {
		return mrb_obj_iv_get ( this->mState, obj, sym );
	}

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_vtype MOAIRubyState::GetType ( mrb_value value ) {

	return mrb_type ( value );
}

//----------------------------------------------------------------//
template <>
double MOAIRubyState::GetParamValue < double >( int idx, const double value ) {

	if ( this->ParamIsType ( idx, MRB_TT_FLOAT ) ) {
		return this->ToCValue < double >( this->GetParamValue ( idx ) );
	}
	else if ( this->ParamIsType ( idx, MRB_TT_FIXNUM ) ) {
		return ( double )this->ToCValue < s64 >( this->GetParamValue ( idx ) );
	}
	return value;
}

//----------------------------------------------------------------//
template <>
float MOAIRubyState::GetParamValue < float >( int idx, const float value ) {

	if ( this->ParamIsType ( idx, MRB_TT_FLOAT ) ) {
		return this->ToCValue < float >( this->GetParamValue ( idx ) );
	}
	else if ( this->ParamIsType ( idx, MRB_TT_FIXNUM ) ) {
		return ( float )this->ToCValue < s32 >( this->GetParamValue ( idx ) );
	}
	return value;
}

//----------------------------------------------------------------//
//template <>
//int MOAIRubyState::GetValue < int >( int idx, const int value ) {
//
//	if ( this->IsType ( idx, LUA_TNUMBER )) {
//		return ( int )lua_tonumber ( this->mState, idx );
//	}
//	return value;
//}

//----------------------------------------------------------------//
template <>
s8 MOAIRubyState::GetParamValue < s8 >( int idx, const s8 value ) {

	if ( this->ParamIsType ( idx, MRB_TT_FIXNUM ) ) {
		return this->ToCValue < s8 >( this->GetParamValue ( idx ) );
	}
	else if ( this->ParamIsType ( idx, MRB_TT_FLOAT ) ) {
		return ( s8 )this->ToCValue < float >( this->GetParamValue ( idx ) );
	}
	return value;
}

//----------------------------------------------------------------//
template <>
s16 MOAIRubyState::GetParamValue < s16 >( int idx, const s16 value ) {

	if ( this->ParamIsType ( idx, MRB_TT_FIXNUM ) ) {
		return this->ToCValue < s16 >( this->GetParamValue ( idx ) );
	}
	else if ( this->ParamIsType ( idx, MRB_TT_FLOAT ) ) {
		return ( s16 )this->ToCValue < float >( this->GetParamValue ( idx ) );
	}
	return value;
}

//----------------------------------------------------------------//
template <>
s32 MOAIRubyState::GetParamValue < s32 >( int idx, const s32 value ) {

	if ( this->ParamIsType ( idx, MRB_TT_FIXNUM ) ) {
		return this->ToCValue < s32 >( this->GetParamValue ( idx ) );
	}
	else if ( this->ParamIsType ( idx, MRB_TT_FLOAT ) ) {
		return ( s32 )this->ToCValue < float >( this->GetParamValue ( idx ) );
	}
	return value;
}

//----------------------------------------------------------------//
template <>
s64 MOAIRubyState::GetParamValue < s64 >( int idx, const s64 value ) {

	if ( this->ParamIsType ( idx, MRB_TT_FIXNUM ) || this->ParamIsType ( idx, MRB_TT_FLOAT ) ) {
		return this->ToCValue < s64 >( this->GetParamValue ( idx ) );
	}
	else if ( this->ParamIsType ( idx, MRB_TT_FLOAT ) ) {
		return ( s64 )this->ToCValue < double >( this->GetParamValue ( idx ) );
	}
	return value;
}

//----------------------------------------------------------------//
template <>
u8 MOAIRubyState::GetParamValue < u8 >( int idx, const u8 value ) {

	if ( this->ParamIsType ( idx, MRB_TT_FIXNUM ) ) {
		return this->ToCValue < u8 >( this->GetParamValue ( idx ) );
	}
	else if ( this->ParamIsType ( idx, MRB_TT_FLOAT ) ) {
		return ( u8 )this->ToCValue < float >( this->GetParamValue ( idx ) );
	}
	return value;
}

//----------------------------------------------------------------//
template <>
u16 MOAIRubyState::GetParamValue < u16 >( int idx, const u16 value ) {

	if ( this->ParamIsType ( idx, MRB_TT_FIXNUM ) ) {
		return this->ToCValue < u16 >( this->GetParamValue ( idx ) );
	}
	else if ( this->ParamIsType ( idx, MRB_TT_FLOAT ) ) {
		return ( u16 )this->ToCValue < float >( this->GetParamValue ( idx ) );
	}
	return value;
}

//----------------------------------------------------------------//
template <>
u32 MOAIRubyState::GetParamValue < u32 >( int idx, const u32 value ) {

	if ( this->ParamIsType ( idx, MRB_TT_FIXNUM ) ) {
		return this->ToCValue < u32 >( this->GetParamValue ( idx ) );
	}
	else if ( this->ParamIsType ( idx, MRB_TT_FLOAT ) ) {
		return ( u32 )this->ToCValue < float >( this->GetParamValue ( idx ) );
	}
	return value;
}

//----------------------------------------------------------------//
template <>
u64 MOAIRubyState::GetParamValue < u64 >( int idx, const u64 value ) {

	if ( this->ParamIsType ( idx, MRB_TT_FIXNUM ) ) {
		return this->ToCValue < u64 >( this->GetParamValue ( idx ) );
	}
	else if ( this->ParamIsType ( idx, MRB_TT_FLOAT ) ) {
		return ( u64 )this->ToCValue < double >( this->GetParamValue ( idx ) );
	}
	return value;
}

//----------------------------------------------------------------//
template <>
const void* MOAIRubyState::GetParamValue < const void* >( int idx, const void* value ) {

	if ( this->ParamIsType ( idx, MRB_TT_CPTR )) {

		//return ( void* )lua_touserdata ( this->mState, idx );
		return this->ToCValue < const void* >( this->GetParamValue ( idx ) );
	}
	return value;
}

//----------------------------------------------------------------//
template <>
ZLBox MOAIRubyState::GetParamValue < ZLBox >( int idx, const ZLBox value ) {

	if ( !this->ParamIsType ( idx, MRB_TT_ARRAY ) ) {
		if ( this->CheckParams ( idx, "NNNNNN" ) ) {

			ZLBox box;

			box.mMin.mX = (float )this->GetParamValue < float >( idx + 0, value.mMin.mX );
			box.mMin.mY = (float )this->GetParamValue < float >( idx + 1, value.mMin.mY );
			box.mMin.mZ = (float )this->GetParamValue < float >( idx + 2, value.mMin.mZ );

			box.mMax.mX = (float )this->GetParamValue < float >( idx + 3, value.mMax.mX );
			box.mMax.mY = (float )this->GetParamValue < float >( idx + 4, value.mMax.mY );
			box.mMax.mZ = (float )this->GetParamValue < float >( idx + 5, value.mMax.mZ );

			return box;
		}
	}
	else {

		mrb_value ary = this->GetParamValue ( idx );

		ZLBox box;

		box.mMin.mX = this->ToCValue < float >( mrb_ary_ref ( this->mState, ary, 0 ) );
		box.mMin.mY = this->ToCValue < float >( mrb_ary_ref ( this->mState, ary, 1 ) );
		box.mMin.mZ = this->ToCValue < float >( mrb_ary_ref ( this->mState, ary, 2 ) );

		box.mMax.mX = this->ToCValue < float >( mrb_ary_ref ( this->mState, ary, 3 ) );
		box.mMax.mY = this->ToCValue < float >( mrb_ary_ref ( this->mState, ary, 4 ) );
		box.mMax.mZ = this->ToCValue < float >( mrb_ary_ref ( this->mState, ary, 5 ) );

		return box;
	}
	return value;
}

//----------------------------------------------------------------//
template <>
ZLColorVec MOAIRubyState::GetParamValue < ZLColorVec >( int idx, const ZLColorVec value ) {
	
	ZLColorVec color;
	
	color.mR	= this->GetParamValue < float >( idx + 0, value.mR );
	color.mG	= this->GetParamValue < float >( idx + 1, value.mG );
	color.mB	= this->GetParamValue < float >( idx + 2, value.mB );
	color.mA	= this->GetParamValue < float >( idx + 3, value.mA );
	
	return color;
}

//----------------------------------------------------------------//
template <>
ZLAffine2D MOAIRubyState::GetParamValue < ZLAffine2D >( int idx, const ZLAffine2D value ) {

	ZLAffine2D mtx;
	
	for ( int row = 0; row < 2; ++row ) {
		for ( int col = 0; col < 3; ++col ) {
	
			int element = ZLAffine2D::GetIndex ( row, col );
			mtx.m [ element ] = this->GetParamValue < float >( idx + ( row * 3 ) + col, value.m [ element ]);
		}
	}	
	return mtx;
}

//----------------------------------------------------------------//
template <>
ZLAffine3D MOAIRubyState::GetParamValue < ZLAffine3D >( int idx, const ZLAffine3D value ) {

	ZLAffine3D mtx;
	
	for ( int row = 0; row < 3; ++row ) {
		for ( int col = 0; col < 4; ++col ) {
	
			int element = ZLAffine3D::GetIndex ( row, col );
			mtx.m [ element ] = this->GetParamValue < float >( idx + ( row * 4 ) + col, value.m [ element ]);
		}
	}	
	return mtx;
}

//----------------------------------------------------------------//
template <>
ZLMatrix3x3 MOAIRubyState::GetParamValue < ZLMatrix3x3 >( int idx, const ZLMatrix3x3 value ) {

	ZLMatrix3x3 mtx;
	
	for ( int row = 0; row < 3; ++row ) {
		for ( int col = 0; col < 3; ++col ) {
	
			int element = ZLMatrix3x3::GetIndex ( row, col );
			mtx.m [ element ] = this->GetParamValue < float >( idx + ( row * 3 ) + col, value.m [ element ]);
		}
	}	
	return mtx;
}

//----------------------------------------------------------------//
template <>
ZLMatrix4x4 MOAIRubyState::GetParamValue < ZLMatrix4x4 >( int idx, const ZLMatrix4x4 value ) {

	ZLMatrix4x4 mtx;
	
	for ( int row = 0; row < 4; ++row ) {
		for ( int col = 0; col < 4; ++col ) {
	
			int element = ZLMatrix4x4::GetIndex ( row, col );
			mtx.m [ element ] = this->GetParamValue < float >( idx + ( row * 4 ) + col, value.m [ element ]);
		}
	}	
	return mtx;
}

//----------------------------------------------------------------//
template <>
ZLQuaternion MOAIRubyState::GetParamValue < ZLQuaternion >( int idx, const ZLQuaternion value ) {

	ZLQuaternion quat;
		
	quat.mV.mX	= this->GetParamValue < float >( idx + 0, value.mV.mX );
	quat.mV.mY	= this->GetParamValue < float >( idx + 1, value.mV.mY );
	quat.mV.mZ	= this->GetParamValue < float >( idx + 2, value.mV.mZ );
	quat.mS		= this->GetParamValue < float >( idx + 3, value.mS );
	
	return quat;
}

//----------------------------------------------------------------//
template <>
ZLRect MOAIRubyState::GetParamValue < ZLRect >( int idx, const ZLRect value ) {

	ZLRect rect = value;

	if ( this->CheckParams ( idx, "NNNN", false )) {
		
		rect.mXMin		= this->GetParamValue < float >( idx + 0, value.mXMin );
		rect.mYMin		= this->GetParamValue < float >( idx + 1, value.mXMin );
		rect.mXMax		= this->GetParamValue < float >( idx + 2, value.mXMin );
		rect.mYMax		= this->GetParamValue < float >( idx + 3, value.mXMin );
		
		return rect;
	}
	else if ( this->CheckParams ( idx, "NN", false )) {
		
		rect.mXMin		= value.mXMin;
		rect.mYMin		= value.mYMin;
		rect.mXMax		= value.mXMin + this->GetParamValue < float > ( idx + 1, 0 );
		rect.mYMax		= value.mYMin + this->GetParamValue < float > ( idx + 2, 0 );
	}
	else if ( this->ParamIsType ( idx, MRB_TT_FLOAT ) || this->ParamIsType ( idx, MRB_TT_FIXNUM ) ) {
	
		float size = this->GetParamValue < float >( idx, 0 );
		
		rect.mXMin		= value.mXMin;
		rect.mYMin		= value.mYMin;
		rect.mXMax		= value.mXMin + size;
		rect.mYMax		= value.mYMin + size;
	}
	
	return rect;
}

//----------------------------------------------------------------//
template <>
ZLVec2D MOAIRubyState::GetParamValue < ZLVec2D >( int idx, const ZLVec2D value ) {

	ZLVec2D vec;
		
	vec.mX = this->GetParamValue < float >( idx + 0, value.mX );
	vec.mY = this->GetParamValue < float >( idx + 1, value.mY );
	
	return vec;
}

//----------------------------------------------------------------//
template <>
ZLVec3D MOAIRubyState::GetParamValue < ZLVec3D >( int idx, const ZLVec3D value ) {

	ZLVec3D vec;
		
	vec.mX = this->GetParamValue < float >( idx + 0, value.mX );
	vec.mY = this->GetParamValue < float >( idx + 1, value.mY );
	vec.mZ = this->GetParamValue < float >( idx + 2, value.mZ );
	
	return vec;
}

//----------------------------------------------------------------//
template <>
ZLVec4D MOAIRubyState::GetParamValue < ZLVec4D >( int idx, const ZLVec4D value ) {

	ZLVec4D vec;
		
	vec.mX = this->GetParamValue < float >( idx + 0, value.mX );
	vec.mY = this->GetParamValue < float >( idx + 1, value.mY );
	vec.mZ = this->GetParamValue < float >( idx + 2, value.mZ );
	vec.mW = this->GetParamValue < float >( idx + 3, value.mW );
	
	return vec;
}

//----------------------------------------------------------------//
bool MOAIRubyState::HasField ( mrb_value klass, cc8* name ) {

	return mrb_iv_defined ( this->mState, klass, mrb_intern_cstr ( this->mState, name ) );
}

//----------------------------------------------------------------//
bool MOAIRubyState::HasFieldWithType ( mrb_value klass, cc8* name, mrb_vtype type ) {

	mrb_value field = mrb_iv_get ( this->mState, klass, mrb_intern_cstr ( this->mState, name ) );

	bool hasField = mrb_type ( field ) == type;
	
	return hasField;
}

//----------------------------------------------------------------//
bool MOAIRubyState::HexDecode ( int idx ) {

	ZLHexAdapter hex;
	return this->Decode ( idx, hex );
}

//----------------------------------------------------------------//
bool MOAIRubyState::HexEncode ( int idx ) {

	ZLHexAdapter hex;
	return this->Encode ( idx, hex );
}

//----------------------------------------------------------------//
bool MOAIRubyState::Inflate ( int idx, int windowBits ) {

	ZLDeflateReader inflater;
	inflater.SetWindowBits ( windowBits );
	
	return this->Decode ( idx, inflater );
}

//----------------------------------------------------------------//
bool MOAIRubyState::IsNil ( mrb_value value ) {
	
	return mrb_nil_p ( value );
}

//----------------------------------------------------------------//
bool MOAIRubyState::IsNilOrUndef ( mrb_value value ) {

	return ( mrb_nil_p ( value ) || mrb_undef_p ( value ) );
}

//----------------------------------------------------------------//
bool MOAIRubyState::IsType ( mrb_value value, mrb_vtype type ) {

	return ( this->GetType ( value ) == type );
}

//----------------------------------------------------------------//
bool MOAIRubyState::IsValid () {

	return ( this->mState != 0 );
}

//----------------------------------------------------------------//
bool MOAIRubyState::LogErrors ( u32 level, FILE* file, int status ) {

	if ( status != 0 ) {
	
		RObject* exception = this->mState->exc;
		if ( exception ) {
			STLString msg;
			msg.write ( "-- %s\n", RSTRING_PTR ( mrb_inspect ( this->mState, mrb_obj_value ( exception ) ) ) );
			msg.write ( "-- %s\n", RSTRING_PTR ( mrb_inspect ( this->mState, mrb_exc_backtrace ( this->mState, mrb_obj_value ( exception ) ) ) ) );
			ZLLog::Get ().LogF ( level, file, "%s\n", msg.c_str () );
		}
		return true;
	}
	return false;
}

//----------------------------------------------------------------//
void MOAIRubyState::LogStackDump ( u32 level, FILE* file ) {
	STLString stackDump = this->GetStackDump ();
	ZLLog::Get ().LogF ( level, file, stackDump );
}

//----------------------------------------------------------------//
void MOAIRubyState::LogStackTrace ( u32 level, FILE* file, cc8* title, int stackLevel ) {
	STLString stackTrace = this->GetStackTrace ( title, stackLevel );
	ZLLog::Get ().LogF ( level, file, stackTrace.str ());
}

//----------------------------------------------------------------//
MOAIRubyState::MOAIRubyState () :
	mState ( 0 ) {
}

//----------------------------------------------------------------//
MOAIRubyState::MOAIRubyState ( mrb_state* state ) :
	mState ( state ) {
}

//----------------------------------------------------------------//
MOAIRubyState::~MOAIRubyState () {
}

//----------------------------------------------------------------//
void MOAIRubyState::ReportBadCast ( int idx, cc8* typeName ) {
	MOAILogF ( *this, ZLLog::LOG_ERROR, MOAISTRING_BadCast_DS, idx, typeName );
}

//----------------------------------------------------------------//
bool MOAIRubyState::ParamIsType ( int idx, mrb_vtype type ) {
	return this->IsType ( this->GetParamValue ( idx ), type );
}

//----------------------------------------------------------------//
int MOAIRubyState::RealParamIndex ( int idx ) {

	if ( idx > 0 ) {
		//MOAILogF ( this->mState, ZLLog::LOG_DEBUG, "MOAIRubyState::RealParamIndex(%d) ==>> %d ", idx, idx - 1 );
		return idx - 1;
	}
	return 0;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::Run ( void* data, size_t size, int nArgs, int nResults ) {

	return mrb_load_nstring ( this->mState, ( cc8* )data, size );
}

//----------------------------------------------------------------//
void MOAIRubyState::SetInstanceField ( mrb_value obj, cc8* key, mrb_value value, bool force ) {

	//ZLLog_DebugF ( ZLLog::CONSOLE, "moai-lib-core::MOAIRubyState::SetInstanceField() %s\n", key );

	if ( mrb_nil_p ( obj ) ) {
		ZLLog_DebugF ( ZLLog::CONSOLE, "moai-lib-core::MOAIRubyState::SetInstanceField() %s nil return...\n", key );
		return;
	}

	mrb_sym const sym = mrb_intern_cstr ( this->mState, key );

	if ( !mrb_iv_defined ( this->mState, obj, sym ) || force ) {
		mrb_iv_set ( this->mState, obj, sym, value );
	}

	//ZLLog_DebugF ( ZLLog::CONSOLE, "moai-lib-core::MOAIRubyState::SetInstanceField() %s ok!\n", key );
}

//----------------------------------------------------------------//
void MOAIRubyState::SetRObjectField ( RObject* obj, cc8* key, mrb_value value, bool force ) {

	mrb_sym const sym = mrb_intern_cstr ( this->mState, key );

	if ( !mrb_obj_iv_defined ( this->mState, obj, sym ) || force ) {
		mrb_obj_iv_set ( this->mState, obj, sym, value );
	}
}

//----------------------------------------------------------------//
void MOAIRubyState::SetClassField ( RClass* klass, cc8* key, mrb_value value, bool force ) {

	mrb_sym const sym = mrb_intern_cstr ( this->mState, key );

	mrb_value const obj = mrb_obj_value ( klass );

	if ( !mrb_iv_defined ( this->mState, obj, sym ) || force ) {
		mrb_iv_set ( this->mState, obj, sym, value );
	}
}

//----------------------------------------------------------------//
void MOAIRubyState::SetPath ( cc8* path ) {

	// TODO: add path to $:($LOAD_PATHs)

}

//----------------------------------------------------------------//
template <>
bool MOAIRubyState::ToCValue < bool >( const mrb_value& value ) {
	return mrb_bool ( value );
}

//----------------------------------------------------------------//
template <>
cc8* MOAIRubyState::ToCValue < cc8* >( const mrb_value& value ) {
	return mrb_string_value_ptr ( this->mState, value );
}

//----------------------------------------------------------------//
template <>
double MOAIRubyState::ToCValue < double >( const mrb_value& value ) {
	return mrb_float ( value );
}

//----------------------------------------------------------------//
template <>
float MOAIRubyState::ToCValue < float >( const mrb_value& value ) {
	return mrb_float ( value );
}

//----------------------------------------------------------------//
template <>
s8 MOAIRubyState::ToCValue < s8 >( const mrb_value& value ) {
	return ( s8 )mrb_int ( this->mState, value );
}

//----------------------------------------------------------------//
template <>
s16 MOAIRubyState::ToCValue < s16 >( const mrb_value& value ) {
	return ( s16 )mrb_int ( this->mState, value );
}

//----------------------------------------------------------------//
template <>
s32 MOAIRubyState::ToCValue < s32 >( const mrb_value& value ) {
	return ( s32 )mrb_int ( this->mState, value );
}

//----------------------------------------------------------------//
template <>
s64 MOAIRubyState::ToCValue < s64 >( const mrb_value& value ) {
	return ( s64 )mrb_int ( this->mState, value );
}

//----------------------------------------------------------------//
template <>
u8 MOAIRubyState::ToCValue < u8 >( const mrb_value& value ) {
	return ( u8 )mrb_int ( this->mState, value );
}

//----------------------------------------------------------------//
template <>
u16 MOAIRubyState::ToCValue < u16 >( const mrb_value& value ) {
	return ( u16 )mrb_int ( this->mState, value );
}

//----------------------------------------------------------------//
template <>
u32 MOAIRubyState::ToCValue < u32 >( const mrb_value& value ) {
	return ( u32 )mrb_int ( this->mState, value );
}

//----------------------------------------------------------------//
template <>
u64 MOAIRubyState::ToCValue < u64 >( const mrb_value& value ) {
	return ( u64 )mrb_int ( this->mState, value );
}

//----------------------------------------------------------------//
template <>
const void* MOAIRubyState::ToCValue < const void* >( const mrb_value& value ) {
	return mrb_ptr ( value );
}

//----------------------------------------------------------------//
template <>
mrb_value MOAIRubyState::ToRValue < bool >( bool value ) {
	return mrb_bool_value ( value );
}

//----------------------------------------------------------------//
template <>
mrb_value MOAIRubyState::ToRValue < cc8* >( cc8* value ) {
	return mrb_str_new_cstr ( this->mState, value );
}

//----------------------------------------------------------------//
template <>
mrb_value MOAIRubyState::ToRValue < double >( double value ) {
	return mrb_float_value ( this->mState, value );
}

//----------------------------------------------------------------//
template <>
mrb_value MOAIRubyState::ToRValue < float >( float value ) {
	return mrb_float_value ( this->mState, value );
}

//----------------------------------------------------------------//
template <>
mrb_value MOAIRubyState::ToRValue < s8 >( s8 value ) {
	return mrb_fixnum_value ( value );
}

//----------------------------------------------------------------//
template <>
mrb_value MOAIRubyState::ToRValue < s16 >( s16 value ) {
	return mrb_fixnum_value ( value );
}

//----------------------------------------------------------------//
template <>
mrb_value MOAIRubyState::ToRValue < s32 >( s32 value ) {
	return mrb_fixnum_value ( value );
}

//----------------------------------------------------------------//
template <>
mrb_value MOAIRubyState::ToRValue < s64 >( s64 value ) {
	return mrb_fixnum_value ( value );
}

//----------------------------------------------------------------//
template <>
mrb_value MOAIRubyState::ToRValue < u8 >( u8 value ) {
	return mrb_fixnum_value ( value );
}

//----------------------------------------------------------------//
template <>
mrb_value MOAIRubyState::ToRValue < u16 >( u16 value ) {
	return mrb_fixnum_value ( value );
}

//----------------------------------------------------------------//
template <>
mrb_value MOAIRubyState::ToRValue < u32 >( u32 value ) {
	return mrb_fixnum_value ( value );
}

//----------------------------------------------------------------//
template <>
mrb_value MOAIRubyState::ToRValue < u64 >( u64 value ) {
	return mrb_fixnum_value ( value );
}

//----------------------------------------------------------------//
template <>
mrb_value MOAIRubyState::ToRValue < void* >( void* value ) {
	return mrb_cptr_value ( this->mState, value );
}

//----------------------------------------------------------------//
template <>
mrb_value MOAIRubyState::ToRValue < MOAIRubyObject* > ( MOAIRubyObject* value ) {

	return value != 0 ? value->GetMRBObject () : mrb_nil_value ();
}

//----------------------------------------------------------------//
template <>
mrb_value MOAIRubyState::ToRValue < MOAIRubyObject& > ( MOAIRubyObject& value ) {

	return value.GetMRBObject ();
}

//----------------------------------------------------------------//
template <>
mrb_value MOAIRubyState::ToRValue < STLString > ( STLString value ) {

	return mrb_str_new_cstr ( this->mState, value.c_str () );
}


//----------------------------------------------------------------//
template <>
mrb_value MOAIRubyState::ToRValue < ZLMatrix4x4 > ( ZLMatrix4x4 value ) {

	mrb_value ret [ 16 ];
	ret [ 0 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C0_R0 ] );
	ret [ 1 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C1_R0 ] );
	ret [ 2 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C2_R0 ] );
	ret [ 3 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C3_R0 ] );

	ret [ 4 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C0_R1 ] );
	ret [ 5 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C1_R1 ] );
	ret [ 6 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C2_R1 ] );
	ret [ 7 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C3_R1 ] );

	ret [ 8 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C0_R2 ] );
	ret [ 9 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C1_R2 ] );
	ret [ 10 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C2_R2 ] );
	ret [ 11 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C3_R2 ] );

	ret [ 12 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C0_R3 ] );
	ret [ 13 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C1_R3 ] );
	ret [ 14 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C2_R3 ] );
	ret [ 15 ] = this->ToRValue ( value.m [ ZLMatrix4x4::C3_R3 ] );

	return mrb_ary_new_from_values ( this->mState, 16, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::ToSymbol ( cc8* str ) {

	return mrb_symbol_value ( mrb_intern_cstr ( this->mState, str ) );
}

//----------------------------------------------------------------//
mrb_value MOAIRubyState::YieldThread ( int nResults ) {
	
	// TODO: mrb_yield
	return mrb_yield ( this->mState, mrb_nil_value (), mrb_nil_value () );
}
