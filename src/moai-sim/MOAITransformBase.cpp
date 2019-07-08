// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAITransformBase.h>

//================================================================//
// MOAITransformBase
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getWorldDir
	@text	Returns the normalized direction vector of the transform.
			This value is returned in world space so includes parent
			transforms (if any).
	
	@in		MOAITransformBase self
	@out	number xDirection
	@out	number yDirection
	@out	number zDirection
*/
mrb_value MOAITransformBase::_getWorldDir ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransformBase, "U" )

	self->ForceUpdate ();

	ZLVec3D worldDir = self->mLocalToWorldMtx.GetHeading ();

	return state.Get ( worldDir );
}

//----------------------------------------------------------------//
/**	@lua	getWorldLoc
	@text	Get the transform's location in world space.
	
	@in		MOAITransformBase self
	@out	number xLoc
	@out	number yLoc
	@out	number zLoc
*/
mrb_value MOAITransformBase::_getWorldLoc ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransformBase, "U" )

	self->ForceUpdate ();
	
	ZLVec3D worldLoc = self->mLocalToWorldMtx.GetTranslation ();

	return state.Get ( worldLoc );
}

//----------------------------------------------------------------//
/**	@lua	getWorldRot
	@text	Get the transform's rotation in world space.
	
	@in		MOAITransformBase self
	@out	number degrees
*/
mrb_value MOAITransformBase::_getWorldRot ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransformBase, "U" )

	self->ForceUpdate ();

	return state.ToRValue ( self->mLocalToWorldMtx.GetRot () );
}

//----------------------------------------------------------------//
/**	@lua	getWorldScl
	@text	Get the transform's scale in world space.
	
	@in		MOAITransformBase self
	@out	number xScale
	@out	number yScale
	@out	number zScale
*/
mrb_value MOAITransformBase::_getWorldScl ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransformBase, "U" )

	self->ForceUpdate ();

	ZLVec3D worldScl = self->mLocalToWorldMtx.GetStretch ();

	return state.Get ( worldScl );
}

//----------------------------------------------------------------//
/**	@lua	getWorldXAxis
	@text	Return the X axis of the coordinate system (includes scale);
	
	@in		MOAITransform self
	@out	number x
	@out	number y
	@out	number z
*/
mrb_value MOAITransformBase::_getWorldXAxis ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransformBase, "U" )

	self->ForceUpdate ();

	ZLVec3D xAxis = self->mLocalToWorldMtx.GetXAxis ();

	return state.Get ( xAxis );
}

//----------------------------------------------------------------//
/**	@lua	getWorldXAxis
	@text	Return the Y axis of the coordinate system (includes scale);
	
	@in		MOAITransform self
	@out	number x
	@out	number y
	@out	number z
*/
mrb_value MOAITransformBase::_getWorldYAxis ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransformBase, "U" )

	self->ForceUpdate ();

	ZLVec3D yAxis = self->mLocalToWorldMtx.GetYAxis ();

	return state.Get ( yAxis );
}

//----------------------------------------------------------------//
/**	@lua	getWorldXAxis
	@text	Return the Z axis of the coordinate system (includes scale);
	
	@in		MOAITransform self
	@out	number x
	@out	number y
	@out	number z
*/
mrb_value MOAITransformBase::_getWorldZAxis ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransformBase, "U" )

	self->ForceUpdate ();

	ZLVec3D zAxis = self->mLocalToWorldMtx.GetZAxis ();

	return state.Get ( zAxis );
}

//----------------------------------------------------------------//
/**	@lua	getWorldXNormal
	@text	Return the X axis of the coordinate system (normalized);
	
	@in		MOAITransform self
	@out	number x
	@out	number y
	@out	number z
	@opt	number length
*/
mrb_value MOAITransformBase::_getWorldXNormal ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransformBase, "U" )

	self->ForceUpdate ();

	ZLVec3D axis = self->mLocalToWorldMtx.GetXAxis ();
	float length = axis.NormSafe ();

	mrb_value ret [ 4 ];
	ret [ 0 ] = state.ToRValue ( axis.mX );
	ret [ 1 ] = state.ToRValue ( axis.mY );
	ret [ 2 ] = state.ToRValue ( axis.mZ );
	ret [ 3 ] = state.ToRValue ( length );

	return mrb_ary_new_from_values ( state, 4, ret );
}

//----------------------------------------------------------------//
/**	@lua	getWorldYNormal
	@text	Return the Y axis of the coordinate system (normalized);
	
	@in		MOAITransform self
	@out	number x
	@out	number y
	@out	number z
	@opt	number length
*/
mrb_value MOAITransformBase::_getWorldYNormal ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransformBase, "U" )

	self->ForceUpdate ();

	ZLVec3D axis = self->mLocalToWorldMtx.GetYAxis ();
	float length = axis.NormSafe ();

	mrb_value ret [ 4 ];
	ret [ 0 ] = state.ToRValue ( axis.mX );
	ret [ 1 ] = state.ToRValue ( axis.mY );
	ret [ 2 ] = state.ToRValue ( axis.mZ );
	ret [ 3 ] = state.ToRValue ( length );

	return mrb_ary_new_from_values ( state, 4, ret );
}

//----------------------------------------------------------------//
/**	@lua	getWorldZNormal
	@text	Return the Z axis of the coordinate system (normalized);
	
	@in		MOAITransform self
	@out	number x
	@out	number y
	@out	number z
	@opt	number length
*/
mrb_value MOAITransformBase::_getWorldZNormal ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransformBase, "U" )

	self->ForceUpdate ();

	ZLVec3D axis = self->mLocalToWorldMtx.GetZAxis ();
	float length = axis.NormSafe ();

	mrb_value ret [ 4 ];
	ret [ 0 ] = state.ToRValue ( axis.mX );
	ret [ 1 ] = state.ToRValue ( axis.mY );
	ret [ 2 ] = state.ToRValue ( axis.mZ );
	ret [ 3 ] = state.ToRValue ( length );

	return mrb_ary_new_from_values ( state, 4, ret );
}

//----------------------------------------------------------------//
/**	@lua	modelToWorld
	@text	Transform a point in model space to world space.
	
	@in		MOAITransform self
	@opt	number x			Default value is 0.
	@opt	number y			Default value is 0.
	@opt	number z			Default value is 0.
	@opt	number w			Default value is 1.
	@out	number x
	@out	number y
	@out	number z
	@out	number w
*/
mrb_value MOAITransformBase::_modelToWorld ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransformBase, "U" )

	self->ForceUpdate ();

	ZLVec4D loc;
	loc.mX = state.GetParamValue < float >( 1, 0.0f );
	loc.mY = state.GetParamValue < float >( 2, 0.0f );
	loc.mZ = state.GetParamValue < float >( 3, 0.0f );
	loc.mW = state.GetParamValue < float >( 4, 1.0f );

	ZLAffine3D modelToWorld = self->GetLocalToWorldMtx ();
	modelToWorld.Transform ( loc );

	return state.Get ( loc );
}

//----------------------------------------------------------------//
/**	@lua	setParent
	@text	This method has been deprecated. Use MOAINode setAttrLink instead.
	
	@in		MOAITransformBase self
	@opt	MOAINode parent		Default value is nil.
	@out	nil
*/
mrb_value MOAITransformBase::_setParent ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransformBase, "U" )

	MOAINode* parent = state.GetRubyObject < MOAINode >( 1, true );
	
	self->SetAttrLink ( PACK_ATTR ( MOAITransformBase, INHERIT_TRANSFORM ), parent, PACK_ATTR ( MOAITransformBase, TRANSFORM_TRAIT ));
	
	//MOAILogF ( state, MOAISTRING_FunctionDeprecated_S, "setParent" );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	worldToModel
	@text	Transform a point in world space to model space.
	
	@in		MOAITransform self
	@opt	number x			Default value is 0.
	@opt	number y			Default value is 0.
	@opt	number z			Default value is 0.
	@opt	number w			Default value is 1.
	@out	number x
	@out	number y
	@out	number z
	@out	number w
*/
mrb_value MOAITransformBase::_worldToModel ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransformBase, "U" )

	self->ForceUpdate ();

	ZLVec4D loc;
	loc.mX = state.GetParamValue < float >( 1, 0.0f );
	loc.mY = state.GetParamValue < float >( 2, 0.0f );
	loc.mZ = state.GetParamValue < float >( 3, 0.0f );
	loc.mW = state.GetParamValue < float >( 4, 1.0f );

	ZLAffine3D worldToModel = self->GetWorldToLocalMtx ();
	worldToModel.Transform ( loc );

	return state.Get ( loc );
}

//================================================================//
// MOAITransformBase
//================================================================//

//----------------------------------------------------------------//
const ZLAffine3D& MOAITransformBase::GetLocalToWorldMtx () const {

	return this->mLocalToWorldMtx;
}

//----------------------------------------------------------------//
const ZLAffine3D* MOAITransformBase::GetLocTrait () const {

	return &this->mLocalToWorldMtx;
}

//----------------------------------------------------------------//
const ZLAffine3D* MOAITransformBase::GetTransformTrait () const {

	return &this->mLocalToWorldMtx;
}

//----------------------------------------------------------------//
const ZLAffine3D& MOAITransformBase::GetWorldToLocalMtx () const {

	return this->mWorldToLocalMtx;
}

//----------------------------------------------------------------//
MOAITransformBase::MOAITransformBase () {
	
	RTTI_SINGLE ( MOAINode )
	
	this->mLocalToWorldMtx.Ident ();
	this->mWorldToLocalMtx.Ident ();
}

//----------------------------------------------------------------//
MOAITransformBase::~MOAITransformBase () {
}

//----------------------------------------------------------------//
void MOAITransformBase::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	MOAINode::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "ATTR_WORLD_X_LOC",	MOAITransformBaseAttr::Pack ( ATTR_WORLD_X_LOC ));
	state.DefineClassConst ( klass, "ATTR_WORLD_Y_LOC",	MOAITransformBaseAttr::Pack ( ATTR_WORLD_Y_LOC ));
	state.DefineClassConst ( klass, "ATTR_WORLD_Z_LOC",	MOAITransformBaseAttr::Pack ( ATTR_WORLD_Z_LOC ));
	state.DefineClassConst ( klass, "ATTR_WORLD_Z_ROT",	MOAITransformBaseAttr::Pack ( ATTR_WORLD_Z_ROT ));
	state.DefineClassConst ( klass, "ATTR_WORLD_X_SCL",	MOAITransformBaseAttr::Pack ( ATTR_WORLD_X_SCL ));
	state.DefineClassConst ( klass, "ATTR_WORLD_Y_SCL",	MOAITransformBaseAttr::Pack ( ATTR_WORLD_Y_SCL ));
	state.DefineClassConst ( klass, "ATTR_WORLD_Z_SCL",	MOAITransformBaseAttr::Pack ( ATTR_WORLD_Z_SCL ));
	state.DefineClassConst ( klass, "TRANSFORM_TRAIT",	MOAITransformBaseAttr::Pack ( TRANSFORM_TRAIT ));
	
	state.DefineClassConst ( klass, "INHERIT_LOC",			MOAITransformBaseAttr::Pack ( INHERIT_LOC ));
	state.DefineClassConst ( klass, "INHERIT_TRANSFORM",	MOAITransformBaseAttr::Pack ( INHERIT_TRANSFORM ));
}

//----------------------------------------------------------------//
void MOAITransformBase::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAINode::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getWorldDir", _getWorldDir, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getWorldLoc", _getWorldLoc, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getWorldRot", _getWorldRot, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getWorldScl", _getWorldScl, MRB_ARGS_NONE () );

	state.DefineInstanceMethod ( klass, "getWorldXAxis", _getWorldXAxis, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getWorldYAxis", _getWorldYAxis, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getWorldZAxis", _getWorldZAxis, MRB_ARGS_NONE () );

	state.DefineInstanceMethod ( klass, "getWorldXNormal", _getWorldXNormal, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getWorldYNormal", _getWorldYNormal, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getWorldZNormal", _getWorldZNormal, MRB_ARGS_NONE () );

	state.DefineInstanceMethod ( klass, "modelToWorld", _modelToWorld, MRB_ARGS_ARG ( 0, 4 ) );
	state.DefineInstanceMethod ( klass, "setParent", _setParent, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineInstanceMethod ( klass, "worldToModel", _worldToModel, MRB_ARGS_ARG ( 0, 4 ) );
	
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAITransformBase::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	// TODO: these values may need to be cached for performance reasons
	if ( MOAITransformBaseAttr::Check ( attrID )) {

		switch ( UNPACK_ATTR ( attrID )) {
			
			case ATTR_WORLD_X_LOC:
				attr.Apply ( this->mLocalToWorldMtx.m [ ZLAffine3D::C3_R0 ], op, MOAIAttribute::ATTR_READ );
				return true;
			
			case ATTR_WORLD_Y_LOC:
				attr.Apply ( this->mLocalToWorldMtx.m [ ZLAffine3D::C3_R1 ], op, MOAIAttribute::ATTR_READ );
				return true;
			
			case ATTR_WORLD_Z_LOC:
				attr.Apply ( this->mLocalToWorldMtx.m [ ZLAffine3D::C3_R2 ], op, MOAIAttribute::ATTR_READ );
				return true;
			
			case ATTR_WORLD_Z_ROT: {
				float rot = ( float )( atan2 ( this->mLocalToWorldMtx.m [ ZLAffine3D::C0_R0 ], this->mLocalToWorldMtx.m [ ZLAffine3D::C0_R1 ]) * R2D );
				attr.Apply ( rot, op, MOAIAttribute::ATTR_READ );
				return true;
			}
			case ATTR_WORLD_X_SCL: {
				
				ZLVec3D axis;
			
				axis.mX =	this->mLocalToWorldMtx.m [ ZLAffine3D::C0_R0 ];
				axis.mY =	this->mLocalToWorldMtx.m [ ZLAffine3D::C0_R1 ];
				axis.mZ =	this->mLocalToWorldMtx.m [ ZLAffine3D::C0_R2 ];
			
				attr.Apply ( axis.Length (), op, MOAIAttribute::ATTR_READ );
				return true;
			}
			case ATTR_WORLD_Y_SCL: {
				
				ZLVec3D axis;
			
				axis.mX =	this->mLocalToWorldMtx.m [ ZLAffine3D::C1_R0 ];
				axis.mY =	this->mLocalToWorldMtx.m [ ZLAffine3D::C1_R1 ];
				axis.mZ =	this->mLocalToWorldMtx.m [ ZLAffine3D::C1_R2 ];
				
				attr.Apply ( axis.Length (), op, MOAIAttribute::ATTR_READ );
				return true;
			}
			case ATTR_WORLD_Z_SCL: {
				
				ZLVec3D axis;
			
				axis.mX =	this->mLocalToWorldMtx.m [ ZLAffine3D::C2_R0 ];
				axis.mY =	this->mLocalToWorldMtx.m [ ZLAffine3D::C2_R1 ];
				axis.mZ =	this->mLocalToWorldMtx.m [ ZLAffine3D::C2_R2 ];
				
				attr.Apply ( axis.Length (), op, MOAIAttribute::ATTR_READ );
				return true;
			}
			case TRANSFORM_TRAIT:
			
				attr.ApplyNoAdd ( this->mLocalToWorldMtx, op, MOAIAttribute::ATTR_READ );
				return true;
		}
	}
	return false;
}

//----------------------------------------------------------------//
void MOAITransformBase::MOAINode_Update () {
	
	this->MOAITransformBase_BuildLocalToWorldMtx ( this->mLocalToWorldMtx );
	
	MOAIAttribute attr;
	if ( this->PullLinkedAttr ( MOAITransformBaseAttr::Pack ( INHERIT_TRANSFORM ), attr )) {
		const ZLAffine3D inherit = attr.GetValue ( ZLAffine3D::IDENT );
		this->mLocalToWorldMtx.Append ( inherit );
	}
	else {
	
		if ( this->PullLinkedAttr ( MOAITransformBaseAttr::Pack ( INHERIT_LOC ), attr )) {
			
			const ZLAffine3D inherit = attr.GetValue ( ZLAffine3D::IDENT );
			
			ZLVec3D loc = this->mLocalToWorldMtx.GetTranslation ();
			
			inherit.Transform ( loc );
			
			this->mLocalToWorldMtx.m [ ZLAffine3D::C3_R0 ] = loc.mX;
			this->mLocalToWorldMtx.m [ ZLAffine3D::C3_R1 ] = loc.mY;
			this->mLocalToWorldMtx.m [ ZLAffine3D::C3_R2 ] = loc.mZ;
		}
	}
	
	this->mWorldToLocalMtx.Inverse ( this->mLocalToWorldMtx );
}
