// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAITransform.h>
#include <moai-sim/MOAISim.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	addLoc
	@text	Adds a delta to the transform's location.
	
	@in		MOAITransform self
	@in		number xDelta
	@in		number yDelta
	@in		number zDelta
	@out	nil
*/
mrb_value MOAITransform::_addLoc ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )
	
	ZLVec3D loc = self->GetLoc ();
	
	loc.mX += state.GetParamValue < float >( 1, 0.0f );
	loc.mY += state.GetParamValue < float >( 2, 0.0f );
	loc.mZ += state.GetParamValue < float >( 3, 0.0f );
	
	self->SetLoc ( loc );
	self->ScheduleUpdate ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	addPiv
	@text	Adds a delta to the transform's pivot.
	
	@in		MOAITransform self
	@in		number xDelta
	@in		number yDelta
	@in		number zDelta
	@out	nil
*/
mrb_value MOAITransform::_addPiv ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )
	
	ZLVec3D piv = self->GetPiv ();
	
	piv.mX += state.GetParamValue < float >( 1, 0.0f );
	piv.mY += state.GetParamValue < float >( 2, 0.0f );
	piv.mZ += state.GetParamValue < float >( 3, 0.0f );
	
	self->SetPiv ( piv );
	self->ScheduleUpdate ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	addRot
	@text	Adds a delta to the transform's rotation
	
	@in		MOAITransform self
	@in		number xDelta		In degrees.
	@in		number yDelta		In degrees.
	@in		number zDelta		In degrees.
	@out	nil
*/
mrb_value MOAITransform::_addRot ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )
	
	ZLVec3D rot = self->GetRot ();
	rot.mX += state.GetParamValue < float >( 1, 0.0f );
	rot.mY += state.GetParamValue < float >( 2, 0.0f );
	rot.mZ += state.GetParamValue < float >( 3, 0.0f );
	
	self->SetRot ( rot );
	self->ScheduleUpdate ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	addScl
	@text	Adds a delta to the transform's scale
	
	@in		MOAITransform self
	@in		number xSclDelta
	@opt	number ySclDelta		Default value is xSclDelta.
	@opt	number zSclDelta		Default value is 0.
	@out	nil
*/
mrb_value MOAITransform::_addScl ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )
	
	ZLVec3D scl = self->GetScl ();
	
	float xSclDelta = state.GetParamValue < float >( 1, 0.0f );
	float ySclDelta = state.GetParamValue < float >( 2, xSclDelta );
	float zSclDelta = state.GetParamValue < float >( 3, 0.0f );
	
	scl.mX += xSclDelta;
	scl.mY += ySclDelta;
	scl.mZ += zSclDelta;
	
	self->SetScl ( scl );
	self->ScheduleUpdate ();
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	getLoc
	@text	Returns the transform's current location.
	
	@in		MOAITransform self
	@out	number xLoc
	@out	number yLoc
	@out	number zLoc
*/
mrb_value MOAITransform::_getLoc ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )
	
	return state.Get ( self->mLoc );
}

//----------------------------------------------------------------//
/**	@lua	getPiv
	@text	Returns the transform's current pivot.
	
	@in		MOAITransform self
	@out	number xPiv
	@out	number yPiv
	@out	number zPiv
*/
mrb_value MOAITransform::_getPiv ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )
	
	return state.Get ( self->mPiv );
}

//----------------------------------------------------------------//
/**	@lua	getRot
	@text	Returns the transform's current rotation.
	
	@in		MOAITransform self
	@out	number xRot			Rotation in degrees.
	@out	number yRot			Rotation in degrees.
	@out	number zRot			Rotation in degrees.
*/
mrb_value	MOAITransform::_getRot ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )
	
	return state.Get ( self->mRot );
}

//----------------------------------------------------------------//
/**	@lua	getScl
	@text	Returns the transform's current scale.
	
	@in		MOAITransform self
	@out	number xScl
	@out	number yScl
	@out	number zScl
*/
mrb_value	MOAITransform::_getScl ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )
	
	return state.Get ( self->mScale );
}

//----------------------------------------------------------------//
/**	@lua	move
	@text	Animate the transform by applying a delta. Creates and returns
			a MOAIEaseDriver initialized to apply the delta.
	
	@in		MOAITransform self
	@in		number xDelta		Delta to be added to x.
	@in		number yDelta		Delta to be added to y.
	@in		number zDelta		Delta to be added to z.
	@in		number xRotDelta	Delta to be added to x rot (in degrees).
	@in		number yRotDelta	Delta to be added to y rot (in degrees).
	@in		number zRotDelta	Delta to be added to z rot (in degrees).
	@in		number xSclDelta	Delta to be added to x scale.
	@in		number ySclDelta	Delta to be added to y scale.
	@in		number zSclDelta	Delta to be added to z scale.
	@in		number length		Length of animation in seconds.
	@opt	number mode			The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
								MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.

	@out	MOAIEaseDriver easeDriver
*/
mrb_value MOAITransform::_move ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )
	
	float delay		= state.GetParamValue < float >( 10, 0.0f );
	
	if ( delay > 0.0f ) {
	
		u32 mode = state.GetParamValue < u32 >( 11, ZLInterpolate::kSmooth );
		
		MOAIEaseDriver* action = state.CreateClassInstance < MOAIEaseDriver >();
		
		action->ParseForMove ( state, 1, self, 8, mode,
			MOAITransformAttr::Pack ( ATTR_X_LOC ), 0.0f,
			MOAITransformAttr::Pack ( ATTR_Y_LOC ), 0.0f,
			MOAITransformAttr::Pack ( ATTR_Z_LOC ), 0.0f,
			MOAITransformAttr::Pack ( ATTR_X_ROT ), 0.0f,
			MOAITransformAttr::Pack ( ATTR_Y_ROT ), 0.0f,
			MOAITransformAttr::Pack ( ATTR_Z_ROT ), 0.0f,
			MOAITransformAttr::Pack ( ATTR_X_SCL ), 0.0f,
			MOAITransformAttr::Pack ( ATTR_Y_SCL ), 0.0f,
			MOAITransformAttr::Pack ( ATTR_Z_SCL ), 0.0f
		);
		
		action->SetSpan ( delay );
		action->Start ( 0, false );
		
		return state.ToRValue < MOAIRubyObject* >( action );
	}

	if ( !state.CheckVector ( 1, 8, 0, 0 )) { // TODO: epsilon?

		self->mLoc.mX += state.GetParamValue < float >( 1, 0.0f );
		self->mLoc.mY += state.GetParamValue < float >( 2, 0.0f );
		self->mLoc.mZ += state.GetParamValue < float >( 3, 0.0f );
		
		self->mRot.mX += state.GetParamValue < float >( 4, 0.0f );
		self->mRot.mY += state.GetParamValue < float >( 5, 0.0f );
		self->mRot.mZ += state.GetParamValue < float >( 6, 0.0f );
		
		self->mScale.mX += state.GetParamValue < float >( 7, 0.0f );
		self->mScale.mY += state.GetParamValue < float >( 8, 0.0f );
		self->mScale.mZ += state.GetParamValue < float >( 9, 0.0f );
		
		self->ScheduleUpdate ();
	}

	return context;
}

//----------------------------------------------------------------//
/**	@lua	moveLoc
	@text	Animate the transform by applying a delta. Creates and returns
			a MOAIEaseDriver initialized to apply the delta.
	
	@in		MOAITransform self
	@in		number xDelta		Delta to be added to x.
	@in		number yDelta		Delta to be added to y.
	@in		number zDelta		Delta to be added to z.
	@in		number length		Length of animation in seconds.
	@opt	number mode			The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
								MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.

	@out	MOAIEaseDriver easeDriver
*/
mrb_value MOAITransform::_moveLoc ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )
	
	float delay		= state.GetParamValue < float >( 4, 0.0f );
	
	if ( delay > 0.0f ) {
	
		u32 mode = state.GetParamValue < u32 >( 5, ZLInterpolate::kSmooth );
		
		MOAIEaseDriver* action = state.CreateClassInstance < MOAIEaseDriver >();
		
		action->ParseForMove ( state, 1, self, 3, mode,
			MOAITransformAttr::Pack ( ATTR_X_LOC ), 0.0f,
			MOAITransformAttr::Pack ( ATTR_Y_LOC ), 0.0f,
			MOAITransformAttr::Pack ( ATTR_Z_LOC ), 0.0f
		);
		
		action->SetSpan ( delay );
		action->Start ( 0, false );
		
		return state.ToRValue < MOAIRubyObject* >( action );
	}
	
	if ( !state.CheckVector ( 1, 3, 0, 0 )) { // TODO: epsilon?
	
		self->mLoc.mX += state.GetParamValue < float >( 1, 0.0f );
		self->mLoc.mY += state.GetParamValue < float >( 2, 0.0f );
		self->mLoc.mZ += state.GetParamValue < float >( 3, 0.0f );
		self->ScheduleUpdate ();
	}
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	movePiv
	@text	Animate the transform by applying a delta. Creates and returns
			a MOAIEaseDriver initialized to apply the delta.
	
	@in		MOAITransform self
	@in		number xDelta		Delta to be added to xPiv.
	@in		number yDelta		Delta to be added to yPiv.
	@in		number zDelta		Delta to be added to zPiv.
	@in		number length		Length of animation in seconds.
	@opt	number mode			The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
								MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.

	@out	MOAIEaseDriver easeDriver
*/
mrb_value MOAITransform::_movePiv ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )
	
	float delay		= state.GetParamValue < float >( 4, 0.0f );
	
	if ( delay > 0.0f ) {
	
		u32 mode = state.GetParamValue < u32 >( 5, ZLInterpolate::kSmooth );
		
		MOAIEaseDriver* action = state.CreateClassInstance < MOAIEaseDriver >();
		
		action->ParseForMove ( state, 1, self, 3, mode,
			MOAITransformAttr::Pack ( ATTR_X_PIV ), 0.0f,
			MOAITransformAttr::Pack ( ATTR_Y_PIV ), 0.0f,
			MOAITransformAttr::Pack ( ATTR_Z_PIV ), 0.0f
		);
		
		action->SetSpan ( delay );
		action->Start ( 0, false );
		
		return state.ToRValue < MOAIRubyObject* >( action );
	}
	
	if ( !state.CheckVector ( 1, 3, 0, 0 )) { // TODO: epsilon?
	
		self->mPiv.mX += state.GetParamValue < float >( 1, 0.0f );
		self->mPiv.mY += state.GetParamValue < float >( 2, 0.0f );
		self->mPiv.mZ += state.GetParamValue < float >( 3, 0.0f );
		self->ScheduleUpdate ();
	}

	return context;
}

//----------------------------------------------------------------//
/**	@lua	moveRot
	@text	Animate the transform by applying a delta. Creates and returns
			a MOAIEaseDriver initialized to apply the delta.
	
	@in		MOAITransform self
	@in		number xDelta		Delta to be added to xRot (in degrees).
	@in		number yDelta		Delta to be added to yRot (in degrees).
	@in		number zDelta		Delta to be added to zRot (in degrees).
	@in		number length		Length of animation in seconds.
	@opt	number mode			The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
								MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.

	@out	MOAIEaseDriver easeDriver
*/
mrb_value MOAITransform::_moveRot ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )
	
	float delay		= state.GetParamValue < float >( 4, 0.0f );
	
	if ( delay > 0.0f ) {
	
		u32 mode = state.GetParamValue < u32 >( 5, ZLInterpolate::kSmooth );
		
		MOAIEaseDriver* action = state.CreateClassInstance < MOAIEaseDriver >();
		
		action->ParseForMove ( state, 1, self, 3, mode,
			MOAITransformAttr::Pack ( ATTR_X_ROT ), 0.0f,
			MOAITransformAttr::Pack ( ATTR_Y_ROT ), 0.0f,
			MOAITransformAttr::Pack ( ATTR_Z_ROT ), 0.0f
		);
		
		action->SetSpan ( delay );
		action->Start ( 0, false );
		
		return state.ToRValue < MOAIRubyObject* >( action );
	}
	
	if ( !state.CheckVector ( 1, 3, 0, 0 )) { // TODO: epsilon?
	
		self->mRot.mX += state.GetParamValue < float >( 1, 0.0f );
		self->mRot.mY += state.GetParamValue < float >( 2, 0.0f );
		self->mRot.mZ += state.GetParamValue < float >( 3, 0.0f );
		self->ScheduleUpdate ();
	}

	return context;
}

//----------------------------------------------------------------//
/**	@lua	moveScl
	@text	Animate the transform by applying a delta. Creates and returns
			a MOAIEaseDriver initialized to apply the delta.
	
	@in		MOAITransform self
	@in		number xSclDelta	Delta to be added to x scale.
	@in		number ySclDelta	Delta to be added to y scale.
	@in		number zSclDelta	Delta to be added to z scale.
	@in		number length		Length of animation in seconds.
	@opt	number mode			The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
								MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.

	@out	MOAIEaseDriver easeDriver
*/
mrb_value MOAITransform::_moveScl ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )

	float delay		= state.GetParamValue < float >( 4, 0.0f );

	if ( delay > 0.0f ) {
	
		u32 mode = state.GetParamValue < u32 >( 5, ZLInterpolate::kSmooth );
		
		MOAIEaseDriver* action = state.CreateClassInstance < MOAIEaseDriver >();
		
		action->ParseForMove ( state, 1, self, 3, mode,
			MOAITransformAttr::Pack ( ATTR_X_SCL ), 0.0f,
			MOAITransformAttr::Pack ( ATTR_Y_SCL ), 0.0f,
			MOAITransformAttr::Pack ( ATTR_Z_SCL ), 0.0f
		);
		
		action->SetSpan ( delay );
		action->Start ( 0, false );
		
		return state.ToRValue < MOAIRubyObject* >( action );
	}
	
	if ( !state.CheckVector ( 1, 3, 0, 0 )) { // TODO: epsilon?
	
		self->mScale.mX += state.GetParamValue < float >( 1, 0.0f );
		self->mScale.mY += state.GetParamValue < float >( 2, 0.0f );
		self->mScale.mZ += state.GetParamValue < float >( 3, 0.0f );
		self->ScheduleUpdate ();
	}
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	seek
	@text	Animate the transform by applying a delta. Delta is computed
			given a target value. Creates and returns a MOAIEaseDriver
			initialized to apply the delta.
	
	@in		MOAITransform self
	@in		number xGoal		Desired resulting value for x.
	@in		number yGoal		Desired resulting value for y.
	@in		number zGoal		Desired resulting value for z.
	@in		number xRotGoal		Desired resulting value for x rot (in degrees).
	@in		number yRotGoal		Desired resulting value for y rot (in degrees).
	@in		number zRotGoal		Desired resulting value for z rot (in degrees).
	@in		number xSclGoal		Desired resulting value for x scale.
	@in		number ySclGoal		Desired resulting value for y scale.
	@in		number zSclGoal		Desired resulting value for z scale.
	@in		number length		Length of animation in seconds.
	@opt	number mode			The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
								MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.

	@out	MOAIEaseDriver easeDriver
*/
mrb_value MOAITransform::_seek ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )
	
	float delay		= state.GetParamValue < float >( 10, 0.0f );
	
	if ( delay > 0.0f ) {
		
		u32 mode = state.GetParamValue < u32 >( 11, ZLInterpolate::kSmooth );

		MOAIEaseDriver* action = state.CreateClassInstance < MOAIEaseDriver >();
		
		action->ParseForSeek ( state, 1, self, 8, mode,
			MOAITransformAttr::Pack ( ATTR_X_LOC ), self->mLoc.mX, 0.0f,
			MOAITransformAttr::Pack ( ATTR_Y_LOC ), self->mLoc.mY, 0.0f,
			MOAITransformAttr::Pack ( ATTR_Z_LOC ), self->mLoc.mZ, 0.0f,
			MOAITransformAttr::Pack ( ATTR_X_ROT ), self->mRot.mX, 0.0f,
			MOAITransformAttr::Pack ( ATTR_Y_ROT ), self->mRot.mY, 0.0f,
			MOAITransformAttr::Pack ( ATTR_Z_ROT ), self->mRot.mZ, 0.0f,
			MOAITransformAttr::Pack ( ATTR_X_SCL ), self->mScale.mX, 1.0f,
			MOAITransformAttr::Pack ( ATTR_Y_SCL ), self->mScale.mY, 1.0f,
			MOAITransformAttr::Pack ( ATTR_Z_SCL ), self->mScale.mZ, 1.0f
		);
		
		action->SetSpan ( delay );
		action->Start ( 0, false );
		
		return state.ToRValue < MOAIRubyObject* >( action );
	}
	
	ZLVec3D loc = state.GetVec3D < float >( 1, 0.0f );
	ZLVec3D rot = state.GetVec3D < float >( 4, 0.0f );
	ZLVec3D scl = state.GetVec3D < float >( 7, 1.0f );
	
	if ( !loc.Compare( self->mLoc ) || !rot.Compare( self->mRot ) || !scl.Compare( self->mScale )) {
		self->SetLoc ( loc );
		self->SetRot ( rot );
		self->SetScl ( scl );
		self->ScheduleUpdate ();
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	seekLoc
	@text	Animate the transform by applying a delta. Delta is computed
			given a target value. Creates and returns a MOAIEaseDriver
			initialized to apply the delta.
	
	@in		MOAITransform self
	@in		number xGoal		Desired resulting value for x.
	@in		number yGoal		Desired resulting value for y.
	@in		number zGoal		Desired resulting value for z.
	@in		number length		Length of animation in seconds.
	@opt	number mode			The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
								MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.

	@out	MOAIEaseDriver easeDriver
*/
mrb_value MOAITransform::_seekLoc ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )

	float delay		= state.GetParamValue < float >( 4, 0.0f );
	
	if ( delay > 0.0f ) {

		u32 mode = state.GetParamValue < u32 >( 5, ZLInterpolate::kSmooth );		
		
		MOAIEaseDriver* action = state.CreateClassInstance < MOAIEaseDriver >();
		
		action->ParseForSeek ( state, 1, self, 3, mode,
			MOAITransformAttr::Pack ( ATTR_X_LOC ), self->mLoc.mX, 0.0f,
			MOAITransformAttr::Pack ( ATTR_Y_LOC ), self->mLoc.mY, 0.0f,
			MOAITransformAttr::Pack ( ATTR_Z_LOC ), self->mLoc.mZ, 0.0f
		);
		
		action->SetSpan ( delay );
		action->Start ( 0, false );
		
		return state.ToRValue < MOAIRubyObject* >( action );
	}
	
	ZLVec3D loc = state.GetVec3D < float >( 1, 0.0f );
	if ( !loc.Compare ( self->mLoc )) {
		self->SetLoc ( loc );
		self->ScheduleUpdate ();
	}
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	seekPiv
	@text	Animate the transform by applying a delta. Delta is computed
			given a target value. Creates and returns a MOAIEaseDriver
			initialized to apply the delta.
	
	@in		MOAITransform self
	@in		number xGoal		Desired resulting value for xPiv.
	@in		number yGoal		Desired resulting value for yPiv.
	@in		number zGoal		Desired resulting value for zPiv.
	@in		number length		Length of animation in seconds.
	@opt	number mode			The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
								MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.

	@out	MOAIEaseDriver easeDriver
*/
mrb_value MOAITransform::_seekPiv ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )

	float delay		= state.GetParamValue < float >( 4, 0.0f );
	
	if ( delay > 0.0f ) {

		u32 mode = state.GetParamValue < u32 >( 5, ZLInterpolate::kSmooth );		
		
		MOAIEaseDriver* action = state.CreateClassInstance < MOAIEaseDriver >();
		
		action->ParseForSeek ( state, 1, self, 3, mode,
			MOAITransformAttr::Pack ( ATTR_X_PIV ), self->mPiv.mX, 0.0f,
			MOAITransformAttr::Pack ( ATTR_Y_PIV ), self->mPiv.mY, 0.0f,
			MOAITransformAttr::Pack ( ATTR_Z_PIV ), self->mPiv.mZ, 0.0f
		);
		
		action->SetSpan ( delay );
		action->Start ( 0, false );
		
		return state.ToRValue < MOAIRubyObject* >( action );
	}
	
	ZLVec3D piv = state.GetVec3D < float >( 1, 0.0f );
	if ( !piv.Compare ( self->mPiv )) {
		self->SetPiv ( piv );
		self->ScheduleUpdate ();
	}
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	seekRot
	@text	Animate the transform by applying a delta. Delta is computed
			given a target value. Creates and returns a MOAIEaseDriver
			initialized to apply the delta.
	
	@in		MOAITransform self
	@in		number xRotGoal		Desired resulting value for x rot (in degrees).
	@in		number yRotGoal		Desired resulting value for y rot (in degrees).
	@in		number zRotGoal		Desired resulting value for z rot (in degrees).
	@in		number length		Length of animation in seconds.
	@opt	number mode			The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
								MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.

	@out	MOAIEaseDriver easeDriver
*/
mrb_value MOAITransform::_seekRot ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )

	float delay		= state.GetParamValue < float >( 4, 0.0f );
	
	if ( delay > 0.0f ) {
	
		u32 mode = state.GetParamValue < u32 >( 5, ZLInterpolate::kSmooth );
		
		MOAIEaseDriver* action = state.CreateClassInstance < MOAIEaseDriver >();
		
		action->ParseForSeek ( state, 1, self, 3, mode,
			MOAITransformAttr::Pack ( ATTR_X_ROT ), self->mRot.mX, 0.0f,
			MOAITransformAttr::Pack ( ATTR_Y_ROT ), self->mRot.mY, 0.0f,
			MOAITransformAttr::Pack ( ATTR_Z_ROT ), self->mRot.mZ, 0.0f
		);
		
		action->SetSpan ( delay );
		action->Start ( 0, false );
		
		return state.ToRValue < MOAIRubyObject* >( action );
	}
	
	ZLVec3D rot = state.GetVec3D < float >( 1, 0.0f );
	if ( !rot.Compare ( self->mRot )) {
		self->SetRot ( rot );
		self->ScheduleUpdate ();
	}
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	seekScl
	@text	Animate the transform by applying a delta. Delta is computed
			given a target value. Creates and returns a MOAIEaseDriver
			initialized to apply the delta.
	
	@in		MOAITransform self
	@in		number xSclGoal		Desired resulting value for x scale.
	@in		number ySclGoal		Desired resulting value for y scale.
	@in		number zSclGoal		Desired resulting value for z scale.
	@in		number length		Length of animation in seconds.
	@opt	number mode			The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
								MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.

	@out	MOAIEaseDriver easeDriver
*/
mrb_value MOAITransform::_seekScl ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )

	float delay		= state.GetParamValue < float >( 4, 0.0f );
	
	if ( delay > 0.0f ) {
	
		u32 mode = state.GetParamValue < u32 >( 5, ZLInterpolate::kSmooth );

		MOAIEaseDriver* action = state.CreateClassInstance < MOAIEaseDriver >();
		
		action->ParseForSeek ( state, 1, self, 3, mode,
			MOAITransformAttr::Pack ( ATTR_X_SCL ), self->mScale.mX, 1.0f,
			MOAITransformAttr::Pack ( ATTR_Y_SCL ), self->mScale.mY, 1.0f,
			MOAITransformAttr::Pack ( ATTR_Z_SCL ), self->mScale.mZ, 1.0f
		);
		
		action->SetSpan ( delay );
		action->Start ( 0, false );
		return state.ToRValue < MOAIRubyObject* >( action );
	}
	
	ZLVec3D scl = state.GetVec3D < float >( 1, 1.0f );
	if ( !scl.Compare ( self->mScale )) {
		self->SetScl ( scl );
		self->ScheduleUpdate ();
	}
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setLoc
	@text	Sets the transform's location.
	
	@in		MOAITransform self
	@opt	number x				Default value is 0.
	@opt	number y				Default value is 0.
	@opt	number z				Default value is 0.
	@out	nil
*/
mrb_value MOAITransform::_setLoc ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )
	
	ZLVec3D loc = state.GetVec3D < float >( 1, 0.0f );
	
	if ( !loc.Compare ( self->mLoc )) {
		self->SetLoc ( loc );
		self->ScheduleUpdate ();
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setPiv
	@text	Sets the transform's pivot.
	
	@in		MOAITransform self
	@opt	number xPiv			Default value is 0.
	@opt	number yPiv			Default value is 0.
	@opt	number zPiv			Default value is 0.
	@out	nil
*/
mrb_value MOAITransform::_setPiv ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )
	
	ZLVec3D piv = state.GetVec3D < float >( 1, 0.0f );
	
	if ( !piv.Compare ( self->mPiv )) {
		self->SetPiv ( piv );
		self->ScheduleUpdate ();
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setRot
	@text	Sets the transform's rotation.
	
	@in		MOAITransform self
	@opt	number xRot			Default value is 0.
	@opt	number yRot			Default value is 0.
	@opt	number zRot			Default value is 0.
	@out	nil
*/
mrb_value MOAITransform::_setRot ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )
	
	ZLVec3D rot = state.GetVec3D < float >( 1, 0.0f );
	
	if ( !rot.Compare ( self->mRot )) {
		self->SetRot ( rot );
		self->ScheduleUpdate ();
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setScl
	@text	Sets the transform's scale.
	
	@in		MOAITransform self
	@in		number xScl
	@opt	number yScl			Default value is xScl.
	@opt	number zScl			Default value is 1.
	@out	nil
*/
mrb_value MOAITransform::_setScl ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )
	
	ZLVec3D scl;
	
	scl.mX = state.GetParamValue < float >( 1, 0.0f );
	scl.mY = state.GetParamValue < float >( 2, scl.mX );
	scl.mZ = state.GetParamValue < float >( 3, 1.0f );
	
	if ( !scl.Compare ( self->mScale )) {
		self->SetScl ( scl );
		self->ScheduleUpdate ();
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setShearByX
	@text	Sets the shear for the Y and Z axes by X.
	
	@in		MOAITransform self
	@in		number yx			Default value is 0.
	@opt	number zx			Default value is 0.
	@out	nil
*/
mrb_value MOAITransform::_setShearByX ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )

	self->mShearYX = state.GetParamValue < float >( 1, 0.0f );
	self->mShearZX = state.GetParamValue < float >( 2, 0.0f );

	self->ScheduleUpdate ();

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setShearByY
	@text	Sets the shear for the X and Z axes by Y.
	
	@in		MOAITransform self
	@in		number xy			Default value is 0.
	@opt	number zy			Default value is 0.
	@out	nil
*/
mrb_value MOAITransform::_setShearByY ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )

	self->mShearXY = state.GetParamValue < float >( 1, 0.0f );
	self->mShearZY = state.GetParamValue < float >( 2, 0.0f );

	self->ScheduleUpdate ();

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setShearByZ
	@text	Sets the shear for the X and Y axes by Z.
	
	@in		MOAITransform self
	@in		number xz			Default value is 0.
	@opt	number yz			Default value is 0.
	@out	nil
*/
mrb_value MOAITransform::_setShearByZ ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAITransform, "U" )

	self->mShearXZ = state.GetParamValue < float >( 1, 0.0f );
	self->mShearYZ = state.GetParamValue < float >( 2, 0.0f );

	self->ScheduleUpdate ();

	return context;
}

//================================================================//
// MOAITransform
//================================================================//

//----------------------------------------------------------------//
float MOAITransform::ClampEuler ( float r ) {

	if ( r >= 360.0f ) {
		r = ( float )fmod ( r, 360.0f );
	}
	else if ( r < 0.0f ) {
		r = 360.0f + ( float )fmod ( r, 360.0f );
	}
	return r;
}

//----------------------------------------------------------------//
ZLAffine3D MOAITransform::GetBillboardMtx ( const ZLAffine3D& faceCameraMtx ) const {

	ZLAffine3D billboardMtx = this->GetLocalToWorldMtx ();
		
	ZLVec3D piv;
	ZLVec3D worldLoc;
	
	// world space location for prop
	worldLoc.mX = billboardMtx.m [ ZLAffine3D::C3_R0 ];
	worldLoc.mY = billboardMtx.m [ ZLAffine3D::C3_R1 ];
	worldLoc.mZ = billboardMtx.m [ ZLAffine3D::C3_R2 ];
	
	// just the rotate/scale matrices
	billboardMtx.m [ ZLAffine3D::C3_R0 ] = 0.0f;
	billboardMtx.m [ ZLAffine3D::C3_R1 ] = 0.0f;
	billboardMtx.m [ ZLAffine3D::C3_R2 ] = 0.0f;
	
	// remove original pivot
	piv = this->mPiv;
	billboardMtx.Transform ( piv );
	worldLoc.Add ( piv );
	
	// orient to face the camera
	billboardMtx.Append ( faceCameraMtx );
	
	// add new pivot
	piv = this->mPiv;
	billboardMtx.Transform ( piv );
	worldLoc.Sub ( piv );
	
	// remove the original pivot
	billboardMtx.m [ ZLAffine3D::C3_R0 ] = worldLoc.mX;
	billboardMtx.m [ ZLAffine3D::C3_R1 ] = worldLoc.mY;
	billboardMtx.m [ ZLAffine3D::C3_R2 ] = worldLoc.mZ;
	
	return billboardMtx;
}

//----------------------------------------------------------------//
MOAITransform::MOAITransform () :
	mShearYX ( 0.0f ),
	mShearZX ( 0.0f ),
	mShearXY ( 0.0f ),
	mShearZY ( 0.0f ),
	mShearXZ ( 0.0f ),
	mShearYZ ( 0.0f ),
	mPiv ( 0.0f, 0.0f, 0.0f ),
	mLoc ( 0.0f, 0.0f, 0.0f ),
	mScale ( 1.0f, 1.0f, 1.0f ),
	mRot ( 0.0f, 0.0f, 0.0f ),
	mEulerOrder ( EULER_XYZ ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAITransformBase )
	RTTI_END
}

//----------------------------------------------------------------//
MOAITransform::~MOAITransform () {
}

//----------------------------------------------------------------//
void MOAITransform::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	MOAITransformBase::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "ATTR_X_PIV",			MOAITransformAttr::Pack ( ATTR_X_PIV ));
	state.DefineClassConst ( klass, "ATTR_Y_PIV",			MOAITransformAttr::Pack ( ATTR_Y_PIV ));
	state.DefineClassConst ( klass, "ATTR_Z_PIV",			MOAITransformAttr::Pack ( ATTR_Z_PIV ));
	state.DefineClassConst ( klass, "ATTR_X_LOC",			MOAITransformAttr::Pack ( ATTR_X_LOC ));
	state.DefineClassConst ( klass, "ATTR_Y_LOC",			MOAITransformAttr::Pack ( ATTR_Y_LOC ));
	state.DefineClassConst ( klass, "ATTR_Z_LOC",			MOAITransformAttr::Pack ( ATTR_Z_LOC ));
	state.DefineClassConst ( klass, "ATTR_X_ROT",			MOAITransformAttr::Pack ( ATTR_X_ROT ));
	state.DefineClassConst ( klass, "ATTR_Y_ROT",			MOAITransformAttr::Pack ( ATTR_Y_ROT ));
	state.DefineClassConst ( klass, "ATTR_Z_ROT",			MOAITransformAttr::Pack ( ATTR_Z_ROT ));
	state.DefineClassConst ( klass, "ATTR_X_SCL",			MOAITransformAttr::Pack ( ATTR_X_SCL ));
	state.DefineClassConst ( klass, "ATTR_Y_SCL",			MOAITransformAttr::Pack ( ATTR_Y_SCL ));
	state.DefineClassConst ( klass, "ATTR_Z_SCL",			MOAITransformAttr::Pack ( ATTR_Z_SCL ));
	state.DefineClassConst ( klass, "ATTR_ROTATE_QUAT",	MOAITransformAttr::Pack ( ATTR_ROTATE_QUAT ));
	state.DefineClassConst ( klass, "ATTR_TRANSLATE",		MOAITransformAttr::Pack ( ATTR_TRANSLATE ));
}

//----------------------------------------------------------------//
void MOAITransform::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAITransformBase::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "addLoc", _addLoc, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "addPiv", _addPiv, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "addRot", _addRot, MRB_ARGS_REQ ( 3 ) );
	state.DefineInstanceMethod ( klass, "addScl", _addScl, MRB_ARGS_ARG ( 1, 2 ) );
	state.DefineInstanceMethod ( klass, "getLoc", _getLoc, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getPiv", _getPiv, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getScl", _getScl, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "move", _move, MRB_ARGS_ARG ( 10, 1 ) );
	state.DefineInstanceMethod ( klass, "moveLoc", _moveLoc, MRB_ARGS_ARG ( 4, 1 ) );
	state.DefineInstanceMethod ( klass, "movePiv", _movePiv, MRB_ARGS_ARG ( 4, 1 ) );
	state.DefineInstanceMethod ( klass, "moveRot", _moveRot, MRB_ARGS_ARG ( 4, 1 ) );
	state.DefineInstanceMethod ( klass, "moveScl", _moveScl, MRB_ARGS_ARG ( 4, 1 ) );
	state.DefineInstanceMethod ( klass, "seek", _seek, MRB_ARGS_ARG ( 10, 1 ) );
	state.DefineInstanceMethod ( klass, "seekLoc", _seekLoc, MRB_ARGS_ARG ( 4, 1 ) );
	state.DefineInstanceMethod ( klass, "seekPiv", _seekLoc, MRB_ARGS_ARG ( 4, 1 ) );
	state.DefineInstanceMethod ( klass, "seekRot", _seekRot, MRB_ARGS_ARG ( 4, 1 ) );
	state.DefineInstanceMethod ( klass, "setLoc", _setLoc, MRB_ARGS_ARG ( 0, 3 ) );
	state.DefineInstanceMethod ( klass, "setPiv", _setPiv, MRB_ARGS_ARG ( 0, 3 ) );
	state.DefineInstanceMethod ( klass, "setRot", _setRot, MRB_ARGS_ARG ( 0, 3 ) );
	state.DefineInstanceMethod ( klass, "setScl", _setScl, MRB_ARGS_ARG ( 1, 2 ) );
	state.DefineInstanceMethod ( klass, "setShearByX", _setShearByX, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "setShearByY", _setShearByY, MRB_ARGS_ARG ( 1, 1 ) );
	state.DefineInstanceMethod ( klass, "setShearByZ", _setShearByZ, MRB_ARGS_ARG ( 1, 1 ) );

}

//----------------------------------------------------------------//
void MOAITransform::SerializeIn ( MOAIRubyState& state, MOAIDeserializer& serializer ) {
	UNUSED ( serializer );
	
	/*this->mPiv.mX		= state.GetFieldValue < float >( -1, "mPiv.mX", 0.0f );
	this->mPiv.mY		= state.GetFieldValue < float >( -1, "mPiv.mY", 0.0f );

	this->mLoc.mX		= state.GetFieldValue < float >( -1, "mLoc.mX", 0.0f );
	this->mLoc.mY		= state.GetFieldValue < float >( -1, "mLoc.mY", 0.0f );

	this->mScale.mX		= state.GetFieldValue < float >( -1, "mScale.mX", 1.0f );
	this->mScale.mY		= state.GetFieldValue < float >( -1, "mScale.mY", 1.0f );

	this->mRot.mZ		= state.GetFieldValue < float >( -1, "mDegrees", 0.0f );*/
}

//----------------------------------------------------------------//
void MOAITransform::SerializeOut ( MOAIRubyState& state, MOAISerializer& serializer ) {
	UNUSED ( serializer );

	state.SetField ( -1, "mPiv.mX", this->mPiv.mX );
	state.SetField ( -1, "mPiv.mY", this->mPiv.mY );
	
	state.SetField ( -1, "mLoc.mX", this->mLoc.mX );
	state.SetField ( -1, "mLoc.mY", this->mLoc.mY );
	
	state.SetField ( -1, "mScale.mX", this->mScale.mX );
	state.SetField ( -1, "mScale.mY", this->mScale.mY );
	
	state.SetField ( -1, "mDegrees", this->mRot.mZ );
}

//----------------------------------------------------------------//
void MOAITransform::SetLoc ( float x, float y, float z ) {

	this->mLoc.mX = x;
	this->mLoc.mY = y;
	this->mLoc.mZ = z;
}

//----------------------------------------------------------------//
void MOAITransform::SetPiv ( float x, float y, float z ) {

	this->mPiv.mX = x;
	this->mPiv.mY = y;
	this->mPiv.mZ = z;
}

//----------------------------------------------------------------//
void MOAITransform::SetRot ( float x, float y, float z ) {

	this->mRot.mX = x;
	this->mRot.mY = y;
	this->mRot.mZ = z;
}

//----------------------------------------------------------------//
void MOAITransform::SetScl ( float x, float y, float z ) {

	this->mScale.mX = x;
	this->mScale.mY = y;
	this->mScale.mZ = z;
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAITransform::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	if ( MOAITransformAttr::Check ( attrID )) {

		switch ( UNPACK_ATTR ( attrID )) {
		
			case ATTR_X_PIV:
				this->mPiv.mX = attr.Apply ( this->mPiv.mX, op, MOAIAttribute::ATTR_READ_WRITE );
				return true;
				
			case ATTR_Y_PIV:
				this->mPiv.mY = attr.Apply ( this->mPiv.mY, op, MOAIAttribute::ATTR_READ_WRITE );
				return true;
				
			case ATTR_Z_PIV:
				this->mPiv.mZ = attr.Apply ( this->mPiv.mZ, op, MOAIAttribute::ATTR_READ_WRITE );
				return true;
				
			case ATTR_X_LOC:
				this->mLoc.mX = attr.Apply ( this->mLoc.mX, op, MOAIAttribute::ATTR_READ_WRITE );
				return true;
				
			case ATTR_Y_LOC:
				this->mLoc.mY = attr.Apply ( this->mLoc.mY, op, MOAIAttribute::ATTR_READ_WRITE );
				return true;
				
			case ATTR_Z_LOC:
				this->mLoc.mZ = attr.Apply ( this->mLoc.mZ, op, MOAIAttribute::ATTR_READ_WRITE );
				return true;
				
			case ATTR_X_ROT:
				this->mRot.mX = attr.Apply ( this->mRot.mX, op, MOAIAttribute::ATTR_READ_WRITE );
				return true;
				
			case ATTR_Y_ROT:
				this->mRot.mY = attr.Apply ( this->mRot.mY, op, MOAIAttribute::ATTR_READ_WRITE );
				return true;
				
			case ATTR_Z_ROT:
				this->mRot.mZ = attr.Apply ( this->mRot.mZ, op, MOAIAttribute::ATTR_READ_WRITE );
				return true;
				
			case ATTR_X_SCL:
				this->mScale.mX = attr.Apply ( this->mScale.mX, op, MOAIAttribute::ATTR_READ_WRITE );
				return true;
				
			case ATTR_Y_SCL:
				this->mScale.mY = attr.Apply ( this->mScale.mY, op, MOAIAttribute::ATTR_READ_WRITE );
				return true;
				
			case ATTR_Z_SCL:
				this->mScale.mZ = attr.Apply ( this->mScale.mZ, op, MOAIAttribute::ATTR_READ_WRITE );
				return true;
				
			case ATTR_ROTATE_QUAT: {
			
				// TODO: cache rotation as quat to support read/write, delta adds?

				attr.SetFlags ( MOAIAttribute::ATTR_READ_WRITE );

				if ( op == MOAIAttribute::ADD ) {

					ZLQuaternion quat ( this->mRot.mX, this->mRot.mY, this->mRot.mZ );
					quat = attr.Apply ( quat, op, MOAIAttribute::ATTR_WRITE );
					quat.Get ( this->mRot.mX, this->mRot.mY, this->mRot.mZ );
				}
				else if ( op != MOAIAttribute::CHECK ) {

					ZLQuaternion quat ( 0.0f, 0.0f, 0.0f, 0.0f );
					quat = attr.Apply ( quat, op, MOAIAttribute::ATTR_WRITE );
					quat.Get ( this->mRot.mX, this->mRot.mY, this->mRot.mZ );
				}
				return true;
			}
			case ATTR_TRANSLATE:
				this->mLoc = attr.Apply ( this->mLoc, op, MOAIAttribute::ATTR_READ_WRITE );
				return true;
		}
	}
	return MOAITransformBase::MOAINode_ApplyAttrOp ( attrID, attr, op );
}

//----------------------------------------------------------------//
void MOAITransform::MOAITransformBase_BuildLocalToWorldMtx ( ZLAffine3D& localToWorldMtx ) {

	float xRot = ClampEuler ( this->mRot.mX ) * ( float )D2R;
	float yRot = ClampEuler ( this->mRot.mY ) * ( float )D2R;
	float zRot = ClampEuler ( this->mRot.mZ ) * ( float )D2R;

	if ( this->mEulerOrder == EULER_XYZ ) {

		localToWorldMtx.ScRoTr (
			this->mScale.mX,
			this->mScale.mY,
			this->mScale.mZ,
			xRot,
			yRot,
			zRot,
			this->mLoc.mX,
			this->mLoc.mY,
			this->mLoc.mZ
		);
	}
	else {
	
		localToWorldMtx.Scale ( this->mScale.mX, this->mScale.mY, this->mScale.mZ );
	
		ZLAffine3D euler [ 3 ];
		
		euler [ 0 ].RotateX ( xRot );
		euler [ 1 ].RotateY ( yRot );
		euler [ 2 ].RotateZ ( zRot );
		
		u32 idx = this->mEulerOrder & 0x03;
		localToWorldMtx.Append ( euler [ idx ]);
		
		idx = ( this->mEulerOrder >> 0x02 ) & 0x03;
		localToWorldMtx.Append ( euler [ idx ]);
		
		idx = ( this->mEulerOrder >> 0x04 ) & 0x03;
		localToWorldMtx.Append ( euler [ idx ]);
		
		localToWorldMtx.m [ ZLAffine3D::C3_R0 ] = this->mLoc.mX;
		localToWorldMtx.m [ ZLAffine3D::C3_R1 ] = this->mLoc.mY;
		localToWorldMtx.m [ ZLAffine3D::C3_R2 ] = this->mLoc.mZ;
	}
	
	ZLAffine3D shear;
	shear.Shear ( this->mShearYX, this->mShearZX, this->mShearXY, this->mShearZY, this->mShearXZ, this->mShearYZ );
	localToWorldMtx.Prepend ( shear );
	
	if (( this->mPiv.mX != 0.0f ) || ( this->mPiv.mY != 0.0f ) || ( this->mPiv.mZ != 0.0f )) {
		
		ZLAffine3D pivot;
		pivot.Translate ( -this->mPiv.mX, -this->mPiv.mY, -this->mPiv.mZ );
		localToWorldMtx.Prepend ( pivot );
	}
}
