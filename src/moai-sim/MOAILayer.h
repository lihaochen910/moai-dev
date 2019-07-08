// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAILAYER_H
#define	MOAILAYER_H

#include <moai-sim/MOAIDrawable.h>

class MOAIColor;
class MOAIFrameBuffer;
class MOAIPartition;

//================================================================//
// MOAILayer
//================================================================//
class MOAILayer :
	public virtual MOAIRubyObject,
	public virtual MOAIDrawable {
private:

	u32				mClearFlags;
	u32				mClearColor;
	u32				mClearMode;

	MOAIRubySharedPtr < MOAIColor >			mClearColorNode;
	MOAIRubySharedPtr < MOAIFrameBuffer >	mFrameBuffer;

	//----------------------------------------------------------------//
	static mrb_value		_draw					( mrb_state* M, mrb_value context );
	static mrb_value		_getClearMode			( mrb_state* M, mrb_value context );
	static mrb_value		_getFrameBuffer			( mrb_state* M, mrb_value context );
	static mrb_value		_pushRenderPass			( mrb_state* M, mrb_value context );
	static mrb_value		_setClearColor			( mrb_state* M, mrb_value context );
	static mrb_value		_setClearDepth			( mrb_state* M, mrb_value context );
	static mrb_value		_setClearMode			( mrb_state* M, mrb_value context );
	static mrb_value		_setFrameBuffer			( mrb_state* M, mrb_value context );

protected:

	//----------------------------------------------------------------//
	void			ClearSurface			();

	//----------------------------------------------------------------//
	void			MOAIDrawable_Draw		( int subPrimID );

public:

	GET_SET ( u32, ClearFlags, mClearFlags );
	GET_SET ( u32, ClearMode, mClearMode );

	enum {
		CLEAR_NEVER,
		CLEAR_ALWAYS,
		CLEAR_ON_BUFFER_FLAG,
	};
	
	DECL_RUBY_FACTORY ( MOAILayer, MOAIRubyObject )

	//----------------------------------------------------------------//
	MOAIFrameBuffer*	GetFrameBuffer			();
						MOAILayer				();
						~MOAILayer				();
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void				SetClearColor			( MOAIColor* color );
	void				SetFrameBuffer			( MOAIFrameBuffer* frameBuffer = 0 );
};

#endif
