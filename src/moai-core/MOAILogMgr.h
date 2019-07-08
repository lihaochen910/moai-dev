// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAILOGMGR_H
#define	MOAILOGMGR_H

#include <moai-core/MOAIRuby.h>
#include <moai-core/strings.h>

// TODO: these have never made a lot of sense here. find someplace better for them.

#define MOAI_LUA_SETUP(type,str)
#define MOAI_LUA_SETUP_CLASS(str)
#define MOAI_LUA_SETUP_SINGLE(type,str)

/*type* self = MOAILogMgr::Get ().RubySetup < type >( state, context, str );	\*/
/*type* self = type::GetRubyObject ( M, context );							\*/
#define MOAI_RUBY_SETUP(type,str)												\
	MOAIRubyState state ( M );													\
	type* self = state.GetRubyObject < type >( context );						\
	if ( !self ) return context;

#define MOAI_RUBY_SETUP_CLASS(str)												\
	MOAIRubyState state ( M );													\
	if ( !MOAILogMgr::Get ().RubySetupClass ( state, str ) ) return context;

#define MOAI_RUBY_SETUP_SINGLE(type,str)										\
	MOAIRubyState state ( M );													\
	type* self = MOAILogMgr::Get ().RubySetupSingle < type >( state, str );		\
	if ( !self ) return context;

//================================================================//
// MOAILogMgr
//================================================================//
/**	@lua	MOAILogMgr
	@text	Singleton for managing debug log messages and log level.

	@const LOG_NONE		No logging
	@const LOG_ERROR	Error level
	@const LOG_WARNING	Warning level
	@const LOG_STATUS	Status level
	@const LOG_DEBUG	Debug level
*/
class MOAILogMgr :
	public ZLContextClass < MOAILogMgr, MOAIRubyObject > {
private:

	FILE*			mFile;
	bool			mTypeCheckRubyParams;

	//----------------------------------------------------------------//
	static mrb_value		_closeFile					( mrb_state* M, mrb_value self );
	static mrb_value		_isDebugBuild				( mrb_state* M, mrb_value self );
	static mrb_value		_log						( mrb_state* M, mrb_value self );
	static mrb_value		_openFile					( mrb_state* M, mrb_value self );
	static mrb_value		_setLogLevel				( mrb_state* M, mrb_value self );
	static mrb_value		_setTypeCheckLuaParams		( mrb_state* M, mrb_value self );

public:
	
	DECL_RUBY_SINGLETON ( MOAILogMgr )
	
	//GET ( FILE*, File, mFile ? mFile : ZLLog::CONSOLE )
	
	//----------------------------------------------------------------//
	static bool				CheckFileExists			( cc8* filename, mrb_state* M = 0 );
	static bool				CheckIndex				( size_t idx, size_t size, mrb_state* M = 0 );
	static bool				CheckIndexPlusOne		( size_t idx, size_t size, mrb_state* M = 0 );
	static bool				CheckReserve			( size_t idx, size_t size, mrb_state* M = 0 );
	void					CloseFile				();
	void					LogF					( mrb_state*L, u32 level, cc8* message, ... );
	void					LogV					( mrb_state*L, u32 level, cc8* message, va_list args );
							MOAILogMgr				();
							~MOAILogMgr				();
	void					OpenFile				( cc8* filename );
	void					RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	bool					RubySetupClass			( MOAIRubyState& state, cc8* typeStr );
	
	//----------------------------------------------------------------//
	template < typename TYPE >
	TYPE* RubySetup ( MOAIRubyState& state, mrb_value context, cc8* typeStr ) {
	
		if ( this->mTypeCheckRubyParams && typeStr ) {
			if ( !state.CheckParams ( typeStr, true ) ) return 0;
		}
		//return state.GetRubyObject < TYPE >( 1, true );
		//return TYPE::GetRubyObject ( state, context );
		return 0;
	}
	
	//----------------------------------------------------------------//
	template < typename TYPE >
	TYPE* RubySetupSingle ( MOAIRubyState& state, cc8* typeStr ) {
	
		if ( this->mTypeCheckRubyParams && typeStr ) {
			if ( !state.CheckParams ( 1, typeStr, true ) ) return 0;
		}
		return ZLContextMgr::Get ()->GetGlobal < TYPE >();
	}
};

#define MOAILogF(M,level,message,...)		MOAILogMgr::Get ().LogF ( M, level, message, ##__VA_ARGS__ )
#define MOAILogV(M,level,message,args)		MOAILogMgr::Get ().LogV ( M, level, message, args )

#endif
