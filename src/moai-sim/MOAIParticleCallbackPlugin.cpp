// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIParticleCallbackPlugin.h>

//================================================================//
// MOAIParticleCallbackPlugin
//================================================================//

//----------------------------------------------------------------//
void MOAIParticleCallbackPlugin::Init ( AKUParticleInitFunc initFunc, AKUParticleRenderFunc renderFunc, int size ) {

	this->mInitFunc		= initFunc;
	this->mRenderFunc	= renderFunc;
	this->mSize			= size;
}

//----------------------------------------------------------------//
MOAIParticleCallbackPlugin::MOAIParticleCallbackPlugin () :
	mInitFunc ( 0 ),
	mRenderFunc ( 0 ) {

	RTTI_SINGLE ( MOAIParticlePlugin )
}

//----------------------------------------------------------------//
MOAIParticleCallbackPlugin::~MOAIParticleCallbackPlugin () {
}

//----------------------------------------------------------------//
void MOAIParticleCallbackPlugin::OnInit ( float* particle, float* registers ) {

	if ( this->mInitFunc ) {
		this->mInitFunc ( particle, registers );
	}
}

//----------------------------------------------------------------//
void MOAIParticleCallbackPlugin::OnRender ( float* particle, float* registers, AKUParticleSprite* sprite, float t0, float t1, float term ) {

	if ( this->mRenderFunc ) {
		this->mRenderFunc ( particle, registers, sprite, t0, t1, term );
	}
}

//----------------------------------------------------------------//
void MOAIParticleCallbackPlugin::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAIParticlePlugin::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIParticleCallbackPlugin::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIParticlePlugin::RegisterRubyFuncs ( state, klass );
}

