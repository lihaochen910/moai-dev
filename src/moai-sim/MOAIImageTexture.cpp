// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIImageTexture.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	updateRegion
	@text	Update either a sub-region of the texture or the whole
			texture to match changes in the image. Updated regions will
			be reloaded from the image the next time the texture is bound.

	@in		MOAIImageTexture self
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@out	nil
*/
mrb_value MOAIImageTexture::_updateRegion ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIImageTexture, "U" )
	
	if ( state.GetParamsCount () > 1 ) {
		ZLIntRect rect = state.GetRect < int >( 1 );
		self->UpdateRegion ( rect );
	}
	else {
		self->UpdateRegion ();
	}
	return context;
}

//================================================================//
// MOAIImageTexture
//================================================================//

//----------------------------------------------------------------//
void MOAIImageTexture::OnClearDirty () {

	this->mRegion.Clear ();
}

//----------------------------------------------------------------//
MOAIImageTexture::MOAIImageTexture () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAITextureBase )
		RTTI_EXTEND ( MOAIImage )
	RTTI_END
	
	this->mDebugName = "(stacktrace from MOAIImageTexture)";
}

//----------------------------------------------------------------//
MOAIImageTexture::~MOAIImageTexture () {
}

//----------------------------------------------------------------//
bool MOAIImageTexture::OnGPUCreate () {

	if ( !this->IsOK ()) return false;
	
	this->mRegion.Clear ();
	if ( this->CreateTextureFromImage ( *this )) {
		return this->OnGPUUpdate ();
	}
	return false;
}

//----------------------------------------------------------------//
bool MOAIImageTexture::OnGPUUpdate () {

	bool result = true;

	if ( this->mRegion.Area () > 0 ) {
		result = this->UpdateTextureFromImage ( *this, this->mRegion );
		this->mRegion.Clear ();
	}
	return result && MOAITextureBase::OnGPUUpdate ();
}

//----------------------------------------------------------------//
void MOAIImageTexture::OnImageStatusChanged	( bool isOK ) {

	if ( isOK ) {
		this->FinishInit ();
	}
}

//----------------------------------------------------------------//
void MOAIImageTexture::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	MOAITextureBase::RegisterRubyClass ( state, klass );
	MOAIImage::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIImageTexture::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAITextureBase::RegisterRubyFuncs ( state, klass );
	MOAIImage::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "invalidate", _updateRegion, MRB_ARGS_ARG ( 0, 4 ) );
	state.DefineInstanceMethod ( klass, "updateRegion", _updateRegion, MRB_ARGS_ARG ( 0, 4 ) );
	
}

//----------------------------------------------------------------//
void MOAIImageTexture::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	MOAITextureBase::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIImageTexture::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	MOAITextureBase::SerializeOut ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIImageTexture::UpdateRegion () {
	
	this->mRegion = this->GetRect ();
	this->ScheduleForGPUUpdate ();
}

//----------------------------------------------------------------//
void MOAIImageTexture::UpdateRegion ( ZLIntRect rect ) {
	
	rect.Bless ();
	this->GetRect ().Clip ( rect );
	
	if ( this->GetState () == STATE_NEEDS_GPU_UPDATE ) {
		this->mRegion.Grow ( rect );
	}
	else {
		this->mRegion = rect;
	}
	
	this->ScheduleForGPUUpdate ();
}
