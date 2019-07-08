// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAISHADER_H
#define	MOAISHADER_H

#include <moai-sim/MOAIShaderUniformSchema.h>
#include <moai-sim/MOAIShaderProgram.h>

//================================================================//
// MOAIShader
//================================================================//
/**	@lua	MOAIShader
	@text	This represents the "instance" of a shader program. Its purpose
			is to enable sharing of a single program across multiple sets of
			uniform values and to expose uniform values as MOAINode attributes.
*/
class MOAIShader :
	public virtual MOAINode {
protected:

	friend class MOAIGfxStateGPUCache;
	friend class MOAIShaderProgram;

	MOAIRubySharedPtr < MOAIShaderProgram >		mProgram;
	ZLLeanArray < u8 >							mPendingUniformBuffer;

	//----------------------------------------------------------------//
	static mrb_value				_getAttributeID				( mrb_state* M, mrb_value context );
	static mrb_value				_setProgram					( mrb_state* M, mrb_value context );
	static mrb_value				_setUniform					( mrb_state* M, mrb_value context );
	static mrb_value				_setUniformArrayItem		( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	bool					IsDirty					();

	//----------------------------------------------------------------//
	bool					MOAINode_ApplyAttrOp		( u32 attrID, MOAIAttribute& attr, u32 op );

public:

	DECL_RUBY_FACTORY ( MOAIShader, MOAINode )

	GET ( MOAIShaderProgram*, Program, mProgram )

	//----------------------------------------------------------------//
	void					ApplyUniforms			();
	static MOAIShader*		AffirmShader			( MOAIRubyState& state, int idx );
	void					BindUniforms			();
	void					Bless					();
	bool					HasDirtyUniforms		();
							MOAIShader				();
							~MOAIShader				();
	void					RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void					RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void					ResizeUniformArray		( u32 uniformID, u32 count );
	void					ScheduleTextures		();
	void					SetProgram				( MOAIShaderProgram* program );
	void					UpdateUniforms			();
};

#endif
