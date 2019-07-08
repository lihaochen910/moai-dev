// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIScissorRect.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getRect
	@text	Return the extents of the scissor rect.
	
	@in		MOAIScissorRect self
	@out	number xMin
	@out	number yMin
	@out	number xMax
	@out	number yMax
*/
mrb_value MOAIScissorRect::_getRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIScissorRect, "U" )
	
	return state.Get ( self->mRect );
}

//----------------------------------------------------------------//
/**	@lua	setRect
	@text	Sets the extents of the scissor rect.
	
	@in		number x1					The X coordinate of the rect's upper-left point.
	@in		number y1					The Y coordinate of the rect's upper-left point.
	@in		number x2					The X coordinate of the rect's lower-right point.
	@in		number y2					The Y coordinate of the rect's lower-right point.
	@out	nil
*/
mrb_value MOAIScissorRect::_setRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIScissorRect, "U" )
	
	self->mRect = state.GetRect < float >( 1 );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setScissorRect
	@text	Set or clear the parent scissor rect.
	
	@in		MOAIScissorRect self
	@opt	MOAIScissorRect parent		Default value is nil.
	@out	nil
*/
mrb_value MOAIScissorRect::_setScissorRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIScissorRect, "U" )
	
	MOAIScissorRect* scissorRect = state.GetRubyObject < MOAIScissorRect >( 1, true );
	self->mScissorRect.Set ( *self, scissorRect );
	
	return context;
}

//================================================================//
// MOAIScissorRect
//================================================================//

//----------------------------------------------------------------//
ZLRect MOAIScissorRect::GetScissorRect ( const ZLMatrix4x4& worldToWndMtx ) const {

	ZLVec3D vtx3D [ 4 ];

	vtx3D [ 0 ].mX = this->mRect.mXMin;
	vtx3D [ 0 ].mY = this->mRect.mYMin;
	vtx3D [ 0 ].mZ = 0.0f;

	vtx3D [ 1 ].mX = this->mRect.mXMin;
	vtx3D [ 1 ].mY = this->mRect.mYMax;
	vtx3D [ 1 ].mZ = 0.0f;
	
	vtx3D [ 2 ].mX = this->mRect.mXMax;
	vtx3D [ 2 ].mY = this->mRect.mYMax;
	vtx3D [ 2 ].mZ = 0.0f;

	vtx3D [ 3 ].mX = this->mRect.mXMax;
	vtx3D [ 3 ].mY = this->mRect.mYMin;
	vtx3D [ 3 ].mZ = 0.0f;

	ZLMatrix4x4 mtx ( this->GetLocalToWorldMtx ());
	mtx.Append ( worldToWndMtx );
	
	mtx.Project ( vtx3D [ 0 ]);
	mtx.Project ( vtx3D [ 1 ]);
	mtx.Project ( vtx3D [ 2 ]);
	mtx.Project ( vtx3D [ 3 ]);
	
	ZLRect scissorRect;

	scissorRect.Init ( vtx3D [ 0 ]);
	scissorRect.Grow ( vtx3D [ 1 ]);
	scissorRect.Grow ( vtx3D [ 2 ]);
	scissorRect.Grow ( vtx3D [ 3 ]);

	if ( this->mScissorRect ) {
		ZLRect parentRect = this->mScissorRect->GetScissorRect ( worldToWndMtx );
		parentRect.Clip ( scissorRect );
	}

	return scissorRect;
}

//----------------------------------------------------------------//
MOAIScissorRect::MOAIScissorRect () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAITransform )
	RTTI_END
	
	this->mRect.Init ( 0.0f, 0.0f, 0.0f, 0.0f );
}

//----------------------------------------------------------------//
MOAIScissorRect::~MOAIScissorRect () {

	this->mScissorRect.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAIScissorRect::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAITransform::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIScissorRect::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAITransform::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getRect", _getRect, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setRect", _setRect, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "setScissorRect", _setScissorRect, MRB_ARGS_ARG ( 0, 1 ) );

}

