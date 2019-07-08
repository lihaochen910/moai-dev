// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <moai-sim/MOAIMaterialBatch.h>
#include <moai-sim/MOAIMaterialBatchHolder.h>


//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatchHolder::_getBlendMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )
	return self->mMaterialBatch ? self->mMaterialBatch->GetBlendMode ( state, 1 ) : mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatchHolder::_getCullMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )
	return self->mMaterialBatch ? self->mMaterialBatch->GetCullMode ( state, 1 ) : mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatchHolder::_getDepthMask ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )
	return self->mMaterialBatch ? self->mMaterialBatch->GetDepthMask ( state, 1 ) : mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatchHolder::_getDepthTest ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )
	return self->mMaterialBatch ? self->mMaterialBatch->GetDepthTest ( state, 1 ) : mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatchHolder::_getLight ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )
	return self->mMaterialBatch ? self->mMaterialBatch->GetLight ( state, 1 ) : mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@name	getMaterialBatch
	@text	Return the material batch attached to the prop.
	
	@in		MOAIMaterialBatchHolder self
	@out	MOAIMaterialBatch materialBatch
*/
mrb_value MOAIMaterialBatchHolder::_getMaterialBatch ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )
	
	return state.ToRValue < MOAIRubyObject* >( self->mMaterialBatch );
}

//----------------------------------------------------------------//
/**	@name	getShader
	@text	Get the shader at the given index in the prop's material batch,
			ignoring the material's index batch size. If no material batch is
			attached to the prop then nil will be returned.
	
	@in		MOAIMaterialBatchHolder self
	@opt	number idx				Default value is 1.
	@out	MOAIShader shader		Or nil if none exists.
*/
mrb_value MOAIMaterialBatchHolder::_getShader ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )
	
	return self->mMaterialBatch ? self->mMaterialBatch->GetShader ( state, 1 ) : mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@name	getTexture
	@text	Get the texture at the given index in the prop's material batch,
			ignoring the material's index batch size. If no material batch is
			attached to the prop then nil will be returned.
	
	@in		MOAIMaterialBatchHolder self
	@opt	number idx				Default value is 1.
	@out	MOAITexture texture		Or nil if none exists.
*/
mrb_value MOAIMaterialBatchHolder::_getTexture ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )
	
	return self->mMaterialBatch ? self->mMaterialBatch->GetTexture ( state, 1 ) : mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	reserveMaterials
	@text	Reserve materials in the prop's material batch (and creates a
			material batch if none exists).
	
	@in		MOAIMaterialBatchHolder self
	@in		number count
	@out	nil
*/
mrb_value MOAIMaterialBatchHolder::_reserveMaterials ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )
	
	self->AffirmMaterialBatch ()->Reserve ( state.GetParamValue < u32 >( 1, 0 ));
	
	return context;
}

//----------------------------------------------------------------//
/** @lua	Set blend mode using OpenGL source and dest factors. OpenGl blend factor constants are exposed as members of MOAIMaterialBatchHolder.
			See the OpenGL documentation for an explanation of blending constants.

	@in		MOAIMaterialBatchHolder self
	@in		number srcFactor
	@in		number dstFactor
	@out	nil
*/
mrb_value MOAIMaterialBatchHolder::_setBlendMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )

	self->AffirmMaterialBatch ()->SetBlendMode ( state, 1 );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setCullMode
	@text	Sets and enables face culling.
	
	@in		MOAIMaterialBatchHolder self
	@opt	number cullMode			Default value is MOAIMaterialBatchHolder.CULL_NONE.
	@out	nil
*/
mrb_value MOAIMaterialBatchHolder::_setCullMode ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )
	
	self->AffirmMaterialBatch ()->SetCullMode ( state, 1 );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setDepthMask
	@text	Disables or enables depth writing.
	
	@in		MOAIMaterialBatchHolder self
	@opt	boolean depthMask		Default value is true.
	@out	nil
*/
mrb_value MOAIMaterialBatchHolder::_setDepthMask ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )
	
	self->AffirmMaterialBatch ()->SetDepthMask ( state, 1 );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setDepthTest
	@text	Sets and enables depth testing (assuming depth buffer is present).
	
	@in		MOAIMaterialBatchHolder self
	@opt	number depthFunc		Default value is MOAIMaterialBatchHolder.DEPTH_TEST_DISABLE.
	@out	nil
*/
mrb_value MOAIMaterialBatchHolder::_setDepthTest ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )
	
	self->AffirmMaterialBatch ()->SetDepthTest ( state, 1 );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setIndexBatchSize
	@text	Sets and index batch size of the associated matrial batch (and creates
			a material batch if none exists).
	
	@in		MOAIMaterialBatchHolder self
	@opt	number indexBatchSize
	@out	nil
*/
mrb_value MOAIMaterialBatchHolder::_setIndexBatchSize ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )

	self->AffirmMaterialBatch ()->SetIndexBatchSize ( state.GetParamValue < u32 >( 1, 1 ));
	
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMaterialBatchHolder::_setLight ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )
	
	return state.ToRValue < MOAIRubyObject* >( self->AffirmMaterialBatch ()->SetLight ( state, 1 ) );
}

//----------------------------------------------------------------//
/**	@lua	setMaterialBatch
	@text	Sets the prop's material batch.
	
	@in		MOAIMaterialBatchHolder self
	@opt	MOAIMaterialBatch materialBatch
	@out	nil
*/
mrb_value MOAIMaterialBatchHolder::_setMaterialBatch ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )
	
	self->mMaterialBatch.Set ( *self, state.GetRubyObject < MOAIMaterialBatch >( 1, true ));
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setShader
	@text	Sets a shader in the associated material batch. Creates a
			material batch if none exists. Index batch size is ignored.
			
			If no value for 'idx' is provided, then the shader or shader
			preset is expected as the first paramater, and idx defaults to 1.
	
	@overload
	
		@in		MOAIMaterialBatchHolder self
		@in		number idx
		@in		variant shader			Overloaded to accept a MOAIShader or a shader preset.
		@out	MOAIShader shader		The shader that was set or created.
	
	@overload
	
		@in		MOAIMaterialBatchHolder self
		@in		variant shader			Overloaded to accept a MOAIShader or a shader preset.
		@out	MOAIShader shader		The shader that was set or created.
*/
mrb_value MOAIMaterialBatchHolder::_setShader ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )
	
	return state.ToRValue < MOAIRubyObject* >( self->AffirmMaterialBatch ()->SetShader ( state, 1 ) );
}

//----------------------------------------------------------------//
/**	@lua	setTexture
	@text	Sets a texture in the associated material batch. Creates a
			material batch is none exists. Index batch size is ignored.
			
			If no value for 'idx' is provided, then the texture or filename
			is expected as the first paramater, and idx defaults to 1.
	
	@overload
	
		@in		MOAIMaterialBatchHolder self
		@in		number idx
		@in		variant texture			Overloaded to accept a filename, MOAITexture, MOAIImage, MOAIStream or MOAIDataBuffer.
		@out	MOAITexture texture		The texture that was set or created.
	
	@overload
	
		@in		MOAIMaterialBatchHolder self
		@in		variant texture			Overloaded to accept a filename, MOAITexture, MOAIImage, MOAIStream or MOAIDataBuffer.
		@out	MOAITexture texture		The texture that was set or created.
*/
mrb_value MOAIMaterialBatchHolder::_setTexture ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMaterialBatchHolder, "U" )
	
	return state.ToRValue < MOAIRubyObject* >( self->AffirmMaterialBatch ()->SetTexture ( state, 1 ) );
}

//================================================================//
// MOAIMaterialBatchHolder
//================================================================//

//----------------------------------------------------------------//
MOAIMaterialBatch* MOAIMaterialBatchHolder::AffirmMaterialBatch () {

	if ( !this->mMaterialBatch ) {
		//this->mMaterialBatch.Set ( *this, new MOAIMaterialBatch );
		MOAIRubyState& state = MOAIRubyRuntime::Get ().GetMainState ();
		this->mMaterialBatch.Set ( *this, state.CreateClassInstance < MOAIMaterialBatch >() );
	}
	return this->mMaterialBatch;
}

//----------------------------------------------------------------//
MOAIMaterial* MOAIMaterialBatchHolder::GetMaterial ( u32 idx ) {

	return this->mMaterialBatch ? this->mMaterialBatch->GetMaterial ( idx ) : 0;
}

//----------------------------------------------------------------//
MOAIMaterialBatchHolder::MOAIMaterialBatchHolder () {

	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIMaterialBatchHolder::~MOAIMaterialBatchHolder () {

	this->mMaterialBatch.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAIMaterialBatchHolder::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIMaterialBatchHolder::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "getBlendMode", _getBlendMode, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getCullMode", _getCullMode, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getDepthMask", _getDepthMask, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getDepthTest", _getDepthTest, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getLight", _getLight, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getMaterialBatch", _getMaterialBatch, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getShader", _getShader, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getTexture", _getTexture, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "reserveMaterials", _reserveMaterials, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setBlendMode", _setBlendMode, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "setCullMode", _setCullMode, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setDepthMask", _setDepthMask, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setDepthTest", _setDepthTest, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setIndexBatchSize", _setIndexBatchSize, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setLight", _setLight, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setMaterialBatch", _setMaterialBatch, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setShader", _setShader, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setTexture", _setTexture, MRB_ARGS_ANY () );

}

//----------------------------------------------------------------//
void MOAIMaterialBatchHolder::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAIMaterialBatchHolder::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}
