// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-core/MOAILogMgr.h>

#include <moai-core/MOAIRubyState-impl.h>
#include <moai-core/MOAIRubyClass-impl.h>

//================================================================//
// ruby
//================================================================//

//----------------------------------------------------------------//
/**	@ruby	closeFile
	@text	Resets log output to stdout.

	@out	nil
*/
mrb_value MOAILogMgr::_closeFile ( mrb_state* M, mrb_value self ) {
	UNUSED ( M );

	MOAILogMgr::Get ().CloseFile ();

	return self;
}

//----------------------------------------------------------------//
/**	@ruby	isDebugBuild
	@text	Returns a boolean value indicating whether Moai has been
			compiles as a debug build or not.

	@out	boolean isDebugBuild
*/
mrb_value MOAILogMgr::_isDebugBuild ( mrb_state* M, mrb_value self ) {
	UNUSED ( M );

	#ifdef _DEBUG
		return mrb_true_value ();
	#else
		return mrb_false_value ();
	#endif
}

//----------------------------------------------------------------//
/**	@ruby	log
	@text	Alias for print.

	@overload
		@in		string message
		@out	nil
	
	@overload
		@in		number level	Default: LOG_STATUS
		@in		string message
		@out	nil
	
	@overload
		@in		number level	Default: LOG_STATUS
		@in		string token
		@in		string message
		@out	nil
*/
mrb_value MOAILogMgr::_log ( mrb_state* M, mrb_value self ) {
	MOAIRubyState state ( M );

	u32 idx = 1;
	u32 level = ZLLog::LOG_STATUS;

	if ( state.ParamIsType ( 1, MRB_TT_FIXNUM ) ) {
		level = state.GetParamValue < u32 >( 1, ZLLog::LOG_STATUS );
		idx++;
	}
	
	cc8* str1 =  state.GetParamValue < cc8* >( idx++, "" );
	cc8* str2 =  state.GetParamValue < cc8* >( idx, 0 );

	cc8* msg = str1;
	cc8* token = "MOAI";
	
	if ( str2 ) {
		token = str1;
		msg = str2;
	}

	STLString log;
	log.write ( "[%s-%d] %s", token, level, msg );

	ZLLog::Get ().LogF ( level, ZLLog::CONSOLE, "%s", log.c_str () );	// Caller's string may contain % and should NOT be used as a format to LogF

	return self;
}

//----------------------------------------------------------------//
/**	@ruby	openFile
	@text	Opens a new file to receive log messages.

	@in		string filename
	@out	nil
*/
mrb_value MOAILogMgr::_openFile ( mrb_state* M, mrb_value self ) {

	MOAIRubyState state ( M );
	if ( !state.CheckParams ( 1, "S" ) ) return self;

	cc8* filename = state.GetParamValue < cc8* >( 1, "" );
	MOAILogMgr::Get ().OpenFile ( filename );

	return self;
}

//----------------------------------------------------------------//
/**	@ruby	setLogLevel
	@text	Set the logging level.

	@in		number logLevel		One of MOAILogMgr LOG_NONE, LOG_ERROR, LOG_WARNING, LOG_STATUS
	@out	nil
*/
mrb_value MOAILogMgr::_setLogLevel ( mrb_state* M, mrb_value self ) {

	MOAIRubyState state ( M );

	u32 level = state.GetParamValue < u32 >( 1, ZLLog::LOG_NONE );
	ZLLog::Get ().SetLogLevel ( level );

	return self;
}

//----------------------------------------------------------------//
/**	@ruby	setTypeCheckLuaParams
	@text	Set or clear type checking of parameters passed to Lua bound Moai API functions.

	@opt	boolean check		Default value is false.
	@out	nil
*/
mrb_value MOAILogMgr::_setTypeCheckLuaParams ( mrb_state* M, mrb_value self ) {

	MOAIRubyState state ( M );

	bool check = state.GetParamValue < bool >( 1, false );
	MOAILogMgr::Get ().mTypeCheckRubyParams = check;

	return self;
}

//================================================================//
// MOAILogMgr
//================================================================//

//----------------------------------------------------------------//
bool MOAILogMgr::CheckFileExists ( cc8* filename, mrb_state* M ) {
	
	if ( ZLFileSys::CheckFileExists ( filename )) {
		return true;
	}
	
	STLString expand = ZLFileSys::GetAbsoluteFilePath ( filename );
	MOAILogF ( M, ZLLog::LOG_ERROR, MOAISTRING_FileNotFound_S, expand.str () );
	
	return false;
}

//----------------------------------------------------------------//
bool MOAILogMgr::CheckIndex ( size_t idx, size_t size, mrb_state* M ) {

	if ( size == 0 ) {
		MOAILogF ( M, ZLLog::LOG_ERROR, MOAISTRING_IndexNoReserved );
		return false;
	}
	else if ( !( idx < size )) {
		MOAILogF ( M, ZLLog::LOG_ERROR, MOAISTRING_IndexOutOfRange_DDD, ( u32 )idx, 0, ( u32 )( size - 1 ));
		return false;
	}
	return true;
}

//----------------------------------------------------------------//
bool MOAILogMgr::CheckIndexPlusOne ( size_t idx, size_t size, mrb_state* M ) {

	if ( size == 0 ) {
		MOAILogF ( M, ZLLog::LOG_ERROR, MOAISTRING_IndexNoReserved );
		return false;
	}
	else if ( !( idx < size )) {
		MOAILogF ( M, ZLLog::LOG_ERROR, MOAISTRING_IndexOutOfRange_DDD, idx + 1, 1, size );
		return false;
	}
	return true;
}

//----------------------------------------------------------------//
bool MOAILogMgr::CheckReserve ( size_t idx, size_t size, mrb_state* M ) {

	if ( !( idx < size ) ) {
		MOAILogF ( M, ZLLog::LOG_ERROR, MOAISTRING_IndexNoReserved );
		return false;
	}
	return true;
}

//----------------------------------------------------------------//
void MOAILogMgr::CloseFile () {

	if ( this->mFile ) {
		fclose ( this->mFile );
		this->mFile = 0;
		ZLLog::Get ().SetRedirect ( 0 );
	}
}

//----------------------------------------------------------------//
void MOAILogMgr::LogF ( mrb_state* M, u32 level, cc8* message, ... ) {

	va_list args;
	va_start ( args, message );

	this->LogV ( M, level, message, args );

	va_end ( args );
}

//----------------------------------------------------------------//
void MOAILogMgr::LogV ( mrb_state* M, u32 level, cc8* message, va_list args ) {

	if ( ZLLog::Get ().IsEnabled ( level )) {

		if ( M ) {
			ZLLog::Get ().LogF ( level, ZLLog::CONSOLE, "----------------------------------------------------------------\n" );
		}

		ZLLog::Get ().LogV ( level, ZLLog::CONSOLE, message, args );
		
		size_t msgSize = strlen ( message );
		if ( msgSize && ( message [ msgSize - 1 ] != '\n' )) {
			ZLLog::Get ().LogF ( level, ZLLog::CONSOLE, "\n" );
		}
		
		if ( M ) {
			ZLLog::Get ().LogF ( level, ZLLog::CONSOLE, "\n" );
			MOAIRubyState state ( M );
			state.LogStackTrace ( level, ZLLog::CONSOLE, NULL, 0 );
			ZLLog::Get ().LogF ( level, ZLLog::CONSOLE, "\n" );
		}
	}
}

//----------------------------------------------------------------//
MOAILogMgr::MOAILogMgr () :
	mFile ( 0 ),
	mTypeCheckRubyParams ( true ) {

	RTTI_SINGLE ( MOAIRubyObject )
}

//----------------------------------------------------------------//
MOAILogMgr::~MOAILogMgr () {

	this->CloseFile ();
}

//----------------------------------------------------------------//
void MOAILogMgr::OpenFile ( cc8* filename ) {

	this->CloseFile ();

	FILE* file = fopen ( filename, "w" );
	if ( file ) {
		this->mFile = file;
		ZLLog::Get ().SetRedirect ( file );
	}
}

//----------------------------------------------------------------//
void MOAILogMgr::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "LOG_DEBUG", ( u32 )ZLLog::LOG_DEBUG );
	state.DefineClassConst ( klass, "LOG_STATUS", ( u32 )ZLLog::LOG_STATUS );
	state.DefineClassConst ( klass, "LOG_WARNING", ( u32 )ZLLog::LOG_WARNING );
	state.DefineClassConst ( klass, "LOG_ERROR", ( u32 )ZLLog::LOG_ERROR );
	state.DefineClassConst ( klass, "LOG_FATAL", ( u32 )ZLLog::LOG_FATAL );
	state.DefineClassConst ( klass, "LOG_NONE", ( u32 )ZLLog::LOG_NONE );
	state.DefineClassConst ( klass, "LOG_REPORT", ( u32 )ZLLog::LOG_REPORT );

	state.DefineStaticMethod ( klass, "closeFile",				_closeFile, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "log",					_log, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "isDebugBuild",			_isDebugBuild, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "openFile",				_openFile, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "setLogLevel",			_setLogLevel, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "setTypeCheckLuaParams",	_setTypeCheckLuaParams, MRB_ARGS_NONE () );
}

//----------------------------------------------------------------//
bool MOAILogMgr::RubySetupClass ( MOAIRubyState& state, cc8* typeStr ) {

	if ( this->mTypeCheckRubyParams && typeStr ) {
		return state.CheckParams ( 1, typeStr, true );
	}
	return false;
}
