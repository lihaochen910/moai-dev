// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <tesselator.h>

#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGfxResourceClerk.h>
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAIIndexBuffer.h>
#include <moai-sim/MOAIMaterialMgr.h>
#include <moai-sim/MOAIMesh.h>
#include <moai-sim/MOAIMeshSparseQuadTree.h>
#include <moai-sim/MOAIMeshTernaryTree.h>
#include <moai-sim/MOAIRegion.h>
#include <moai-sim/MOAIShader.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAITextureBase.h>
#include <moai-sim/MOAIVectorUtil.h>
#include <moai-sim/MOAIVertexFormat.h>

//================================================================//
// MOAIMeshPrim
//================================================================//
	
//----------------------------------------------------------------//
ZLBox MOAIMeshPrimCoords::GetBounds () {

	ZLBox bounds;
	bounds.Init ( this->mCoords [ 0 ]);
	
	if ( this->mPrimSize > 1 ) {
	
		bounds.Grow ( this->mCoords [ 1 ]);
	
		if ( this->mPrimSize > 2 ) {
			bounds.Grow ( this->mCoords [ 2 ]);
		}
	}
	return bounds;
}

//================================================================//
// MOAIMeshPrimReader
//================================================================//

//----------------------------------------------------------------//
bool MOAIMeshPrimReader::GetPrimCoords ( u32 idx, MOAIMeshPrimCoords& prim ) const {

	// TODO: should return index instead of bool?
	
	assert ( this->mMesh && this->mVertexFormat && this->mVertexBuffer );
	
	prim.mIndex = idx;
	
	switch ( this->mMesh->mPrimType ) {
		
		case ZGL_PRIM_POINTS: {
		
			prim.mPrimSize = 1;
			prim.mCoords [ 0 ] = this->ReadCoord ( idx );
			return true;
		}
		
		case ZGL_PRIM_LINES:
		
			idx = idx * 2;

		case ZGL_PRIM_LINE_LOOP:
		case ZGL_PRIM_LINE_STRIP: {
		
			prim.mPrimSize = 2;
			
			prim.mCoords [ 0 ] = this->ReadCoord ( idx++ );
			prim.mCoords [ 1 ] = this->ReadCoord ( idx );
			return true;
		}
		
		case ZGL_PRIM_TRIANGLES: {
		
			prim.mPrimSize = 3;
		
			idx = idx * 3;
			
			prim.mCoords [ 0 ] = this->ReadCoord ( idx++ );
			prim.mCoords [ 1 ] = this->ReadCoord ( idx++ );
			prim.mCoords [ 2 ] = this->ReadCoord ( idx );
			return true;
		}
		
		case ZGL_PRIM_TRIANGLE_FAN: {
		
			prim.mPrimSize = 3;
		
			prim.mCoords [ 0 ] = this->ReadCoord ( 0 );
		
			idx = idx + 1;
		
			prim.mCoords [ 1 ] = this->ReadCoord ( idx++ );
			prim.mCoords [ 2 ] = this->ReadCoord ( idx );
			return true;
		}
		
		case ZGL_PRIM_TRIANGLE_STRIP: {
		
			// 0   1   2   3   4   5   6
			// 012 213 234 435 456 657 678
		
			prim.mPrimSize = 3;
		
			if ( idx & 1 ) {
				
				// odd
				prim.mCoords [ 0 ] = this->ReadCoord ( idx + 1 );
				prim.mCoords [ 1 ] = this->ReadCoord ( idx );
				prim.mCoords [ 2 ] = this->ReadCoord ( idx + 2 );
			}
			else {
			
				// even
				prim.mCoords [ 0 ] = this->ReadCoord ( idx++ );
				prim.mCoords [ 1 ] = this->ReadCoord ( idx++ );
				prim.mCoords [ 2 ] = this->ReadCoord ( idx );
			}
			return true;
		}
	}
	
	return false;
}

//----------------------------------------------------------------//
bool MOAIMeshPrimReader::Init ( MOAIMesh& mesh, u32 vertexBufferIndex ) {

	MOAIVertexFormat* vertexFormat = mesh.GetVertexFormat ( vertexBufferIndex );
	MOAIVertexBuffer* vertexBuffer = mesh.GetVertexBuffer ( vertexBufferIndex );

	if ( !( vertexFormat && vertexBuffer )) return false;

	if ( !vertexFormat->CountAttributesByUse ( MOAIVertexFormat::ATTRIBUTE_COORD )) return false;

	this->mMesh				= &mesh;
	this->mVertexFormat		= vertexFormat;
	
	this->mAttribute		= vertexFormat->GetAttributeByUse ( MOAIVertexFormat::ATTRIBUTE_COORD, 0 );
	this->mVertexBuffer		= vertexBuffer->ZLCopyOnWrite::GetBuffer ();
	this->mIndexBuffer		= mesh.mIndexBuffer;
	
	this->mTotalPrims = 0;
	
	switch ( this->mMesh->mPrimType ) {
		
		case ZGL_PRIM_POINTS:
		case ZGL_PRIM_LINE_LOOP:
			this->mTotalPrims = mesh.mTotalElements;
			break;
		
		case ZGL_PRIM_LINES:
			this->mTotalPrims = mesh.mTotalElements / 2;
			break;
		
		case ZGL_PRIM_LINE_STRIP:
			this->mTotalPrims = mesh.mTotalElements - 1;
			break;
		
		case ZGL_PRIM_TRIANGLES:
			this->mTotalPrims = mesh.mTotalElements / 3;
			break;
		
		case ZGL_PRIM_TRIANGLE_FAN:
		case ZGL_PRIM_TRIANGLE_STRIP:
			this->mTotalPrims = mesh.mTotalElements - 2;
			break;
	}
	
	return true;
}

//----------------------------------------------------------------//
ZLVec3D MOAIMeshPrimReader::ReadCoord ( u32 idx ) const {

	if ( !( this->mMesh && this->mVertexFormat && this->mAttribute && this->mVertexBuffer )) return ZLVec3D::ORIGIN; // TODO: report error

	idx %= this->mMesh->mTotalElements;

	if ( this->mIndexBuffer ) {
		idx = this->mIndexBuffer->GetIndex ( idx );
	}
	
	const void* packedCoord = this->mVertexFormat->GetAttributeAddress ( *this->mAttribute, this->mVertexBuffer, idx );
	ZLVec4D coord = this->mVertexFormat->UnpackCoord ( packedCoord, *this->mAttribute );
	
	return ZLVec3D ( coord.mX, coord.mY, coord.mZ );
}

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMesh::_buildQuadTree ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMesh, "U" )

	u32 targetPrimsPerNode		= state.GetParamValue < u32 >( 1, MOAIMeshSparseQuadTree::DEFAULT_TARGET_PRIMS_PER_NODE );
	u32 vertexBufferIndex		= state.GetParamValue < u32 >( 2, 1 ) - 1;

	MOAIMeshPrimReader coordReader;
	
	if ( coordReader.Init ( *self, vertexBufferIndex )) {
	
		MOAIMeshSparseQuadTree* quadTree = new MOAIMeshSparseQuadTree ();
		quadTree->Init ( coordReader, targetPrimsPerNode );
		self->mPartition = quadTree;
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMesh::_buildTernaryTree ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMesh, "U" )
	
	u32 axisMask				= state.GetParamValue < u32 >( 1, MOAIMeshTernaryTree::AXIS_MASK_ALL );
	u32 targetPrimsPerNode		= state.GetParamValue < u32 >( 2, MOAIMeshTernaryTree::DEFAULT_TARGET_PRIMS_PER_NODE );
	u32 vertexBufferIndex		= state.GetParamValue < u32 >( 3, 1 ) - 1;
	
	MOAIMeshPrimReader coordReader;
	
	if ( coordReader.Init ( *self, vertexBufferIndex )) {
	
		MOAIMeshTernaryTree* ternaryTree = new MOAIMeshTernaryTree ();
		ternaryTree->Init ( coordReader, targetPrimsPerNode, axisMask );
		self->mPartition = ternaryTree;
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIMesh::_getPrimsForPoint ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMesh, "U" )
	
	// TODO: this is a naive first pass. need to use the partition if one has been created.
	// TODO: handle all prim types or bail if not triangles
	
	MOAIMeshPrimReader primReader;
	
	bool is3D = state.CheckParams ( 1, "NNN", false );
	
	ZLVec3D point = state.GetParamValue < ZLVec3D >( 1, ZLVec3D::ORIGIN );

	mrb_value ary = mrb_ary_new ( M );

	ZLBox meshBounds = self->GetBounds ();
	if ((( is3D ) && meshBounds.Contains ( point )) || meshBounds.Contains ( point, ZLBox::PLANE_XY )) {
		
		if ( primReader.Init ( *self, 0 )) {
			
			u32 basePrim = state.GetParamValue < u32 >( 4, 1 ) - 1;
			u32 nPrims = state.GetParamValue < u32 >( 5, primReader.GetTotalPrims ());
			
			for ( u32 i = basePrim; i < nPrims; ++i ) {
				
				MOAIMeshPrimCoords prim;
				if ( primReader.GetPrimCoords ( i, prim )) {
					
					if ((
						( is3D )
						&&
						ZLBarycentric::PointInTriangle (
							prim.mCoords [ 0 ],
							prim.mCoords [ 1 ],
							prim.mCoords [ 2 ],
							point ))
						||
						ZLBarycentric::PointInTriangle (
							prim.mCoords [ 0 ].Vec2D (),
							prim.mCoords [ 1 ].Vec2D (),
							prim.mCoords [ 2 ].Vec2D (),
							point.Vec2D ())
						) {
						
						mrb_ary_push ( M, ary, state.ToRValue ( i + 1 ) );
					}
				}
			}
		}
	}
	return ary;
}

//----------------------------------------------------------------//
mrb_value MOAIMesh::_getRegionForPrim ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMesh, "U" )

	// TODO: non-triangle meshes? need to support or error out

	u32 basePrimID = state.GetParamValue < u32 >( 1, 1 ) - 1;
	u32 nPrims = state.GetParamValue < u32 >( 2, 1 );

	SafeTesselator tesselator;

	MOAIMeshPrimReader primReader;
	
	if ( primReader.Init ( *self, 0 )) {
		
		for ( u32 i = 0; i < nPrims; ++i ) {
		
			MOAIMeshPrimCoords prim;
			if ( primReader.GetPrimCoords ( basePrimID + i, prim )) {
				
				assert ( prim.mPrimSize == 3 );
				
				ZLVec2D triangle [ 3 ];
				
				triangle [ 0 ] = prim.mCoords [ 0 ].Vec2D ();
				triangle [ 1 ] = prim.mCoords [ 1 ].Vec2D ();
				triangle [ 2 ] = prim.mCoords [ 2 ].Vec2D ();
				
				tesselator.AddContour ( 2, triangle, sizeof ( ZLVec2D ), 3 );
			}
		}
	}
	
	tesselator.Tesselate ( TESS_WINDING_NONZERO, TESS_BOUNDARY_CONTOURS, 0, 0 );

	MOAIRegion* region = state.GetRubyObject < MOAIRegion >( 3, false );
	region = region ? region : state.CreateClassInstance < MOAIRegion >();

	region->Copy ( tesselator );

	return state.ToRValue < MOAIRubyObject* >( region );
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMesh::_intersectRay ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMesh, "U" )
	
	// TODO: this is a naive first pass. need to use the partition if one has been created.
	// TODO: handle all prim types or bail if not triangles
	
	MOAIMeshPrimReader primReader;
	
	ZLVec3D loc		= state.GetParamValue < ZLVec3D >( 1, ZLVec3D::ORIGIN );
	ZLVec3D vec		= state.GetParamValue < ZLVec3D >( 4, ZLVec3D::ORIGIN );
	
	bool hasHit = false;
	float bestTime = 0.0f;
	ZLVec3D bestHit;
	
	if ( primReader.Init ( *self, 0 )) {
	
		u32 totalMeshPrims = primReader.GetTotalPrims ();
		
		for ( u32 i = 0; i < totalMeshPrims; ++i ) {
		
			MOAIMeshPrimCoords prim;
			if ( primReader.GetPrimCoords ( i, prim )) {
				
				float time;
				ZLVec3D hit;
				
				if ( ZLSect::VecToTriangle ( loc, vec, prim.mCoords [ 0 ], prim.mCoords [ 1 ], prim.mCoords [ 2 ], time, hit ) == ZLSect::SECT_HIT ) {
				
					if (( !hasHit ) || ( time < bestTime )) {
						bestTime = time;
						bestHit = hit;
						hasHit = true;
					}
				}
			}
		}
	}
	
	if ( hasHit ) {
		mrb_value ret [ 4 ];
		ret [ 0 ] = state.ToRValue ( bestTime );
		ret [ 1 ] = state.ToRValue ( bestHit.mX );
		ret [ 2 ] = state.ToRValue ( bestHit.mY );
		ret [ 3 ] = state.ToRValue ( bestHit.mZ );

		return mrb_ary_new_from_values ( state, 4, ret );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMesh::_printPartition ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMesh, "U" )

	if ( self->mPartition ) {
		self->mPartition->Print ();
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMesh::_readPrimCoords ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMesh, "U" )

	MOAIStream* stream = state.GetRubyObject < MOAIStream >( 1, true );
	
	if ( stream ) {
	
		u32 basePrimID = state.GetParamValue < u32 >( 2, 1 ) - 1;
		u32 nPrims = state.GetParamValue < u32 >( 3, 1 );

		MOAIMeshPrimReader primReader;
		
		if ( primReader.Init ( *self, 0 )) {
			
			for ( u32 i = 0; i < nPrims; ++i ) {
			
				MOAIMeshPrimCoords prim;
				if ( primReader.GetPrimCoords ( basePrimID + i, prim )) {
					
					stream->Write < ZLVec3D >( prim.mCoords [ 0 ]);
					
					if ( prim.mPrimSize > 1 ) {
					
						stream->Write < ZLVec3D >( prim.mCoords [ 1 ]);
						
						if ( prim.mPrimSize > 2 ) {
						
							stream->Write < ZLVec3D >( prim.mCoords [ 2 ]);
						}
					}
				}
			}
		}
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIMesh::_setBounds ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMesh, "U" )
	
	self->ClearBounds ();
	
	/*if ( state.CheckParams ( 1, "NNNNNN-" )) {
		self->SetBounds ( state.GetParamValue < ZLBox >( 1, self->mBounds ));
	}*/
	self->SetBounds ( state.GetParamValue < ZLBox > ( 1, self->mBounds ) );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setIndexBuffer
	@text	Set the index buffer to render.
	
	@in		MOAIMesh self
	@in		MOAIGfxBuffer indexBuffer
	@out	nil
*/
mrb_value MOAIMesh::_setIndexBuffer ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMesh, "U" )
	
	self->SetIndexBuffer ( state.GetRubyObject < MOAIIndexBuffer >( 1, true ));
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setPenWidth
	@text	Sets the pen with for drawing prims in this vertex buffer.
			Only valid with prim types GL_LINES, GL_LINE_LOOP, GL_LINE_STRIP.
	
	@in		MOAIMesh self
	@in		number penWidth
	@out	nil
*/
mrb_value MOAIMesh::_setPenWidth ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMesh, "UN" )
	
	self->mPenWidth = state.GetParamValue < float >( 1, 1.0f );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setPrimType
	@text	Sets the prim type the buffer represents.
	
	@in		MOAIMesh self
	@in		number primType		One of MOAIMesh GL_POINTS, GL_LINES, GL_TRIANGLES, GL_LINE_LOOP,
								GL_LINE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLE_STRIP
	@out	nil
*/
mrb_value MOAIMesh::_setPrimType ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMesh, "UN" )
	
	self->SetPrimType ( state.GetParamValue < u32 >( 1, 0 ));
	return mrb_nil_value ();
}

//----------------------------------------------------------------///
// TODO: doxygen
mrb_value MOAIMesh::_setTotalElements ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIMesh, "U" )

	self->SetTotalElements ( state.GetParamValue < u32 >( 1, 0 ));
	return mrb_nil_value ();
}

//================================================================//
// MOAIMesh
//================================================================//

//----------------------------------------------------------------//
void MOAIMesh::ClearBounds () {

	this->mBounds = ZLBounds::EMPTY;
}

//----------------------------------------------------------------//
u32 MOAIMesh::CountPrims () const {

	return 0;
}

//----------------------------------------------------------------//
void MOAIMesh::DrawIndex ( u32 idx, MOAIMeshSpan* span ) {

	MOAIMaterialMgr& materialStack = MOAIMaterialMgr::Get ();
	materialStack.Push ( this->GetMaterial ( idx ));
	materialStack.SetShader ( MOAIShaderMgr::MESH_SHADER );
	materialStack.LoadGfxState ();
	materialStack.Pop ();

	//if ( !this->LoadGfxState ( materials, idx, MOAIShaderMgr::MESH_SHADER )) return;

	// TODO: make use of offset and scale

	MOAIGfxState& gfxState = MOAIGfxMgr::Get ().mGfxState;
	if ( gfxState.SetVertexArray ( this )) {

		// I am super lazy, so set this up here instead of adding if's below
		MOAIMeshSpan defaultSpan;
		if ( !span ) {
			defaultSpan.mBase = 0;
			defaultSpan.mTop = this->mTotalElements;
			defaultSpan.mNext = 0;
			span = &defaultSpan;
		}
		
		gfxState.SetPenWidth ( this->mPenWidth );
		
		if ( this->mIndexBuffer ) {
			
			// TODO: turns out we can bind this inside the VAO as well. so there.
			if ( gfxState.SetIndexBuffer ( this->mIndexBuffer )) {
			
				for ( ; span; span = span->mNext ) {
				
					if ( span->mBase == span->mTop ) continue;
					assert (( span->mBase < span->mTop ) && ( span->mTop <= this->mTotalElements ));
				
					gfxState.DrawPrims ( this->mPrimType, span->mBase, ( u32 )( span->mTop - span->mBase ));
				}

				gfxState.SetIndexBuffer ();
			}
		}
		else {
		
			for ( ; span; span = span->mNext ) {
			
				if ( span->mBase == span->mTop ) continue;
				assert (( span->mBase < span->mTop ) && ( span->mTop <= this->mTotalElements ));
			
				gfxState.DrawPrims ( this->mPrimType, span->mBase, ( u32 )( span->mTop - span->mBase ));
			}
		}
		gfxState.SetVertexArray ();
	}
}

//----------------------------------------------------------------//
MOAIMesh::MOAIMesh () :
	mTotalElements ( 0 ),
	mPrimType ( ZGL_PRIM_TRIANGLES ),
	mPenWidth ( 1.0f ),
	mPartition ( 0 ) {

	RTTI_BEGIN
		RTTI_EXTEND ( MOAIDeck )
		RTTI_EXTEND ( MOAIMaterialBatchHolder )
		RTTI_EXTEND ( MOAIVertexArray )
	RTTI_END
	
	this->ClearBounds ();
}

//----------------------------------------------------------------//
MOAIMesh::~MOAIMesh () {

	this->SetIndexBuffer ( 0 );
}

//----------------------------------------------------------------//
void MOAIMesh::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	//MOAIDeck::RegisterRubyFuncs ( state, klass );
	MOAIDeck::RegisterRubyClass ( state, klass );
	MOAIMaterialBatchHolder::RegisterRubyClass ( state, klass );
	MOAIVertexArray::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "GL_POINTS",			( u32 )ZGL_PRIM_POINTS );
	state.DefineClassConst ( klass, "GL_LINES",			( u32 )ZGL_PRIM_LINES );
	state.DefineClassConst ( klass, "GL_TRIANGLES",		( u32 )ZGL_PRIM_TRIANGLES );
	state.DefineClassConst ( klass, "GL_LINE_LOOP",		( u32 )ZGL_PRIM_LINE_LOOP );
	state.DefineClassConst ( klass, "GL_LINE_STRIP",		( u32 )ZGL_PRIM_LINE_STRIP );
	state.DefineClassConst ( klass, "GL_TRIANGLE_FAN",		( u32 )ZGL_PRIM_TRIANGLE_FAN );
	state.DefineClassConst ( klass, "GL_TRIANGLE_STRIP",	( u32 )ZGL_PRIM_TRIANGLE_STRIP );
	
	state.DefineClassConst ( klass, "X_AXIS_MASK",			( u32 )MOAIMeshTernaryTree::X_AXIS_MASK );
	state.DefineClassConst ( klass, "Y_AXIS_MASK",			( u32 )MOAIMeshTernaryTree::Y_AXIS_MASK );
	state.DefineClassConst ( klass, "Z_AXIS_MASK",			( u32 )MOAIMeshTernaryTree::Z_AXIS_MASK );
	state.DefineClassConst ( klass, "AXIS_MASK_ALL",		( u32 )MOAIMeshTernaryTree::AXIS_MASK_ALL );
}

//----------------------------------------------------------------//
void MOAIMesh::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAIDeck::RegisterRubyFuncs ( state, klass );
	MOAIMaterialBatchHolder::RegisterRubyFuncs ( state, klass );
	MOAIVertexArray::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "buildQuadTree",			_buildQuadTree, MRB_ARGS_ARG ( 0, 2 ) );
	state.DefineInstanceMethod ( klass, "buildTernaryTree",			_buildTernaryTree, MRB_ARGS_ARG ( 0, 3 ) );
	state.DefineInstanceMethod ( klass, "getPrimsForPoint",			_getPrimsForPoint, MRB_ARGS_ARG ( 2, 1 ) );
	state.DefineInstanceMethod ( klass, "getRegionForPrim",			_getRegionForPrim, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "intersectRay",				_intersectRay, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "printPartition",			_printPartition, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "readPrimCoords",			_readPrimCoords, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "reserveVAOs",				_reserveVAOs, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "reserveVertexBuffers",		_reserveVertexBuffers, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setBounds",				_setBounds, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "setIndexBuffer",			_setIndexBuffer, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setPenWidth",				_setPenWidth, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setPrimType",				_setPrimType, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setTotalElements",			_setTotalElements, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setVertexBuffer",			_setVertexBuffer, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAIMesh::ReserveVAOs ( u32 total ) {

	for ( size_t i = 0; i < this->mVAOs.Size (); ++i ) {
		MOAIGfxResourceClerk::DeleteOrDiscard ( this->mVAOs [ i ], false );
	}
	this->mVAOs.Init ( total );
}

//----------------------------------------------------------------//
void MOAIMesh::ReserveVertexBuffers ( u32 total ) {

	for ( size_t i = 0; i < this->mVertexBuffers.Size (); ++i ) {
		this->mVertexBuffers [ i ].SetBufferAndFormat ( *this, 0, 0 );
	}
	this->mVertexBuffers.Init ( total );
}

//----------------------------------------------------------------//
void MOAIMesh::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {

	MOAIDeck::SerializeIn ( state, serializer );
	MOAIMaterialBatchHolder::SerializeIn ( state, serializer );
	MOAIVertexArray::SerializeIn ( state, serializer );

	/*this->SetIndexBuffer ( serializer.MemberIDToObject < MOAIIndexBuffer >( state.GetFieldValue < MOAISerializer::ObjID >( -1, "mIndexBuffer", 0 )));

	this->mTotalElements = state.GetFieldValue < u32 >( -1, "mTotalElements", 0 );

	if ( state.PushFieldWithType ( -1, "mBounds", LUA_TTABLE )) {

		this->mBounds.mMin.mX	= state.GetFieldValue < float >( -1, "mMinX", 0 );
		this->mBounds.mMin.mY	= state.GetFieldValue < float >( -1, "mMinY", 0 );
		this->mBounds.mMin.mZ	= state.GetFieldValue < float >( -1, "mMinZ", 0 );

		this->mBounds.mMax.mX	= state.GetFieldValue < float >( -1, "mMaxX", 0 );
		this->mBounds.mMax.mY	= state.GetFieldValue < float >( -1, "mMaxY", 0 );
		this->mBounds.mMax.mZ	= state.GetFieldValue < float >( -1, "mMaxZ", 0 );

		this->mBounds.UpdateStatus ();

		state.Pop ();
	}

	this->mPenWidth = state.GetFieldValue < float >( -1, "mPenWidth", 0 );*/
}

//----------------------------------------------------------------//
void MOAIMesh::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {

	MOAIDeck::SerializeOut ( state, serializer );
	MOAIMaterialBatchHolder::SerializeOut ( state, serializer );
	MOAIVertexArray::SerializeOut ( state, serializer );

	/*state.DefineClassConst ( klass, "mIndexBuffer", serializer.AffirmMemberID ( this->mIndexBuffer ));

	state.SetField < u32 >( -1, "mTotalElements", this->mTotalElements );

	lua_newtable ( state );

		state.SetField < float >( -1, "mMinX", this->mBounds.mMin.mX );
		state.SetField < float >( -1, "mMinY", this->mBounds.mMin.mY );
		state.SetField < float >( -1, "mMinZ", this->mBounds.mMin.mZ );

		state.SetField < float >( -1, "mMaxX", this->mBounds.mMax.mX );
		state.SetField < float >( -1, "mMaxY", this->mBounds.mMax.mY );
		state.SetField < float >( -1, "mMaxZ", this->mBounds.mMax.mZ );

	lua_setfield ( state, -2, "mBounds" );

	state.SetField < u32 >( -1, "mPenWidth", ( u32 )this->mPenWidth );*/
}

//----------------------------------------------------------------//
void MOAIMesh::SetBounds ( const ZLBox& bounds ) {

	this->mBounds.Init ( bounds );
}

//----------------------------------------------------------------//
void MOAIMesh::SetIndexBuffer ( MOAIIndexBuffer* indexBuffer ) {

	this->mIndexBuffer.Set ( *this, indexBuffer );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
ZLBounds MOAIMesh::MOAIDeck_ComputeMaxBounds () {

	return this->mBounds;
}

//----------------------------------------------------------------//
void MOAIMesh::MOAIDeck_Draw ( u32 idx ) {

	this->DrawIndex ( idx, 0 );
}

//----------------------------------------------------------------//
ZLBounds MOAIMesh::MOAIDeck_GetBounds ( u32 idx ) {
	UNUSED ( idx );

	return this->mBounds;
}
