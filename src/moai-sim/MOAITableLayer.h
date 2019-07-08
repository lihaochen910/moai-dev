// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAITABLELAYER_H
#define	MOAITABLELAYER_H

#include <moai-sim/MOAILayer.h>

class MOAIColor;
class MOAIFrameBuffer;
class MOAIPartition;

//================================================================//
// MOAITableLayer
//================================================================//
class MOAITableLayer :
	public virtual MOAILayer {
protected:

	MOAIRubyStrongRef	mRenderTable;

	//----------------------------------------------------------------//
	static mrb_value	_getRenderTable			( mrb_state* M, mrb_value context );
	static mrb_value	_setRenderTable			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void				MOAIDrawable_Draw		( int subPrimID );

public:

	DECL_RUBY_FACTORY ( MOAITableLayer, MOAILayer )

	//----------------------------------------------------------------//
						MOAITableLayer			();
						~MOAITableLayer			();
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
