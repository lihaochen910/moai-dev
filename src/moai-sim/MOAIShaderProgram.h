// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAISHADERPROGRAM_H
#define	MOAISHADERPROGRAM_H

#include <moai-sim/MOAIGfxResource.h>
#include <moai-sim/MOAINode.h>
#include <moai-sim/MOAIShaderUniform.h>

#define		OPENGL_PREPROC		"#define LOWP\n #define MEDP\n #define HIGHP\n"
#define		OPENGL_ES_PREPROC	"#define LOWP lowp\n #define MEDP mediump\n #define HIGHP highp\n"
#define		WEBGL_PREPROC		"precision mediump int;\n precision mediump float;\n"

class MOAIShader;
class MOAITextureBase;

//================================================================//
// MOAIShaderProgramGlobal
//================================================================//
class MOAIShaderProgramGlobal {
private:

	friend class MOAIShader;
	friend class MOAIShaderGlobals;
	friend class MOAIShaderProgram;

	u32		mGlobalID;
	u32		mUniformID;
	u32		mIndex;

public:

	//----------------------------------------------------------------//
		MOAIShaderProgramGlobal		();
};

//================================================================//
// MOAIShaderProgramTexture
//================================================================//
class MOAIShaderProgramTexture {
private:

	friend class MOAIShaderProgram;

	u32				mName;
	u32				mUnit;
	ZLStrongPtr < MOAITextureBase > mTexture;

public:

	//----------------------------------------------------------------//
		MOAIShaderProgramTexture	();
};


//================================================================//
// MOAIShaderProgram
//================================================================//
/**	@lua	MOAIShaderProgram
	@text	Programmable shader class. This represents the shader
			program itself, which may be shared by one or more
			MOAIShader instances. The shader program holds the graphics
			resource, and the shader instances apply the (non-global)
			uniform values.

	@const	UNIFORM_FLOAT
	@const	UNIFORM_INDEX
	@const	UNIFORM_INT
	@const	UNIFORM_MATRIX_F3
	@const	UNIFORM_MATRIX_F4
	@const	UNIFORM_VECTOR_F4
	
	@const	GLOBAL_VIEW_PROJ
	@const	GLOBAL_VIEW_WIDTH
	@const	GLOBAL_VIEW_HEIGHT
	@const	GLOBAL_WORLD
	@const	GLOBAL_WORLD_INVERSE
	@const	GLOBAL_WORLD_VIEW
	@const	GLOBAL_WORLD_VIEW_INVERSE
	@const	GLOBAL_WORLD_VIEW_PROJ
*/
class MOAIShaderProgram :
	public MOAIGfxResource,
	public MOAIShaderUniformSchema {
protected:

	friend class MOAIShader;
	friend class MOAIGfxMgr;

	enum {
		GFX_EVENT_UPDATED_UNIFORMS = GFX_EVENT_TOTAL,
	};

	STLString		mVertexShaderSource;
	STLString		mFragmentShaderSource;

	ZLGfxHandle		mProgram;
	ZLGfxHandle		mVertexShader;
	ZLGfxHandle		mFragmentShader;

	typedef STLMap < u32, STLString >::iterator AttributeMapIt;
	STLMap < u32, STLString > mAttributeMap;

	ZLLeanArray < MOAIShaderUniform >			mUniforms;
	ZLLeanArray < MOAIShaderProgramGlobal >		mGlobals;
	ZLLeanArray < MOAIShaderProgramTexture >	mTextures;

	size_t										mMaxCount;
	size_t										mUniformBufferSize;

	ZLLeanArray < u8 >							mUniformBuffer;

	//----------------------------------------------------------------//
	static mrb_value			_declareUniform				( mrb_state* M, mrb_value context );
	static mrb_value			_load						( mrb_state* M, mrb_value context );
	static mrb_value			_reserveGlobals				( mrb_state* M, mrb_value context );
	static mrb_value			_reserveTextures			( mrb_state* M, mrb_value context );
	static mrb_value			_reserveUniforms			( mrb_state* M, mrb_value context );
	static mrb_value			_setGlobal					( mrb_state* M, mrb_value context );
	static mrb_value			_setTexture					( mrb_state* M, mrb_value context );
	static mrb_value			_setVertexAttribute			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void				AffirmUniforms				();
	void				ApplyUniforms				( ZLLeanArray < u8 >& buffer );
	void				BindUniforms				();
	ZLGfxHandle			CompileShader				( u32 type, cc8* source );
	MOAIShaderUniform*	GetUniform					( u32 uniformID );
	void				InitUniformBuffer			( ZLLeanArray < u8 >& buffer );
	bool				OnCPUCreate					();
	void				OnCPUDestroy				();
	void				OnGPUBind					();
	bool				OnGPUCreate					();
	void				OnGPUDeleteOrDiscard		( bool shouldDelete );
	void				OnGPUUnbind					();
	bool				OnGPUUpdate					();
	void				OnUniformLocation			( u32 addr, void* userdata );
	mrb_value			ReserveGlobals				( mrb_state* M, int idx );
	void				ScheduleTextures			();
	mrb_value			SetGlobal					( mrb_state* M, int idx );
	void				UpdateUniforms				( ZLLeanArray < u8 >& buffer );
	
	//----------------------------------------------------------------//
	MOAIShaderUniformHandle				MOAIShaderUniformSchema_GetUniformHandle	( void* buffer, u32 uniformID ) const;
	
public:

	DECL_RUBY_FACTORY ( MOAIShaderProgram, MOAIInstanceEventSource )

	//----------------------------------------------------------------//
	void				Clear						();
	void				DeleteShaders				();
	void				DeclareUniform				( u32 idx, cc8* name, u32 type, u32 width = 1, u32 count = 1 );
	void				Load						( cc8* vshSource, cc8* fshSource );
						MOAIShaderProgram			();
						~MOAIShaderProgram			();
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void				ReserveAttributes			( u32 nAttributes );
	void				ReserveGlobals				( u32 nGlobals );
	void				ReserveTextures				( u32 nTextures );
	void				ReserveUniforms				( u32 nUniforms );
	void				SetGlobal					( u32 idx, u32 globalID, u32 uniformID, u32 index );
	void				SetTexture					( u32 idx, u32 name, u32 unit, MOAITextureBase* fallback );
	void				SetTexture					( u32 idx, MOAITextureBase* texture, u32 unit );
	void				SetVertexAttribute			( u32 idx, cc8* attribute );
};

#endif
