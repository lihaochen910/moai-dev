// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAISHADERMGR_H
#define	MOAISHADERMGR_H

class MOAIShader;
class MOAIShaderProgram;

//================================================================//
// MOAIShaderMgr
//================================================================//
/**	@lua	MOAIShaderMgr
	@text	Shader presets.
	
	@const DECK2D_SHADER
	@const DECK2D_SNAPPING_SHADER
	@const DECK2D_TEX_ONLY_SHADER
	@const FONT_SHADER
	@const FONT_SNAPPING_SHADER
	@const FONT_EFFECTS_SHADER
	@const LINE_SHADER
	@const LINE_SHADER_3D
	@const MESH_SHADER
*/
class MOAIShaderMgr :
	public ZLContextClass < MOAIShaderMgr, MOAIRubyObject > {
public:

	enum Preset {
		DECK2D_SHADER,
		DECK2D_SNAPPING_SHADER,
		DECK2D_TEX_ONLY_SHADER,
		FONT_SHADER,
		FONT_SNAPPING_SHADER,
		FONT_EFFECTS_SHADER,
		LINE_SHADER,
		LINE_SHADER_3D,
		MESH_SHADER,
		TOTAL_SHADERS,
	};

	static const u32 UNKNOWN_SHADER = 0xffffffff;

private:
	
	MOAIShaderProgram*	mPrograms [ TOTAL_SHADERS ];
	MOAIShader*			mShaders [ TOTAL_SHADERS ];
	
	//----------------------------------------------------------------//
	static mrb_value				_getProgram				( mrb_state* M, mrb_value context );
	static mrb_value				_getShader				( mrb_state* M, mrb_value context );
	
public:
	
	DECL_RUBY_SINGLETON ( MOAIShaderMgr )
	
	//----------------------------------------------------------------//
	void					AffirmAll				();
	MOAIShaderProgram*		GetProgram				( u32 shaderID );
	MOAIShader*				GetShader				( u32 shaderID );
							MOAIShaderMgr			();
							~MOAIShaderMgr			();
	void					RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void					RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
