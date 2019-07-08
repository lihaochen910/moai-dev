// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIMATERIALHOLDER_H
#define	MOAIMATERIALHOLDER_H

#include <moai-sim/MOAIMaterial.h>

//================================================================//
// MOAIMaterialHolder
//================================================================//
// TODO: doxygen
class MOAIMaterialHolder :
	public virtual MOAIRubyObject {
protected:
	
	MOAIMaterial mMaterial;

	//----------------------------------------------------------------//
	static mrb_value			_getBlendMode			( mrb_state* M, mrb_value context );
	static mrb_value			_getCullMode			( mrb_state* M, mrb_value context );
	static mrb_value			_getDepthMask			( mrb_state* M, mrb_value context );
	static mrb_value			_getDepthTest			( mrb_state* M, mrb_value context );
	static mrb_value			_getLight				( mrb_state* M, mrb_value context );
	static mrb_value			_getShader				( mrb_state* M, mrb_value context );
	static mrb_value			_getTexture				( mrb_state* M, mrb_value context );
	static mrb_value			_setBlendMode			( mrb_state* M, mrb_value context );
	static mrb_value			_setCullMode			( mrb_state* M, mrb_value context );
	static mrb_value			_setDepthMask			( mrb_state* M, mrb_value context );
	static mrb_value			_setDepthTest			( mrb_state* M, mrb_value context );
	static mrb_value			_setLight				( mrb_state* M, mrb_value context );
	static mrb_value			_setShader				( mrb_state* M, mrb_value context );
	static mrb_value			_setTexture				( mrb_state* M, mrb_value context );

public:

	GET_CONST ( MOAIMaterial*, Material, &mMaterial )

	//----------------------------------------------------------------//
					MOAIMaterialHolder			();
					~MOAIMaterialHolder			();
	void			RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
};

#endif
