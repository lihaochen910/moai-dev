// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/headers.h>
#include <moai-sim/host_particles.h>

//================================================================//
// aku-particles
//================================================================//

//----------------------------------------------------------------//
void AKUNewParticlePlugin ( mrb_state* M, AKUParticleInitFunc initFunc, AKUParticleRenderFunc renderFunc, int size ) {
	UNUSED ( M );
	UNUSED ( initFunc );
	UNUSED ( renderFunc );
	UNUSED ( size );
	
	/*MOAIRubyState state ( M );

	MOAIParticleCallbackPlugin* plugin = new MOAIParticleCallbackPlugin ();
	plugin->Init ( initFunc, renderFunc, size );
	plugin->PushLuaUserdata ( state );*/
}

//----------------------------------------------------------------//
void AKUSetParticlePreset ( const char* presetTable, const char* presetName, AKUParticleInitFunc initFunc, AKUParticleRenderFunc renderFunc, int size ) {

	//mrb_state* M = AKUGetRubyState ();
	//
	//lua_getglobal ( L, presetTable );
	//
	//if ( lua_isnil ( L, -1 )) { // TODO: nil or none?
	//	lua_newtable ( L );
	//	lua_setglobal ( L, presetTable );
	//}
	//
	//lua_getglobal ( L, presetTable );
	//assert ( lua_isnil ( L, -1 ) == false );
	//
	//AKUNewParticlePlugin ( L, initFunc, renderFunc, size );
	//lua_setfield ( L, -2, presetName );
	//
	//lua_pop ( L, 1 );
}
