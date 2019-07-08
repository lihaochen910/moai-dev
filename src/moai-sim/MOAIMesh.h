// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIMESH_H
#define	MOAIMESH_H

#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIMaterialBatchHolder.h>
#include <moai-sim/MOAIVertexArray.h>

class MOAIIndexBuffer;
class MOAIMesh;
class MOAIMeshPartition;
class MOAISelectionSpan;
class MOAITextureBase;
class MOAIVertexAttribute;

//================================================================//
// MOAIMeshSpan
//================================================================//
class MOAIMeshSpan {
public:
	
	size_t				mBase;
	size_t				mTop;
	
	MOAIMeshSpan*		mPrev;
	MOAIMeshSpan*		mNext;
};

//================================================================//
// MOAIMeshPrimCoords
//================================================================//
class MOAIMeshPrimCoords {
public:
	
	static const u32 PRIM_POINT			= 1;
	static const u32 PRIM_LINE			= 2;
	static const u32 PRIM_TRIANGLE		= 3;
	static const u32 MAX_COORDS			= 3;
	
	u32			mIndex;
	u32			mPrimSize;
	ZLVec3D		mCoords [ MAX_COORDS ];
	
	//----------------------------------------------------------------//
	ZLBox		GetBounds		();
};

//================================================================//
// MOAIMeshPrimReader
//================================================================//
class MOAIMeshPrimReader {
private:

	friend class MOAIMesh;

	MOAIMesh*				mMesh;
	MOAIVertexFormat*		mVertexFormat;
	u32						mTotalPrims;
	
	const MOAIVertexAttribute*	mAttribute;
	const void*					mVertexBuffer;
	MOAIIndexBuffer*			mIndexBuffer;

	//----------------------------------------------------------------//
	bool		Init			( MOAIMesh& mesh, u32 vertexBufferIndex );
	ZLVec3D		ReadCoord		( u32 idx ) const;

public:

	GET_CONST ( u32, TotalPrims, mTotalPrims )

	//----------------------------------------------------------------//
	bool		GetPrimCoords	( u32 idx, MOAIMeshPrimCoords& prim ) const;
};

//================================================================//
// MOAIMesh
//================================================================//
/**	@lua	MOAIMesh
	@text	Loads a texture and renders the contents of a vertex buffer.
			Grid drawing not supported.
	
	@const	GL_POINTS
	@const	GL_LINES
	@const	GL_TRIANGLES
	@const	GL_LINE_LOOP
	@const	GL_LINE_STRIP
	@const	GL_TRIANGLE_FAN
	@const	GL_TRIANGLE_STRIP
*/
class MOAIMesh :
	public MOAIDeck,
	public MOAIMaterialBatchHolder,
	public MOAIVertexArray {
protected:

	friend class MOAIMeshPrimReader;

	MOAIRubySharedPtr < MOAIIndexBuffer > mIndexBuffer;

	u32			mTotalElements;
	ZLBounds	mBounds;

	u32			mPrimType;
	
	float		mPenWidth;
	
	MOAIMeshPartition*	mPartition;

	//----------------------------------------------------------------//
	static mrb_value			_buildQuadTree				( mrb_state* M, mrb_value context );
	static mrb_value			_buildTernaryTree			( mrb_state* M, mrb_value context );
	static mrb_value			_getPrimsForPoint			( mrb_state* M, mrb_value context );
	static mrb_value			_getRegionForPrim			( mrb_state* M, mrb_value context );
	static mrb_value			_intersectRay				( mrb_state* M, mrb_value context );
	static mrb_value			_printPartition				( mrb_state* M, mrb_value context );
	static mrb_value			_readPrimCoords				( mrb_state* M, mrb_value context );
	static mrb_value			_setBounds					( mrb_state* M, mrb_value context );
	static mrb_value			_setIndexBuffer				( mrb_state* M, mrb_value context );
	static mrb_value			_setPenWidth				( mrb_state* M, mrb_value context );
	static mrb_value			_setPrimType				( mrb_state* M, mrb_value context );
	static mrb_value			_setTotalElements			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	ZLBounds			MOAIDeck_ComputeMaxBounds		();
	void				MOAIDeck_Draw					( u32 idx );
	ZLBounds			MOAIDeck_GetBounds				( u32 idx );

public:

	DECL_RUBY_FACTORY ( MOAIMesh, MOAIDeck )
	
	GET_SET ( u32, PrimType, mPrimType )
	GET_SET ( u32, TotalElements, mTotalElements )
	
	//----------------------------------------------------------------//
	void				ClearBounds					();
	u32					CountPrims					() const;
	void				DrawIndex					( u32 idx, MOAIMeshSpan* span );
						MOAIMesh					();
						~MOAIMesh					();
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void				ReserveVAOs					( u32 total );
	void				ReserveVertexBuffers		( u32 total );
	void				SerializeIn					( MOAIRubyState& state, MOAIDeserializer& serializer );
	void				SerializeOut				( MOAIRubyState& state, MOAISerializer& serializer );
	void				SetBounds					( const ZLBox& bounds );
	void				SetIndexBuffer				( MOAIIndexBuffer* indexBuffer );
};

#endif
