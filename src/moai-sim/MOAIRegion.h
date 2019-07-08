// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIREGION_H
#define MOAIREGION_H

class MOAIIndexBuffer;
class MOAITransform;
class MOAIVertexBuffer;
class MOAIVertexFormat;

class SafeTesselator;

//================================================================//
// MOAIRegion
//================================================================//
// TODO: doxygen
class MOAIRegion :
	public virtual MOAIRubyObject {
private:
	
	ZLLeanArray < ZLPolygon2D > mPolygons;
	
	//----------------------------------------------------------------//
	static mrb_value		_append				( mrb_state* M, mrb_value context );
	static mrb_value		_bless				( mrb_state* M, mrb_value context );
	static mrb_value		_boolean			( mrb_state* M, mrb_value context );
	static mrb_value		_clear				( mrb_state* M, mrb_value context );
	static mrb_value		_clip				( mrb_state* M, mrb_value context );
	static mrb_value		_convexHull			( mrb_state* M, mrb_value context );
	static mrb_value		_copy				( mrb_state* M, mrb_value context );
	static mrb_value		_countPolygons		( mrb_state* M, mrb_value context );
	static mrb_value		_cull				( mrb_state* M, mrb_value context );
	static mrb_value		_drawDebug			( mrb_state* M, mrb_value context );
	static mrb_value		_edge				( mrb_state* M, mrb_value context );
	static mrb_value		_findExtremity		( mrb_state* M, mrb_value context );
	static mrb_value		_getDistance		( mrb_state* M, mrb_value context );
	static mrb_value		_getPolygon			( mrb_state* M, mrb_value context );
	static mrb_value		_getTriangles		( mrb_state* M, mrb_value context );
	static mrb_value		_getVertices		( mrb_state* M, mrb_value context );
	static mrb_value		_pad				( mrb_state* M, mrb_value context );
	static mrb_value		_pointInside		( mrb_state* M, mrb_value context );
	static mrb_value		_print				( mrb_state* M, mrb_value context );
	static mrb_value		_reservePolygons	( mrb_state* M, mrb_value context );
	static mrb_value		_reserveVertices	( mrb_state* M, mrb_value context );
	static mrb_value		_reverseWinding		( mrb_state* M, mrb_value context );
	static mrb_value		_setVertex			( mrb_state* M, mrb_value context );
	static mrb_value		_setWinding			( mrb_state* M, mrb_value context );
	static mrb_value		_snap				( mrb_state* M, mrb_value context );
	static mrb_value		_stroke				( mrb_state* M, mrb_value context );
	static mrb_value		_tesselate			( mrb_state* M, mrb_value context );
	static mrb_value		_translate			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void					Read					( ZLStream& verts, ZLStream& polySizes );
	bool					ShouldCull				( const ZLPolygon2D& poly, u32 flag, bool checkArea, float minArea );

public:
	
	enum {
		BOOLEAN_AND,
		BOOLEAN_NOT,
		BOOLEAN_OR,
		BOOLEAN_XOR,
	};
	
	enum {
		WINDING_CLOCKWISE,
		WINDING_ANTICLOCKWISE,
	};
	
	DECL_RUBY_FACTORY ( MOAIRegion, MOAIRubyObject )

	GET_CONST ( size_t, Size, mPolygons.Size ())

	//----------------------------------------------------------------//
	int						AddFillContours			( SafeTesselator& tess, u32 mask = 0xffffffff ) const;
	void					Append					( const MOAIRegion& regionA, const MOAIRegion& regionB );
	void					Bless					();
	void					Boolean					( const MOAIRegion& regionA, const MOAIRegion& regionB, u32 operation );
	void					BooleanAnd				( const MOAIRegion& regionA, const MOAIRegion& regionB );
	void					BooleanNot				( const MOAIRegion& regionA, const MOAIRegion& regionB );
	void					BooleanOr				( const MOAIRegion& regionA, const MOAIRegion& regionB );
	void					BooleanXor				( const MOAIRegion& regionA, const MOAIRegion& regionB );
	void					Clear					();
	void					Clip					( const MOAIRegion& region, ZLPlane2D plane );
	void					Clip					( const MOAIRegion& region, const MOAIRegion& clip, const ZLAffine3D* mtx = 0 );
	int						CombineAndTesselate		( const MOAIRegion& regionA, const MOAIRegion& regionB, int windingRule );
	ZLSizeResult			ConvexHull				( ZLStream& vtxStream, size_t nVerts );
	void					Copy					( const MOAIRegion& region );
	void					Copy					( const SafeTesselator& tess );
	void					Copy					( const SafeTesselator& tess, ZLAffine2D& transform );
	void					Cull					( const MOAIRegion& region, u32 flag, bool checkArea = false, float minArea = 0.0f );
	void					DrawDebug				() const;
	void					Edge					( const MOAIRegion& region, const ZLVec2D& offset );
	bool					FindExtremity			( ZLVec2D n, ZLVec2D& e );
	bool					GetDistance				( const ZLVec2D& point, float& d ) const;
	bool					GetDistance				( const ZLVec2D& point, float& d, ZLVec2D& p ) const;
	ZLPolygon2D&			GetPolygon				( u32 idx );
	const ZLPolygon2D&		GetPolygon				( u32 idx ) const;
	u32						GetTriangles			( SafeTesselator& tess ) const;
	u32						GetTriangles			( MOAIVertexFormat& format, ZLStream& vtxStream, ZLStream& idxStream ) const;
	u32						GetTriangles			( MOAIVertexFormat& format, MOAIVertexBuffer& vtxBuffer, MOAIIndexBuffer& idxBuffer, u32 idxSizeInBytex ) const;
	ZLSizeResult			GetVertices				( ZLStream& vtxStream ) const;
							MOAIRegion				();
							~MOAIRegion				();
	void					Pad						( const MOAIRegion& region, float pad );
	bool					PointInside				( const ZLVec2D& p, float pad ) const;
	void					Print					() const;
	void					RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void					RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	ZLResultCode			ReservePolygons			( size_t size );
	ZLResultCode			ReserveVertices			( size_t idx, size_t size );
	void					ReverseWinding			( const MOAIRegion& region );
	void					SerializeIn				( MOAIRubyState& state, MOAIDeserializer& serializer );
	void					SerializeOut			( MOAIRubyState& state, MOAISerializer& serializer );
	void					SetWinding				( u32 winding );
	void					Snap					( const MOAIRegion& region, float xSnap, float ySnap );
	void					Stroke					( const MOAIRegion& region, float exterior, bool strokeExterior, float interior, bool strokeInterior );
	int						Tesselate				( const MOAIRegion& region, int windingRule );
	void					Transform				( const MOAIRegion& region, const ZLAffine2D& transform );
};

#endif
