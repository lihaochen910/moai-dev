// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIMATRIX_H
#define	MOAIMATRIX_H

#include <moai-sim/MOAITransformBase.h>

//================================================================//
// MOAIMatrix
//================================================================//
// TODO: doxygen
class MOAIMatrix :
	public MOAITransformBase,
	public ZLAffine3D {
protected:

	//----------------------------------------------------------------//
	static mrb_value		_getMatrix				( mrb_state* M, mrb_value context );
	static mrb_value		_invert					( mrb_state* M, mrb_value context );
	static mrb_value		_setMatrix				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	bool			MOAINode_ApplyAttrOp						( u32 attrID, MOAIAttribute& attr, u32 op );
	void			MOAITransformBase_BuildLocalToWorldMtx		( ZLAffine3D& localToWorldMtx );

public:

	DECL_RUBY_FACTORY ( MOAIMatrix, MOAINode )
	DECL_ATTR_HELPER ( MOAIMatrix )
	
	enum {
		ATTR_MATRIX,
	};
	
	//----------------------------------------------------------------//
					MOAIMatrix				();
					~MOAIMatrix				();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			SerializeIn				( MOAIRubyState& state, MOAIDeserializer& serializer );
	void			SerializeOut			( MOAIRubyState& state, MOAISerializer& serializer );
};

#endif
