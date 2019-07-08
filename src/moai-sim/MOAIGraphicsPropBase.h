// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIGRAPHICSPROPBASE_H
#define	MOAIGRAPHICSPROPBASE_H

#include <moai-sim/MOAIColor.h>
#include <moai-sim/MOAIDrawable.h>
#include <moai-sim/MOAIPartitionHull.h>
#include <moai-sim/MOAIMaterialBatchHolder.h>
#include <moai-sim/MOAITransform.h>

class MOAICellCoord;
class MOAIDeck;
class MOAIGrid;
class MOAILayoutFrame;
class MOAIMaterialBatch;
class MOAIOverlapPrim2D;
class MOAIPartition;
class MOAIPartitionCell;
class MOAIPartitionLevel;
class MOAIPartitionResultBuffer;
class MOAIScissorRect;
class MOAIShader;
class MOAISurfaceSampler2D;
class MOAITextureBase;

//================================================================//
// MOAIGraphicsPropBase
//================================================================//
/**	@lua	MOAIGraphicsPropBase
	@text	Base class for graphics props.
	
	@const	FRAME_FROM_DECK
	@const	FRAME_FROM_PARENT
	@const	FRAME_FROM_SELF

	@const  GL_FUNC_ADD
	@const  GL_FUNC_SUBTRACT
	@const  GL_FUNC_REVERSE_SUBTRACT
	
	@const	GL_ONE
	@const	GL_ZERO
	@const	GL_DST_ALPHA
	@const	GL_DST_COLOR
	@const	GL_SRC_COLOR
	@const	GL_ONE_MINUS_DST_ALPHA
	@const	GL_ONE_MINUS_DST_COLOR
	@const	GL_ONE_MINUS_SRC_ALPHA
	@const	GL_ONE_MINUS_SRC_COLOR
	@const	GL_SRC_ALPHA
	@const	GL_SRC_ALPHA_SATURATE
	
	@const	DEPTH_TEST_DISABLE
	@const	DEPTH_TEST_NEVER
	@const	DEPTH_TEST_LESS
	@const	DEPTH_TEST_EQUAL
	@const	DEPTH_TEST_LESS_EQUAL
	@const	DEPTH_TEST_GREATER
	@const	DEPTH_TEST_NOTEQUAL
	@const	DEPTH_TEST_GREATER_EQUAL
	@const	DEPTH_TEST_ALWAYS
	
	@const	CULL_NONE
	@const	CULL_ALL
	@const	CULL_BACK
	@const	CULL_FRONT
*/
class MOAIGraphicsPropBase :
	public virtual MOAIPartitionHull,
	public virtual MOAIColor,
	public virtual MOAIDrawable,
	public virtual MOAIMaterialBatchHolder {
protected:
	
	//----------------------------------------------------------------//
	static mrb_value		_draw					( mrb_state* M, mrb_value context );
	static mrb_value		_getBillboard			( mrb_state* M, mrb_value context );
	static mrb_value		_getScissorRect			( mrb_state* M, mrb_value context );
	static mrb_value		_isVisible				( mrb_state* M, mrb_value context );
	static mrb_value		_setBillboard			( mrb_state* M, mrb_value context );
	static mrb_value		_setParent				( mrb_state* M, mrb_value context );
	static mrb_value		_setScissorRect			( mrb_state* M, mrb_value context );
	static mrb_value		_setUVTransform			( mrb_state* M, mrb_value context );
	static mrb_value		_setVisible				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void			MOAIDrawable_DrawDebug						( int subPrimID );
	void			MOAIPartitionHull_AddToSortBuffer			( MOAIPartitionResultBuffer& buffer, u32 key );
	u32				MOAIPartitionHull_AffirmInterfaceMask		( MOAIPartition& partition );

protected:

	u32										mBillboard;
	u32										mDisplayFlags;
	
	// TODO: should be attributes?
	MOAIRubySharedPtr < MOAITransformBase >	mUVTransform;
	MOAIRubySharedPtr < MOAIScissorRect >	mScissorRect;

	//----------------------------------------------------------------//
	void					LoadUVTransform				();
	void					LoadVertexTransform			();
	void					PopGfxState					();
	void					PushGfxState				();

	//----------------------------------------------------------------//
	virtual ZLMatrix4x4		MOAIGraphicsPropBase_GetWorldDrawingMtx		(); // factors in billboard flags
	bool					MOAINode_ApplyAttrOp						( u32 attrID, MOAIAttribute& attr, u32 op );
	void					MOAINode_Update								();

public:

	DECL_ATTR_HELPER ( MOAIGraphicsPropBase )
	DECL_RUBY_ABSTRACT ( MOAIGraphicsPropBase )

	enum {
		DEBUG_DRAW_PARTITION_CELLS,
		DEBUG_DRAW_PARTITION_PADDED_CELLS,
		DEBUG_DRAW_AXIS,
		DEBUG_DRAW_DIAGONALS,
		DEBUG_DRAW_MODEL_BOUNDS,
		DEBUG_DRAW_WORLD_BOUNDS,
		TOTAL_DEBUG_LINE_STYLES,
	};

	enum {
		BILLBOARD_NONE,
		BILLBOARD_NORMAL,
		BILLBOARD_ORTHO,
		BILLBOARD_COMPASS,
		BILLBOARD_COMPASS_SCALE,
		BILLBOARD_SCREEN,
	};

	enum {
		ATTR_SCISSOR_RECT,
		
		ATTR_LOCAL_VISIBLE,		// direct access to the prop's 'local' visbility setting
		ATTR_VISIBLE,			// read only - reflects the composite state of visibility
		INHERIT_VISIBLE,		// used to *pull* parent visibility via inheritance
		
		INHERIT_FRAME,
		FRAME_TRAIT,
		
		TOTAL_ATTR,
	};

	enum {
		FLAGS_LOCAL_VISIBLE			= 0x01,
		FLAGS_VISIBLE				= 0x02, // this is a composite of FLAGS_LOCAL_VISIBLE plus the parent's ATTR_VISIBLE
	};

	static const u32 DEFAULT_FLAGS = FLAGS_LOCAL_VISIBLE | FLAGS_VISIBLE;;

	//----------------------------------------------------------------//
	MOAIRubyClass*			GetSuperRubyClass			();
	bool					IsVisible					();
							MOAIGraphicsPropBase		();
	virtual					~MOAIGraphicsPropBase		();
	void					RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void					RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void					Render						();
	void					SerializeIn					( MOAIRubyState& state, MOAIDeserializer& serializer );
	void					SerializeOut				( MOAIRubyState& state, MOAISerializer& serializer );
	void					SetVisible					( bool visible );
};

#endif
