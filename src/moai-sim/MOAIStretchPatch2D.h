// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAISTRETCHPATCH2D_H
#define	MOAISTRETCHPATCH2D_H

#include <moai-sim/MOAIStretchDeck.h>
#include <moai-sim/MOAIMaterialBatchHolder.h>

//================================================================//
// MOAIStretchPatchSpan
//================================================================//
class MOAIStretchPatchSpan {
private:

	friend class MOAIStretchPatch2D;

	float	mPercent;		// total percent of native width of height covered by this span
	bool	mCanStretch;	// can this span stretch?
};

//================================================================//
// MOAIStretchPatch2D
//================================================================//
/**	@lua	MOAIStretchPatch2D
	@text	Moai implementation of a 9-patch. Textured quad with any number of
			stretchable and non-stretchable 'bands.' Grid drawing not supported.
*/
class MOAIStretchPatch2D :
	public MOAIStretchDeck,
	public MOAIMaterialBatchHolder {
private:

	ZLLeanArray < MOAIStretchPatchSpan >	mRows;
	ZLLeanArray < MOAIStretchPatchSpan >	mCols;

	// native dimensions of image; will stretch to fill model rect
	ZLRect					mRect;
	ZLLeanArray < ZLRect >	mUVRects;

	float	mXFix;
	float	mXFlex;
	
	float	mYFix;
	float	mYFlex;
	
	bool	mNeedsUpdate;

	//----------------------------------------------------------------//
	static mrb_value		_ninePatch				( mrb_state* M, mrb_value context );
	static mrb_value		_reserveColumns			( mrb_state* M, mrb_value context );
	static mrb_value		_reserveRows			( mrb_state* M, mrb_value context );
	static mrb_value		_reserveUVRects			( mrb_state* M, mrb_value context );
	static mrb_value		_setColumn				( mrb_state* M, mrb_value context );
	static mrb_value		_setRect				( mrb_state* M, mrb_value context );
	static mrb_value		_setRow					( mrb_state* M, mrb_value context );
	static mrb_value		_setUVRect				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void			DrawStretch				( u32 idx, float xStretch, float yStretch );
	void			UpdateParams			();

	//----------------------------------------------------------------//
	ZLBounds				MOAIDeck_ComputeMaxBounds				();
	void					MOAIDeck_Draw							( u32 idx );
	ZLBounds				MOAIDeck_GetBounds						( u32 idx );
	MOAICollisionShape*		MOAIDeck_GetCollisionShape				( u32 idx );
	bool					MOAIDeck_Overlap						( u32 idx, const ZLVec2D& vec, u32 granularity, ZLBounds* result );
	bool					MOAIDeck_Overlap						( u32 idx, const ZLVec3D& vec, u32 granularity, ZLBounds* result );

public:
	
	DECL_RUBY_FACTORY ( MOAIStretchPatch2D, MOAIStretchDeck )
	
	//----------------------------------------------------------------//
					MOAIStretchPatch2D		();
					~MOAIStretchPatch2D		();
	void			SerializeIn				( MOAIRubyState& state, MOAIDeserializer& serializer );
	void			SerializeOut			( MOAIRubyState& state, MOAISerializer& serializer );
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			SetColumn				( u32 idx, float percent, bool canStretch );
	void			SetRow					( u32 idx, float percent, bool canStretch );
};

#endif
