// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIEaseType.h>

//================================================================//
// MOAIEaseType
//================================================================//

//----------------------------------------------------------------//
void MOAIEaseType::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "EASE_IN",							( u32 )ZLInterpolate::kEaseIn );
	state.DefineClassConst ( klass, "EASE_OUT",							( u32 )ZLInterpolate::kEaseOut );
	state.DefineClassConst ( klass, "EXTRA_SHARP_EASE_IN",				( u32 )ZLInterpolate::kExtraSharpEaseIn );
	state.DefineClassConst ( klass, "EXTRA_SHARP_EASE_OUT",				( u32 )ZLInterpolate::kExtraSharpEaseOut );
	state.DefineClassConst ( klass, "EXTRA_SHARP_SMOOTH",				( u32 )ZLInterpolate::kExtraSharpSmooth );
	state.DefineClassConst ( klass, "EXTRA_SHARP_SMOOTH_EASE_OUT",		( u32 )ZLInterpolate::kExtraSharpSmoothEaseOut );
	state.DefineClassConst ( klass, "FLAT",								( u32 )ZLInterpolate::kFlat );
	state.DefineClassConst ( klass, "LINEAR",							( u32 )ZLInterpolate::kLinear );
	state.DefineClassConst ( klass, "SHARP_EASE_IN",					( u32 )ZLInterpolate::kSharpEaseIn );
	state.DefineClassConst ( klass, "SHARP_EASE_OUT",					( u32 )ZLInterpolate::kSharpEaseOut );
	state.DefineClassConst ( klass, "SHARP_SMOOTH",						( u32 )ZLInterpolate::kSharpSmooth );
	state.DefineClassConst ( klass, "SHARP_SMOOTH_EASE_OUT",			( u32 )ZLInterpolate::kSharpSmoothEaseOut );
	state.DefineClassConst ( klass, "SMOOTH",							( u32 )ZLInterpolate::kSmooth );
	state.DefineClassConst ( klass, "SMOOTH_EASE_OUT",					( u32 )ZLInterpolate::kSmoothEaseOut );
	state.DefineClassConst ( klass, "SOFT_EASE_IN",						( u32 )ZLInterpolate::kSoftEaseIn );
	state.DefineClassConst ( klass, "SOFT_EASE_OUT",					( u32 )ZLInterpolate::kSoftEaseOut );
	state.DefineClassConst ( klass, "SOFT_SMOOTH",						( u32 )ZLInterpolate::kSoftSmooth );
	state.DefineClassConst ( klass, "SOFT_SMOOTH_EASE_OUT",				( u32 )ZLInterpolate::kSoftSmoothEaseOut );
	state.DefineClassConst ( klass, "SINE_EASE_IN", 					( u32 )ZLInterpolate::kSineEaseIn );
	state.DefineClassConst ( klass, "SINE_EASE_OUT", 					( u32 )ZLInterpolate::kSineEaseOut );
	state.DefineClassConst ( klass, "SINE_SMOOTH", 						( u32 )ZLInterpolate::kSineSmooth );
	state.DefineClassConst ( klass, "CIRC_EASE_IN", 					( u32 )ZLInterpolate::kCircEaseIn );
	state.DefineClassConst ( klass, "CIRC_EASE_OUT", 					( u32 )ZLInterpolate::kCircEaseOut );
	state.DefineClassConst ( klass, "CIRC_SMOOTH", 						( u32 )ZLInterpolate::kCircSmooth );
	state.DefineClassConst ( klass, "BOUNCE_IN", 						( u32 )ZLInterpolate::kBounceIn );
	state.DefineClassConst ( klass, "BOUNCE_OUT", 						( u32 )ZLInterpolate::kBounceOut );
	state.DefineClassConst ( klass, "BOUNCE_SMOOTH", 					( u32 )ZLInterpolate::kBounceSmooth );
	state.DefineClassConst ( klass, "ELASTIC_IN", 						( u32 )ZLInterpolate::kElasticIn );
	state.DefineClassConst ( klass, "ELASTIC_OUT", 						( u32 )ZLInterpolate::kElasticOut );
	state.DefineClassConst ( klass, "ELASTIC_SMOOTH", 					( u32 )ZLInterpolate::kElasticSmooth );
	state.DefineClassConst ( klass, "BACK_EASE_IN", 					( u32 )ZLInterpolate::kBackEaseIn );
	state.DefineClassConst ( klass, "BACK_EASE_OUT", 					( u32 )ZLInterpolate::kBackEaseOut );
	state.DefineClassConst ( klass, "BACK_EASE_SMOOTH", 				( u32 )ZLInterpolate::kBackSmooth );

}
