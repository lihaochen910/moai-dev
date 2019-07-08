// lua
extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

// temporarilty replace calls here; TODO: roll into MOAILuaState
//#if LUA_VERSION_NUM >= 502
//	#define lua_objlen(L,idx) lua_rawlen(L,idx)
//	#define LUA_HOOKTAILRET LUA_HOOKTAILCALL
//#endif

// mruby
#include <mruby.h>
#include <mruby/string.h>
#include <mruby/compile.h>
#include <mruby/variable.h>
#include <mruby/array.h>
#include <mruby/hash.h>
#include <mruby/error.h>
#include <mruby/data.h>
#include <mruby/class.h>

#include <zl-util/headers.h>
#include <moai_config.h>
