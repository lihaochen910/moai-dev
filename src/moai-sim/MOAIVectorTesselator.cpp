// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIGeometryWriter.h>
#include <moai-sim/MOAIGfxBuffer.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIGrid.h>
#include <moai-sim/MOAIIndexBuffer.h>
#include <moai-sim/MOAIRegion.h>
#include <moai-sim/MOAIShaderMgr.h>
#include <moai-sim/MOAIVectorCombo.h>
#include <moai-sim/MOAIVectorEllipse.h>
#include <moai-sim/MOAIVectorTesselator.h>
#include <moai-sim/MOAIVectorPoly.h>
#include <moai-sim/MOAIVectorPoly.h>
#include <moai-sim/MOAIVectorRect.h>
#include <moai-sim/MOAIVectorUtil.h>
#include <moai-sim/MOAIVertexFormat.h>
#include <moai-sim/MOAIVertexFormatMgr.h>
#include <tesselator.h>

//================================================================//
// MOAIVectorTesselatorWriter
//================================================================//

//----------------------------------------------------------------//
MOAIVectorTesselatorWriter::MOAIVectorTesselatorWriter () :
	mFlushStyle ( true ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIVectorTesselatorWriter::~MOAIVectorTesselatorWriter () {
}

//----------------------------------------------------------------//
MOAIVectorShape* MOAIVectorTesselatorWriter::ReadShape ( ZLStream& stream ) {

	#define CMD_CASE(cmd, type, member, fallback)										\
		case MOAIVectorTesselatorWriter::cmd:											\
			this->mStyle.member = stream.Read < type >(( type )fallback );				\
			break;

	u8 cmd;

	do {
		
		cmd = stream.Read < u8 >( MOAIVectorTesselatorWriter::VECTOR_CMD_DONE ).Value ();
		
		switch ( cmd ) {
			
			CMD_CASE ( STYLE_CMD_FILL,					u8,				mFillStyle,				MOAIVectorStyle::FILL_SOLID )
			CMD_CASE ( STYLE_CMD_FILL_COLOR,			u32,			mFillColor,				0xffffffff )
			CMD_CASE ( STYLE_CMD_LINE,					u8,				mLineStyle,				MOAIVectorStyle::LINE_NONE )
			CMD_CASE ( STYLE_CMD_LINE_COLOR,			u32,			mLineColor,				0xffffffff )
			CMD_CASE ( STYLE_CMD_LINE_WIDTH,			float,			mLineWidth,				1.0f )
			CMD_CASE ( STYLE_CMD_STROKE,				u8,				mStrokeStyle,			MOAIVectorStyle::STROKE_NONE )
			CMD_CASE ( STYLE_CMD_STROKE_COLOR,			u32,			mStrokeColor,			0xffffffff )
			CMD_CASE ( STYLE_CMD_STROKE_WIDTH,			float,			mStrokeWidth,			1.0f )
			CMD_CASE ( STYLE_CMD_STROKE_DEPTH_BIAS,		float,			mStrokeDepthBias,		0.0f )
			CMD_CASE ( STYLE_CMD_JOIN,					u8,				mJoinStyle,				MOAIVectorStyle::JOIN_MITER )
			CMD_CASE ( STYLE_CMD_CAP,					u8,				mCapStyle,				MOAIVectorStyle::CAP_BUTT )
			CMD_CASE ( STYLE_CMD_MITER_LIMIT,			float,			mMiterLimit,			5.0f )
			CMD_CASE ( STYLE_CMD_WINDING_RULE,			u8,				mWindingRule,			TESS_WINDING_ODD )
			CMD_CASE ( STYLE_CMD_CIRCLE_RESOLUTION,		u16,			mCircleResolution,		MOAIVectorStyle::DEFAULT_CIRCLE_RESOLUTION )
			CMD_CASE ( STYLE_CMD_EXTRUDE,				float,			mExtrude,				0.0f )
			CMD_CASE ( STYLE_CMD_Z_OFFSET,				float,			mZOffset,				0.0f )
			CMD_CASE ( STYLE_CMD_LIGHT_VEC,				ZLVec3D,		mLightVec,				ZLVec3D::ORIGIN )
			CMD_CASE ( STYLE_CMD_LIGHT_COLOR,			u32,			mLightColor,			0xffffffff )
			CMD_CASE ( STYLE_CMD_LIGHT_CURVE,			u8,				mLightCurve,			ZLInterpolate::kLinear )
			CMD_CASE ( STYLE_CMD_SHADOW_COLOR,			u32,			mShadowColor,			0xff000000 )
			CMD_CASE ( STYLE_CMD_SHADOW_CURVE,			u8,				mShadowCurve,			ZLInterpolate::kLinear )
			CMD_CASE ( STYLE_CMD_FILL_EXTRA_ID,			u32,			mFillExtraID,			0 )
			CMD_CASE ( STYLE_CMD_STROKE_EXTRA_ID,		u32,			mStrokeExtraID,			0 )
			CMD_CASE ( STYLE_CMD_MERGE_NORMALS,			float,			mMergeNormals,			0.0f )
			
			case MOAIVectorTesselatorWriter::STYLE_CMD_DRAWING_TO_WORLD:
			
				this->mStyle.mDrawingToWorld = stream.Read ( this->mStyle.mDrawingToWorld );
				this->mStyle.mWorldToDrawing.Inverse ( this->mStyle.mDrawingToWorld );
				
				break;
			
			case MOAIVectorTesselatorWriter::VECTOR_CMD_SHAPE: {
				
				u8 shapeType = stream.Read < u8 >( MOAIVectorShape::UNKNOWN );
				MOAIVectorShape* shape = MOAIVectorShape::Create ( shapeType );
				assert ( shape );
				
				shape->mStyle = this->mStyle; // MUST DO THIS BEFORE READ!
				shape->Read ( stream, *this );
				
				return shape;
			}
			default:
				break;
		}
	}
	while ( cmd != MOAIVectorTesselatorWriter::VECTOR_CMD_DONE );
	
	return 0;
}

//----------------------------------------------------------------//
void MOAIVectorTesselatorWriter::WriteShape ( ZLStream& stream, const MOAIVectorShape& shape ) {

	bool forceWrite = this->mFlushStyle;
	this->mFlushStyle = false;
	
	#define WRITE_STYLE_CMD(cmd, type, member)										\
		if ( forceWrite || ( shape.mStyle.member != this->mStyle.member )) {		\
			stream.Write < u8 >(( u8 )MOAIVectorTesselatorWriter::cmd );			\
			stream.Write < type >(( type )shape.mStyle.member );					\
			this->mStyle.member = shape.mStyle.member;								\
		}
	
	#define WRITE_STYLE_CMD_FORCE(cmd, type, member)								\
		stream.Write < u8 >(( u8 )MOAIVectorTesselatorWriter::cmd );				\
		stream.Write < type >(( type )shape.mStyle.member );						\
		this->mStyle.member = shape.mStyle.member;									\

	WRITE_STYLE_CMD ( STYLE_CMD_FILL,					u8,				mFillStyle )
	WRITE_STYLE_CMD ( STYLE_CMD_FILL_COLOR,				u32,			mFillColor )
	WRITE_STYLE_CMD ( STYLE_CMD_LINE,					u8,				mLineStyle )
	WRITE_STYLE_CMD ( STYLE_CMD_LINE_COLOR,				u32,			mLineColor )
	WRITE_STYLE_CMD ( STYLE_CMD_LINE_WIDTH,				float,			mLineWidth )
	WRITE_STYLE_CMD ( STYLE_CMD_STROKE,					u8,				mStrokeStyle )
	WRITE_STYLE_CMD ( STYLE_CMD_STROKE_COLOR,			u32,			mStrokeColor )
	WRITE_STYLE_CMD ( STYLE_CMD_STROKE_WIDTH,			float,			mStrokeWidth )
	WRITE_STYLE_CMD ( STYLE_CMD_STROKE_DEPTH_BIAS,		float,			mStrokeDepthBias )
	WRITE_STYLE_CMD ( STYLE_CMD_JOIN,					u8,				mJoinStyle )
	WRITE_STYLE_CMD ( STYLE_CMD_CAP,					u8,				mCapStyle )
	WRITE_STYLE_CMD ( STYLE_CMD_MITER_LIMIT,			float,			mMiterLimit )
	WRITE_STYLE_CMD ( STYLE_CMD_WINDING_RULE,			u8,				mWindingRule )
	WRITE_STYLE_CMD ( STYLE_CMD_CIRCLE_RESOLUTION,		u16,			mCircleResolution )
	WRITE_STYLE_CMD ( STYLE_CMD_EXTRUDE,				float,			mExtrude )
	WRITE_STYLE_CMD ( STYLE_CMD_Z_OFFSET,				float,			mZOffset )
	WRITE_STYLE_CMD ( STYLE_CMD_LIGHT_VEC,				ZLVec3D,		mLightVec )
	WRITE_STYLE_CMD ( STYLE_CMD_LIGHT_COLOR,			u32,			mLightColor )
	WRITE_STYLE_CMD ( STYLE_CMD_LIGHT_CURVE,			u8,				mLightCurve )
	WRITE_STYLE_CMD ( STYLE_CMD_SHADOW_COLOR,			u32,			mShadowColor )
	WRITE_STYLE_CMD ( STYLE_CMD_SHADOW_CURVE,			u8,				mShadowCurve )
	WRITE_STYLE_CMD ( STYLE_CMD_DRAWING_TO_WORLD,		ZLAffine2D,		mDrawingToWorld )
	WRITE_STYLE_CMD ( STYLE_CMD_FILL_EXTRA_ID,			u32,			mFillExtraID )
	WRITE_STYLE_CMD ( STYLE_CMD_STROKE_EXTRA_ID,		u32,			mStrokeExtraID )
	WRITE_STYLE_CMD ( STYLE_CMD_MERGE_NORMALS,			float,			mMergeNormals )
	
	stream.Write < u8 >( MOAIVectorTesselatorWriter::VECTOR_CMD_SHAPE );
	stream.Write < u8 >( shape.GetType ());
	shape.Write ( stream, *this );
}

//================================================================//
// MOAIVectorDrawingVertexWriter
//================================================================//
class MOAIVectorDrawingVertexWriter :
	public ZLAbstractVertexWriter2D {
private:

	MOAIVectorTesselator* mDrawing;

public:

	SET ( MOAIVectorTesselator*, Drawing, mDrawing )

	//----------------------------------------------------------------//
	void WriteVertex ( const ZLVec2D& v ) {
		assert ( this->mDrawing );
		this->mDrawing->PushVertex ( v.mX, v.mY );
	}
};

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_clearShapes ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->ClearShapes ();
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_clearStyles ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->mStyle.Default ();
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_clearTransforms ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->ClearTransforms ();
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_drawingToWorld ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "UNN" )
	
	ZLVec2D vec;
	vec.mX = state.GetParamValue ( 1, 0.0f );
	vec.mY = state.GetParamValue ( 2, 0.0f );

	self->mStyle.mDrawingToWorld.Transform ( vec );
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( vec.mX );
	ret [ 1 ] = state.ToRValue ( vec.mY );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_drawingToWorldVec ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "UNN" )
	
	ZLVec2D vec;
	vec.mX = state.GetParamValue ( 1, 0.0f );
	vec.mY = state.GetParamValue ( 2, 0.0f );

	self->mStyle.mDrawingToWorld.TransformVec ( vec );
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( vec.mX );
	ret [ 1 ] = state.ToRValue ( vec.mY );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_finish ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	int error = self->Finish ();
	
	bool hasContent = self->mShapeStack.GetTop () > 0;
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( error );
	ret [ 1 ] = state.ToRValue ( hasContent );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_getExtrude ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( self->mStyle.mExtrude );
	ret [ 1 ] = state.ToRValue ( self->mStyle.mZOffset );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_getTransform ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	const ZLAffine2D& drawingToWorld = self->mStyle.mDrawingToWorld;
	
	mrb_value ret [ 6 ];
	ret [ 0 ] = state.ToRValue ( drawingToWorld.m [ ZLAffine2D::C0_R0 ] );
	ret [ 1 ] = state.ToRValue ( drawingToWorld.m [ ZLAffine2D::C0_R1 ] );
	ret [ 2 ] = state.ToRValue ( drawingToWorld.m [ ZLAffine2D::C1_R0 ] );
	ret [ 3 ] = state.ToRValue ( drawingToWorld.m [ ZLAffine2D::C1_R1 ] );
	ret [ 4 ] = state.ToRValue ( drawingToWorld.m [ ZLAffine2D::C2_R0 ] );
	ret [ 5 ] = state.ToRValue ( drawingToWorld.m [ ZLAffine2D::C2_R1 ] );

	return mrb_ary_new_from_values ( state, 6, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_openWriter ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )

	MOAIVectorTesselatorWriter* writer = state.CreateClassInstance < MOAIVectorTesselatorWriter >();
		
	return state.ToRValue < MOAIRubyObject* >( writer );
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_pushBezierVertices ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	ZLVec2D p0;
	ZLVec2D p1;
	ZLVec2D p2;
	ZLVec2D p3;
	
	p0.mX			= state.GetParamValue < float >( 1, 0.0f );
	p0.mY			= state.GetParamValue < float >( 2, 0.0f );
	
	p1.mX			= state.GetParamValue < float >( 3, 0.0f );
	p1.mY			= state.GetParamValue < float >( 4, 0.0f );
	
	p2.mX			= state.GetParamValue < float >( 5, 0.0f );
	p2.mY			= state.GetParamValue < float >( 6, 0.0f );
	
	p3.mX			= state.GetParamValue < float >( 7, 0.0f );
	p3.mY			= state.GetParamValue < float >( 8, 0.0f );
	
	self->PushBezierVertices ( p0, p1, p2, p3 );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_pushCombo ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->PushCombo ();
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_pushEllipse ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	float x			= state.GetParamValue < float >( 1, 0.0f );
	float y			= state.GetParamValue < float >( 2, 0.0f );
	float xRad		= state.GetParamValue < float >( 3, 0.0f );
	float yRad		= state.GetParamValue < float >( 4, xRad );
	
	self->PushEllipse ( x, y, xRad, yRad );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_pushPoly ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	ZLVec2D* vertices = 0;
	u32 total = ( state.GetParamsCount () - 1 ) >> 1;
	bool closed = state.GetParamValue < bool >( -1, true );
	
	if ( total ) {
		vertices = ( ZLVec2D* )alloca ( total * sizeof ( ZLVec2D ));
		
		for ( u32 i = 0; i < total; ++i ) {
			vertices [ i ].mX = state.GetParamValue < float >(( i << 1 ) + 2, 0 );
			vertices [ i ].mY = state.GetParamValue < float >(( i << 1 ) + 3, 0 );
		}
	}
	
	self->PushPoly ( vertices, total, closed );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_pushRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	ZLRect rect = state.GetRect < float >( 1 );
	self->PushRect ( rect.mXMin, rect.mYMin, rect.mXMax, rect.mYMax );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_pushRegion ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	MOAIRegion* region = state.GetRubyObject < MOAIRegion >( 1, true );
	
	if ( region ) {
		self->PushCombo ();
		self->PushRegion ( *region );
		self->Finish ();
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_pushRotate ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	float x		= state.GetParamValue < float >( 1, 0.0f );
	float y		= state.GetParamValue < float >( 2, 0.0f );
	float r		= state.GetParamValue < float >( 3, 0.0f );

	self->PushRotate ( x, y, r * ( float )D2R );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_pushScale ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	float x			= state.GetParamValue < float >( 1, 1.0f );
	float y			= state.GetParamValue < float >( 2, 1.0f );
	
	self->PushScale ( x, y );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_pushSkew ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	float yx		= state.GetParamValue < float >( 1, 0.0f );
	float xy		= state.GetParamValue < float >( 2, 0.0f );
	
	self->PushSkew ( yx, xy );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_pushTransform ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	float a			= state.GetParamValue < float >( 1, 1.0f );
	float b			= state.GetParamValue < float >( 2, 0.0f );
	float c			= state.GetParamValue < float >( 3, 0.0f );
	float d			= state.GetParamValue < float >( 4, 1.0f );
	
	float tx		= state.GetParamValue < float >( 5, 0.0f );
	float ty		= state.GetParamValue < float >( 6, 0.0f );
	
	self->PushTransform ( a, b, c, d, tx, ty );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_pushTranslate ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	float x			= state.GetParamValue < float >( 1, 1.0f );
	float y			= state.GetParamValue < float >( 2, 0.0f );
	
	self->PushTranslate ( x, y );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_pushVertex ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	float x		= state.GetParamValue < float >( 1, 0.0f );
	float y		= state.GetParamValue < float >( 2, 0.0f );
	
	self->PushVertex ( x, y );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_readShapes ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	MOAIStream* stream = state.GetRubyObject < MOAIStream >( 1, false );
	
	if ( stream ) {
		self->ReadShapes ( *stream );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_reserveVertexExtras ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )

	u32 total		= state.GetParamValue < u32 >( 1, 0 );
	size_t size		= state.GetParamValue < u32 >( 2, 0 );
	
	self->ReserveVertexExtras ( total, size );
	return mrb_nil_value ();	
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setCapStyle ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )

	self->mStyle.mCapStyle = ( u8 )state.GetParamValue < u32 >( 1, MOAIVectorStyle::CAP_BUTT );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setCircleResolution ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->mStyle.mCircleResolution = ( u16 )state.GetParamValue < u32 >( 1, MOAIVectorStyle::DEFAULT_CIRCLE_RESOLUTION );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setDepthBias ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->mDepthBias = state.GetParamValue < float >( 1, 0.0f );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setExtrude ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->mStyle.mExtrude = state.GetParamValue < float >( 1, 0.0f );
	self->mStyle.mZOffset = state.GetParamValue < float >( 2, 0.0f );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setFillColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )

	self->mStyle.mFillColor = state.GetColor ( 1, 1.0f, 1.0f, 1.0f, 1.0f );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setFillStyle ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )

	self->mStyle.mFillStyle = ( u8 )state.GetParamValue < u32 >( 1, MOAIVectorStyle::FILL_NONE );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setFillExtra ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->mStyle.mFillExtraID = state.GetParamValue < u32 >( 1, 1 ) - 1;
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setJoinStyle ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )

	self->mStyle.mJoinStyle = ( u8 )state.GetParamValue < u32 >( 1, MOAIVectorStyle::JOIN_MITER );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setLightColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->mStyle.mLightColor = state.GetColor ( 1, 1.0f, 1.0f, 1.0f, 1.0f );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setLightCurve ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )

	self->mStyle.mLightCurve = ( u8 )state.GetParamValue < u32 >( 1, ZLInterpolate::kLinear );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setLightVec ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->mStyle.mLightVec.mX = state.GetParamValue < float >( 1, 0.0f );
	self->mStyle.mLightVec.mY = state.GetParamValue < float >( 2, 0.0f );
	self->mStyle.mLightVec.mZ = state.GetParamValue < float >( 3, 0.0f );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setLineColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->mStyle.mLineColor = state.GetColor ( 1, 1.0f, 1.0f, 1.0f, 1.0f );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setLineStyle ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->mStyle.mLineStyle = ( u8 )state.GetParamValue < u32 >( 1, MOAIVectorStyle::LINE_NONE );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setLineWidth ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->mStyle.mLineWidth = state.GetParamValue < float >( 1, 0.0f );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setMergeNormals ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )

	self->mStyle.mMergeNormals = state.GetParamValue < float >( 1, 0.0f );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setMiterLimit ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )

	self->mStyle.mMiterLimit = state.GetParamValue < float >( 1, 0.0f );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setPolyClosed ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )

	self->mPolyClosed = state.GetParamValue < bool >( 1, false );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setShadowColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->mStyle.mShadowColor = state.GetColor ( 1, 0.0f, 0.0f, 0.0f, 1.0f );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setShadowCurve ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )

	self->mStyle.mShadowCurve = ( u8 )state.GetParamValue < u32 >( 1, ZLInterpolate::kLinear );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setStrokeColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )

	self->mStyle.mStrokeColor = state.GetColor ( 1, 1.0f, 1.0f, 1.0f, 1.0f );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setStrokeDepthBias ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->mStyle.mStrokeDepthBias = state.GetParamValue < float >( 1, 0.0f );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setStrokeExtra ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->mStyle.mStrokeExtraID =  state.GetParamValue < u32 >( 1, 1 ) - 1;
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setStrokeStyle ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->mStyle.mStrokeStyle = ( u8 )state.GetParamValue < u32 >( 1, MOAIVectorStyle::STROKE_CENTER );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setStrokeWidth ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->mStyle.mStrokeWidth = state.GetParamValue < float >( 1, 0.0f );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setVerbose ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )

	self->mVerbose = state.GetParamValue < bool >( 1, false );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setVertexExtra ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	/*u32 idx = state.GetParamValue < u32 >( 1, 1 )- 1;
	
	size_t len;
	void* extra = ( void* )lua_tolstring ( state, 3, &len );
	
	self->SetVertexExtra ( idx, extra, len );*/
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_setWindingRule ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	self->mStyle.mWindingRule = ( u8 )state.GetParamValue < u32 >( 1, ( u32 )TESS_WINDING_ODD );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_tesselate ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	u32 base = 0;
	int totalElements = 0;
	
	MOAIVertexBuffer* vtxBuffer		= state.GetRubyObject < MOAIVertexBuffer >( 1, false );
	MOAIIndexBuffer* idxBuffer		= state.GetRubyObject < MOAIIndexBuffer >( 2, false );

	if ( vtxBuffer && idxBuffer ) {
	
		u32 idxSizeInBytes			= state.GetParamValue < u32 >( 3, 4 );
		MOAIVertexFormat* format	= state.GetRubyObject < MOAIVertexFormat >( 4, false );
		u32 flags					= state.GetParamValue < u32 >( 5, MOAIVectorTesselator::TESSELATE_ALL );
		
		base = ( u32 )( idxBuffer->GetCursor () / idxSizeInBytes ); // TODO: cast
		totalElements = self->Tesselate ( *vtxBuffer, *idxBuffer, *format, idxSizeInBytes, flags );
	}
	else {
		
		MOAIStream* vtxStream		= state.GetRubyObject < MOAIStream >( 1, false );
		MOAIStream* idxStream		= state.GetRubyObject < MOAIStream >( 2, false );
		MOAIVertexFormat* format	= state.GetRubyObject < MOAIVertexFormat >( 3, false );
		u32 flags					= state.GetParamValue < u32 >( 4, MOAIVectorTesselator::TESSELATE_ALL );
		
		if ( vtxStream && idxStream && format ) {
			base = ( u32 )( idxStream->GetCursor () / 4 ); // TODO: cast
			totalElements = self->Tesselate ( *vtxStream, *idxStream, *format, flags );
		}
	}
	
	MOAIRegion* region = state.GetRubyObject < MOAIRegion >( 1, false );
	
	if ( region ) {
		u32 flags		= state.GetParamValue < u32 >( 2, MOAIVectorTesselator::TESSELATE_ALL );
		totalElements	= self->Tesselate ( *region, flags );
	}
	
	mrb_value ret [ 3 ];
	ret [ 0 ] = state.ToRValue ( totalElements );
	ret [ 1 ] = state.ToRValue ( base );
	ret [ 2 ] = state.ToRValue ( base + totalElements );

	return mrb_ary_new_from_values ( state, 3, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_worldToDrawing ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "UNN" )
	
	ZLVec2D vec;
	vec.mX = state.GetParamValue ( 1, 0.0f );
	vec.mY = state.GetParamValue ( 2, 0.0f );

	self->mStyle.mWorldToDrawing.Transform ( vec );
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( vec.mX );
	ret [ 1 ] = state.ToRValue ( vec.mY );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_worldToDrawingVec ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "UNN" )
	
	ZLVec2D vec;
	vec.mX = state.GetParamValue ( 1, 0.0f );
	vec.mY = state.GetParamValue ( 2, 0.0f );

	self->mStyle.mWorldToDrawing.TransformVec ( vec );
	
	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( vec.mX );
	ret [ 1 ] = state.ToRValue ( vec.mY );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
mrb_value MOAIVectorTesselator::_writeShapes ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIVectorTesselator, "U" )
	
	MOAIStream* stream = state.GetRubyObject < MOAIStream >( 1, false );
	MOAIVectorTesselatorWriter* writer = state.GetRubyObject < MOAIVectorTesselatorWriter >( 2, false );
	
	if ( writer ) {
		self->WriteShapes ( *stream, writer );
	}
	return mrb_nil_value ();
}

//================================================================//
// MOAIVectorTesselator
//================================================================//

//----------------------------------------------------------------//
void MOAIVectorTesselator::Clear () {

	this->ClearShapes ();
	
	for ( u32 i = 0; i < this->mVtxExtras.Size (); ++i ) {
		free ( this->mVtxExtras [ i ]);
	}
	this->mVtxExtraSize = 0;
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::ClearShapes () {

	for ( u32 i = 0; i < this->mShapeStack.GetTop (); ++i ) {
		MOAIVectorShape* shape = this->mShapeStack [ i ];
		if ( shape ) {
			delete this->mShapeStack [ i ];
		}
	}
	
	this->mShapeStack.Reset ();
	this->mVertexStack.Reset ();
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::ClearTransforms () {

	this->mMatrixStack.Reset ();
	this->mStyle.mDrawingToWorld.Ident ();
	this->mStyle.mWorldToDrawing.Ident ();
}

//----------------------------------------------------------------//
u32 MOAIVectorTesselator::CountVertices ( const MOAIVertexFormat& format, ZLStream& vtxStream ) {

	return ( u32 )( vtxStream.GetCursor () / format.GetVertexSize ()); // TODO: cast
}

//----------------------------------------------------------------//
int MOAIVectorTesselator::Finish () {

	// TODO: check overflow
	u32 vertsTop = ( u32 )this->mVertexStack.GetTop ();
	u32 shapesTop = ( u32 )this->mShapeStack.GetTop ();
	
	int error = 0;
	
	if ( vertsTop ) {
	
		assert ( shapesTop );
		MOAIVectorShape* shape = this->mShapeStack [ shapesTop - 1 ];
		bool result = shape->SetVertices ( this->mVertexStack, vertsTop, this->mPolyClosed );
		
		assert ( result );
		UNUSED ( result );
		
		this->mVertexStack.Reset ();
	}
	else {

		if ( shapesTop >= 2 ) {
			
			bool done = false;
			for ( int i = ( shapesTop - 2 ); ( i >= 0 ) && ( !done ); --i ) {
				int c = i + 1;
				MOAIVectorShape* shape = this->mShapeStack [ i ];
				if ( shape->CanGroup () && shape->GroupShapes ( &this->mShapeStack [ c ], shapesTop - c )) {
					this->mShapeStack.SetTop ( c );
					done = true;
				}
			}
		}
		
		if ( this->mShapeStack.GetTop () == shapesTop ) {
			this->mDepthOffset = 0.0f;
		}
	}
	
	return error;
}

//----------------------------------------------------------------//
MOAIVectorTesselator::MOAIVectorTesselator () :
	mDepthBias ( 0.0f ),
	mDepthOffset ( 0.0f ),
	mVerbose ( false ),
	mVtxExtraSize ( 0 ) {
	
	this->mStyle.Default ();
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIVectorTesselator::~MOAIVectorTesselator () {

	this->Clear ();
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::PopTransform () {

	this->mMatrixStack.Pop ();
	
	ZLAffine2D transform;
	transform.Ident ();
	
	for ( u32 i = 0; i < this->mMatrixStack.GetTop (); ++i ) {
		transform.Prepend ( this->mMatrixStack [ i ]);
	}
	this->mStyle.mDrawingToWorld = transform;
	this->mStyle.mWorldToDrawing.Inverse ( this->mStyle.mDrawingToWorld );
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::PushBezierVertices ( const ZLVec2D& p0, const ZLVec2D& p1, const ZLVec2D& p2, const ZLVec2D& p3 ) {

	MOAIVectorDrawingVertexWriter writer;
	writer.SetDrawing ( this );

	ZLCubicBezier2D curve;
	curve.Init ( p0, p1, p2, p3 );
	curve.Flatten ( writer );
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::PushCombo () {

	MOAIVectorCombo* combo = new MOAIVectorCombo ();
	this->PushShape ( combo );
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::PushEllipse ( float x, float y, float xRad, float yRad ) {

	MOAIVectorEllipse* ellipse = new MOAIVectorEllipse ();
	this->PushShape ( ellipse );
	ellipse->Init ( x, y, xRad, yRad );
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::PushPoly ( const ZLVec2D* vertices, u32 total, bool closed ) {

	this->mPolyClosed = closed;

	MOAIVectorPoly* poly = new MOAIVectorPoly ();
	this->PushShape ( poly );
	poly->SetVertices ( vertices, total, closed );
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::PushRect ( float xMin, float yMin, float xMax, float yMax ) {

	MOAIVectorRect* vectorRect = new MOAIVectorRect ();
	this->PushShape ( vectorRect );
	vectorRect->Init ( xMin, yMin, xMax, yMax );
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::PushRegion ( const MOAIRegion& region ) {

	size_t size = region.GetSize ();
	for ( size_t i = 0; i < size; ++i ) {
	
		const ZLPolygon2D& polygon = region.GetPolygon (( u32 )i );
		this->PushPoly ( polygon.GetVertices(), ( u32 )polygon.GetSize (), true ); // TODO: cast
	}
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::PushRotate ( float x, float y, float r ) {

	ZLAffine2D transform;

	if (( x != 0.0f ) || ( y != 0.0f )) {
		
		ZLAffine2D mtx;
		
		transform.Translate ( -x, -y );
		
		mtx.Rotate ( r );
		transform.Append ( mtx );
	
		mtx.Translate ( x, y );
		transform.Append ( mtx );
		
		this->PushTransform ( transform );
	}
	else {
		transform.Rotate ( r );
		this->PushTransform ( transform );
	}
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::PushScale ( float x, float y ) {
	
	ZLAffine2D transform;
	transform.Scale ( x, y );
	this->PushTransform ( transform );
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::PushShape ( MOAIVectorShape* shape ) {

	shape->mStyle = this->mStyle;
	this->mShapeStack.Push ( shape );
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::PushSkew ( float yx, float xy ) {

	ZLAffine2D transform;
	transform.Shear ( Tan ( yx ), Tan ( xy ));
	this->PushTransform ( transform );
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::PushTransform ( const ZLAffine2D& transform ) {

	this->mMatrixStack.Push ( transform );
	this->mStyle.mDrawingToWorld.Prepend ( transform );
	this->mStyle.mWorldToDrawing.Inverse ( this->mStyle.mDrawingToWorld );
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::PushTransform ( float a, float b, float c, float d, float tx, float ty ) {

	ZLAffine2D transform;
	
	transform.m [ ZLAffine2D::C0_R0 ] = a;
	transform.m [ ZLAffine2D::C0_R1 ] = b;
	
	transform.m [ ZLAffine2D::C1_R0 ] = c;
	transform.m [ ZLAffine2D::C1_R1 ] = d;
	
	transform.m [ ZLAffine2D::C2_R0 ] = tx;
	transform.m [ ZLAffine2D::C2_R1 ] = ty;
	
	this->PushTransform ( transform );
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::PushTranslate ( float x, float y ) {

	ZLAffine2D transform;
	transform.Translate ( x, y );
	this->PushTransform ( transform );
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::PushVertex ( float x, float y ) {

	ZLVec2D vertex ( x, y );
	this->mVertexStack.Push ( vertex );
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::ReadShapes ( ZLStream& stream ) {

	this->ClearShapes ();

	MOAIVectorTesselatorWriter writer;
	writer.mStyle = this->mStyle;
	
	while ( MOAIVectorShape* shape = writer.ReadShape ( stream )) {
		this->mShapeStack.Push ( shape );
	}
	this->mStyle = writer.mStyle;
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	state.DefineClassConst ( klass, "FILL_NONE",					( u32 )MOAIVectorStyle::FILL_NONE );
	state.DefineClassConst ( klass, "FILL_SOLID",					( u32 )MOAIVectorStyle::FILL_SOLID );
	
	state.DefineClassConst ( klass, "LINE_NONE",					( u32 )MOAIVectorStyle::LINE_NONE );
	state.DefineClassConst ( klass, "LINE_VECTOR",					( u32 )MOAIVectorStyle::LINE_VECTOR );
	
	state.DefineClassConst ( klass, "STROKE_NONE",					( u32 )MOAIVectorStyle::STROKE_NONE );
	state.DefineClassConst ( klass, "STROKE_CENTER",				( u32 )MOAIVectorStyle::STROKE_CENTER );
	state.DefineClassConst ( klass, "STROKE_INTERIOR",				( u32 )MOAIVectorStyle::STROKE_INTERIOR );
	state.DefineClassConst ( klass, "STROKE_EXTERIOR",				( u32 )MOAIVectorStyle::STROKE_EXTERIOR );
	
	state.DefineClassConst ( klass, "JOIN_BEVEL",					( u32 )MOAIVectorStyle::JOIN_BEVEL );
	state.DefineClassConst ( klass, "JOIN_MITER",					( u32 )MOAIVectorStyle::JOIN_MITER );
	state.DefineClassConst ( klass, "JOIN_ROUND",					( u32 )MOAIVectorStyle::JOIN_ROUND );
	
	state.DefineClassConst ( klass, "CAP_BUTT",					( u32 )MOAIVectorStyle::CAP_BUTT );
	state.DefineClassConst ( klass, "CAP_ROUND",					( u32 )MOAIVectorStyle::CAP_ROUND );
	state.DefineClassConst ( klass, "CAP_POINTY",					( u32 )MOAIVectorStyle::CAP_POINTY );
	state.DefineClassConst ( klass, "CAP_SQUARE",					( u32 )MOAIVectorStyle::CAP_SQUARE );
	
	state.DefineClassConst ( klass, "TESS_WINDING_ODD",			( u32 )TESS_WINDING_ODD );
	state.DefineClassConst ( klass, "TESS_WINDING_NONZERO",		( u32 )TESS_WINDING_NONZERO );
	state.DefineClassConst ( klass, "TESS_WINDING_POSITIVE",		( u32 )TESS_WINDING_POSITIVE );
	state.DefineClassConst ( klass, "TESS_WINDING_NEGATIVE",		( u32 )TESS_WINDING_NEGATIVE );
	state.DefineClassConst ( klass, "TESS_WINDING_ABS_GEQ_TWO",	( u32 )TESS_WINDING_ABS_GEQ_TWO );
	
	state.DefineClassConst ( klass, "TESSELATE_FILLS",				( u32 )TESSELATE_FILLS );
	state.DefineClassConst ( klass, "TESSELATE_STROKES",			( u32 )TESSELATE_STROKES );
	state.DefineClassConst ( klass, "TESSELATE_ALL",				( u32 )TESSELATE_ALL );
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "clearShapes",			_clearShapes, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "clearStyles",			_clearStyles, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "clearTransforms",		_clearTransforms, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "drawingToWorld",		_drawingToWorld, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "drawingToWorldVec",	_drawingToWorldVec, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "finish",				_finish, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getExtrude",			_getExtrude, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getTransform",			_getTransform, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "openWriter",			_openWriter, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "pushBezierVertices",	_pushBezierVertices, MRB_ARGS_REQ ( 8 ) );
	state.DefineInstanceMethod ( klass, "pushCombo",			_pushCombo, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "pushEllipse",			_pushEllipse, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "pushPoly",				_pushPoly, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "pushRect",				_pushRect, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "pushRegion",			_pushRegion, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "pushRotate",			_pushRotate, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "pushScale",			_pushScale, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "pushSkew",				_pushSkew, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "pushTransform",		_pushTransform, MRB_ARGS_REQ ( 6 ) );
	state.DefineInstanceMethod ( klass, "pushTranslate",		_pushTranslate, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "pushVertex",			_pushVertex, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "readShapes",			_readShapes, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "reserveVertexExtras",	_reserveVertexExtras, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setCapStyle",			_setCapStyle, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setCircleResolution",	_setCircleResolution, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setDepthBias",			_setDepthBias, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setExtrude",			_setExtrude, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setFillColor",			_setFillColor, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "setFillStyle",			_setFillStyle, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setFillExtra",			_setFillExtra, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setJoinStyle",			_setJoinStyle, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setLightColor",		_setLightColor, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "setLightCurve",		_setLightCurve, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setLightVec",			_setLightVec, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "setLineColor",			_setLineColor, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "setLineStyle",			_setLineStyle, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setLineWidth",			_setLineWidth, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setMergeNormals",		_setMergeNormals, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setMiterLimit",		_setMiterLimit, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setPolyClosed",		_setPolyClosed, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setShadowColor",		_setShadowColor, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "setShadowCurve",		_setShadowCurve, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setStrokeColor",		_setStrokeColor, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "setStrokeDepthBias",	_setStrokeDepthBias, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setStrokeExtra",		_setStrokeExtra, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setStrokeStyle",		_setStrokeStyle, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setStrokeWidth",		_setStrokeWidth, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setVerbose",			_setVerbose, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "setVertexExtra",		_setVertexExtra, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "setWindingRule",		_setWindingRule, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "tesselate",			_tesselate, MRB_ARGS_ANY () );
	state.DefineInstanceMethod ( klass, "worldToDrawing",		_worldToDrawing, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "worldToDrawingVec",	_worldToDrawingVec, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "writeShapes",			_writeShapes, MRB_ARGS_REQ ( 2 ) );
	
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::ReserveVertexExtras ( u32 total, size_t size ) {

	this->mVtxExtraSize = size;
	this->mVtxExtras.Init ( total );
	
	for ( u32 i = 0; i < this->mVtxExtras.Size (); ++i ) {
		this->mVtxExtras [ i ] = calloc ( 1, this->mVtxExtraSize );
	}
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::SetVertexExtra ( u32 idx, void* extra, size_t size ) {

	size = size <= this->mVtxExtraSize ? size : this->mVtxExtraSize;
	if ( idx < this->mVtxExtras.Size ()) {
		memcpy ( this->mVtxExtras [ idx ], extra, size );
	}
}

//----------------------------------------------------------------//
int MOAIVectorTesselator::Tesselate ( SafeTesselator& tess, u32 flags ) {

	int error = this->Finish ();
	if ( error ) return error;

	for ( u32 i = 0; i < this->mShapeStack.GetTop (); ++i ) {
		MOAIVectorShape* shape = this->mShapeStack [ i ];
		error = shape->Tesselate ( *this, tess, flags );
		if ( error ) return error;
	}
	return ( int )this->mShapeStack.GetTop (); // TODO: fix cast
}

//----------------------------------------------------------------//
int MOAIVectorTesselator::Tesselate ( MOAIRegion& region, u32 flags ) {

	region.Clear ();
	bool useTess = true;
	
	if ( this->mShapeStack.GetTop () == 1 ) {

		useTess = this->mShapeStack [ 0 ]->Tesselate ( *this, region, flags ) != 0;
	}

	if ( useTess ) {
		
		region.Clear ();
	
		SafeTesselator tess;
		if ( !this->Tesselate ( tess, flags )) return 0;
		
		int error = tess.Tesselate (( int )this->mStyle.mWindingRule, TESS_BOUNDARY_CONTOURS, 0, 0 );

		if ( !error ) {
			region.Copy ( tess );
			region.Transform ( region, this->mStyle.mDrawingToWorld );
			region.Bless ();
		}
	}
	
	return ( int )region.GetSize (); // TODO: cast
}

//----------------------------------------------------------------//
int MOAIVectorTesselator::Tesselate ( ZLStream& vtxStream, ZLStream& idxStream, MOAIVertexFormat& format, u32 flags ) {

	int error = this->Finish ();
	if ( error ) return -1;

	int base = ( int )idxStream.GetCursor(); // TODO: cast

	this->mDepthOffset = 0.0f;

	for ( u32 i = 0; i < this->mShapeStack.GetTop (); ++i ) {
		MOAIVectorShape* shape = this->mShapeStack [ i ];
		error = shape->Tesselate ( *this, vtxStream, idxStream, format, flags );
		assert ( error == 0 );
		if ( error ) return -1;
	}
	
	// idx stream is 32-bits, so divide by 4 to get total indices
	return ( int )(( idxStream.GetCursor () - base ) >> 2 ); // TODO: cast
}

//----------------------------------------------------------------//
int MOAIVectorTesselator::Tesselate ( MOAIVertexBuffer& vtxBuffer, MOAIIndexBuffer& idxBuffer, MOAIVertexFormat& format, u32 idxSizeInBytes, u32 flags ) {
	
	ZLMemStream vtxStream;
	ZLMemStream idxStream;
	
	int totalIndices = this->Tesselate ( vtxStream, idxStream, format, flags );
	
	if ( totalIndices > 0 ) {
		return MOAIGeometryWriter::GetMesh ( format, vtxStream, vtxStream.GetLength (), idxStream, idxStream.GetLength (), vtxBuffer, idxBuffer, idxSizeInBytes );
	}
	
	// something went wrong, return error code
	return totalIndices;
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::WriteShapes ( ZLStream& stream, MOAIVectorTesselatorWriter* writer ) {

	assert ( writer );

	MOAIVectorTesselatorWriter defaultWriter;
	writer = writer ? writer : &defaultWriter;
	
	u32 nShapes = this->mShapeStack.GetTop ();

	if ( nShapes ) {
		for ( u32 i = 0; i < nShapes; ++i ) {
			MOAIVectorShape* shape = this->mShapeStack [ i ];
			writer->WriteShape ( stream, *shape );
		}
	}
	stream.Write < u8 >( MOAIVectorTesselatorWriter::VECTOR_CMD_DONE );
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::WriteSkirt ( SafeTesselator& tess, ZLStream& vtxStream, ZLStream& idxStream, MOAIVertexFormat& format, const MOAIVectorStyle& style, const ZLColorVec& fillColor, u32 vertexExtraID ) {

	u32 base = this->CountVertices ( format, vtxStream );
	float z = style.GetExtrude ();
	float zOff = style.GetZOffset ();

	ZLVec3D lightVec3D = style.GetLightVec ();
	ZLVec2D lightVec ( lightVec3D.mX, lightVec3D.mY );
	
	float lightLen = lightVec.Norm ();
	bool doLighting = lightLen != 0.0f ? true : false;
	
	ZLColorVec lightColor = style.mLightColor;
	u32 lightCurve = style.mLightCurve;
	float lightAlpha = lightColor.mA;
	lightColor.mA = 1.0f;

	ZLColorVec shadowColor = style.mShadowColor;
	u32 shadowCurve = style.mShadowCurve;
	float shadowAlpha = shadowColor.mA;
	shadowColor.mA = 1.0f;

	float normalsMergeCos = style.GetMergeNormalsCosine ();

	u32 color32 = fillColor.PackRGBA ();

	const int* elems = tessGetElements ( tess.mTess );
	const int nelems = tessGetElementCount ( tess.mTess );
	const float* verts = tessGetVertices ( tess.mTess );
	
	for ( int i = 0; i < nelems; ++i ) {
		
		int b = elems [( i * 2 )];
		int n = elems [( i * 2 ) + 1 ]; // vertices
		
		ZLVec2D* vertices = ( ZLVec2D* )alloca ( n * sizeof ( ZLVec2D ));
		assert ( vertices );
		
		ZLVec2D* faceNormals = ( ZLVec2D* )alloca ( n * sizeof ( ZLVec2D ));
		assert ( faceNormals );
		
		for ( int j = 0; j < n; ++j ) {
			
			ZLVec2D v0 = (( ZLVec2D* )verts )[ b + j ];
			style.mDrawingToWorld.Transform ( v0 );
			vertices [ j ] = v0;
		}
		
		for ( int j = 0; j < n; ++j ) {
			
			const ZLVec2D& v0 = vertices [ j ];
			const ZLVec2D& v1 = vertices [( j + 1 ) % n ];
			
			ZLVec2D e = v1;
			e.Sub ( v0 );
			e.Rotate90Anticlockwise ();
			e.Norm ();
			
			faceNormals [ j ] = e;
		}
		
		for ( int j = 0; j < n; ++j ) {
			
			const ZLVec2D& v0 = vertices [ j ];
			const ZLVec2D& v1 = vertices [( j + 1 ) % n ];
			
			const ZLVec2D& n0 = faceNormals [( j + n - 1 ) % n ]; // prev
			const ZLVec2D& n1 = faceNormals [ j ]; // current
			const ZLVec2D& n2 = faceNormals [( j + 1 ) % n ]; // next

			ZLVec2D normal0 = n1;
			if ( n1.Dot ( n0 ) > normalsMergeCos ) {
				normal0.Add ( n0 );
				normal0.Norm ();
			}
			
			ZLVec2D normal1 = n1;
			if ( n1.Dot ( n2 ) > normalsMergeCos ) {
				normal1.Add ( n2 );
				normal1.Norm ();
			}
			
			u32 color0 = color32;
			u32 color1 = color32;
			
			if ( doLighting ) {
				
				ZLColorVec color;
				float dot;
				
				dot = lightVec.Dot ( normal0 );
				if ( dot < 0.0f ) {
					color.Lerp ( shadowCurve, fillColor, shadowColor, -dot * shadowAlpha );
				}
				else {
					color.Lerp ( lightCurve, fillColor, lightColor, dot * lightAlpha );
				}
				color0 = color.PackRGBA ();
				
				dot = lightVec.Dot ( normal1 );
				if ( dot < 0.0f ) {
					color.Lerp ( shadowCurve, fillColor, shadowColor, -dot * shadowAlpha );
				}
				else {
					color.Lerp ( lightCurve, fillColor, lightColor, dot * lightAlpha );
				}
				color1 = color.PackRGBA ();
			}
			
			this->WriteVertex ( vtxStream, format, v0.mX, v0.mY, zOff, normal0.mX, normal0.mY, 0.0f, color0, vertexExtraID );
			this->WriteVertex ( vtxStream, format, v1.mX, v1.mY, zOff, normal1.mX, normal1.mY, 0.0f, color1, vertexExtraID );
			this->WriteVertex ( vtxStream, format, v0.mX, v0.mY, z + zOff, normal0.mX, normal0.mY, 0.0f, color0, vertexExtraID );
			this->WriteVertex ( vtxStream, format, v1.mX, v1.mY, z + zOff, normal1.mX, normal1.mY, 0.0f, color1, vertexExtraID );
			
			idxStream.Write < u32 >( base + 0 );
			idxStream.Write < u32 >( base + 1 );
			idxStream.Write < u32 >( base + 3 );
			
			idxStream.Write < u32 >( base + 0 );
			idxStream.Write < u32 >( base + 3 );
			idxStream.Write < u32 >( base + 2 );
			
			base += 4;
		}
	}
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::WriteTriangles ( SafeTesselator& tess, ZLStream& vtxStream, ZLStream& idxStream, MOAIVertexFormat& format, const MOAIVectorStyle& style, float z, u32 color, u32 vertexExtraID ) {

	u32 base = this->CountVertices ( format, vtxStream );

	z = z != 0.0f ? z : this->mDepthOffset;

	STLString log;

	if ( this->mVerbose ) {
		log = "";
		log.write ( "WRITING VERTICES:\n" );
	}

	const float* verts = tessGetVertices ( tess.mTess );
	const int nverts = tessGetVertexCount ( tess.mTess );
	
	for ( int i = 0; i < nverts; ++i ) {
		
		ZLVec2D vert = (( const ZLVec2D* )verts )[ i ];
		
		if ( this->mVerbose ) {
			log.write ( "%d: %f, %f\n", i, vert.mX, vert.mY );
		}
		
		style.mDrawingToWorld.Transform ( vert );
		this->WriteVertex ( vtxStream, format, vert.mX, vert.mY, z, 0.0f, 0.0f, 1.0f, color, vertexExtraID );
	}
	
	if ( this->mVerbose ) {
		log.write ( "\n" );
		ZLLogF ( ZLLog::CONSOLE, log.c_str ());
	}

	this->mDepthOffset += this->mDepthBias;
	
	if ( this->mVerbose ) {
		ZLLogF ( ZLLog::CONSOLE, "WRITING INDICES:\n" );
	}

	const int* elems = tessGetElements ( tess.mTess );
	const int nelems = tessGetElementCount ( tess.mTess );
	
	for ( int i = 0; i < nelems; ++i ) {
		const int* tri = &elems [ i * 3 ];
		
		if ( this->mVerbose ) {
			ZLLogF ( ZLLog::CONSOLE, "%d: %d, %d, %d\n", i, tri [ 0 ], tri [ 1 ], tri [ 2 ]);
		}
		
		idxStream.Write < u32 >( base + tri [ 0 ]);
		idxStream.Write < u32 >( base + tri [ 1 ]);
		idxStream.Write < u32 >( base + tri [ 2 ]);
	}
	
	if ( this->mVerbose ) {
		ZLLogF ( ZLLog::CONSOLE, "\n" );
	}
}

//----------------------------------------------------------------//
void MOAIVectorTesselator::WriteVertex ( ZLStream& stream, MOAIVertexFormat& format, float x, float y, float z, float xn, float yn, float zn, u32 color, u32 vertexExtraID ) {

	size_t base = stream.GetCursor ();
	format.WriteAhead ( stream );

	size_t vertexSize = format.GetVertexSize ();
	if ( this->mVtxExtraSize && ( this->mVtxExtraSize < vertexSize )) {
	
		stream.SetCursor ( base + ( vertexSize - this->mVtxExtraSize ));
		
		vertexExtraID = vertexExtraID % this->mVtxExtras.Size ();
		stream.WriteBytes ( this->mVtxExtras [ vertexExtraID ], this->mVtxExtraSize );
		
		stream.SetCursor ( base );
	}
		
	format.WriteCoord ( stream, 0, x, y, z, 1.0f );
	format.WriteColor ( stream, 0, color );
	format.WriteNormal ( stream, 0, xn, yn, zn );
	
	// next vertex
	format.SeekVertex ( stream, base, 1 );
}
