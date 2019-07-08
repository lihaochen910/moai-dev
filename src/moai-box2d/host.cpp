// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-box2d/host.h>
#include <moai-box2d/headers.h>

//================================================================//
// aku-box2d
//================================================================//

//----------------------------------------------------------------//
void AKUBox2DAppFinalize () {
}

//----------------------------------------------------------------//
void AKUBox2DAppInitialize () {
}

//----------------------------------------------------------------//
void AKUBox2DContextInitialize () {

	REGISTER_RUBY_CLASS ( MOAIBox2DArbiter )
	REGISTER_RUBY_CLASS ( MOAIBox2DBody )
	REGISTER_RUBY_CLASS ( MOAIBox2DDistanceJoint )
	REGISTER_RUBY_CLASS ( MOAIBox2DFixture )
	REGISTER_RUBY_CLASS ( MOAIBox2DFrictionJoint )
	REGISTER_RUBY_CLASS ( MOAIBox2DGearJoint )
	REGISTER_RUBY_CLASS ( MOAIBox2DMouseJoint )
	REGISTER_RUBY_CLASS ( MOAIBox2DPrismaticJoint )
	REGISTER_RUBY_CLASS ( MOAIBox2DPulleyJoint )
	REGISTER_RUBY_CLASS ( MOAIBox2DRopeJoint )
	REGISTER_RUBY_CLASS ( MOAIBox2DRevoluteJoint )
	REGISTER_RUBY_CLASS ( MOAIBox2DWeldJoint )
	REGISTER_RUBY_CLASS ( MOAIBox2DWheelJoint )
	REGISTER_RUBY_CLASS ( MOAIBox2DWorld )
}
