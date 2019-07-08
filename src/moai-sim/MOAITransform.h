// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAITRANSFORM_H
#define	MOAITRANSFORM_H

#include <moai-sim/MOAIEaseDriver.h>
#include <moai-sim/MOAITransformBase.h>

//================================================================//
// MOAITransform
//================================================================//
/**	@lua	MOAITransform
	@text	Transformation hierarchy node.

	@attr	ATTR_X_PIV
	@attr	ATTR_Y_PIV
	@attr	ATTR_Z_PIV
	
	@attr	ATTR_X_LOC
	@attr	ATTR_Y_LOC
	@attr	ATTR_Z_LOC
	
	@attr	ATTR_X_ROT
	@attr	ATTR_Y_ROT
	@attr	ATTR_Z_ROT
	
	@attr	ATTR_X_SCL
	@attr	ATTR_Y_SCL
	@attr	ATTR_Z_SCL
	
	@attr	ATTR_ROTATE_QUAT
	@attr	ATTR_TRANSLATE
	
	@attr	INHERIT_LOC
	@attr	INHERIT_TRANSFORM
*/
class MOAITransform :
	public MOAITransformBase {
protected:

	float			mShearYX;
	float			mShearZX;

	float			mShearXY;
	float			mShearZY;
	
	float			mShearXZ;
	float			mShearYZ;

	ZLVec3D			mPiv;
	ZLVec3D			mLoc;
	ZLVec3D			mScale;
	ZLVec3D			mRot;		// Euler angles, in degrees

	u32				mEulerOrder;

	//----------------------------------------------------------------//
	static mrb_value	_addLoc			( mrb_state* M, mrb_value context );
	static mrb_value	_addPiv			( mrb_state* M, mrb_value context );
	static mrb_value	_addRot			( mrb_state* M, mrb_value context );
	static mrb_value	_addScl			( mrb_state* M, mrb_value context );
	static mrb_value	_getLoc			( mrb_state* M, mrb_value context );
	static mrb_value	_getPiv			( mrb_state* M, mrb_value context );
	static mrb_value	_getRot			( mrb_state* M, mrb_value context );
	static mrb_value	_getScl			( mrb_state* M, mrb_value context );
	static mrb_value	_move			( mrb_state* M, mrb_value context );
	static mrb_value	_moveLoc		( mrb_state* M, mrb_value context );
	static mrb_value	_movePiv		( mrb_state* M, mrb_value context );
	static mrb_value	_moveRot		( mrb_state* M, mrb_value context );
	static mrb_value	_moveScl		( mrb_state* M, mrb_value context );
	static mrb_value	_seek			( mrb_state* M, mrb_value context );
	static mrb_value	_seekLoc		( mrb_state* M, mrb_value context );
	static mrb_value	_seekPiv		( mrb_state* M, mrb_value context );
	static mrb_value	_seekRot		( mrb_state* M, mrb_value context );
	static mrb_value	_seekScl		( mrb_state* M, mrb_value context );
	static mrb_value	_setLoc			( mrb_state* M, mrb_value context );
	static mrb_value	_setPiv			( mrb_state* M, mrb_value context );
	static mrb_value	_setRot			( mrb_state* M, mrb_value context );
	static mrb_value	_setScl			( mrb_state* M, mrb_value context );
	static mrb_value	_setShearByX	( mrb_state* M, mrb_value context );
	static mrb_value	_setShearByY	( mrb_state* M, mrb_value context );
	static mrb_value	_setShearByZ	( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	static float	ClampEuler									( float r );

	//----------------------------------------------------------------//
	bool			MOAINode_ApplyAttrOp						( u32 attrID, MOAIAttribute& attr, u32 op );
	void			MOAITransformBase_BuildLocalToWorldMtx		( ZLAffine3D& localToWorldMtx );

public:

	DECL_RUBY_FACTORY ( MOAITransform, MOAINode )
	DECL_ATTR_HELPER ( MOAITransform )

	enum {
		EULER_XYZ		= ( 2 << 0x04 ) + ( 1 << 0x02 ) + ( 0 << 0x00 ),
		EULER_XZY		= ( 1 << 0x04 ) + ( 2 << 0x02 ) + ( 0 << 0x00 ),
		EULER_YXZ		= ( 2 << 0x04 ) + ( 0 << 0x02 ) + ( 1 << 0x00 ),
		EULER_YZX		= ( 0 << 0x04 ) + ( 2 << 0x02 ) + ( 1 << 0x00 ),
		EULER_ZXY		= ( 1 << 0x04 ) + ( 0 << 0x02 ) + ( 2 << 0x00 ),
		EULER_ZYX		= ( 0 << 0x04 ) + ( 1 << 0x02 ) + ( 2 << 0x00 ),
	};

	enum {
		ATTR_X_PIV,
		ATTR_Y_PIV,
		ATTR_Z_PIV,
		
		ATTR_X_LOC,
		ATTR_Y_LOC,
		ATTR_Z_LOC,
		
		ATTR_X_ROT,
		ATTR_Y_ROT,
		ATTR_Z_ROT,
		
		ATTR_X_SCL,
		ATTR_Y_SCL,
		ATTR_Z_SCL,
		
		ATTR_ROTATE_QUAT,
		ATTR_TRANSLATE,
		
		TOTAL_ATTR,
	};
	
	GET_SET ( ZLVec3D, Piv, mPiv )
	GET_SET ( ZLVec3D, Loc, mLoc )
	GET_SET ( ZLVec3D, Scl, mScale )
	GET_SET ( ZLVec3D, Rot, mRot )
	
	GET_SET ( float, XLoc, mLoc.mX )
	GET_SET ( float, YLoc, mLoc.mY )
	GET_SET ( float, ZLoc, mLoc.mZ )
	
	GET_SET ( u32, EulerOrder, mEulerOrder )
	
	//----------------------------------------------------------------//
	ZLAffine3D				GetBillboardMtx				( const ZLAffine3D& faceCameraMtx ) const;
							MOAITransform				();
							~MOAITransform				();
	void					RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void					RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void					SerializeIn					( MOAIRubyState& state, MOAIDeserializer& serializer );
	void					SerializeOut				( MOAIRubyState& state, MOAISerializer& serializer );
	void					SetLoc						( float x, float y, float z );
	void					SetPiv						( float x, float y, float z );
	void					SetRot						( float x, float y, float z );
	void					SetScl						( float x, float y, float z );
};

#endif
