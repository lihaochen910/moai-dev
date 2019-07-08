// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIParticleTimedEmitter.h>
#include <moai-sim/MOAIParticleSystem.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	setFrequency
	@text	Set timer frequency.
	
	@in		MOAIParticleTimedEmitter self
	@in		number min
	@opt	number max		Default value is min.
	@out	nil
*/
mrb_value MOAIParticleTimedEmitter::_setFrequency ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleTimedEmitter, "UN" )

	float min = state.GetParamValue < float >( 1, 1.0f );
	float max = state.GetParamValue < float >( 2, min );

	self->SetFrequencyRange ( min, max );
	return mrb_nil_value ();
}

//================================================================//
// MOAIParticleTimedEmitter
//================================================================//

//----------------------------------------------------------------//
float MOAIParticleTimedEmitter::GetRandomFrequency () {

	return ZLFloat::Rand ( this->mMinFrequency, this->mMaxFrequency );
}

//----------------------------------------------------------------//
MOAIParticleTimedEmitter::MOAIParticleTimedEmitter () :
	mTime ( 0.0f ),
	mEmitTime ( 0.0f ),
	mMinFrequency ( 1.0f ),
	mMaxFrequency ( 1.0f ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIParticleEmitter )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIParticleTimedEmitter::~MOAIParticleTimedEmitter () {
}

//----------------------------------------------------------------//
void MOAIParticleTimedEmitter::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIParticleEmitter::RegisterRubyClass ( state, klass );

}

//----------------------------------------------------------------//
void MOAIParticleTimedEmitter::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAIParticleEmitter::RegisterRubyFuncs ( state, klass );
	
	state.DefineInstanceMethod ( klass, "setFrequency",		_setFrequency, MRB_ARGS_ARG ( 1, 1 ) );
}

//----------------------------------------------------------------//
void MOAIParticleTimedEmitter::SetFrequencyRange ( float min, float max ) {

	this->mMinFrequency = min;
	this->mMaxFrequency = max;
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAIParticleTimedEmitter::MOAIAction_Update ( double step ) {

	this->mTime += ( float )step;
	if ( !this->mSystem ) return;
	if ( this->mTime < this->mEmitTime ) return;
	
	u32 emission = this->GetRandomEmission ();
	this->Surge ( emission );
	
	this->mEmitTime = this->mTime + this->GetRandomFrequency ();
}
