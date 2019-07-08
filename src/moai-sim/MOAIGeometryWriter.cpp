 // Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <moai-sim/MOAIGeometryWriter.h>
#include <moai-sim/MOAIIndexBuffer.h>
#include <moai-sim/MOAIMesh.h>
#include <moai-sim/MOAIRegion.h>
#include <moai-sim/MOAIVertexBuffer.h>
#include <moai-sim/MOAIVertexFormat.h>

//================================================================//
// VertexComparator
//================================================================//

//----------------------------------------------------------------//
class VertexComparator {
public:

	ZLStream&					mStream;
	const MOAIVertexFormat&		mVertexFormat;
	
	void*	mBuffer0;
	void*	mBuffer1;
	
	//----------------------------------------------------------------//
	bool operator () ( const size_t v0, const size_t v1 ) const {
		
		size_t vertexSize = this->mVertexFormat.GetVertexSize ();
		
		this->mStream.SetCursor ( v0 );
		this->mStream.ReadBytes ( this->mBuffer0, vertexSize );
		
		this->mStream.SetCursor ( v1 );
		this->mStream.ReadBytes ( this->mBuffer1, vertexSize );
		
		return ( this->mVertexFormat.Compare ( this->mBuffer0, this->mBuffer1, 0.00001f, 0.00001f ) == -1 );
	}

	//----------------------------------------------------------------//
	VertexComparator ( ZLStream& stream, const MOAIVertexFormat& format, void* buffer0, void* buffer1 ) :
		mStream ( stream ),
		mVertexFormat ( format ),
		mBuffer0 ( buffer0 ),
		mBuffer1 ( buffer1 ) {
	}
};

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	applyColor
	@text	Apply a color to every vertex in a mesh. A MOAIRegion and/or
			a blending mode may optionally be provided.
			
			Blending mode is one of COLOR_ADD, COLOR_MULTIPLY, COLOR_OVERWRITE,
			COLOR_SUBTRACT.

	@overload
	
		@in		MOAIVertexFormat format
		@in		MOAIStream vtxStream
		@opt	number length
		@in		ZLColorVec color
		@opt	number blendMode
		@out 	nil
	
	@overload
	
		@in		MOAIVertexFormat format
		@in		MOAIStream vtxStream
		@opt	number length
		@in		ZLColorVec color
		@in		MOAIRegion region
		@opt	number pad
		@opt	number blendMode
		@out 	nil
*/
mrb_value MOAIGeometryWriter::_applyColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIGeometryWriter, "" )
	
	MOAIVertexFormat* format	= state.GetRubyObject < MOAIVertexFormat >( 1, true );
	MOAIStream* vtxStream			= state.GetRubyObject < MOAIStream >( 2, true );
	
	if ( format && vtxStream ) {
	
		size_t length = state.GetParamValue < u32 >( 3, ( u32 )( vtxStream->GetLength () - vtxStream->GetCursor ()));
	
		ZLColorVec color	= state.GetColor ( 4, 0.0f, 0.0f, 0.0f, 1.0f );
		MOAIRegion* region	= state.GetRubyObject < MOAIRegion >( 7, false );
		
		if ( region ) {
		
			float pad	= state.GetParamValue < float >( 9, 0.0f );
			u32 mode	= state.GetParamValue < u32 >( 10, COLOR_MULTIPLY );
			
			MOAIGeometryWriter::ApplyColor ( *format, *vtxStream, length, mode, *region, pad, color );
		}
		else {
		
			u32 mode	= state.GetParamValue < u32 >( 8, COLOR_MULTIPLY );
			MOAIGeometryWriter::ApplyColor ( *format, *vtxStream, length, mode, color );
		}
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	applyLightFromImage
	@text	Apply vertex colors from an image as a sperical map indexed by
			vertex normals. Middle of image is the equator, with the poles at
			image top and bottom. (Resolution diminishes as sample approaches
			poles.)
			
			An optional linear alpha gradient may be also be applied.
			
			Blending mode is one of COLOR_ADD, COLOR_MULTIPLY, COLOR_OVERWRITE,
			COLOR_SUBTRACT.
	
	@in		MOAIVertexFormat format
	@in		MOAIStream vtxStream
	@opt	number length
	@in		MOAIImage image
	@opt	number blendMode
	@opt	number a0
	@opt	number a1
	@opt	number x0
	@opt	number y0
	@opt	number z0
	@opt	number x1
	@opt	number y1
	@opt	number z1
	@out 	nil
*/
mrb_value MOAIGeometryWriter::_applyLightFromImage ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIGeometryWriter, "" )
	
	MOAIVertexFormat* format	= state.GetRubyObject < MOAIVertexFormat >( 1, true );
	MOAIStream* vtxStream		= state.GetRubyObject < MOAIStream >( 2, true );
	MOAIImage* image			= MOAIImage::AffirmImage ( state, 4 );
	
	if ( format && vtxStream && image ) {
	
		size_t length				= state.GetParamValue < u32 >( 3, ( u32 )( vtxStream->GetLength () - vtxStream->GetCursor ()));
	
		u32 mode					= state.GetParamValue < u32 >( 5, COLOR_MULTIPLY );
	
		bool gradient				= state.CheckParams ( 6, "NNNNNNNN", false );
		
		float a0					= state.GetParamValue < float >( 7, 0.0f );
		float a1					= state.GetParamValue < float >( 8, 0.0f );
		
		ZLVec3D v0					= state.GetVec3D ( 9, 0.0f );
		ZLVec3D v1					= state.GetVec3D ( 12, 0.0f );
	
		self->ApplyLightFromImage ( *format, *vtxStream, length, mode, *image, gradient, a0, a1, v0, v1 );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	applyLinearGradient
	@text	Apply a linear color gradient.
			
			Blending mode is one of COLOR_ADD, COLOR_MULTIPLY, COLOR_OVERWRITE,
			COLOR_SUBTRACT.
	
	@in		MOAIVertexFormat format
	@in		MOAIStream vtxStream
	@opt	number length
	@opt	number x0
	@opt	number y0
	@opt	number z0
	@opt	number x1
	@opt	number y1
	@opt	number z1
	@opt	number r0
	@opt	number g0
	@opt	number b0
	@opt	number a0
	@opt	number r1
	@opt	number g1
	@opt	number b1
	@opt	number a1
	@opt	boolean cap0
	@opt	boolean cap1
	@opt	number blendMode
	@out 	nil
*/
mrb_value MOAIGeometryWriter::_applyLinearGradient ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIGeometryWriter, "" )
	
	MOAIVertexFormat* format	= state.GetRubyObject < MOAIVertexFormat >( 1, true );
	MOAIStream* vtxStream		= state.GetRubyObject < MOAIStream >( 2, true );
	
	if ( format && vtxStream  ) {
	
		size_t length	= state.GetParamValue < u32 >( 3, ( u32 )( vtxStream->GetLength () - vtxStream->GetCursor ()));
	
		ZLVec3D v0		= state.GetVec3D ( 4, 0.0f );
		ZLVec3D v1		= state.GetVec3D ( 7, 0.0f );
	
		ZLColorVec c0	= state.GetColor ( 10, 0.0f, 0.0f, 0.0f, 1.0f );
		ZLColorVec c1	= state.GetColor ( 14, 0.0f, 0.0f, 0.0f, 1.0f );
		
		bool cap0		= state.GetParamValue < bool >( 18, false );
		bool cap1		= state.GetParamValue < bool >( 19, false );
		
		u32 mode		= state.GetParamValue < u32 >( 20, COLOR_MULTIPLY );
		
		MOAIGeometryWriter::ApplyLinearGradient ( *format, *vtxStream, length, mode, v0, v1, c0, c1, cap0, cap1 );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getMesh
	@text	Return the vertices as an MOAIVertexBuffer and MOAIIndexBuffer, or
			as a ready-to-render MOAIMesh.
			
	@overload
	
		@in		MOAIVertexFormat format
		@in		MOAIStream vtxStream
		@in		number vtxStreamLength
		@in		MOAIStream idxStream
		@in		number idxStreamLength
		@in		MOAIVertexBuffer vtxBuffer
		@in		MOAIIndexBuffer idxBuffer
		@opt	number idxSizeInBytes
		@out 	number totalElements
	
	@overload
	
		@in		MOAIVertexFormat format
		@in		MOAIStream vtxStream
		@in		number vtxStreamLength
		@in		MOAIStream idxStream
		@in		number idxStreamLength
		@opt	number idxSizeInBytes
		@out 	MOAIMesh mesh
*/
mrb_value MOAIGeometryWriter::_getMesh ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIGeometryWriter, "" )

	MOAIVertexFormat* format	= state.GetRubyObject < MOAIVertexFormat >( 1, true );
	MOAIStream* vtxStream		= state.GetRubyObject < MOAIStream >( 2, false );
	MOAIStream* idxStream		= state.GetRubyObject < MOAIStream >( 4, false );
	
	if ( format && vtxStream && idxStream ) {
	
		size_t vtxStreamLength = state.GetParamValue < u32 >( 3, ( u32 )( vtxStream->GetLength () - vtxStream->GetCursor ()));
		size_t idxStreamLength = state.GetParamValue < u32 >( 5, ( u32 )( idxStream->GetLength () - idxStream->GetCursor ()));
	
		MOAIVertexBuffer* vtxBuffer		= state.GetRubyObject < MOAIVertexBuffer >( 6, false );
		MOAIIndexBuffer* idxBuffer		= state.GetRubyObject < MOAIIndexBuffer >( 7, false );
	
		if ( vtxBuffer && idxBuffer ) {
			
			u32 idxSizeInBytes = state.GetParamValue < u32 >( 8, 4 );
			u32 totalElements = MOAIGeometryWriter::GetMesh ( *format, *vtxStream, vtxStreamLength, *idxStream, idxStreamLength, *vtxBuffer, *idxBuffer, idxSizeInBytes );
			
			return state.ToRValue ( totalElements );
		}
		else {
	
			u32 idxSizeInBytes = state.GetParamValue < u32 >( 6, 4 );
			MOAIMesh* mesh = MOAIGeometryWriter::GetMesh ( *format, *vtxStream, vtxStreamLength, *idxStream, idxStreamLength, idxSizeInBytes );
			return state.ToRValue < MOAIRubyObject* >( mesh );
		}
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIGeometryWriter::_offsetIndices ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIGeometryWriter, "" )
	
	MOAIStream* idxStream = state.GetRubyObject < MOAIStream >( 1, true );
	
	if ( idxStream ) {
	
		size_t length	= state.GetParamValue < u32 >( 2, ( u32 )( idxStream->GetLength () - idxStream->GetCursor ()));
		s32 offset		= state.GetParamValue < s32 >( 3, 0 );
	
		MOAIGeometryWriter::OffsetIndices ( *idxStream, length, offset );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	pruneVertices
	@text	Remove duplicate vertices from the vertex stream and update
			or create the index stream. Index stream is assuming to be 4 bytes.
	
	@in		MOAIVertexFormat format
	@in		MOAIStream vtxStream
	@in		MOAIStream idxStream
	@out 	nil
*/
mrb_value MOAIGeometryWriter::_pruneVertices ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIGeometryWriter, "" )
	
	MOAIVertexFormat* format	= state.GetRubyObject < MOAIVertexFormat >( 1, true );
	MOAIStream* vtxStream		= state.GetRubyObject < MOAIStream >( 2, true );
	MOAIStream* idxStream		= state.GetRubyObject < MOAIStream >( 3, true );
	
	if ( format && vtxStream && idxStream ) {
		MOAIGeometryWriter::PruneVertices ( *format, *vtxStream, *idxStream );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	snapCoords
	@text	Snap vertex coordinates to a give resolution.
	
	@in		MOAIVertexFormat format
	@in		MOAIStream vtxStream
	@opt	number length
	@in		number xSnap
	@opt	number ySnap		Default value is xSnap.
	@opt	number zSnap		Default valie is ySnap.
	@out 	nil
*/
mrb_value MOAIGeometryWriter::_snapCoords ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIGeometryWriter, "" )
	
	MOAIVertexFormat* format	= state.GetRubyObject < MOAIVertexFormat >( 1, true );
	MOAIStream* vtxStream		= state.GetRubyObject < MOAIStream >( 2, true );
	
	if ( format && vtxStream ) {
	
		size_t length	= state.GetParamValue < u32 >( 3, ( u32 )( vtxStream->GetLength () - vtxStream->GetCursor ()));
	
		float xSnap = state.GetParamValue < float >( 4, 0.0f );
		float ySnap = state.GetParamValue < float >( 5, xSnap );
		float zSnap = state.GetParamValue < float >( 6, ySnap );
	
		MOAIGeometryWriter::SnapCoords ( *format, *vtxStream, length, xSnap, ySnap, zSnap );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIGeometryWriter::_translateCoords ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIGeometryWriter, "" )
	
	MOAIVertexFormat* format	= state.GetRubyObject < MOAIVertexFormat >( 1, true );
	MOAIStream* vtxStream			= state.GetRubyObject < MOAIStream >( 2, true );
	
	if ( format && vtxStream ) {
	
		size_t length	= state.GetParamValue < u32 >( 3, ( u32 )( vtxStream->GetLength () - vtxStream->GetCursor ()));
	
		float xOff = state.GetParamValue < float >( 4, 0.0f );
		float yOff = state.GetParamValue < float >( 5, 0.0f );
		float zOff = state.GetParamValue < float >( 6, 0.0f );
	
		MOAIGeometryWriter::TranslateCoords ( *format, *vtxStream, length, xOff, yOff, zOff );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	writeBox
	@text	Writes the vertices of a box.
	
	@in		MOAIVertexFormat format
	@in		MOAIStream vtxStream
	@in		number xMin
	@in		number yMin
	@in		number zMin
	@in		number xMax
	@in		number yMax
	@in		number zMax
	@out 	nil
*/
mrb_value MOAIGeometryWriter::_writeBox ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIGeometryWriter, "" )
	
	MOAIVertexFormat* format	= state.GetRubyObject < MOAIVertexFormat >( 1, true );
	MOAIStream* stream			= state.GetRubyObject < MOAIStream >( 2, true );
	
	if ( format && stream  ) {
		MOAIGeometryWriter::WriteBox ( *format, *stream, state.GetBox ( 3 ));
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: implement
mrb_value MOAIGeometryWriter::_writeCone ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIGeometryWriter, "" )
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	writeCube
	@text	Writes the vertices of a cube.
	
	@in		MOAIVertexFormat format
	@in		MOAIStream vtxStream
	@opt	number size		Default value is 1.
	@opt	number x		Default value is 0.
	@opt	number y		Default value is 0.
	@opt	number z		Default value is 0.
	@out 	nil
*/
mrb_value MOAIGeometryWriter::_writeCube ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIGeometryWriter, "" )
	
	MOAIVertexFormat* format	= state.GetRubyObject < MOAIVertexFormat >( 1, true );
	MOAIStream* stream			= state.GetRubyObject < MOAIStream >( 2, true );
	
	if ( format && stream  ) {
	
		float size = state.GetParamValue < float >( 3, 1.0f );
		if ( size > 0.0f ) {
			
			ZLVec3D origin	= state.GetVec3D ( 4, 0.0f );
			
			float halfSize = size * 0.5f;
			
			ZLBox box;
			
			box.mMin.mX = origin.mX - halfSize;
			box.mMin.mY = origin.mY - halfSize;
			box.mMin.mZ = origin.mZ - halfSize;
			
			box.mMax.mX = origin.mX + halfSize;
			box.mMax.mY = origin.mY + halfSize;
			box.mMax.mZ = origin.mZ + halfSize;
	
			MOAIGeometryWriter::WriteBox ( *format, *stream, box );
		}
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: implement
mrb_value MOAIGeometryWriter::_writeCylinder ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIGeometryWriter, "" )
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: implement
mrb_value MOAIGeometryWriter::_writeIcoSphere ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIGeometryWriter, "" )
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: implement
mrb_value MOAIGeometryWriter::_writeUVSphere ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIGeometryWriter, "" )
	return mrb_nil_value ();
}

//================================================================//
// MOAIGeometryWriter
//================================================================//

//----------------------------------------------------------------//
void MOAIGeometryWriter::ApplyColor ( const MOAIVertexFormat& format, ZLStream& vtxStream, size_t length, u32 mode, const ZLColorVec& color ) {
	
	size_t base = vtxStream.GetCursor ();
	u32 total = ( u32 )( length / format.GetVertexSize ());
	
	for ( u32 i = 0; i < total; ++i ) {
		format.SeekVertex ( vtxStream, base, i );
		MOAIGeometryWriter::WriteColor ( format, vtxStream, mode, color.mR, color.mG, color.mB, color.mA );
	}
	format.SeekVertex ( vtxStream, base, total );
}

//----------------------------------------------------------------//
void MOAIGeometryWriter::ApplyColor ( const MOAIVertexFormat& format, ZLStream& vtxStream, size_t length, u32 mode, const MOAIRegion& region, float pad, const ZLColorVec& color ) {
	
	size_t base = vtxStream.GetCursor ();
	u32 total = ( u32 )( length / format.GetVertexSize ());
	
	for ( u32 i = 0; i < total; ++i ) {
	
		format.SeekVertex ( vtxStream, base, i );

		ZLVec3D coord = format.ReadCoord ( vtxStream, 0 );
		ZLVec2D point ( coord.mX, coord.mY );

		if ( region.PointInside ( point, pad )) {
			MOAIGeometryWriter::WriteColor ( format, vtxStream, mode, color.mR, color.mG, color.mB, color.mA );
		}
	}
	format.SeekVertex ( vtxStream, base, total );
}

//----------------------------------------------------------------//
void MOAIGeometryWriter::ApplyLightFromImage ( const MOAIVertexFormat& format, ZLStream& vtxStream, size_t length, u32 mode, MOAIImage& image, bool gradient, float a0, float a1, const ZLVec3D& v0, const ZLVec3D& v1 ) {
	
	float width		= ( float )image.GetWidth ();
	float height	= ( float )image.GetHeight ();
	
	size_t base = vtxStream.GetCursor ();
	u32 total = ( u32 )( length / format.GetVertexSize ());
	
	ZLVec3D norm = v1;
	norm.Sub ( v0 );
	float rayLength = norm.Norm ();
	
	ZLPlane3D plane;
	plane.Init ( v0, norm );
	
	for ( u32 i = 0; i < total; ++i ) {
	
		format.SeekVertex ( vtxStream, base, i );
	
		float alpha = 1.0f;
	
		if ( gradient ) {
			
			ZLVec3D coord = format.ReadCoord ( vtxStream, 0 );
			float dist = ZLDist::VecToPlane ( coord, plane );
			
			if ( dist <= 0.0f ) {
				alpha = a0;
			}
			else if ( dist >= rayLength ) {
				alpha = a1;
			}
			else {
				alpha = a0 + (( a1 - a0 ) * ( dist / rayLength ));
			}
		}
		
		ZLVec3D normal = format.ReadNormal ( vtxStream, 0 );

		ZLVec2D hVec ( normal.mX, normal.mY );
		hVec.Norm ();

		float x = ( float )(( hVec.Radians () / TWOPI ) * width );
		float y = ( float )(( normal.Radians ( ZLVec3D::Z_AXIS ) / PI ) * height );

		u32 sampleColor = image.SampleColor ( x, y, MOAIImage::FILTER_LINEAR, true, true );
		ZLColorVec color ( sampleColor );

		MOAIGeometryWriter::WriteColor ( format, vtxStream, mode, color.mR, color.mG, color.mB, color.mA * alpha );
	}
	
	format.SeekVertex ( vtxStream, base, total );
}

//----------------------------------------------------------------//
void MOAIGeometryWriter::ApplyLinearGradient ( const MOAIVertexFormat& format, ZLStream& vtxStream, size_t length, u32 mode, const ZLVec3D& v0, const ZLVec3D& v1, const ZLColorVec& c0, const ZLColorVec& c1, bool cap0, bool cap1 ) {
	
	ZLVec3D norm = v1;
	norm.Sub ( v0 );
	float rayLength = norm.Norm ();
	
	ZLPlane3D plane;
	plane.Init ( v0, norm );
	
	size_t base = vtxStream.GetCursor ();
	u32 total = ( u32 )( length / format.GetVertexSize ());
	
	for ( u32 i = 0; i < total; ++i ) {
	
		format.SeekVertex ( vtxStream, base, i );
	
		ZLVec3D coord = format.ReadCoord ( vtxStream, 0 );
		float dist = ZLDist::VecToPlane ( coord, plane );
		
		ZLColorVec color;
		
		if ( dist <= 0.0f ) {
		
			if ( cap0 && ( dist < 0.0f )) continue;
			color = c0;
		}
		else if ( dist >= rayLength ) {
		
			if ( cap1 && ( dist > rayLength )) continue;
			color = c1;
		}
		else {
		
			color.Lerp ( ZLInterpolate::kLinear, c0, c1, dist / rayLength );
		}
		MOAIGeometryWriter::WriteColor ( format, vtxStream, mode, color.mR, color.mG, color.mB, color.mA );
	}
	
	format.SeekVertex ( vtxStream, base, total );
}

//----------------------------------------------------------------//
MOAIMesh* MOAIGeometryWriter::GetMesh ( const MOAIVertexFormat& format, ZLStream& vtxStream, size_t vtxStreamLength, ZLStream& idxStream, size_t idxStreamLength, u32 idxSizeInBytes ) {
	MOAIRubyState state = MOAIRubyRuntime::Get ().State ();

	MOAIVertexBuffer* vtxBuffer = state.CreateClassInstance < MOAIVertexBuffer >();
	MOAIIndexBuffer* idxBuffer = state.CreateClassInstance < MOAIIndexBuffer >();

	u32 totalElements = MOAIGeometryWriter::GetMesh ( format, vtxStream, vtxStreamLength, idxStream, idxStreamLength, *vtxBuffer, *idxBuffer, idxSizeInBytes );
	
	MOAIMesh* mesh = state.CreateClassInstance < MOAIMesh >();
	
	mesh->SetVertexBuffer ( 0, vtxBuffer, ( MOAIVertexFormat* )&format ); // not ideal, but we're gonna do it
	mesh->SetIndexBuffer ( idxBuffer );

	mesh->SetPrimType ( ZGL_PRIM_TRIANGLES );
	mesh->SetTotalElements ( totalElements );
	
	ZLBox bounds;
	format.ComputeBounds ( bounds, vtxBuffer->GetSharedConstBuffer ()->GetConstData (), vtxBuffer->GetSize ());
	
	mesh->SetBounds ( bounds );
	
	return mesh;
}

//----------------------------------------------------------------//
u32 MOAIGeometryWriter::GetMesh ( const MOAIVertexFormat& format, ZLStream& vtxStream, size_t vtxStreamLength, ZLStream& idxStream, size_t idxStreamLength, MOAIVertexBuffer& vtxBuffer, MOAIIndexBuffer& idxBuffer, u32 idxSizeInBytes ) {
	
	vtxStream.Seek ( 0, SEEK_SET );
	idxStream.Seek ( 0, SEEK_SET );
	
//	if ( vtxBuffer.GetBufferCount () == 0 ) {
//		vtxBuffer.ReserveVBOs ( 1 );
//	}
//	
//	if ( idxBuffer.GetBufferCount () == 0 ) {
//		idxBuffer.ReserveVBOs ( 1 );
//	}
	
	vtxBuffer.CopyFromStream ( vtxStream, vtxStreamLength );
	
	idxBuffer.SetIndexSize ( idxSizeInBytes );
	
	if ( idxStream.GetLength ()) {
		
		idxBuffer.CopyFromStream ( idxStream, idxStreamLength, 4 );
	}
	else {
	
		size_t totalVertices = vtxBuffer.GetSize () / format.GetVertexSize ();
		for ( size_t i = 0; i < totalVertices; ++i ) {
			idxBuffer.WriteIndex (( u32 )i ); // TODO: check overflow
		}
	}
	
	return ( u32 )( idxBuffer.GetSize () / idxSizeInBytes );
}

//----------------------------------------------------------------//
MOAIGeometryWriter::MOAIGeometryWriter () {
	
	RTTI_SINGLE ( MOAIRubyObject )
}

//----------------------------------------------------------------//
MOAIGeometryWriter::~MOAIGeometryWriter () {
}

//----------------------------------------------------------------//
void MOAIGeometryWriter::OffsetIndices ( ZLStream& idxStream, size_t length, s32 offset ) {

	size_t totalIdx = length >> 2;
	if ( !totalIdx ) return;

	for ( size_t i = 0; i < totalIdx; ++i ) {
		
		u32 idx = idxStream.Read < u32 >( 0 );
		idxStream.Seek ( -4, SEEK_CUR );
		idxStream.Write < u32 >(( u32 )( idx + offset ));
	}
}

//----------------------------------------------------------------//
void MOAIGeometryWriter::PruneVertices ( const MOAIVertexFormat& format, MOAIStream& vtxStream, MOAIStream& idxStream ) {

	size_t vtxStreamBase = vtxStream.GetCursor ();

	u32 vtxSize = format.GetVertexSize ();
	size_t inputVtxCount = ( vtxStream.GetLength () - vtxStreamBase ) / vtxSize;
	
	if ( !inputVtxCount ) return;
	
	ZLLeanArray < u32 > indexMap;
	indexMap.Init ( inputVtxCount );
	
	void* vtxBuffer0 = alloca ( vtxSize );
	void* vtxBuffer1 = alloca ( vtxSize );

	typedef STLMap < size_t, u32, VertexComparator > VertexMap;
	typedef VertexMap::iterator VertexMapIt;

	VertexMap vertexMap ( VertexComparator ( vtxStream, format, vtxBuffer0, vtxBuffer1 ));
	
	u32 vtxCount = 0;
	
	for ( u32 i = 0; i < inputVtxCount; ++i ) {
	
		size_t vertex = vtxStreamBase + ( i * vtxSize );
		
		u32 index = 0;
		
		VertexMapIt existing = vertexMap.find ( vertex );
		
		if ( existing != vertexMap.end ()) {
			index = existing->second;
			//printf ( "REDUCE: %d -> %d\n", i, index );
		}
		else {
		
			vtxStream.SetCursor ( vertex );
			vtxStream.ReadBytes ( vtxBuffer0, vtxSize );
		
			index = vtxCount++;
			vertex = vtxStreamBase + ( index * vtxSize );
			
			vtxStream.SetCursor ( vertex );
			vtxStream.WriteBytes ( vtxBuffer0, vtxSize );
			
			vertexMap [ vertex ] = index;
		}
		indexMap [ i ] = index;
		
		//printf ( "INDEX: %d -> %d\n", i, index );
	}
	
	size_t totalIdx = idxStream.GetLength () >> 2; // index size is assumed to always be 4 bytes
	
	if ( totalIdx ) {
	
		for ( size_t i = 0; i < totalIdx; ++i ) {
		
			u32 idx = idxStream.Read < u32 >( 0 );
			idx = indexMap [ idx ];
			idxStream.Seek ( -4, SEEK_CUR );
			idxStream.Write < u32 >( idx );
		}
	}
	else {
		
		for ( size_t i = 0; i < inputVtxCount; ++i ) {
			
			u32 idx = indexMap [ i ];
			idxStream.Write < u32 >( idx );
		}
	}
	
	if ( vtxCount < inputVtxCount ) {
		vtxStream.SetLength ( vtxCount * vtxSize );
		vtxStream.Compact ();
	}
}

//----------------------------------------------------------------//
void MOAIGeometryWriter::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "COLOR_ADD",				( u32 )COLOR_ADD );
	state.DefineClassConst ( klass, "COLOR_MULTIPLY",			( u32 )COLOR_MULTIPLY );
	state.DefineClassConst ( klass, "COLOR_OVERWRITE",			( u32 )COLOR_OVERWRITE );
	state.DefineClassConst ( klass, "COLOR_SUBTRACT",			( u32 )COLOR_SUBTRACT );

	state.DefineStaticMethod ( klass, "applyColor",				_applyColor, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "applyLightFromImage",	_applyLightFromImage, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "applyLinearGradient",	_applyLinearGradient, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "getMesh",				_getMesh, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "offsetIndices",			_offsetIndices, MRB_ARGS_REQ ( 3 ) );
	state.DefineStaticMethod ( klass, "pruneVertices",			_pruneVertices, MRB_ARGS_REQ ( 2 ) );
	state.DefineStaticMethod ( klass, "snapCoords",				_snapCoords, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "translateCoords",		_translateCoords, MRB_ARGS_REQ ( 6 ) );
	state.DefineStaticMethod ( klass, "writeBox",				_writeBox, MRB_ARGS_REQ ( 8 ) );
	state.DefineStaticMethod ( klass, "writeCone",				_writeCone, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "writeCube",				_writeCube, MRB_ARGS_ARG ( 1, 4 ) );
	state.DefineStaticMethod ( klass, "writeCylinder",			_writeCylinder, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "writeIcoSphere",			_writeIcoSphere, MRB_ARGS_ANY () );
	state.DefineStaticMethod ( klass, "writeUVSphere",			_writeUVSphere, MRB_ARGS_ANY () );

}

//----------------------------------------------------------------//
void MOAIGeometryWriter::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIGeometryWriter::SnapCoords ( const MOAIVertexFormat& format, ZLStream& vtxStream, size_t length, float xSnap, float ySnap, float zSnap ) {
	
	size_t base = vtxStream.GetCursor ();
	u32 total = ( u32 )( length / format.GetVertexSize ());
	
	for ( u32 i = 0; i < total; ++i ) {
		
		format.SeekVertex ( vtxStream, base, i );
		
		ZLVec4D coord = format.ReadCoord ( vtxStream, 0 );
		
		coord.mX = xSnap == 0.0f ? coord.mX : floorf (( coord.mX / xSnap ) + 0.5f ) * xSnap;
		coord.mY = ySnap == 0.0f ? coord.mY : floorf (( coord.mY / ySnap ) + 0.5f ) * ySnap;
		coord.mZ = zSnap == 0.0f ? coord.mZ : floorf (( coord.mZ / zSnap ) + 0.5f ) * zSnap;
		
		format.WriteCoord ( vtxStream, 0, coord.mX, coord.mY, coord.mZ, coord.mW );
	}
}

//----------------------------------------------------------------//
void MOAIGeometryWriter::TranslateCoords ( const MOAIVertexFormat& format, ZLStream& vtxStream, size_t length, float xOff, float yOff, float zOff ) {
	
	size_t base = vtxStream.GetCursor ();
	u32 total = ( u32 )( length / format.GetVertexSize ());
	
	for ( u32 i = 0; i < total; ++i ) {
		
		format.SeekVertex ( vtxStream, base, i );
		
		ZLVec4D coord = format.ReadCoord ( vtxStream, 0 );
		
		coord.mX += xOff;
		coord.mY += yOff;
		coord.mZ += zOff;
		
		format.WriteCoord ( vtxStream, 0, coord.mX, coord.mY, coord.mZ, coord.mW );
	}
}

//----------------------------------------------------------------//
void MOAIGeometryWriter::WriteBox ( const MOAIVertexFormat& format, ZLStream& vtxStream, const ZLBox& box ) {

	float xMin = box.mMin.mX;
	float yMin = box.mMin.mY;
	float zMin = box.mMin.mZ;
	
	float xMax = box.mMax.mX;
	float yMax = box.mMax.mY;
	float zMax = box.mMax.mZ;
	
	// back
	ZLVec3D v0 ( xMax, yMin, zMin );
	ZLVec3D v1 ( xMin, yMin, zMin );
	ZLVec3D v2 ( xMin, yMax, zMin );
	ZLVec3D v3 ( xMax, yMax, zMin );

	// front
	ZLVec3D v4 ( xMin, yMin, zMax );
	ZLVec3D v5 ( xMax, yMin, zMax );
	ZLVec3D v6 ( xMax, yMax, zMax );
	ZLVec3D v7 ( xMin, yMax, zMax );

	MOAIGeometryWriter::WriteQuad ( format, vtxStream, v0, v1, v2, v3, ZLVec3D ( 0.0f, 0.0f, -1.0f )); // back
	MOAIGeometryWriter::WriteQuad ( format, vtxStream, v4, v5, v6, v7, ZLVec3D ( 0.0f, 0.0f, 1.0f )); // front
	
	MOAIGeometryWriter::WriteQuad ( format, vtxStream, v1, v4, v7, v2, ZLVec3D ( -1.0f, 0.0f, 0.0f )); // left
	MOAIGeometryWriter::WriteQuad ( format, vtxStream, v5, v0, v3, v6, ZLVec3D ( 1.0f, 0.0f, 0.0f )); // right

	MOAIGeometryWriter::WriteQuad ( format, vtxStream, v5, v4, v1, v0, ZLVec3D ( 0.0f, -1.0f, 0.0f )); // bottom
	MOAIGeometryWriter::WriteQuad ( format, vtxStream, v2, v7, v6, v3, ZLVec3D ( 0.0f, 1.0f, 0.0f )); // top
}

//----------------------------------------------------------------//
void MOAIGeometryWriter::WriteColor ( const MOAIVertexFormat& format, ZLStream& vtxStream, u32 mode, float r, float g, float b, float a ) {

	ZLColorVec dstColor ( r, g, b, a );
	
	if ( mode != COLOR_OVERWRITE ) {
	
		ZLColorVec srcColor = dstColor;
		ZLColorVec orgColor = format.ReadColor( vtxStream, 0 );
	
		dstColor = orgColor;

		switch ( mode ) {
		
			case COLOR_ADD:
				srcColor.mA = 0.0f;
				dstColor.AddAndClamp ( srcColor );
				break;

			case COLOR_MULTIPLY:
				srcColor.mA = 1.0f;
				dstColor.Modulate ( srcColor );
				break;

			case COLOR_SUBTRACT:
				srcColor.mA = 0.0f;
				dstColor.SubAndClamp ( srcColor );
				break;
		}

		dstColor.Lerp ( ZLInterpolate::kLinear, orgColor, dstColor, a );
	}
	format.WriteColor ( vtxStream, 0, dstColor.mR, dstColor.mG, dstColor.mB, dstColor.mA );
}

//----------------------------------------------------------------//
void MOAIGeometryWriter::WriteCylinder ( const MOAIVertexFormat& format, ZLStream& vtxStream, const ZLVec3D& v0, const ZLVec3D& v1, const ZLVec2D& r0, const ZLVec2D& r1 ) {
	UNUSED ( format );
	UNUSED ( vtxStream );
	UNUSED ( v0 );
	UNUSED ( v1 );
	UNUSED ( r0 );
	UNUSED ( r1 );
}

//----------------------------------------------------------------//
void MOAIGeometryWriter::WriteIcoSphere ( const MOAIVertexFormat& format, ZLStream& vtxStream, const ZLVec3D& v0, const ZLVec3D& r ) {
	UNUSED ( format );
	UNUSED ( vtxStream );
	UNUSED ( v0 );
	UNUSED ( r );
}

//----------------------------------------------------------------//
void MOAIGeometryWriter::WriteQuad ( const MOAIVertexFormat& format, ZLStream& vtxStream, const ZLVec3D& v0, const ZLVec3D& v1, const ZLVec3D& v2, const ZLVec3D& v3, const ZLVec3D& n0 ) {

	MOAIGeometryWriter::WriteQuad ( format, vtxStream, v0, v1, v2, v3, n0, n0, n0, n0 );
}

//----------------------------------------------------------------//
void MOAIGeometryWriter::WriteQuad ( const MOAIVertexFormat& format, ZLStream& vtxStream, const ZLVec3D& v0, const ZLVec3D& v1, const ZLVec3D& v2, const ZLVec3D& v3, const ZLVec3D& n0, const ZLVec3D& n1, const ZLVec3D& n2, const ZLVec3D& n3 ) {

	MOAIGeometryWriter::WriteVertex ( format, vtxStream, v0, n0 );
	MOAIGeometryWriter::WriteVertex ( format, vtxStream, v1, n1 );
	MOAIGeometryWriter::WriteVertex ( format, vtxStream, v2, n2 );
	
	MOAIGeometryWriter::WriteVertex ( format, vtxStream, v0, n0 );
	MOAIGeometryWriter::WriteVertex ( format, vtxStream, v2, n2 );
	MOAIGeometryWriter::WriteVertex ( format, vtxStream, v3, n3 );
}

//----------------------------------------------------------------//
void MOAIGeometryWriter::WriteUVSphere ( const MOAIVertexFormat& format, ZLStream& vtxStream, const ZLVec3D& v0, const ZLVec3D& r ) {
	UNUSED ( format );
	UNUSED ( vtxStream );
	UNUSED ( v0 );
	UNUSED ( r );
}

//----------------------------------------------------------------//
void MOAIGeometryWriter::WriteVertex ( const MOAIVertexFormat& format, ZLStream& vtxStream, const ZLVec3D& coord, const ZLVec3D& normal ) {

	size_t base = vtxStream.GetCursor ();
	format.WriteAhead ( vtxStream );
	format.WriteCoord ( vtxStream, 0, coord.mX, coord.mY, coord.mZ, 1.0f );
	format.WriteNormal ( vtxStream, 0, normal.mX, normal.mY, normal.mZ );
	format.SeekVertex ( vtxStream, base, 1 );
}
