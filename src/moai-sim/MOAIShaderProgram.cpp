// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIColor.h>
#include <moai-sim/MOAIEaseDriver.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGfxResourceClerk.h>
#include <moai-sim/MOAIMaterialMgr.h>
#include <moai-sim/MOAIShaderProgram.h>
#include <moai-sim/MOAITransformBase.h>

//================================================================//
// MOAIShaderProgramGlobal
//================================================================//
	
//----------------------------------------------------------------//
MOAIShaderProgramGlobal::MOAIShaderProgramGlobal () :
	mGlobalID ( INVALID_INDEX ),
	mUniformID ( INVALID_INDEX ),
	mIndex ( 0 ) {
}

//================================================================//
// MOAIShaderProgramTexture
//================================================================//

//----------------------------------------------------------------//
MOAIShaderProgramTexture::MOAIShaderProgramTexture () :
	mName ( MOAI_UNKNOWN_MATERIAL_GLOBAL ),
	mUnit ( 0 ) {
}

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	declareUniform
	@text	Declares a uniform mapping.

	@in		MOAIShaderProgram self
	@in		number idx
	@in		string name
	@opt	number type		One of MOAIShaderProgram.UNIFORM_TYPE_FLOAT, MOAIShaderProgram.UNIFORM_TYPE_INDEX, MOAIShaderProgram.UNIFORM_TYPE_INT,
							MOAIShaderProgram.UNIFORM_TYPE_MATRIX_3X3, MOAIShaderProgram.UNIFORM_TYPE_MATRIX_4X4
	@opt	number width	Used for vector uniforms. Default value is 1. Should be no greather than 4.
	@opt	number count	Declare an array of uniforms. Default value is 1.
	@out	nil
*/
mrb_value MOAIShaderProgram::_declareUniform ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIShaderProgram, "UNSN" )

	u32 idx				= state.GetParamValue < u32 >( 1, 1 ) - 1;
	STLString name		= state.GetParamValue < cc8* >( 2, "" );
	u32 type			= state.GetParamValue < u32 >( 3, MOAIShaderUniform::UNIFORM_TYPE_FLOAT );
	u32 width			= state.GetParamValue < u32 >( 4, 1 );
	u32 count			= state.GetParamValue < u32 >( 5, 1 );

	self->DeclareUniform ( idx, name, type, width, count );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	load
	@text	Load a shader program.

	@in		MOAIShaderProgram self
	@in		string vertexShaderSource
	@in		string fragmentShaderSource
	@out	nil
*/
mrb_value MOAIShaderProgram::_load ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIShaderProgram, "USS" )

	cc8* vtxSource	= state.GetParamValue < cc8* >( 1, 0 );
	cc8* frgSource	= state.GetParamValue < cc8* >( 2, 0 );

	self->Load ( vtxSource, frgSource );

	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIShaderProgram::_reserveGlobals ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIShaderProgram, "U" )

	return self->ReserveGlobals ( M, 1 );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIShaderProgram::_reserveTextures ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIShaderProgram, "U" )

	self->ReserveTextures ( state.GetParamValue < u32 >( 2, 0 ));
	return context;
}

//----------------------------------------------------------------//
/**	@lua	reserveUniforms
	@text	Reserve shader uniforms.

	@in		MOAIShaderProgram self
	@opt	number nUniforms	Default value is 0.
	@out	nil
*/
mrb_value MOAIShaderProgram::_reserveUniforms ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIShaderProgram, "U" )

	u32 nUniforms = state.GetParamValue < u32 >( 1, 0 );
	self->ReserveUniforms ( nUniforms );

	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIShaderProgram::_setGlobal ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIShaderProgram, "UNNN" )
	
	return self->SetGlobal ( M, 1 );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIShaderProgram::_setTexture ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIShaderProgram, "U" )
	
	u32 idx = state.GetParamValue < u32 >( 1, 1 ) - 1;
	u32 unit = state.GetParamValue < u32 >( 3, 1 ) - 1;
	
	if ( state.ParamIsType ( 2, MRB_TT_DATA )) {
	
		self->SetTexture ( idx, state.GetRubyObject < MOAITextureBase >( 2, true ), unit );
	}
	else {
	
		self->SetTexture (
			idx,
			state.GetParamValue < u32 >( 2, MOAI_UNKNOWN_MATERIAL_GLOBAL + 1 ) - 1,
			unit,
			state.GetRubyObject < MOAITextureBase >( 4, false )
		);
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setVertexAttribute
	@text	Names a shader vertex attribute.

	@in		MOAIShaderProgram self
	@in		number index	Default value is 1.
	@in		string name		Name of attribute.
	@out	nil
*/
mrb_value MOAIShaderProgram::_setVertexAttribute ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIShaderProgram, "UNS" )

	u32 idx					= state.GetParamValue < u32 >( 1, 1 ) - 1;
	STLString attribute		= state.GetParamValue < cc8* >( 2, "" );

	self->SetVertexAttribute ( idx, attribute );

	return context;
}

//================================================================//
// MOAIShaderProgram
//================================================================//

//----------------------------------------------------------------//
void MOAIShaderProgram::AffirmUniforms () {

	if ( this->mUniformBufferSize ) return;
	
	this->mUniformBufferSize = 0;
	size_t nUniforms = this->mUniforms.Size ();
	
	for ( size_t i = 0; i < nUniforms; ++i ) {
		
		MOAIShaderUniform& uniform = this->mUniforms [ i ];
		uniform.mCPUOffset = this->mUniformBufferSize;
		this->mUniformBufferSize += uniform.GetSize ();
		
		this->mMaxCount = this->mMaxCount < uniform.mCount ? uniform.mCount : this->mMaxCount;
	}
	
	this->mUniformBuffer.Clear ();
	this->mUniformBuffer.Init ( this->mUniformBufferSize );
	this->mUniformBuffer.Fill ( 0xff );
}

//----------------------------------------------------------------//
void MOAIShaderProgram::ApplyUniforms ( ZLLeanArray < u8 >& buffer ) {

	this->mUniformBuffer.CopyFrom ( buffer);
}

//----------------------------------------------------------------//
void MOAIShaderProgram::BindUniforms () {
	
	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	size_t nUniforms = this->mUniforms.Size ();
	
	for ( u32 i = 0; i < nUniforms; ++i ) {
	
		MOAIShaderUniformHandle uniform = this->GetUniformHandle ( this->mUniformBuffer, i );
		
		if ( uniform.IsValid ()) {
			this->mUniforms [ i ].Bind ( uniform.mBuffer );
		}
	}
}

//----------------------------------------------------------------//
void MOAIShaderProgram::Clear () {

	this->mVertexShaderSource.clear ();
	this->mFragmentShaderSource.clear ();

	this->mAttributeMap.clear ();
	this->mUniforms.Clear ();
	this->mGlobals.Clear ();
	
	this->Destroy ();
}

//----------------------------------------------------------------//
ZLGfxHandle MOAIShaderProgram::CompileShader ( u32 type, cc8* source ) {

	MOAIGfxMgr& gfxMgr = MOAIGfxMgr::Get ();
	ZLGfx& gfx = gfxMgr.GetDrawingAPI ();

	ZLGfxHandle shader = gfx.CreateShader ( type );

	STLString buffer;

	buffer.append ( gfxMgr.IsOpenGLES () ? OPENGL_ES_PREPROC : OPENGL_PREPROC );
	if (( type == ZGL_SHADER_TYPE_FRAGMENT ) && gfxMgr.IsOpenGLES ()) {
		buffer.append ( WEBGL_PREPROC );
	}

	buffer.append ( source );

	gfx.PushSection ();

	gfx.ShaderSource ( shader, buffer.c_str (), buffer.size ());
	gfx.CompileShader ( shader, true );

	if ( gfx.PushErrorHandler ()) {
		gfx.DeleteResource ( shader );
	}

	gfx.PopSection ();

	return shader;
}

//----------------------------------------------------------------//
void MOAIShaderProgram::DeclareUniform ( u32 idx, cc8* name, u32 type, u32 width, u32 count ) {

	if ( idx < this->mUniforms.Size ()) {

		MOAIShaderUniform& uniform = this->mUniforms [ idx ];
		uniform.mName = name;
		uniform.Init ( type, width, count );
	}
}

//----------------------------------------------------------------//
MOAIShaderUniform* MOAIShaderProgram::GetUniform ( u32 uniformID ) {

	return uniformID < this->mUniforms.Size () ? &this->mUniforms [ uniformID ] : 0;
}

//----------------------------------------------------------------//
void MOAIShaderProgram::InitUniformBuffer ( ZLLeanArray < u8 >& buffer ) {

	this->AffirmUniforms ();

	buffer.Clear ();
	
	size_t nUniforms = this->mUniforms.Size ();
	buffer.Init ( this->mUniformBufferSize );
	
	for ( size_t i = 0; i < nUniforms; ++i ) {
		MOAIShaderUniformHandle uniform = this->GetUniformHandle ( buffer, i );
		uniform.Default ( this->mUniforms [ i ].mCount );
	}
}

//----------------------------------------------------------------//
void MOAIShaderProgram::Load ( cc8* vshSource, cc8* fshSource ) {

	if ( vshSource && fshSource ) {

		this->mVertexShaderSource = vshSource;
		this->mFragmentShaderSource = fshSource;
		
		this->FinishInit ();
	}
}

//----------------------------------------------------------------//
MOAIShaderProgram::MOAIShaderProgram () :
	mMaxCount ( 0 ),
	mUniformBufferSize ( 0 ) {

	RTTI_BEGIN
		RTTI_EXTEND ( MOAIGfxResource )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIShaderProgram::~MOAIShaderProgram () {

	this->Clear ();
}

//----------------------------------------------------------------//
bool MOAIShaderProgram::OnCPUCreate () {

	return true;
}

//----------------------------------------------------------------//
void MOAIShaderProgram::OnCPUDestroy () {
}

//----------------------------------------------------------------//
void MOAIShaderProgram::OnGPUBind () {

	// use shader program.
	MOAIGfxMgr::GetDrawingAPI ().UseProgram ( this->mProgram );
}

//----------------------------------------------------------------//
bool MOAIShaderProgram::OnGPUCreate () {

	ZLGfx& gfx = MOAIGfxMgr::GetDrawingAPI ();

	this->mVertexShader = this->CompileShader ( ZGL_SHADER_TYPE_VERTEX, this->mVertexShaderSource );
	this->mFragmentShader = this->CompileShader ( ZGL_SHADER_TYPE_FRAGMENT, this->mFragmentShaderSource );
	this->mProgram = gfx.CreateProgram ();

	// TODO: error handling
//	if ( !( this->mVertexShader && this->mFragmentShader && this->mProgram )) {
//		this->Clear ();
//		return false;
//	}

	gfx.AttachShader ( this->mProgram, this->mVertexShader );
	gfx.AttachShader ( this->mProgram, this->mFragmentShader );

	// bind attribute locations.
	// this needs to be done prior to linking.
	AttributeMapIt attrMapIt = this->mAttributeMap.begin ();
	for ( ; attrMapIt != this->mAttributeMap.end (); ++attrMapIt ) {
		gfx.BindAttribLocation ( this->mProgram, attrMapIt->first, attrMapIt->second.str ());
	}

	gfx.LinkProgram ( this->mProgram, true );

	// get the uniform locations
	for ( u32 i = 0; i < this->mUniforms.Size (); ++i ) {
		MOAIShaderUniform& uniform = this->mUniforms [ i ];
		gfx.GetUniformLocation ( this->mProgram, uniform.mName, this, ( void* )(( size_t )i )); // TODO: cast?
	}

	gfx.DeleteResource ( this->mVertexShader );
	gfx.DeleteResource ( this->mFragmentShader );

	//AJV TODO - does the attribute map ever need to be cleared?
	//this->mAttributeMap.clear ();
	
	return true;
}

//----------------------------------------------------------------//
void MOAIShaderProgram::OnGPUDeleteOrDiscard ( bool shouldDelete ) {

	MOAIGfxResourceClerk::DeleteOrDiscard ( this->mVertexShader, shouldDelete );
	MOAIGfxResourceClerk::DeleteOrDiscard ( this->mFragmentShader, shouldDelete );
	MOAIGfxResourceClerk::DeleteOrDiscard ( this->mProgram, shouldDelete );
}

//----------------------------------------------------------------//
void MOAIShaderProgram::OnGPUUnbind () {

	MOAIGfxMgr::GetDrawingAPI ().UseProgram ( ZLGfxResource::UNBIND );
}

//----------------------------------------------------------------//
bool MOAIShaderProgram::OnGPUUpdate () {

	return true;
}

//----------------------------------------------------------------//
void MOAIShaderProgram::OnUniformLocation ( u32 addr, void* userdata ) {

	size_t i = ( size_t )userdata;
	
	if ( i < this->mUniforms.Size ()) {
		this->mUniforms [ i ].mGPUBase = addr;
	}
}

//----------------------------------------------------------------//
void MOAIShaderProgram::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAIGfxResource::RegisterRubyClass ( state, klass );

	state.DefineClassConst ( klass, "UNIFORM_TYPE_FLOAT",						( u32 )MOAIShaderUniform::UNIFORM_TYPE_FLOAT );
	state.DefineClassConst ( klass, "UNIFORM_TYPE_INT",						( u32 )MOAIShaderUniform::UNIFORM_TYPE_INT );
	
	state.DefineClassConst ( klass, "UNIFORM_WIDTH_VEC_2",						( u32 )MOAIShaderUniform::UNIFORM_WIDTH_VEC_2 );
	state.DefineClassConst ( klass, "UNIFORM_WIDTH_VEC_3",						( u32 )MOAIShaderUniform::UNIFORM_WIDTH_VEC_3 );
	state.DefineClassConst ( klass, "UNIFORM_WIDTH_VEC_4",						( u32 )MOAIShaderUniform::UNIFORM_WIDTH_VEC_4 );
	state.DefineClassConst ( klass, "UNIFORM_WIDTH_MATRIX_3X3",				( u32 )MOAIShaderUniform::UNIFORM_WIDTH_MATRIX_3X3 );
	state.DefineClassConst ( klass, "UNIFORM_WIDTH_MATRIX_4X4",				( u32 )MOAIShaderUniform::UNIFORM_WIDTH_MATRIX_4X4 );
	
	state.DefineClassConst ( klass, "GLOBAL_CLIP_TO_DISPLAY_MTX",				( u32 )MOAIGfxState::CLIP_TO_DISPLAY_MTX );
	state.DefineClassConst ( klass, "GLOBAL_CLIP_TO_MODEL_MTX",				( u32 )MOAIGfxState::CLIP_TO_MODEL_MTX );
	state.DefineClassConst ( klass, "GLOBAL_CLIP_TO_VIEW_MTX",					( u32 )MOAIGfxState::CLIP_TO_VIEW_MTX );
	state.DefineClassConst ( klass, "GLOBAL_CLIP_TO_WINDOW_MTX",				( u32 )MOAIGfxState::CLIP_TO_WINDOW_MTX );
	state.DefineClassConst ( klass, "GLOBAL_CLIP_TO_WORLD_MTX",				( u32 )MOAIGfxState::CLIP_TO_WORLD_MTX );

	state.DefineClassConst ( klass, "GLOBAL_DISPLAY_TO_CLIP_MTX",				( u32 )MOAIGfxState::DISPLAY_TO_CLIP_MTX );
	state.DefineClassConst ( klass, "GLOBAL_DISPLAY_TO_MODEL_MTX",				( u32 )MOAIGfxState::DISPLAY_TO_MODEL_MTX );
	state.DefineClassConst ( klass, "GLOBAL_DISPLAY_TO_VIEW_MTX",				( u32 )MOAIGfxState::DISPLAY_TO_VIEW_MTX );
	state.DefineClassConst ( klass, "GLOBAL_DISPLAY_TO_WORLD_MTX",				( u32 )MOAIGfxState::DISPLAY_TO_WORLD_MTX );

	state.DefineClassConst ( klass, "GLOBAL_MODEL_TO_CLIP_MTX",				( u32 )MOAIGfxState::MODEL_TO_CLIP_MTX );
	state.DefineClassConst ( klass, "GLOBAL_MODEL_TO_DISPLAY_MTX",				( u32 )MOAIGfxState::MODEL_TO_DISPLAY_MTX );
	state.DefineClassConst ( klass, "GLOBAL_MODEL_TO_UV_MTX",					( u32 )MOAIGfxState::MODEL_TO_UV_MTX );
	state.DefineClassConst ( klass, "GLOBAL_MODEL_TO_VIEW_MTX",				( u32 )MOAIGfxState::MODEL_TO_VIEW_MTX );
	state.DefineClassConst ( klass, "GLOBAL_MODEL_TO_WORLD_MTX",				( u32 )MOAIGfxState::MODEL_TO_WORLD_MTX );

	state.DefineClassConst ( klass, "GLOBAL_NORMAL_CLIP_TO_DISPLAY_MTX",		( u32 )MOAIGfxState::NORMAL_CLIP_TO_DISPLAY_MTX );
	state.DefineClassConst ( klass, "GLOBAL_NORMAL_CLIP_TO_MODEL_MTX",			( u32 )MOAIGfxState::NORMAL_CLIP_TO_MODEL_MTX );
	state.DefineClassConst ( klass, "GLOBAL_NORMAL_CLIP_TO_VIEW_MTX",			( u32 )MOAIGfxState::NORMAL_CLIP_TO_VIEW_MTX );
	state.DefineClassConst ( klass, "GLOBAL_NORMAL_CLIP_TO_WINDOW_MTX",		( u32 )MOAIGfxState::NORMAL_CLIP_TO_WINDOW_MTX );
	state.DefineClassConst ( klass, "GLOBAL_NORMAL_CLIP_TO_WORLD_MTX",			( u32 )MOAIGfxState::NORMAL_CLIP_TO_WORLD_MTX );

	state.DefineClassConst ( klass, "GLOBAL_NORMAL_DISPLAY_TO_CLIP_MTX",		( u32 )MOAIGfxState::NORMAL_DISPLAY_TO_CLIP_MTX );
	state.DefineClassConst ( klass, "GLOBAL_NORMAL_DISPLAY_TO_MODEL_MTX",		( u32 )MOAIGfxState::NORMAL_DISPLAY_TO_MODEL_MTX );
	state.DefineClassConst ( klass, "GLOBAL_NORMAL_DISPLAY_TO_VIEW_MTX",		( u32 )MOAIGfxState::NORMAL_DISPLAY_TO_VIEW_MTX );
	state.DefineClassConst ( klass, "GLOBAL_NORMAL_DISPLAY_TO_WORLD_MTX",		( u32 )MOAIGfxState::NORMAL_DISPLAY_TO_WORLD_MTX );

	state.DefineClassConst ( klass, "GLOBAL_NORMAL_MODEL_TO_CLIP_MTX",			( u32 )MOAIGfxState::NORMAL_MODEL_TO_CLIP_MTX );
	state.DefineClassConst ( klass, "GLOBAL_NORMAL_MODEL_TO_DISPLAY_MTX",		( u32 )MOAIGfxState::NORMAL_MODEL_TO_DISPLAY_MTX );
	state.DefineClassConst ( klass, "GLOBAL_NORMAL_MODEL_TO_UV_MTX",			( u32 )MOAIGfxState::NORMAL_MODEL_TO_UV_MTX );
	state.DefineClassConst ( klass, "GLOBAL_NORMAL_MODEL_TO_VIEW_MTX",			( u32 )MOAIGfxState::NORMAL_MODEL_TO_VIEW_MTX );
	state.DefineClassConst ( klass, "GLOBAL_NORMAL_MODEL_TO_WORLD_MTX",		( u32 )MOAIGfxState::NORMAL_MODEL_TO_WORLD_MTX );

	state.DefineClassConst ( klass, "GLOBAL_NORMAL_WORLD_TO_DISPLAY_MTX",		( u32 )MOAIGfxState::NORMAL_WORLD_TO_DISPLAY_MTX );
	state.DefineClassConst ( klass, "GLOBAL_NORMAL_WORLD_TO_VIEW_MTX",			( u32 )MOAIGfxState::NORMAL_WORLD_TO_VIEW_MTX );

	state.DefineClassConst ( klass, "GLOBAL_NORMAL_UV_TO_MODEL_MTX",			( u32 )MOAIGfxState::NORMAL_UV_TO_MODEL_MTX );

	state.DefineClassConst ( klass, "GLOBAL_NORMAL_VIEW_TO_CLIP_MTX",			( u32 )MOAIGfxState::NORMAL_VIEW_TO_CLIP_MTX );
	state.DefineClassConst ( klass, "GLOBAL_NORMAL_VIEW_TO_DISPLAY_MTX",		( u32 )MOAIGfxState::NORMAL_VIEW_TO_DISPLAY_MTX );
	state.DefineClassConst ( klass, "GLOBAL_NORMAL_VIEW_TO_MODEL_MTX",			( u32 )MOAIGfxState::NORMAL_VIEW_TO_MODEL_MTX );
	state.DefineClassConst ( klass, "GLOBAL_NORMAL_VIEW_TO_WORLD_MTX",			( u32 )MOAIGfxState::NORMAL_VIEW_TO_WORLD_MTX );

	state.DefineClassConst ( klass, "GLOBAL_NORMAL_WINDOW_TO_CLIP_MTX",		( u32 )MOAIGfxState::NORMAL_WINDOW_TO_CLIP_MTX );
	state.DefineClassConst ( klass, "GLOBAL_NORMAL_WORLD_TO_CLIP_MTX",			( u32 )MOAIGfxState::NORMAL_WORLD_TO_CLIP_MTX );
	state.DefineClassConst ( klass, "GLOBAL_NORMAL_WORLD_TO_MODEL_MTX",		( u32 )MOAIGfxState::NORMAL_WORLD_TO_MODEL_MTX );

	state.DefineClassConst ( klass, "GLOBAL_PEN_COLOR",						( u32 )MOAIGfxState::PEN_COLOR );

	state.DefineClassConst ( klass, "GLOBAL_UV_TO_MODEL_MTX",					( u32 )MOAIGfxState::UV_TO_MODEL_MTX );

	state.DefineClassConst ( klass, "GLOBAL_VIEW_TO_CLIP_MTX",					( u32 )MOAIGfxState::VIEW_TO_CLIP_MTX );
	state.DefineClassConst ( klass, "GLOBAL_VIEW_TO_DISPLAY_MTX",				( u32 )MOAIGfxState::VIEW_TO_DISPLAY_MTX );
	state.DefineClassConst ( klass, "GLOBAL_VIEW_TO_MODEL_MTX",				( u32 )MOAIGfxState::VIEW_TO_MODEL_MTX );
	state.DefineClassConst ( klass, "GLOBAL_VIEW_TO_WORLD_MTX",				( u32 )MOAIGfxState::VIEW_TO_WORLD_MTX );

	state.DefineClassConst ( klass, "GLOBAL_WINDOW_TO_CLIP_MTX",				( u32 )MOAIGfxState::WINDOW_TO_CLIP_MTX );

	state.DefineClassConst ( klass, "GLOBAL_WORLD_TO_CLIP_MTX",				( u32 )MOAIGfxState::WORLD_TO_CLIP_MTX );
	state.DefineClassConst ( klass, "GLOBAL_WORLD_TO_DISPLAY_MTX",				( u32 )MOAIGfxState::WORLD_TO_DISPLAY_MTX );
	state.DefineClassConst ( klass, "GLOBAL_WORLD_TO_MODEL_MTX",				( u32 )MOAIGfxState::WORLD_TO_MODEL_MTX );
	state.DefineClassConst ( klass, "GLOBAL_WORLD_TO_VIEW_MTX",				( u32 )MOAIGfxState::WORLD_TO_VIEW_MTX );

	state.DefineClassConst ( klass, "GLOBAL_VIEW_HEIGHT",						( u32 )MOAIGfxState::VIEW_HEIGHT );
	state.DefineClassConst ( klass, "GLOBAL_VIEW_WIDTH",						( u32 )MOAIGfxState::VIEW_WIDTH );
	
	state.DefineClassConst ( klass, "GLOBAL_VIEW_HALF_HEIGHT",					( u32 )MOAIGfxState::VIEW_HALF_HEIGHT );
	state.DefineClassConst ( klass, "GLOBAL_VIEW_HALF_WIDTH",					( u32 )MOAIGfxState::VIEW_HALF_WIDTH );
}

//----------------------------------------------------------------//
void MOAIShaderProgram::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIGfxResource::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "declareUniform", _declareUniform, MRB_ARGS_REQ ( 5 ) );
	state.DefineInstanceMethod ( klass, "load", _load, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "reserveGlobals", _reserveGlobals, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "reserveTextures", _reserveTextures, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "reserveUniforms", _reserveUniforms, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setGlobal", _setGlobal, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "setTexture", _setTexture, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "setVertexAttribute", _setVertexAttribute, MRB_ARGS_REQ ( 2 ) );

}

//----------------------------------------------------------------//
void MOAIShaderProgram::ReserveGlobals ( u32 nGlobals ) {

	this->mGlobals.Init ( nGlobals );
}

//----------------------------------------------------------------//
mrb_value MOAIShaderProgram::ReserveGlobals ( mrb_state* M, int idx ) {

	MOAIRubyState state ( M );

	u32 nGlobals = state.GetParamValue < u32 >( idx, 0 );
	this->ReserveGlobals ( nGlobals );

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
void MOAIShaderProgram::ReserveTextures ( u32 nTextures ) {

	this->mTextures.Init ( nTextures );
}

//----------------------------------------------------------------//
void MOAIShaderProgram::ReserveUniforms ( u32 nUniforms ) {

	this->mUniforms.Init ( nUniforms );
}

//----------------------------------------------------------------//
void MOAIShaderProgram::ScheduleTextures () {

	MOAIGfxMgr& gfx = MOAIGfxMgr::Get ();
	MOAIMaterialMgr& materialStack = MOAIMaterialMgr::Get ();

	size_t nTextures = this->mTextures.Size ();
	for ( u32 i = 0; i < nTextures; ++i ) {
	
		MOAIShaderProgramTexture& shaderTexture = this->mTextures [ i ];
		
		MOAITextureBase* texture = shaderTexture.mTexture;
		
		// load texture by name
		if ( !texture ) {
			texture = materialStack.GetTexture ( shaderTexture.mName );
		}
		
		gfx.mGfxState.SetTexture ( texture, shaderTexture.mUnit );
	}
}

//----------------------------------------------------------------//
void MOAIShaderProgram::SetGlobal ( u32 idx, u32 globalID, u32 uniformID, u32 index ) {
	
	MOAIShaderProgramGlobal& global = this->mGlobals [ idx ];
	
	global.mUniformID	= uniformID;
	global.mIndex		= index;
	global.mGlobalID	= globalID;
}

//----------------------------------------------------------------//
mrb_value MOAIShaderProgram::SetGlobal ( mrb_state* M, int idx ) {

	MOAIRubyState state ( M );

	u32 globalIdx	= state.GetParamValue < u32 >( idx, 1 ) - 1;
	u32 globalID	= state.GetParamValue < u32 >( idx + 1, INVALID_INDEX );
	u32 uniformID	= state.GetParamValue < u32 >( idx + 2, 1 ) - 1;
	u32 index		= state.GetParamValue < u32 >( idx + 3, 1 ) - 1;
	
	this->SetGlobal ( globalIdx, globalID, uniformID, index );

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
void MOAIShaderProgram::SetTexture ( u32 idx, u32 name, u32 unit, MOAITextureBase* fallback ) {

	if ( idx < this->mTextures.Size ()) {
	
		MOAIShaderProgramTexture& shaderTexture = this->mTextures [ idx ];
		shaderTexture.mName = name;
		shaderTexture.mUnit = unit;
		shaderTexture.mTexture = fallback;
	}
}

//----------------------------------------------------------------//
void MOAIShaderProgram::SetTexture ( u32 idx, MOAITextureBase* texture, u32 unit ) {

	if ( idx < this->mTextures.Size ()) {
	
		MOAIShaderProgramTexture& shaderTexture = this->mTextures [ idx ];
		shaderTexture.mName = MOAI_UNKNOWN_MATERIAL_GLOBAL;
		shaderTexture.mUnit = unit;
		shaderTexture.mTexture = texture;
	}
}

//----------------------------------------------------------------//
void MOAIShaderProgram::SetVertexAttribute ( u32 idx, cc8* attribute ) {

	if ( attribute ) {
		this->mAttributeMap [ idx ] = attribute;
	}
}

//----------------------------------------------------------------//
void MOAIShaderProgram::UpdateUniforms ( ZLLeanArray < u8 >& buffer ) {

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	
	ZLRect viewRect = gfxState.GetViewRect ();

	// NOTE: matrices are submitted *column major*; it is up to the shader to transform vertices correctly
	// vert * matrix implicitely transposes the matrix; martix * vert uses the matrix as submitted

	u32 nGlobals = this->mGlobals.Size ();

	for ( u32 i = 0; i < nGlobals; ++i ) {
	
		const MOAIShaderProgramGlobal& global = this->mGlobals [ i ];
		
		if ( global.mUniformID == INVALID_INDEX ) continue;
		
		MOAIShaderUniformHandle uniform = this->GetUniformHandle ( buffer, global.mUniformID, global.mIndex );
		if ( !uniform.IsValid ()) continue;
		
		if ( global.mGlobalID < MOAIGfxState::TOTAL_MATRICES ) {
		
			uniform.SetValue ( gfxState.GetMtx ( global.mGlobalID ));
		}
		else {
		
			switch ( global.mGlobalID ) {
				
				case MOAIGfxState::PEN_COLOR:
				
					uniform.SetValue ( gfxState.GetFinalColor ());
					break;
				
				case MOAIGfxState::VIEW_HALF_HEIGHT:
				
					uniform.SetValue ( viewRect.Height () * 0.5f );
					break;
					
				case MOAIGfxState::VIEW_HALF_WIDTH: {
				
					uniform.SetValue ( viewRect.Width () * 0.5f );
					break;
				}
				case MOAIGfxState::VIEW_HEIGHT:
				
					uniform.SetValue ( viewRect.Height ());
					break;
					
				case MOAIGfxState::VIEW_WIDTH:
				
					uniform.SetValue ( viewRect.Width ());
					break;
			}
		}
	}
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
MOAIShaderUniformHandle MOAIShaderProgram::MOAIShaderUniformSchema_GetUniformHandle ( void* buffer, u32 uniformID ) const {

	MOAIShaderUniformHandle uniform;
	uniform.mBuffer = 0;

	if ( uniformID < this->mUniforms.Size ()) {
		const MOAIShaderUniform& programUniform = this->mUniforms [ uniformID ];
		uniform.mType		= programUniform.mType;
		uniform.mWidth		= programUniform.mWidth;
		uniform.mBuffer		= ( void* )(( size_t )buffer + this->mUniforms [ uniformID ].mCPUOffset );
	}
	return uniform;
}
