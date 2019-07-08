// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIRUBYARENASTORE_H
#define MOAIRUBYARENASTORE_H

#include <moai-core/MOAIRuby.h>

//================================================================//
// MOAIRubyArenaStore
//================================================================//
/**	@ruby	MOAIRubyArenaStore
	@text	Example class for extending Moai using MOAILuaObject.
			Copy this object, rename it and add your own stuff.
			Just don't forget to register it with the runtime
			using the REGISTER_LUA_CLASS macro (see moaicore.cpp).
*/
class MOAIRubyArenaStore {
private:
	
	mrb_state*	mrb;
	int			ai;

public:
	
	//----------------------------------------------------------------//
	MOAIRubyArenaStore ( mrb_state* mrb ) :
		mrb ( mrb ),
		ai ( mrb_gc_arena_save ( mrb ) ) {
	}

	//----------------------------------------------------------------//
	~MOAIRubyArenaStore () {
		mrb_gc_arena_restore ( mrb, ai );
	}

};

#endif
