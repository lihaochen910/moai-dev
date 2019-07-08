// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIParticle.h>
#include <moai-sim/MOAIParticleForce.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	initAttractor
	@text	Greater force is exerted on particles as they approach
			attractor.
	
	@in		MOAIParticleForce self
	@in		number radius			Size of the attractor.
	@opt	number magnitude		Strength of the attractor.
	@out	nil
*/
mrb_value MOAIParticleForce::_initAttractor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleForce, "UNN" )

	self->mShape	= ATTRACTOR;
	self->mRadius	= state.GetParamValue < float >( 1, 0.0f );
	self->mPull		= state.GetParamValue < float >( 2, 0.0f );
	
	self->ScheduleUpdate ();

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	initBasin
	@text	Greater force is exerted on particles as they leave
			attractor.
	
	@in		MOAIParticleForce self
	@in		number radius				Size of the attractor.
	@opt	number magnitude			Strength of the attractor.
	@out	nil
*/
mrb_value MOAIParticleForce::_initBasin ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleForce, "UNN" )

	self->mShape	= BASIN;
	self->mRadius	= state.GetParamValue < float >( 1, 0.0f );
	self->mPull		= state.GetParamValue < float >( 2, 0.0f );

	self->ScheduleUpdate ();

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	initLinear
	@text	A constant linear force will be applied to the particles.
	
	@in		MOAIParticleForce self
	@in		number x
	@opt	number y
	@out	nil
*/
mrb_value MOAIParticleForce::_initLinear ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleForce, "UNN" )

	self->mShape = LINEAR;
	self->mVec.mX = state.GetParamValue < float >( 1, 0.0f );
	self->mVec.mY = state.GetParamValue < float >( 2, 0.0f );

	self->ScheduleUpdate ();

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	initRadial
	@text	A constant radial force will be applied to the particles.
	
	@in		MOAIParticleForce self
	@in		number magnitude
	@out	nil
*/
mrb_value MOAIParticleForce::_initRadial ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleForce, "UN" )

	self->mShape = RADIAL;
	self->mPull = state.GetParamValue < float >( 1, 0.0f );

	self->ScheduleUpdate ();

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setType
	@text	Set the type of force. FORCE will factor in the particle's mass. GRAVITY
			will ignore the particle's mass. OFFSET will ignore both mass and damping.
	
	@in		MOAIParticleForce self
	@in		number type				One of MOAIParticleForce.FORCE, MOAIParticleForce.GRAVITY, MOAIParticleForce.OFFSET
	@out	nil
*/
mrb_value MOAIParticleForce::_setType ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleForce, "UN" )

	self->mType = state.GetParamValue < u32 >( 1, self->mType );

	return mrb_nil_value ();
}

//================================================================//
// MOAIParticleForce
//================================================================//

//----------------------------------------------------------------//
void MOAIParticleForce::Eval ( const ZLVec3D& loc, float mass, ZLVec3D& acceleration, ZLVec3D& offset ) {

	ZLVec3D force ( 0.0f, 0.0f, 0.0f );
	ZLVec3D origin = this->mLocalToWorldMtx.GetTranslation ();

	switch ( this->mShape ) {
		
		case ATTRACTOR: {
		
			ZLVec3D vec = origin;
			vec.Sub ( loc );
			
			float dist = vec.NormSafe () / this->mRadius;
			if ( dist <= 1.0f ) {
				
				dist = 1.0f - dist;
				dist = dist * dist * this->mPull;
				
				force.mX = vec.mX * dist;
				force.mY = vec.mY * dist;
			}
			break;
		}
		case BASIN: {
			
			ZLVec3D vec = origin;
			vec.Sub ( loc );
			
			float dist = vec.NormSafe () / this->mRadius;
			
			dist = dist * dist * this->mPull;
			
			force.mX = vec.mX * dist;
			force.mY = vec.mY * dist;
			
			break;
		}
		case LINEAR: {
			force = this->mWorldVec;
			break;
		}
		case RADIAL: {
			
			force = origin;
			force.Sub ( loc );
			force.NormSafe ();
			force.Scale ( this->mPull );
			
			break;
		}
	}
	
	switch ( this->mType ) {
	
		case FORCE:
			force.Scale ( 1.0f / mass );
			acceleration.Add ( force );
			break;
	
		case GRAVITY:
			acceleration.Add ( force );
			break;
		
		case OFFSET:
			offset.Add ( force );
			break;
	}
}

//----------------------------------------------------------------//
MOAIParticleForce::MOAIParticleForce () :
	mShape ( LINEAR ),
	mType ( GRAVITY ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAITransform )
	RTTI_END
	
	this->mVec.Init ( 0.0f, 0.0f, 0.0f );
	
	this->mWorldLoc.Init ( 0.0f, 0.0f, 0.0f );
	this->mWorldVec.Init ( 0.0f, 0.0f, 0.0f );
}

//----------------------------------------------------------------//
MOAIParticleForce::~MOAIParticleForce () {
}

//----------------------------------------------------------------//
void MOAIParticleForce::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAITransform::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "FORCE", ( u32 )FORCE );
	state.DefineClassConst ( klass, "GRAVITY", ( u32 )GRAVITY );
	state.DefineClassConst ( klass, "OFFSET", ( u32 )OFFSET );
}

//----------------------------------------------------------------//
void MOAIParticleForce::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAITransform::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "initAttractor",	_initAttractor, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "initBasin",		_initBasin, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "initLinear",		_initLinear, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "initRadial",		_initRadial, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setType",			_setType, MRB_ARGS_REQ ( 1 ) );
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAIParticleForce::MOAINode_Update () {

	this->MOAITransformBase::MOAINode_Update ();
	
	const ZLAffine3D& localToWorldMtx = this->GetLocalToWorldMtx ();
	
	this->mWorldLoc = localToWorldMtx.GetTranslation ();
	
	this->mWorldVec = this->mVec;
	localToWorldMtx.TransformVec ( this->mWorldVec );
}
