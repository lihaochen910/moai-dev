// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAILight.h>
#include <moai-sim/MOAIMaterial.h>
#include <moai-sim/MOAIMaterialBatch.h>
#include <moai-sim/MOAIPartitionHull.h>
#include <moai-sim/MOAIShader.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAITexture.h>
#include <moai-sim/MOAITextureBase.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatch::_getBlendMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatch, "U" )
	return self->GetBlendMode ( state, 1 );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatch::_getCullMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatch, "U" )
	return self->GetCullMode ( state, 1 );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatch::_getDepthMask ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatch, "U" )
	return self->GetDepthMask ( state, 1 );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatch::_getDepthTest ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatch, "U" )
	return self->GetDepthTest ( state, 1 );
}

//----------------------------------------------------------------//
/**	@lua	getIndexBatchSize
	@text	Get the index batch size.
	
	@in		MOAIMaterialBatch self
	@out	number indexBatchSize
*/
mrb_value MOAIMaterialBatch::_getIndexBatchSize ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatch, "U" );
	return state.ToRValue ( self->mIndexBatchSize );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatch::_getLight ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatch, "U" )
	return self->GetLight ( state, 1 );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatch::_getShader ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatch, "U" )
	return self->GetShader ( state, 1 );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatch::_getTexture ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatch, "U" )
	return self->GetTexture ( state, 1 );
}

//----------------------------------------------------------------//
/**	@lua	reserveMaterials
	@text	Reserve material indices.
	
	@in		MOAIMaterialBatch self
	@opt	number count
	@out	nil
*/
mrb_value MOAIMaterialBatch::_reserveMaterials ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatch, "U" )
	self->Reserve ( state.GetParamValue ( 1, 0 ));
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
/** @lua	Set blend mode using OpenGL source and dest factors. OpenGl blend factor constants are exposed as members of MOAIMaterialBatch.
			See the OpenGL documentation for an explanation of blending constants.

	@in		MOAIMaterialBatch self
	@in		number srcFactor
	@in		number dstFactor
	@out	nil
*/
mrb_value MOAIMaterialBatch::_setBlendMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatch, "U" )
	self->SetBlendMode ( state, 1 );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatch::_setCullMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatch, "U" )
	self->SetCullMode ( state, 1 );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatch::_setDepthMask ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatch, "U" )
	self->SetDepthMask ( state, 1 );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatch::_setDepthTest ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatch, "U" )
	self->SetDepthTest ( state, 1 );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setIndexBatchSize
	@text	Set the index batch size. When a prop or deck is drawing,
			the index is divided by the material batch's index batch size
			to get the material index. In this way sets of deck indices
			may be assigned to material indices. For example, an index
			batch size of 256 for a set of 4 materials would distribute
			1024 deck indices across materials 1 though 4. An index batch
			size of 1 would create a 1 to 1 mapping between deck indices
			and materials.
	
	@in		MOAIMaterialBatch self
	@opt	number indexBatchSize		Default value is 1.
	@out	nil
*/
mrb_value MOAIMaterialBatch::_setIndexBatchSize ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatch, "U" )
	self->mIndexBatchSize = state.GetParamValue < u32 >( 1, 1 );
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatch::_setLight ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatch, "U" )
	return state.ToRValue < MOAIRubyObject* >( self->SetLight ( state, 1 ));
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatch::_setShader ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatch, "U" )
	return state.ToRValue < MOAIRubyObject* >( self->SetShader ( state, 1 ));
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatch::_setTexture ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatch, "U" )
	return state.ToRValue < MOAIRubyObject* >( self->SetTexture ( state, 1 ));
}

//================================================================//
// MOAIMaterialBatch
//================================================================//

//----------------------------------------------------------------//
MOAIMaterial& MOAIMaterialBatch::AffirmMaterial ( u32 idx ) {

	this->mMaterials.Grow ( idx + 1 );
	return this->mMaterials [ idx ];
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::Clear () {

	this->mMaterials.Clear ();
}

//----------------------------------------------------------------//
mrb_value MOAIMaterialBatch::GetBlendMode ( MOAIRubyState& state, int idx ) {

	MOAIMaterial* material = this->RawGetMaterial ( state.GetParamValue < u32 >( idx, 1 ) - 1 );
	if ( material ) {
		return material->mBlendMode.Push ( state );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIMaterialBatch::GetCullMode ( MOAIRubyState& state, int idx ) {

	MOAIMaterial* material = this->RawGetMaterial ( state.GetParamValue < u32 >( idx, 1 ) - 1 );
	if ( material ) {
		return state.ToRValue ( material->mCullMode );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIMaterialBatch::GetDepthMask ( MOAIRubyState& state, int idx ) {

	MOAIMaterial* material = this->RawGetMaterial ( state.GetParamValue < u32 >( idx, 1 ) - 1 );
	if ( material ) {
		return state.ToRValue ( material->mDepthMask );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIMaterialBatch::GetDepthTest ( MOAIRubyState& state, int idx ) {

	MOAIMaterial* material = this->RawGetMaterial ( state.GetParamValue < u32 >( idx, 1 ) - 1 );
	if ( material ) {
		return state.ToRValue ( material->mDepthTest );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIMaterialBatch::GetLight ( MOAIRubyState& state, int idx ) {

	u32 name;
	u32 materialID = MOAIMaterialBatch::GetNamedGlobalID ( state, idx, name );

	MOAIMaterial* material = this->RawGetMaterial ( materialID );
	return state.ToRValue < MOAIRubyObject* >( ( MOAILight* )material->GetLight ( name ) );
}

//----------------------------------------------------------------//
MOAIMaterial* MOAIMaterialBatch::GetMaterial ( u32 idx ) {

	return this->RawGetMaterial ( this->GetRawIndex ( idx ));
}

//----------------------------------------------------------------//
u32 MOAIMaterialBatch::GetMaterialID ( MOAIRubyState& state, int& idx ) {

	//if ( state.ParamIsType ( idx, MRB_TT_FIXNUM ) && ( state.AbsIndex ( idx ) < state.GetTop ())) {
	if ( state.ParamIsType ( idx, MRB_TT_FIXNUM ) ) {
		return state.GetParamValue < u32 >( idx++, 1 ) - 1;
	}
	return 0;
}

//----------------------------------------------------------------//
u32 MOAIMaterialBatch::GetMaterialID ( MOAIRubyState& state, int& idx, bool& set ) {

	set = false;
	
	if ( state.ParamIsType ( idx, MRB_TT_FIXNUM )) {
	
		u32 value = state.GetParamValue < u32 >( idx, 0 );
		
		if (( value == 0 ) || ZLGfx::IsFlag ( value )) {
			set = true;
			return 0;
		}
		idx++;
		set = state.IsNil ( state.GetParamValue ( idx ) );
		return value - 1;
	}
	set = state.IsNil ( state.GetParamValue ( idx ) );
	return 0;
}

//----------------------------------------------------------------//
u32 MOAIMaterialBatch::GetNamedGlobalID ( MOAIRubyState& state, int& idx, u32& name ) {

	u32 materialID		= 0;
	name				= MOAI_UNKNOWN_MATERIAL_GLOBAL;

	if ( state.ParamIsType ( idx, MRB_TT_FIXNUM )) {
	
		if ( state.ParamIsType ( idx + 1, MRB_TT_FIXNUM )) {
	
			materialID		= state.GetParamValue < u32 >( idx++, 1 ) - 1;
			name			= state.GetParamValue < u32 >( idx++, name  + 1 ) - 1;
		}
		else {
			
			materialID		= state.GetParamValue < u32 >( idx++, name + 1 ) - 1;
		}
	}
	return materialID;
}

//----------------------------------------------------------------//
mrb_value MOAIMaterialBatch::GetShader ( MOAIRubyState& state, int idx ) {

	MOAIMaterial* material = this->RawGetMaterial ( state.GetParamValue < u32 >( idx, 1 ) - 1 );
	if ( material && material->mShader ) {
		return state.ToRValue < MOAIRubyObject* >( ( MOAIShader* )material->mShader );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIMaterialBatch::GetTexture ( MOAIRubyState& state, int idx ) {
	
	u32 name;
	u32 materialID = MOAIMaterialBatch::GetNamedGlobalID ( state, idx, name );

	MOAIMaterial* material = this->RawGetMaterial ( materialID );
	return state.ToRValue < MOAIRubyObject* >( ( MOAITexture* )material->GetTexture ( name ) );
}

//----------------------------------------------------------------//
MOAIMaterialBatch::MOAIMaterialBatch () :
	mIndexBatchSize ( 1 ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIMaterialBatch::~MOAIMaterialBatch () {

	this->Clear ();
}

//----------------------------------------------------------------//
MOAIMaterial* MOAIMaterialBatch::RawGetMaterial ( u32 idx ) {

	size_t size = this->mMaterials.Size ();
	return size ? &this->mMaterials [ idx % size ] : 0;
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "getBlendMode", _getBlendMode, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getCullMode", _getCullMode, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getDepthMask", _getDepthMask, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getDepthTest", _getDepthTest, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getLight", _getLight, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getShader", _getShader, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getTexture", _getTexture, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "reserveMaterials", _reserveMaterials, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setBlendMode", _setBlendMode, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setCullMode", _setCullMode, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setDepthMask", _setDepthMask, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setDepthTest", _setDepthTest, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setIndexBatchSize", _setIndexBatchSize, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setLight", _setLight, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setShader", _setShader, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setTexture", _setTexture, MRB_ARGS_NONE () );
	
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::Reserve ( u32 n ) {

	assert ( n < ZGL_FIRST_FLAG ); // probably don't need more than 0x70000000 materials...

	this->Clear ();
	this->mMaterials.Init ( n );
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetBlendMode ( u32 idx ) {

	if ( idx < this->mMaterials.Size ()) {
		this->mMaterials [ idx ].SetBlendMode ();
	}
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetBlendMode ( u32 idx, const MOAIBlendMode& blendMode ) {

	this->AffirmMaterial ( idx ).SetBlendMode ( blendMode );
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetBlendMode ( MOAIRubyState& state, int idx ) {

	bool set;
	u32 materialID = MOAIMaterialBatch::GetMaterialID ( state, idx, set );

	if ( set ) {
		MOAIBlendMode blendMode;
		blendMode.Init ( state, idx );
		this->SetBlendMode ( materialID, blendMode );
	}
	else {
		this->SetBlendMode ( materialID );
	}
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetCullMode ( u32 idx ) {

	if ( idx < this->mMaterials.Size ()) {
		this->mMaterials [ idx ].SetCullMode ();
	}
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetCullMode ( u32 idx, int cullMode ) {

	this->AffirmMaterial ( idx ).SetCullMode ( cullMode );
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetCullMode ( MOAIRubyState& state, int idx ) {

	bool set;
	u32 materialID = MOAIMaterialBatch::GetMaterialID ( state, idx, set );
	
	if ( set ) {
		this->SetCullMode ( materialID, state.GetParamValue < u32 >( idx, 0 ));
	}
	else {
		this->SetCullMode ( materialID );
	}
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetDepthMask ( u32 idx ) {

	if ( idx < this->mMaterials.Size ()) {
		this->mMaterials [ idx ].SetDepthMask ();
	}
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetDepthMask ( u32 idx, bool depthMask ) {

	this->AffirmMaterial ( idx ).SetDepthMask ( depthMask );
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetDepthMask ( MOAIRubyState& state, int idx ) {

	bool set;
	u32 materialID = MOAIMaterialBatch::GetMaterialID ( state, idx, set );
	
	if ( set ) {
		this->SetDepthMask ( materialID, state.GetParamValue < bool >( idx, false ));
	}
	else {
		this->SetDepthMask ( materialID );
	}
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetDepthTest ( u32 idx ) {

	if ( idx < this->mMaterials.Size ()) {
		this->mMaterials [ idx ].SetDepthTest ();
	}
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetDepthTest ( u32 idx, int depthTest ) {

	this->AffirmMaterial ( idx ).SetDepthTest ( depthTest );
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetDepthTest ( MOAIRubyState& state, int idx ) {

	bool set;
	u32 materialID = MOAIMaterialBatch::GetMaterialID ( state, idx, set );
	
	if ( set ) {
		this->SetDepthTest ( materialID, state.GetParamValue < u32 >( idx, 0 ));
	}
	else {
		this->SetDepthTest ( materialID );
	}
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetLight ( u32 idx, u32 name ) {

	if ( idx < this->mMaterials.Size ()) {
		this->mMaterials [ idx ].SetLight ( name );
	}
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetLight ( u32 idx, u32 name, MOAILight* light ) {

	this->AffirmMaterial ( idx ).SetLight ( name, light );
}

//----------------------------------------------------------------//
MOAILight* MOAIMaterialBatch::SetLight ( MOAIRubyState& state, int idx ) {

	u32 name;
	u32 materialID = MOAIMaterialBatch::GetNamedGlobalID ( state, idx, name );

	MOAILight* light = state.GetRubyObject < MOAILight >( idx, true );
	
	this->SetLight ( materialID, name, light );
	return light;
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetShader ( u32 idx ) {

	if ( idx < this->mMaterials.Size ()) {
		this->mMaterials [ idx ].SetShader ();
	}
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetShader ( u32 idx, u32 shaderID ) {

	this->SetShader ( idx, MOAIShaderMgr::Get ().GetShader ( shaderID ));
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetShader ( u32 idx, MOAIShader* shader ) {

	this->AffirmMaterial ( idx ).SetShader ( shader );
}

//----------------------------------------------------------------//
MOAIShader* MOAIMaterialBatch::SetShader ( MOAIRubyState& state, int idx ) {

	u32 materialID = MOAIMaterialBatch::GetMaterialID ( state, idx );
	MOAIShader* shader = MOAIShader::AffirmShader ( state, idx );
	this->SetShader ( materialID, shader );
	return shader;
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetTexture ( u32 idx ) {

	if ( idx < this->mMaterials.Size ()) {
		this->mMaterials [ idx ].SetTexture ();
	}
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetTexture ( u32 idx, MOAITextureBase* texture ) {

	this->AffirmMaterial ( idx ).SetTexture ( texture );
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetTexture ( u32 idx, u32 name ) {

	if ( idx < this->mMaterials.Size ()) {
		this->mMaterials [ idx ].SetTexture ( name );
	}
}

//----------------------------------------------------------------//
void MOAIMaterialBatch::SetTexture ( u32 idx, u32 name, MOAITextureBase* texture ) {

	this->AffirmMaterial ( idx ).SetTexture ( name, texture );
}

//----------------------------------------------------------------//
MOAITextureBase* MOAIMaterialBatch::SetTexture ( MOAIRubyState& state, int idx ) {
	
	u32 name;
	u32 materialID = MOAIMaterialBatch::GetNamedGlobalID ( state, idx, name );

	MOAITextureBase* texture = MOAITexture::AffirmTexture ( state, idx );
	if ( name != MOAI_UNKNOWN_MATERIAL_GLOBAL ) {
		this->SetTexture ( materialID, name, texture );
	}
	else {
		this->SetTexture ( materialID, texture );
	}
	return texture;
}

//----------------------------------------------------------------//
size_t MOAIMaterialBatch::Size () {

	return this->mMaterials.Size ();
}
