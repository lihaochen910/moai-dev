// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAISCISSORRECT_H
#define MOAISCISSORRECT_H

#include <moai-sim/MOAITransform.h>

//================================================================//
// MOAIScissorRect
//================================================================//
/**	@lua	MOAIScissorRect
	@text	Class for clipping props when drawing.
*/
class MOAIScissorRect :
	public MOAITransform {
private:
	
	ZLRect									mRect;
	MOAIRubySharedPtr < MOAIScissorRect >	mScissorRect;
	
	//----------------------------------------------------------------//
	static mrb_value		_getRect				( mrb_state* M, mrb_value context );
	static mrb_value		_setRect				( mrb_state* M, mrb_value context );
	static mrb_value		_setScissorRect			( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_FACTORY ( MOAIScissorRect, MOAITransform )

	//----------------------------------------------------------------//
	ZLRect			GetScissorRect			( const ZLMatrix4x4& worldToWndMtx ) const;
					MOAIScissorRect			();
					~MOAIScissorRect		();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
};

#endif
