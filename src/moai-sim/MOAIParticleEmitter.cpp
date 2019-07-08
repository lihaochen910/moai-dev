// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIParticleEmitter.h>
#include <moai-sim/MOAIParticleSystem.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	setAngle
	@text	Set the size and angle of the emitter.
	
	@in		MOAIParticleEmitter self
	@in		number min					Minimum angle in degrees.
	@in		number max					Maximum angle in degrees.
	@out	nil
*/
mrb_value MOAIParticleEmitter::_setAngle ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleEmitter, "UNN" )

	float min = state.GetParamValue < float >( 1, 0.0f );
	float max = state.GetParamValue < float >( 2, 360.0f );

	self->SetAngleRange ( min, max );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setEmission
	@text	Set the size of each emission.
	
	@in		MOAIParticleEmitter self
	@in		number min					Minimum emission size.
	@opt	number max					Maximum emission size. Defaults to min.
	@out	nil
*/
mrb_value MOAIParticleEmitter::_setEmission ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleEmitter, "UN" )

	u32 min = state.GetParamValue < u32 >( 1, 1 );
	u32 max = state.GetParamValue < u32 >( 2, min );

	self->SetEmissionRange ( min, max );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setMagnitude
	@text	Set the starting magnitude of particles deltas.
	
	@in		MOAIParticleEmitter self
	@in		number min					Minimum magnitude.
	@opt	number max					Maximum magnitude. Defaults to min.
	@out	nil
*/
mrb_value MOAIParticleEmitter::_setMagnitude ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleEmitter, "UN" )

	float min = state.GetParamValue < float >( 1, 1.0f );
	float max = state.GetParamValue < float >( 2, min );

	self->SetMagnitudeRange ( min, max );
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIParticleEmitter::_setMask ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleEmitter, "U" )
	
	self->mMaskProp.Set ( *self, state.GetRubyObject < MOAIPartitionHull >( 1, true ));
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setRadius
	@text	Set the shape and radius of the emitter.
	
	@overload
	
		@in		MOAIParticleEmitter self
		@in		number radius
		@out	nil
	
	@overload
	
		@in		MOAIParticleEmitter self
		@in		number innerRadius
		@in		number outerRadius
		@out	nil
*/
mrb_value MOAIParticleEmitter::_setRadius ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleEmitter, "UN" )
	
	self->mShapeID = CIRCLE;

	if ( state.GetParamsCount () >= 3 ) {
		self->mInnerRadius = state.GetParamValue < float >( 1, 0.0f );
		self->mOuterRadius = state.GetParamValue < float >( 2, 0.0f );
		
	}
	else {
		self->mInnerRadius = 0.0f;
		self->mOuterRadius = state.GetParamValue < float >( 1, 0.0f );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setRect
	@text	Set the shape and dimensions of the emitter.
	
	@in		MOAIParticleEmitter self
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@out	nil
*/
mrb_value MOAIParticleEmitter::_setRect ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleEmitter, "UNNNN" )

	ZLRect rect;
	rect.mXMin = state.GetParamValue < float >( 1, 0.0f );
	rect.mYMin = state.GetParamValue < float >( 2, 0.0f );
	rect.mXMax = state.GetParamValue < float >( 3, 0.0f );
	rect.mYMax = state.GetParamValue < float >( 4, 0.0f );
	
	rect.Bless ();
	
	self->SetRect ( rect );
	self->SetShapeID ( RECT );
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setState
	@text	Set initial state index for new particles.
	
	@in		MOAIParticleEmitter self
	@in		number	state	index of MOAIParticleState in attached MOAIParticleSystem
	@out	nil
*/
mrb_value MOAIParticleEmitter::_setState ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleEmitter, "U" )
	
	self->mParticleState = state.GetParamValue < u32 >( 1, 1 ) - 1;
	
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	setSystem
	@text	Attaches the emitter to a particle system.
	
	@in		MOAIParticleEmitter self
	@in		MOAIParticleSystem system
	@out	nil
*/
mrb_value MOAIParticleEmitter::_setSystem ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleEmitter, "UU" )

	self->mSystem.Set ( *self, state.GetRubyObject < MOAIParticleSystem >( 1, true ));

	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	surge
	@text	Forces the emission of one or more particles.
	
	@in		MOAIParticleEmitter self
	@opt	number total				Size of surge. Default value is a random emission value for emitter.
	@out	nil
*/
mrb_value MOAIParticleEmitter::_surge ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIParticleEmitter, "U" )

	u32 total = state.GetParamValue < u32 >( 1, self->GetRandomEmission ());

	self->Surge ( total );
	return mrb_nil_value ();
}

//================================================================//
// MOAIParticleEmitter
//================================================================//

//----------------------------------------------------------------//
u32 MOAIParticleEmitter::GetRandomEmission () {

	return ( u32 )ZLFloat::Rand (( float )this->mMinEmission, ( float )this->mMaxEmission );
}

//----------------------------------------------------------------//
void MOAIParticleEmitter::GetRandomParticle ( ZLVec3D& loc, ZLVec3D& vec ) {
	
	switch ( this->mShapeID ) {
	
		case CIRCLE:
			
			loc = this->GetRandomVec ( 0.0f, 360.0f, this->mInnerRadius, this->mOuterRadius );
			break;

		default:
		case POINT:
			
			loc.mX = 0.0f;
			loc.mY = 0.0f;
			loc.mZ = 0.0f;
			break;

		case RECT:
		
			loc.mX = ZLFloat::Rand ( this->mRect.mXMin, this->mRect.mXMax );
			loc.mY = ZLFloat::Rand ( this->mRect.mYMin, this->mRect.mYMax );
			loc.mZ = 0.0f;
			break;
	}
	
	vec = this->GetRandomVec ( this->mMinAngle, this->mMaxAngle, this->mMinMagnitude, this->mMaxMagnitude );
}

//----------------------------------------------------------------//
ZLVec3D MOAIParticleEmitter::GetRandomVec ( float minAngle, float maxAngle, float min, float max ) {

	float r = ZLFloat::Rand ( minAngle, maxAngle ) * ( float )D2R;
	float m = ZLFloat::Rand ( min, max );
	
	ZLVec3D vec;
	vec.mX = Cos ( r ) * m;
	vec.mY = Sin ( r ) * m;
	vec.mZ = 0.0f;
	
	return vec;
}

//----------------------------------------------------------------//
bool MOAIParticleEmitter::MaskParticle ( const ZLVec3D& loc ) {

	return this->mMaskProp ? this->mMaskProp->Inside ( loc, 0.0f ) : true;
}

//----------------------------------------------------------------//
MOAIParticleEmitter::MOAIParticleEmitter () :
	mShapeID ( POINT ),
	mInnerRadius ( 0.0f ),
	mOuterRadius ( 0.0f ),
	mMinEmission ( 1 ),
	mMaxEmission ( 1 ),
	mMinAngle ( 0.0f ),
	mMaxAngle ( 360.0f ),
	mMinMagnitude ( 0.0f ),
	mMaxMagnitude ( 1.0f ),
	mEmission ( 0 ),
	mParticleState ( 0 ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAITransform )
		RTTI_EXTEND ( MOAIAction )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIParticleEmitter::~MOAIParticleEmitter () {

	this->mSystem.Set ( *this, 0 );
	this->mMaskProp.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAIParticleEmitter::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAITransform::RegisterRubyClass ( state, klass );
	MOAIAction::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIParticleEmitter::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAITransform::RegisterRubyFuncs ( state, klass );
	MOAIAction::RegisterRubyFuncs ( state, klass );
	
	state.DefineInstanceMethod ( klass, "setAngle",			_setAngle, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setEmission",		_setEmission, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "setMagnitude",		_setMagnitude, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "setMask",			_setMask, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setRadius",		_setRadius, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "setRect",			_setRect, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "setState",			_setState, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setSystem",		_setSystem, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "surge",			_surge, MRB_ARGS_ARG ( 0, 1 ) );

}

//----------------------------------------------------------------//
void MOAIParticleEmitter::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {

	MOAITransform::SerializeIn ( state, serializer );
	MOAIAction::SerializeIn ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIParticleEmitter::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {

	MOAITransform::SerializeOut ( state, serializer );
	MOAIAction::SerializeOut ( state, serializer );
}

//----------------------------------------------------------------//
void MOAIParticleEmitter::SetAngleRange ( float min, float max ) {

	this->mMinAngle = min;
	this->mMaxAngle = max;
}

//----------------------------------------------------------------//
void MOAIParticleEmitter::SetEmissionRange ( u32 min, u32 max ) {

	this->mMinEmission = min;
	this->mMaxEmission = max;
}

//----------------------------------------------------------------//
void MOAIParticleEmitter::SetMagnitudeRange ( float min, float max ) {

	this->mMinMagnitude = min;
	this->mMaxMagnitude = max;
}

//----------------------------------------------------------------//
void MOAIParticleEmitter::Surge ( u32 total ) {
	
	this->mEmission += total;
	this->ScheduleUpdate ();
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAIParticleEmitter::MOAIAction_IsDone () {

	return false;
}

//----------------------------------------------------------------//
void MOAIParticleEmitter::MOAINode_Update () {

	MOAITransform::MOAINode_Update ();
	
	if ( this->mSystem ) {

		ZLVec3D loc;
		ZLVec3D vec;
		for ( u32 i = 0; i < this->mEmission; ++i ) {
			this->GetRandomParticle ( loc, vec );
			this->mLocalToWorldMtx.Transform ( loc );
			
			if ( this->MaskParticle ( loc )) {
			
				this->mLocalToWorldMtx.TransformVec ( vec );
				this->mSystem->PushParticle ( loc.mX, loc.mY, vec.mX, vec.mY, this->mParticleState );
			}
		}
	}
	
	this->mEmission = 0;
}
