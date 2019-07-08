// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <contrib/moai_utf8.h>
#include <moai-sim/MOAIFont.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGlyph.h>
#include <moai-sim/MOAIImage.h>
#include <moai-sim/MOAIStaticGlyphCache.h>
#include <moai-sim/MOAITexture.h>

//================================================================//
// MOAIStaticGlyphCache
//================================================================//

//----------------------------------------------------------------//
void MOAIStaticGlyphCache::ClearTextures () {

	for ( u32 i = 0; i < this->mTextures.Size (); ++i ) {
		this->RubyRelease ( this->mTextures [ i ]); // TODO: ref counting?
	}
	this->mTextures.Clear ();
}

//----------------------------------------------------------------//
MOAIImage* MOAIStaticGlyphCache::GetGlyphImage ( MOAIGlyph& glyph ) {
	UNUSED ( glyph );
	return 0;
}

//----------------------------------------------------------------//
MOAITextureBase* MOAIStaticGlyphCache::GetGlyphTexture ( MOAIGlyph& glyph ) {

	assert ( glyph.GetPageID () < this->mTextures.Size ());
	return this->mTextures [ glyph.GetPageID ()];
}

//----------------------------------------------------------------//
MOAIImage* MOAIStaticGlyphCache::GetImage () {

	return 0;
}

//----------------------------------------------------------------//
MOAITexture* MOAIStaticGlyphCache::GetTexture ( u32 id ) {

	return this->mTextures [ id ];
}

//----------------------------------------------------------------//
bool MOAIStaticGlyphCache::IsDynamic () {

	return false;
}

//----------------------------------------------------------------//
MOAIStaticGlyphCache::MOAIStaticGlyphCache () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIGlyphCache )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIStaticGlyphCache::~MOAIStaticGlyphCache () {

	this->ClearTextures ();
}

//----------------------------------------------------------------//
void MOAIStaticGlyphCache::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	MOAIGlyphCache::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIStaticGlyphCache::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	MOAIGlyphCache::RegisterRubyFuncs ( state, klass );
}

//----------------------------------------------------------------//
void MOAIStaticGlyphCache::ReserveTextures ( u32 total ) {

	this->mTextures.Init ( total );
	this->mTextures.Fill ( 0 );
}


//----------------------------------------------------------------//
void MOAIStaticGlyphCache::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAIStaticGlyphCache::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
int MOAIStaticGlyphCache::SetImage ( MOAIFont& font, MOAIImage& image ) {
	MOAIRubyState& state = MOAIRubyRuntime::Get ().GetMainState ();

	this->ClearTextures ();

	u32 width = image.GetWidth ();
	u32 height = image.GetHeight ();

	if ( !( width && height )) return STATUS_ERROR;

	u32 totalTextures = ( height / width ) + 1;
	this->mTextures.Init ( totalTextures );
	
	u32 y = 0;
	for ( u32 i = 0; i < totalTextures; ++i ) {
		
		MOAITexture* texture = state.CreateClassInstance < MOAITexture >();
		
		u32 textureHeight = height - y;
		textureHeight = textureHeight > width ? width : textureHeight;
		
		texture->Init ( image, 0, y, width, textureHeight, font.GetFilename ());
		texture->SetFilter ( font.GetMinFilter (), font.GetMagFilter ());
		
		y += textureHeight;
		
		this->SetTexture ( i, texture );
	}
	return STATUS_OK;
}

//----------------------------------------------------------------//
void MOAIStaticGlyphCache::SetTexture ( int id, MOAITexture* texture ) {
	this->RubyRetain ( texture ); // TODO: ref counting?
	this->mTextures [ id ] = texture;
}
