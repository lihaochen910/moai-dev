// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAILIGHTFORMAT_H
#define	MOAILIGHTFORMAT_H

#include <moai-sim/MOAIShaderUniformHandle.h>

//================================================================//
// MOAILightFormatUniform
//================================================================//
class MOAILightFormatUniform :
	public MOAIShaderUniformBase {
public:

	size_t				mBase;		// base in uniform buffer
};

//================================================================//
// MOAILightFormat
//================================================================//
class MOAILightFormat :
	public virtual MOAIRubyObject {
private:

	friend class MOAILight;

	u32											mTextures;
	ZLLeanArray < MOAILightFormatUniform >		mUniforms;
	bool										mDirty;
	size_t										mBufferSize;

	//----------------------------------------------------------------//
	static mrb_value					_reserveTextures			( mrb_state* M, mrb_value context );
	static mrb_value					_reserveUniform				( mrb_state* M, mrb_value context );
	static mrb_value					_setUniform					( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void						Bless						();

public:

	DECL_RUBY_FACTORY ( MOAILightFormat, MOAIRubyObject )

	//----------------------------------------------------------------//
	MOAILightFormatUniform*		GetUniform					( u32 uniformID );
								MOAILightFormat				();
								~MOAILightFormat			();
	void						RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void						RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
};

#endif
