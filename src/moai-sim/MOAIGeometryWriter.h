// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIGEOMETRYWRITER_H
#define	MOAIGEOMETRYWRITER_H

#include <moai-sim/MOAIImage.h>
#include <moai-sim/MOAIVertexFormat.h>

class MOAIIndexBuffer;
class MOAIMesh;
class MOAIRegion;
class MOAIVertexBuffer;
class MOAIVertexFormat;

//================================================================//
// MOAIGeometryWriter
//================================================================//
/**	@lua	MOAIGeometryWriter
	@text	Interface for writing and manipulating procedural geometry.
			Geometry is written as vertices to a stream. Vertex layout is
			given by a MOAIVertexFormat. Stream cursor is reset to starting
			location after each write (to make writing multiple passes
			easier).
			
			Operations that affect every vertex of a mesh affect all
			vertices between the stream cursor and the end of the stream.
 
			Once all vertices have been written, stream may be converted
			into a mesh with an index buffer that is calculated
			by finding redundant vertices.
*/
class MOAIGeometryWriter :
	public ZLContextClass < MOAIGeometryWriter, MOAIRubyObject > {
private:
	
	//----------------------------------------------------------------//
	static mrb_value			_applyColor				( mrb_state* M, mrb_value context );
	static mrb_value			_applyLightFromImage	( mrb_state* M, mrb_value context );
	static mrb_value			_applyLinearGradient	( mrb_state* M, mrb_value context );
	static mrb_value			_getMesh				( mrb_state* M, mrb_value context );
	static mrb_value			_offsetIndices			( mrb_state* M, mrb_value context );
	static mrb_value			_pruneVertices			( mrb_state* M, mrb_value context );
	static mrb_value			_snapCoords				( mrb_state* M, mrb_value context );
	static mrb_value			_translateCoords		( mrb_state* M, mrb_value context );
	static mrb_value			_writeBox				( mrb_state* M, mrb_value context );
	static mrb_value			_writeCone				( mrb_state* M, mrb_value context );
	static mrb_value			_writeCube				( mrb_state* M, mrb_value context );
	static mrb_value			_writeCylinder			( mrb_state* M, mrb_value context );
	static mrb_value			_writeIcoSphere			( mrb_state* M, mrb_value context );
	static mrb_value			_writeUVSphere			( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	static void			WriteColor				( const MOAIVertexFormat& format, ZLStream& stream, u32 mode, float r, float g, float b, float a );
	static void			WriteQuad				( const MOAIVertexFormat& format, ZLStream& stream, const ZLVec3D& v0, const ZLVec3D& v1, const ZLVec3D& v2, const ZLVec3D& v3, const ZLVec3D& n0 );
	static void			WriteQuad				( const MOAIVertexFormat& format, ZLStream& stream, const ZLVec3D& v0, const ZLVec3D& v1, const ZLVec3D& v2, const ZLVec3D& v3, const ZLVec3D& n0, const ZLVec3D& n1, const ZLVec3D& n2, const ZLVec3D& n3 );
	static void			WriteVertex				( const MOAIVertexFormat& format, ZLStream& stream, const ZLVec3D& coord, const ZLVec3D& normal );
	
public:
	
	DECL_RUBY_SINGLETON ( MOAIGeometryWriter )
	
	enum {
		COLOR_ADD,
		COLOR_MULTIPLY,
		COLOR_OVERWRITE,
		COLOR_SUBTRACT,
	};
	
	//----------------------------------------------------------------//
	static void			ApplyColor				( const MOAIVertexFormat& format, ZLStream& vtxStream, size_t length, u32 mode, const ZLColorVec& color );
	static void			ApplyColor				( const MOAIVertexFormat& format, ZLStream& vtxStream, size_t length, u32 mode, const MOAIRegion& region, float pad, const ZLColorVec& color );
	static void			ApplyLightFromImage		( const MOAIVertexFormat& format, ZLStream& vtxStream, size_t length, u32 mode, MOAIImage& image, bool gradient, float a0, float a1, const ZLVec3D& v0, const ZLVec3D& v1 );
	static void			ApplyLinearGradient		( const MOAIVertexFormat& format, ZLStream& vtxStream, size_t length, u32 mode, const ZLVec3D& v0, const ZLVec3D& v1, const ZLColorVec& c0, const ZLColorVec& c1, bool cap0, bool cap1 );
	static MOAIMesh*	GetMesh					( const MOAIVertexFormat& format, ZLStream& vtxStream, size_t vtxStreamLength, ZLStream& idxStream, size_t idxStreamLength, u32 idxSizeInBytes );
	static u32			GetMesh					( const MOAIVertexFormat& format, ZLStream& vtxStream, size_t vtxStreamLength, ZLStream& idxStream, size_t idxStreamLength, MOAIVertexBuffer& vtxBuffer, MOAIIndexBuffer& idxBuffer, u32 idxSizeInBytes );
						MOAIGeometryWriter		();
						~MOAIGeometryWriter		();
	static void			OffsetIndices			( ZLStream& idxStream, size_t length, s32 offset );
	static void			PruneVertices			( const MOAIVertexFormat& format, MOAIStream& vtxStream, MOAIStream& idxStream );
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	static void			SnapCoords				( const MOAIVertexFormat& format, ZLStream& vtxStream, size_t length, float xSnap, float ySnap, float zSnap );
	static void			TranslateCoords			( const MOAIVertexFormat& format, ZLStream& vtxStream, size_t length, float xOff, float yOff, float zOff );
	static void			WriteBox				( const MOAIVertexFormat& format, ZLStream& vtxStream, const ZLBox& box );
	static void			WriteCylinder			( const MOAIVertexFormat& format, ZLStream& vtxStream, const ZLVec3D& v0, const ZLVec3D& v1, const ZLVec2D& r0, const ZLVec2D& r1 );
	static void			WriteIcoSphere			( const MOAIVertexFormat& format, ZLStream& vtxStream, const ZLVec3D& v0, const ZLVec3D& r );
	static void			WriteUVSphere			( const MOAIVertexFormat& format, ZLStream& vtxStream, const ZLVec3D& v0, const ZLVec3D& r );
};

#endif
