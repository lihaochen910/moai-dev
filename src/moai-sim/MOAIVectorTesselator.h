// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIVECTORTESSELATOR_H
#define	MOAIVECTORTESSELATOR_H

#include <moai-sim/MOAIRegion.h>
#include <moai-sim/MOAIVectorUtil.h>

class MOAIGfxBuffer;
class MOAIVectorShape;
class MOAIVertexFormat;

class SafeTesselator;

//================================================================//
// MOAIVectorTesselatorWriter
//================================================================//
// TODO: doxygen
class MOAIVectorTesselatorWriter :
	public MOAIRubyObject {
private:

	friend class MOAIVectorTesselator;

	enum {
		STYLE_CMD_FILL,
		STYLE_CMD_FILL_COLOR,
		STYLE_CMD_LINE,
		STYLE_CMD_LINE_COLOR,
		STYLE_CMD_LINE_WIDTH,
		STYLE_CMD_STROKE,
		STYLE_CMD_STROKE_COLOR,
		STYLE_CMD_STROKE_WIDTH,
		STYLE_CMD_STROKE_DEPTH_BIAS,
		STYLE_CMD_JOIN,
		STYLE_CMD_CAP,
		STYLE_CMD_MITER_LIMIT,
		STYLE_CMD_WINDING_RULE,
		STYLE_CMD_CIRCLE_RESOLUTION,
		STYLE_CMD_EXTRUDE,
		STYLE_CMD_Z_OFFSET,
		STYLE_CMD_LIGHT_VEC,
		STYLE_CMD_LIGHT_COLOR,
		STYLE_CMD_LIGHT_CURVE,
		STYLE_CMD_SHADOW_COLOR,
		STYLE_CMD_SHADOW_CURVE,
		STYLE_CMD_DRAWING_TO_WORLD,
		STYLE_CMD_FILL_EXTRA_ID,
		STYLE_CMD_STROKE_EXTRA_ID,
		STYLE_CMD_MERGE_NORMALS,
		
		VECTOR_CMD_SHAPE,
		VECTOR_CMD_DONE,
	};

	bool				mFlushStyle;
	MOAIVectorStyle		mStyle;

public:

	DECL_RUBY_OPAQUE ( MOAIVectorTesselatorWriter )

	//----------------------------------------------------------------//
							MOAIVectorTesselatorWriter			();
							~MOAIVectorTesselatorWriter			();
	MOAIVectorShape*		ReadShape							( ZLStream& stream );
	void					WriteShape							( ZLStream& stream, const MOAIVectorShape& shape );
};

//================================================================//
// MOAIVectorTesselator
//================================================================//
/**	@lua	MOAIVectorTessalator
	@text	Convert vector primitives into triangles.
*/
class MOAIVectorTesselator :
	public MOAIRubyObject {
private:

	enum {
		VERTEX_SIZE = 16,
	};

	ZLLeanStack < MOAIVectorShape*, 16 >	mShapeStack;
	
	ZLLeanStack < ZLVec2D, 32 >				mVertexStack;
	bool									mPolyClosed;
	
	float				mDepthBias;
	float				mDepthOffset;
	
	MOAIVectorStyle		mStyle;
	
	bool				mVerbose;

	ZLLeanStack < ZLAffine2D, 16 > mMatrixStack;

	size_t					mVtxExtraSize;
	ZLLeanArray < void* >	mVtxExtras;

	//----------------------------------------------------------------//
	static mrb_value		_clearShapes			( mrb_state* M, mrb_value context );
	static mrb_value		_clearStyles			( mrb_state* M, mrb_value context );
	static mrb_value		_clearTransforms		( mrb_state* M, mrb_value context );
	static mrb_value		_drawingToWorld			( mrb_state* M, mrb_value context );
	static mrb_value		_drawingToWorldVec		( mrb_state* M, mrb_value context );
	static mrb_value		_finish					( mrb_state* M, mrb_value context );
	static mrb_value		_getExtrude				( mrb_state* M, mrb_value context );
	static mrb_value		_getTransform			( mrb_state* M, mrb_value context );
	static mrb_value		_openWriter				( mrb_state* M, mrb_value context );
	static mrb_value		_pushBezierVertices		( mrb_state* M, mrb_value context );
	static mrb_value		_pushCombo				( mrb_state* M, mrb_value context );
	static mrb_value		_pushEllipse			( mrb_state* M, mrb_value context );
	static mrb_value		_pushPoly				( mrb_state* M, mrb_value context );
	static mrb_value		_pushRect				( mrb_state* M, mrb_value context );
	static mrb_value		_pushRegion             ( mrb_state* M, mrb_value context );
	static mrb_value		_pushRotate				( mrb_state* M, mrb_value context );
	static mrb_value		_pushScale				( mrb_state* M, mrb_value context );
	static mrb_value		_pushSkew				( mrb_state* M, mrb_value context );
	static mrb_value		_pushTransform			( mrb_state* M, mrb_value context );
	static mrb_value		_pushTranslate			( mrb_state* M, mrb_value context );
	static mrb_value		_pushVertex				( mrb_state* M, mrb_value context );
	static mrb_value		_readShapes				( mrb_state* M, mrb_value context );
	static mrb_value		_reserveVertexExtras	( mrb_state* M, mrb_value context );
	static mrb_value		_setCapStyle			( mrb_state* M, mrb_value context );
	static mrb_value		_setCircleResolution	( mrb_state* M, mrb_value context );
	static mrb_value		_setDepthBias			( mrb_state* M, mrb_value context );
	static mrb_value		_setExtrude				( mrb_state* M, mrb_value context );
	static mrb_value		_setFillColor			( mrb_state* M, mrb_value context );
	static mrb_value		_setFillStyle			( mrb_state* M, mrb_value context );
	static mrb_value		_setFillExtra			( mrb_state* M, mrb_value context );
	static mrb_value		_setJoinStyle			( mrb_state* M, mrb_value context );
	static mrb_value		_setLightColor			( mrb_state* M, mrb_value context );
	static mrb_value		_setLightCurve			( mrb_state* M, mrb_value context );
	static mrb_value		_setLightVec			( mrb_state* M, mrb_value context );
	static mrb_value		_setLineColor			( mrb_state* M, mrb_value context );
	static mrb_value		_setLineStyle			( mrb_state* M, mrb_value context );
	static mrb_value		_setLineWidth			( mrb_state* M, mrb_value context );
	static mrb_value		_setMergeNormals		( mrb_state* M, mrb_value context );
	static mrb_value		_setMiterLimit			( mrb_state* M, mrb_value context );
	static mrb_value		_setPolyClosed			( mrb_state* M, mrb_value context );
	static mrb_value		_setShadowColor			( mrb_state* M, mrb_value context );
	static mrb_value		_setShadowCurve			( mrb_state* M, mrb_value context );
	static mrb_value		_setStrokeColor			( mrb_state* M, mrb_value context );
	static mrb_value		_setStrokeDepthBias		( mrb_state* M, mrb_value context );
	static mrb_value		_setStrokeExtra			( mrb_state* M, mrb_value context );
	static mrb_value		_setStrokeStyle			( mrb_state* M, mrb_value context );
	static mrb_value		_setStrokeWidth			( mrb_state* M, mrb_value context );
	static mrb_value		_setVerbose				( mrb_state* M, mrb_value context );
	static mrb_value		_setVertexExtra			( mrb_state* M, mrb_value context );
	static mrb_value		_setWindingRule			( mrb_state* M, mrb_value context );
	static mrb_value		_tesselate				( mrb_state* M, mrb_value context );
	static mrb_value		_worldToDrawing			( mrb_state* M, mrb_value context );
	static mrb_value		_worldToDrawingVec		( mrb_state* M, mrb_value context );
	static mrb_value		_writeShapes			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void			PushShape				( MOAIVectorShape* shape );
		
public:

	static const u32	TESSELATE_FILLS			= 0x01;
	static const u32	TESSELATE_STROKES		= 0x02;
	static const u32	TESSELATE_ALL			= TESSELATE_FILLS | TESSELATE_STROKES;
	
	DECL_RUBY_FACTORY ( MOAIVectorTesselator, MOAIRubyObject )
	
	GET_SET ( MOAIVectorStyle&, Style, mStyle )
	GET_SET ( bool, Verbose, mVerbose )
	GET_SET ( bool, PolyClosed, mPolyClosed )
	GET_SET ( float, DepthBias, mDepthBias )
	
	//----------------------------------------------------------------//
	void				Clear						();
	void				ClearShapes					();
	void				ClearTransforms				();
	u32					CountVertices				( const MOAIVertexFormat& format, ZLStream& vtxStream );
	int					Finish						();
						MOAIVectorTesselator		();
						~MOAIVectorTesselator		();
	void				PopTransform				();
	void				PushBezierVertices			( const ZLVec2D& p0, const ZLVec2D& p1, const ZLVec2D& p2, const ZLVec2D& p3 );
	void				PushCombo					();
	void				PushEllipse					( float x, float y, float xRad, float yRad );
	void				PushPoly					( const ZLVec2D* vertices, u32 total, bool closed );
	void				PushRect					( float xMin, float yMin, float xMax, float yMax );
	void				PushRegion					( const MOAIRegion& region );
	void				PushRotate					( float x, float y, float r );
	void				PushScale					( float x, float y );
	void				PushSkew					( float yx, float xy );
	void				PushTransform				( const ZLAffine2D& transform );
	void				PushTransform				( float a, float b, float c, float d, float tx, float ty );
	void				PushTranslate				( float x, float y);
	void				PushVertex					( float x, float y );
	void				ReadShapes					( ZLStream& stream );
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void				ReserveVertexExtras			( u32 total, size_t size );
	void				SetVertexExtra				( u32 idx, void* extra, size_t size );
	int					Tesselate					( SafeTesselator& tess, u32 flags = TESSELATE_ALL );
	int					Tesselate					( MOAIRegion& region, u32 flags = TESSELATE_ALL );
	int					Tesselate					( ZLStream& vtxStream, ZLStream& idxStream, MOAIVertexFormat& format, u32 flags = TESSELATE_ALL );
	int					Tesselate					( MOAIVertexBuffer& vtxBuffer, MOAIIndexBuffer& idxBuffer, MOAIVertexFormat& format, u32 idxSizeInBytes, u32 flags = TESSELATE_ALL );
	void				WriteShapes					( ZLStream& stream, MOAIVectorTesselatorWriter* writer = 0 );
	void				WriteSkirt					( SafeTesselator& tess, ZLStream& vtxStream, ZLStream& idxStream, MOAIVertexFormat& format, const MOAIVectorStyle& style, const ZLColorVec& fillColor, u32 vertexExtraID );
	void				WriteTriangles				( SafeTesselator& tess, ZLStream& vtxStream, ZLStream& idxStream, MOAIVertexFormat& format, const MOAIVectorStyle& style, float z, u32 color, u32 vertexExtraID );
	void				WriteVertex					( ZLStream& stream, MOAIVertexFormat& format, float x, float y, float z, float xn, float yn, float zn, u32 color, u32 vertexExtraID );
};

#endif
