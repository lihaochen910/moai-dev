// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIDebugLines.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAINodeMgr.h>
#include <moai-sim/MOAISim.h>
#include <moai-sim/MOAITextureBase.h>
#include <moai-sim/MOAIRenderMgr.h>

#if defined(_WIN32)
	#include <windows.h>
	#include <Psapi.h>
#elif defined(__APPLE__) //&& defined(TARGET_IPHONE_SIMULATOR)
	// Not sure if using mach API is disallowed in the app store. :/
	#include <mach/mach.h>
	#include <unistd.h>
#elif defined (__QNX__)
	#include <unistd.h>
#elif defined (__EMSCRIPTEN__)
  #include <unistd.h>
#elif defined (ANDROID)
  #include <unistd.h>
#endif

#define LUA_GC_FUNC_NAME "collectgarbage"

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	clearLoopFlags
	@text	Uses the mask provided to clear the loop flags.

	@opt	number mask		Default value is 0xffffffff.
	@out	nil
*/
mrb_value MOAISim::_clearLoopFlags ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	MOAISim::Get ().mLoopFlags &= ~state.GetParamValue < u32 >( 1, 0xffffffff );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	crash
	@text	Crashes Moai with a null pointer dereference.
 
	@out	nil
*/
mrb_value MOAISim::_crash ( mrb_state* M, mrb_value context ) {
	UNUSED ( M );
	
	int *p = NULL;
	(*p) = 0;
	
	return context;
}

//----------------------------------------------------------------//
mrb_value MOAISim::_collectgarbage ( mrb_state* M, mrb_value context ) {
	UNUSED ( M );
	printf ( "WARNING: 'collectgarbage' replaced by MOAISim. Use MOAISim's 'forceGC', 'setGCStep' and 'setGCActive' instead.\n" );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	enterFullscreenMode
	@text	Enters fullscreen mode on the device if possible.

	@out	nil
*/
mrb_value MOAISim::_enterFullscreenMode ( mrb_state* M, mrb_value context ) {

	MOAIRubyState state ( M );

	EnterFullscreenModeFunc func = MOAISim::Get ().GetEnterFullscreenModeFunc ();
	if ( func ) {
		func ();
	}

	return context;
}

//----------------------------------------------------------------//
/**	@lua	exitFullscreenMode
	@text	Exits fullscreen mode on the device if possible.

	@out	nil
*/
mrb_value MOAISim::_exitFullscreenMode ( mrb_state* M, mrb_value context ) {

	MOAIRubyState state ( M );

	ExitFullscreenModeFunc func = MOAISim::Get ().GetExitFullscreenModeFunc ();
	if ( func ) {
		func ();
	}

	return context;
}

//----------------------------------------------------------------//
// TODO: deprecate
mrb_value MOAISim::_forceGC ( mrb_state* M, mrb_value context ) {
	UNUSED ( M );
	MOAIRubyRuntime::Get ().ForceGarbageCollection ();
	return context;
}

//----------------------------------------------------------------//
/**	@lua	framesToTime
	@text	Converts the number of frames to time passed in seconds.

	@in		number frames		The number of frames.
	@out	number time			The equivalent number of seconds for the specified number of frames.
*/
mrb_value MOAISim::_framesToTime ( mrb_state* M, mrb_value context ) {

	MOAIRubyState state ( M );
	if ( !state.CheckParams ( 1, "N" )) return context;
	
	float frames = state.GetParamValue < float >( 1, 0.0f );
	
	MOAISim& sim = MOAISim::Get ();
	
	return state.ToRValue ( frames * sim.mStep );
}

//----------------------------------------------------------------//
/**	@lua	getActionMgr
	@text	Get the sim's action tree. This is the 'global' action tree
			that all newly started actions are automatically added.

	@out	MOAIActionTree actionMgr
*/
mrb_value MOAISim::_getActionMgr ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	return state.ToRValue < MOAIRubyObject& >( MOAISim::Get ().GetActionMgr () );
}

//----------------------------------------------------------------//
/**	@lua	getDeviceTime
	@text	Gets the raw device clock. This is a replacement for Ruby's os.time ().

	@out	number time			The device clock time in seconds.
*/
mrb_value MOAISim::_getDeviceTime ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	return state.ToRValue ( ZLDeviceTime::GetTimeInSeconds () );
}

//----------------------------------------------------------------//
/**	@lua	getElapsedTime
	@text	Gets the number of seconds elapsed since the application was started.

	@out	number time			The number of elapsed seconds.
*/
mrb_value MOAISim::_getElapsedTime ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	return state.ToRValue ( MOAISim::Get ().mSimTime );
}

//----------------------------------------------------------------//
/**	@lua	getLoopFlags
	@text	Returns the current loop flags.

	@out	number mask
*/
mrb_value MOAISim::_getLoopFlags ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	return state.ToRValue ( MOAISim::Get ().mLoopFlags );
}

//----------------------------------------------------------------//
/**	@lua	getRubyObjectCount
	@text	Gets the total number of objects in memory that inherit MOAIRubyObject. Count includes
			objects that are not bound to the Ruby runtime.

	@out	number count
*/
mrb_value MOAISim::_getRubyObjectCount ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	return state.ToRValue ( MOAIRubyRuntime::Get ().GetObjectCount () );
}

//----------------------------------------------------------------//
/** @lua	getMemoryUsage
	@text	Get the current amount of memory used by MOAI and its subsystems. This will
			attempt to return reasonable estimates where exact values cannot be obtained.
			Some fields represent informational fields (i.e. are not double counted in the
			total, but present to assist debugging) and may be only available on certain
			platforms (e.g. Windows, etc). These fields begin with a '_' character.
 
	@out	table	usage		The breakdown of each subsystem's memory usage, in bytes. There is also a "total" field that contains the summed value.
*/
mrb_value MOAISim::_getMemoryUsage ( mrb_state* M, mrb_value context ) {
	
	//float divisor = 1.0f;
	//
	//if( lua_type(L, 1) == LUA_TSTRING )
	//{
	//	cc8* str = lua_tostring(L, 1);
	//	if( str[0] == 'k' || str[0] == 'K' )
	//		divisor = 1024.0f;
	//	else if( str[0] == 'm' || str[0] == 'M' )
	//		divisor = 1024.0f * 1024.0f;
	//	else if( str[0] == 'b' || str[0] == 'B' )
	//		divisor = 1.0f;
	//}
	//
	//size_t total = 0;
	//
	//lua_newtable(L);
	//
	//size_t count;
	//
	//count = MOAIRubyRuntime::Get().GetMemoryUsage ();
	//lua_pushnumber(L, count / divisor);
	//lua_setfield(L, -2, "lua");
	//total += count;

	//// This is informational only (i.e. don't double count with the previous field).
	//// see: http://pgl.yoyo.org/luai/i/lua_gc
	//int luabytes = lua_gc ( L, LUA_GCCOUNT, 0 ) * 1024 + lua_gc ( L, LUA_GCCOUNTB, 0 );
	//lua_pushnumber ( L, luabytes / divisor  );
	//lua_setfield ( L, -2, "_luagc_count" );
	//
	//count = MOAIGfxMgr::Get ().GetTextureMemoryUsage ();
	//lua_pushnumber ( L, count / divisor );
	//lua_setfield ( L, -2, "texture" );
	//total += count;
	//
	//#if defined(_WIN32)
	//
	//	PROCESS_MEMORY_COUNTERS pmc;

	//	// Print the process identifier.
	//	if ( GetProcessMemoryInfo ( GetCurrentProcess (), &pmc, sizeof ( pmc ))) {
	//		lua_pushnumber ( L, pmc.PagefileUsage / divisor );
	//		lua_setfield ( L, -2, "_sys_vs" );
	//		lua_pushnumber ( L, pmc.WorkingSetSize / divisor );
	//		lua_setfield ( L, -2, "_sys_rss" );
	//	}
	//	
	//#elif defined(__APPLE__) //&& defined(TARGET_IPHONE_SIMULATOR)
	//
	//	// Tricky undocumented mach polling of memory
	//	struct task_basic_info t_info;
	//	mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
	//
	//	kern_return_t kr = task_info (
	//		mach_task_self (),
	//		TASK_BASIC_INFO,
	//		reinterpret_cast < task_info_t >( &t_info ),
	//		&t_info_count
	//	);
	//
	//	// Most likely cause for failure: |task| is a zombie.
	//	if( kr == KERN_SUCCESS ) {
	//		lua_pushnumber ( L, t_info.virtual_size / divisor );
	//		lua_setfield ( L, -2, "_sys_vs" );
	//		lua_pushnumber ( L, t_info.resident_size / divisor );
	//		lua_setfield ( L, -2, "_sys_rss" );
	//	}
	//#endif
	//
	//lua_pushnumber ( L, total / divisor );
	//lua_setfield ( L, -2, "total" );
	
	return context;
}

//----------------------------------------------------------------//
/** @lua	getMemoryUsagePlain
	@text	Returns lua and texture memory usage measured by MOAI subsystems.
			This function tries to avoid allocations to minimize skewing the results.
			Suitable for realtime memory monitoring.
	
	@out	number lua memory usage in bytes
	@out	number texture memory usage in bytes
*/
mrb_value MOAISim::_getMemoryUsagePlain ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	size_t ruby = MOAIRubyRuntime::Get ().GetMemoryUsage ();
	size_t tex = MOAIGfxMgr::Get ().GetTextureMemoryUsage ();

	mrb_value ret [ 2 ];
	ret [ 0 ] = state.ToRValue ( ruby );
	ret [ 1 ] = state.ToRValue ( tex );

	return mrb_ary_new_from_values ( state, 2, ret );
}

//----------------------------------------------------------------//
/**	@lua	getPerformance
	@text	Returns an estimated frames per second and other performance counters 
			based on measurements taken at every render.

	@out	number fps		Estimated frames per second.
	@out	number seconds	Last ActionTree update duration
	@out	number seconds  Last NodeMgr update duration
	@out	number seconds  Last sim duration
	@out	number seconds  Last render duration
*/
mrb_value MOAISim::_getPerformance ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	MOAISim& device = MOAISim::Get ();
	MOAIRenderMgr& renderMgr = MOAIRenderMgr::Get ();

	mrb_value ret [ 5 ];
	ret [ 0 ] = state.ToRValue ( device.mFrameRate );
	ret [ 1 ] = state.ToRValue ( device.mLastActionTreeTime );
	ret [ 2 ] = state.ToRValue ( device.mLastNodeMgrTime );
	ret [ 3 ] = state.ToRValue ( device.mSimDuration );
	ret [ 5 ] = state.ToRValue ( renderMgr.GetRenderDuration () );

	return mrb_ary_new_from_values ( state, 5, ret );
}

//----------------------------------------------------------------//
/**	@lua	getStep
	@text	Gets the amount of time (in seconds) that it takes for one frame to pass.

	@out	number size			The size of the frame; the time it takes for one frame to pass.
*/
mrb_value MOAISim::_getStep ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	return state.ToRValue ( MOAISim::Get ().GetStep () );
}

//----------------------------------------------------------------//
/**	@lua	getStepCount
	@text	Gets the number of times the sim was stepped since the application was started.

	@out	number steps		The number of times the sim was stepped.
*/
mrb_value MOAISim::_getStepCount ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	return state.ToRValue( MOAISim::Get ().mStepCount );
}

//----------------------------------------------------------------//
/**	@lua	hideCursor
	@text	Hides system cursor.

	@out	nil
*/
mrb_value MOAISim::_hideCursor ( mrb_state* M, mrb_value context ) {

	MOAIRubyState state ( M );

	HideCursorFunc func = MOAISim::Get ().GetHideCursorFunc ();
	if ( func ) {
		func ();
	}

	return context;
}

//----------------------------------------------------------------//
/**	@lua	openWindow
	@text	Opens a new window for the application to render on.  This must be called before any rendering can be done, and it must only be called once.

	@in		string title		The title of the window.
	@in		number width		The width of the window in pixels.
	@in		number height		The height of the window in pixels.
	@out	nil
*/
mrb_value MOAISim::_openWindow ( mrb_state* M, mrb_value context ) {
	
	MOAIRubyState state ( M );
	if ( !state.CheckParams ( 1, "SNN" ) ) return context;
	
	cc8* title = state.GetParamValue < cc8* > ( 1, "MOAI" );
	u32 width = state.GetParamValue < u32 >( 2, 640 );
	u32 height = state.GetParamValue < u32 >( 3, 480 );

	OpenWindowFunc openWindow = MOAISim::Get ().GetOpenWindowFunc ();
	if ( openWindow ) {
		MOAIGfxMgr::Get ().SetBufferSize ( width, height );
		openWindow ( title, width, height );
	}

	return context;
}

//----------------------------------------------------------------//
/**	@lua	pauseTimer
	@text	Pauses or unpauses the device timer, preventing any visual updates (rendering) while paused.

	@in		boolean pause		Whether the device timer should be paused.
	@out	nil
*/
mrb_value MOAISim::_pauseTimer ( mrb_state* M, mrb_value context ) {
	
	MOAIRubyState state ( M );
	bool pause = state.GetParamValue < bool >( 1, true );
	
	if ( pause ) {
		MOAISim::Get ().Pause ();
	}
	else {
		MOAISim::Get ().Resume ();
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setBoostThreshold
	@text	Sets the boost threshold, a scalar applied to step. If the gap
			between simulation time and device time is greater than the step
			size multiplied by the boost threshold and MOAISim.SIM_LOOP_ALLOW_BOOST
			is set in the loop flags, then the simulation is updated once with a
			large, variable step to make up the entire gap.

	@opt	number boostThreshold		Default value is DEFAULT_BOOST_THRESHOLD.
	@out	nil
*/
mrb_value MOAISim::_setBoostThreshold ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	MOAISim::Get ().mBoostThreshold = state.GetParamValue < double >( 1, DEFAULT_BOOST_THRESHOLD );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setCpuBudget
	@text	Sets the amount of time (given in simulation steps) to allow
			for updating the simulation.
	
	@in		number budget	Default value is DEFAULT_CPU_BUDGET.
	@out	nil
*/
mrb_value MOAISim::_setCpuBudget ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	MOAISim::Get ().mCpuBudget = state.GetParamValue < u32 >( 1, DEFAULT_CPU_BUDGET );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setGCActive
	@text	Enable incremental garbage collection.
	
	@in		boolean active			Default value is false.
	@out	nil
*/
mrb_value MOAISim::_setGCActive ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	MOAISim::Get ().mGCActive = state.GetParamValue < bool >( 1, false );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setGCStep
	@text	Sets a step to use when running the incremental gc each frame.
	
	@in		number step
	@out	nil
*/
mrb_value MOAISim::_setGCStep ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	MOAISim::Get ().mGCStep = state.GetParamValue < u32 >( 1, 0 );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setLongDelayThreshold
	@text	Sets the long delay threshold. If the simulation step falls behind
			the given threshold, the deficit will be dropped: the simulation will
			neither spin nor boost to catch up.

	@opt	number longDelayThreshold		Default value is DEFAULT_LONG_DELAY_THRESHOLD.
	@out	nil
*/
mrb_value MOAISim::_setLongDelayThreshold ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	MOAISim::Get ().mLongDelayThreshold = state.GetParamValue < double >( 1, DEFAULT_LONG_DELAY_THRESHOLD );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setLoopFlags
	@text	Fine tune behavior of the simulation loop. MOAISim.SIM_LOOP_ALLOW_SPIN
			will allow the simulation step to run multiple times per update to try
			and catch up with device time, but will abort if processing the simulation
			exceeds the configured step time. MOAISim.SIM_LOOP_ALLOW_BOOST will permit
			a *variable* update step if simulation time falls too far behind
			device time (based on the boost threshold). Be warned: this can wreak
			havoc with physics and stepwise animation or game AI.
			
			Three presets are provided: MOAISim.LOOP_FLAGS_DEFAULT, MOAISim.LOOP_FLAGS_FIXED,
			and MOAISim.LOOP_FLAGS_MULTISTEP.

	@opt	number flags		Mask or a combination of MOAISim.SIM_LOOP_FORCE_STEP, MOAISim.SIM_LOOP_ALLOW_BOOST,
								MOAISim.SIM_LOOP_ALLOW_SPIN, MOAISim.SIM_LOOP_NO_DEFICIT, MOAISim.SIM_LOOP_NO_SURPLUS,
								MOAISim.SIM_LOOP_RESET_CLOCK. Default value is 0.
	@out	nil	
*/
mrb_value MOAISim::_setLoopFlags ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	MOAISim::Get ().mLoopFlags |= state.GetParamValue < u32 >( 1, 0 );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setRubyAllocLogEnabled
	@text	Toggles log messages from Ruby allocator.

	@opt	boolean enable			Default value is 'false.'
	@out	nil
*/
mrb_value MOAISim::_setRubyAllocLogEnabled ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	MOAIRubyRuntime::Get ().SetAllocLogEnabled ( state.GetParamValue < bool >( 1, false ) );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setStep
	@text	Sets the size of each simulation step (in seconds).
	
	@in		number step		The step size. Default value is 1 / DEFAULT_STEPS_PER_SECOND.
	@out	nil
*/
mrb_value MOAISim::_setStep ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	MOAISim::Get ().SetStep ( state.GetParamValue < double >( 1, 1.0 / ( double )DEFAULT_STEPS_PER_SECOND ));
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setStepMultiplier
	@text	Runs the simulation multiple times per step (but with a fixed
			step size). This is used to speed up the simulation without
			providing a larger step size (which could destabilize physics
			simulation).
	
	@in		number count		Default value is DEFAULT_STEP_MULTIPLIER.
	@out	nil
*/
mrb_value MOAISim::_setStepMultiplier ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	MOAISim::Get ().mStepMultiplier = state.GetParamValue < u32 >( 1, DEFAULT_STEP_MULTIPLIER );
	return context;
}

//----------------------------------------------------------------//
/**
	@lua	setStepSmoothing
	@text	Average delta time over N last frames. This is useful to
			filter out momentary single-frame spikes. 
			Can make difference even in fixed step setup (helps to avoids double steps).

	@in		number count		Number of frames. Default is 0 (no smoothing).
	@out	nil
*/
mrb_value MOAISim::_setStepSmoothing ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	u32 size = state.GetParamValue < u32 >( 1, 0 );

	MOAISim& device = MOAISim::Get ();
	
	device.mSmoothBuffer.Init ( size );
	device.mSmoothBuffer.Fill ( device.mStep );
	device.mSmoothIdx = 0;
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setTimerError
	@text	Sets the tolerance for timer error. This is a multiplier of step.
			Timer error tolerance is step * timerError.
	
	@in		number timerError		Default value is 0.0.
	@out	nil
*/
mrb_value MOAISim::_setTimerError ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	MOAISim::Get ().mTimerError = state.GetParamValue < double >( 1, 0.0 );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setTraceback
	@text	Sets the function to call when a traceback occurs in Ruby
 
	@in		function callback		Function to execute when the traceback occurs
	@out	nil
*/
mrb_value MOAISim::_setTraceback ( mrb_state* M, mrb_value context ) {
	UNUSED ( M );
	
	MOAIRubyRuntime::Get ().GetTracebackRef ().SetRef ( MOAIRubyRuntime::Get ().GetMainState().GetParamValue ( 1 ) );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setTextInputRect
	@text	Sets text input rect.
 
	@out	nil
 */
mrb_value MOAISim::_setTextInputRect ( mrb_state* M, mrb_value context ) {
	
	MOAIRubyState state ( M );
	ZLIntRect rect = state.GetRect < int >( 1 );
	rect.Bless();
	
	SetTextInputRectFunc func = MOAISim::Get ().GetSetTextInputRectFunc ();
	if ( func ) {
		func ( rect.mXMin, rect.mYMin, rect.mXMax, rect.mYMax );
	}
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	showCursor
	@text	Shows system cursor.

	@out	nil
*/
mrb_value MOAISim::_showCursor ( mrb_state* M, mrb_value context ) {

	MOAIRubyState state ( M );

	ShowCursorFunc func = MOAISim::Get ().GetShowCursorFunc ();
	if ( func ) {
		func ();
	}

	return context;
}

//----------------------------------------------------------------//
/**	@lua	timeToFrames
	@text	Converts the number of time passed in seconds to frames.

	@in		number time			The number of seconds.
	@out	number frames		The equivalent number of frames for the specified number of seconds.
*/
mrb_value MOAISim::_timeToFrames ( mrb_state* M, mrb_value context ) {

	MOAIRubyState state ( M );
	if ( !state.CheckParams ( 1, "N" ) ) return context;
	
	float time = state.GetParamValue < float >( 1, 0.0f );
	
	MOAISim& device = MOAISim::Get ();
	
	return state.ToRValue ( time / device.mStep );
}

//================================================================//
// DOXYGEN
//================================================================//

#ifdef DOXYGEN

	//----------------------------------------------------------------//
	/**	@lua	clearRenderStack
		@text	Alias for MOAIRenderMgr.clearRenderStack (). THIS METHOD
				IS DEPRECATED AND WILL BE REMOVED IN A FUTURE RELEASE.

		@out	nil
	*/
	mrb_value MOAISim::_clearRenderStack ( mrb_state* M, mrb_value context ) {
	}
	
	//----------------------------------------------------------------//
	/**	@lua	popRenderPass
		@text	Alias for MOAIRenderMgr.popRenderPass (). THIS METHOD
				IS DEPRECATED AND WILL BE REMOVED IN A FUTURE RELEASE.

		@out	nil
	*/
	mrb_value MOAISim::_popRenderPass ( mrb_state* M, mrb_value context ) {
	}
	
	//----------------------------------------------------------------//
	/**	@lua	pushRenderPass
		@text	Alias for MOAIRenderMgr.pushRenderPass (). THIS METHOD
				IS DEPRECATED AND WILL BE REMOVED IN A FUTURE RELEASE.

		@in		MOAIDrawable renderable
		@out	nil
	*/
	mrb_value MOAISim::_pushRenderPass ( mrb_state* M, mrb_value context ) {
	}
	
	//----------------------------------------------------------------//
	/**	@lua	removeRenderPass
		@text	Alias for MOAIRenderMgr.removeRenderPass (). THIS METHOD
				IS DEPRECATED AND WILL BE REMOVED IN A FUTURE RELEASE.

		@in		MOAIDrawable renderable
		@out	nil
	*/
	mrb_value MOAISim::_removeRenderPass ( mrb_state* M, mrb_value context ) {
	}

#endif

//================================================================//
// MOAISim
//================================================================//

//----------------------------------------------------------------//
MOAISim::MOAISim () :
	mLoopState ( START ),
	mStep ( 1.0 / ( double )DEFAULT_STEPS_PER_SECOND ),
	mSimTime ( 0.0 ),
	mRealTime ( 0.0 ),
	mFrameTime ( 0.0 ),
	mPauseTime ( 0.0 ),
	mStepCount ( 0 ),
	mFrameRate ( 0.0f ),
	mFrameRateIdx ( 0 ),
	mNodeMgrTime ( 0.0 ),
	mActionTreeTime ( 0.0 ),
	mLastNodeMgrTime ( 0.0 ),
	mLastActionTreeTime ( 0.0 ),
	mLoopFlags ( LOOP_FLAGS_DEFAULT ),
	mBoostThreshold ( DEFAULT_BOOST_THRESHOLD ),
	mLongDelayThreshold ( DEFAULT_LONG_DELAY_THRESHOLD ),
	mCpuBudget ( DEFAULT_CPU_BUDGET ),
	mStepMultiplier ( DEFAULT_STEP_MULTIPLIER ),
	mTimerError ( 0.0 ),
	mSimDuration ( 1.0 / 60.0 ),
	mEnterFullscreenModeFunc ( 0 ),
	mExitFullscreenModeFunc ( 0 ),
	mOpenWindowFunc ( 0 ),
	mSetSimStepFunc ( 0 ),
	mShowCursorFunc ( 0 ),
	mHideCursorFunc ( 0 ),
	mGCActive ( true ),
	mSmoothIdx ( 0 ),
	mGCStep ( 0 ) {
	
	RTTI_SINGLE ( MOAIGlobalEventSource )

	MOAIRubyState& state = MOAIRubyRuntime::Get ().State ();
	
	for ( u32 i = 0; i < FPS_BUFFER_SIZE; ++i ) {
		this->mFrameRateBuffer [ i ] = 0.0f;
	}
	
	this->mFrameTime = ZLDeviceTime::GetTimeInSeconds ();
	
	this->mActionMgr.Set ( *this, state.CreateClassInstance < MOAIActionTree >() );
	this->mActionTree.Set ( *this, state.CreateClassInstance < MOAIActionTree >() );
	
	this->mActionMgr->Start ( this->mActionTree->GetDefaultParent (), false );
}

//----------------------------------------------------------------//
MOAISim::~MOAISim () {

	this->mActionMgr.Set ( *this, 0 );
	this->mActionTree.Set ( *this, 0 );
}

//----------------------------------------------------------------//
double MOAISim::MeasureFrameRate () {

	double frameTime = ZLDeviceTime::GetTimeInSeconds ();
	double delay = frameTime - this->mFrameTime;
	this->mFrameTime = frameTime;
	
	if ( delay > 0.0 ) {
	
		float sample = ( float )( 1.0 / delay );
		
		this->mFrameRateBuffer [ this->mFrameRateIdx++ ] = sample;
		this->mFrameRateIdx %= FPS_BUFFER_SIZE;
		
		sample = 0.0f;
		for ( u32 i = 0; i < FPS_BUFFER_SIZE; ++i ) {
			sample += this->mFrameRateBuffer [ i ];
		}
		this->mFrameRate = sample / ( float )FPS_BUFFER_SIZE;
	}
	
	return delay;
}

//----------------------------------------------------------------//
void MOAISim::OnGlobalsFinalize () {

	this->InvokeListener ( EVENT_FINALIZE );
}

//----------------------------------------------------------------//
void MOAISim::ResetPerformanceTimers () {

	this->mLastActionTreeTime = this->mActionTreeTime;
	this->mLastNodeMgrTime = this->mNodeMgrTime;
	this->mNodeMgrTime = 0.0;
	this->mActionTreeTime = 0.0;
}

//----------------------------------------------------------------//
void MOAISim::Pause () {

	if ( this->mLoopState != PAUSED ) {
		this->InvokeListener ( EVENT_PAUSE );
		this->mLoopState = PAUSED;
		this->mPauseTime = ZLDeviceTime::GetTimeInSeconds ();
	}
}

//----------------------------------------------------------------//
void MOAISim::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "EVENT_FINALIZE",			( u32 )EVENT_FINALIZE );
	state.DefineClassConst ( klass, "EVENT_PAUSE",				( u32 )EVENT_PAUSE );
	state.DefineClassConst ( klass, "EVENT_RESUME",				( u32 )EVENT_RESUME );
	state.DefineClassConst ( klass, "EVENT_STEP",				( u32 )EVENT_STEP );

	state.DefineClassConst ( klass, "SIM_LOOP_FORCE_STEP",		( u32 )SIM_LOOP_FORCE_STEP );
	state.DefineClassConst ( klass, "SIM_LOOP_ALLOW_BOOST",		( u32 )SIM_LOOP_ALLOW_BOOST );
	state.DefineClassConst ( klass, "SIM_LOOP_ALLOW_SPIN",		( u32 )SIM_LOOP_ALLOW_SPIN );
	state.DefineClassConst ( klass, "SIM_LOOP_NO_DEFICIT",		( u32 )SIM_LOOP_NO_DEFICIT );
	state.DefineClassConst ( klass, "SIM_LOOP_NO_SURPLUS",		( u32 )SIM_LOOP_NO_SURPLUS );
	state.DefineClassConst ( klass, "SIM_LOOP_RESET_CLOCK",		( u32 )SIM_LOOP_RESET_CLOCK );
	state.DefineClassConst ( klass, "SIM_LOOP_ALLOW_SOAK",		( u32 )SIM_LOOP_ALLOW_SOAK );

	state.DefineClassConst ( klass, "LOOP_FLAGS_DEFAULT",		( u32 )LOOP_FLAGS_DEFAULT );
	state.DefineClassConst ( klass, "LOOP_FLAGS_FIXED",			( u32 )LOOP_FLAGS_FIXED );
	state.DefineClassConst ( klass, "LOOP_FLAGS_MULTISTEP",		( u32 )LOOP_FLAGS_MULTISTEP );
	state.DefineClassConst ( klass, "LOOP_FLAGS_SOAK",			( u32 )LOOP_FLAGS_SOAK );

	state.DefineClassConst ( klass, "DEFAULT_STEPS_PER_SECOND",		( u32 )DEFAULT_STEPS_PER_SECOND );
	state.DefineClassConst ( klass, "DEFAULT_BOOST_THRESHOLD",		( u32 )DEFAULT_BOOST_THRESHOLD );
	state.DefineClassConst ( klass, "DEFAULT_LONG_DELAY_THRESHOLD",	( u32 )DEFAULT_LONG_DELAY_THRESHOLD );
	state.DefineClassConst ( klass, "DEFAULT_CPU_BUDGET",			( u32 )DEFAULT_CPU_BUDGET );
	state.DefineClassConst ( klass, "DEFAULT_STEP_MULTIPLIER",		( u32 )DEFAULT_STEP_MULTIPLIER );

	state.DefineStaticMethod ( klass, "clearLoopFlags", _clearLoopFlags, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "crash", _crash, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "enterFullscreenMode", _enterFullscreenMode, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "exitFullscreenMode", _exitFullscreenMode, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "forceGC", _forceGC, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "framesToTime", _framesToTime, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "getActionMgr", _getActionMgr, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getDeviceTime", _getDeviceTime, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getElapsedTime", _getElapsedTime, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getListener", &MOAIGlobalEventSource::_getListener < MOAISim >, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getLoopFlags", _getLoopFlags, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getRubyObjectCount", _getRubyObjectCount, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getMemoryUsage", _getMemoryUsage, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getMemoryUsagePlain", _getMemoryUsagePlain, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getPerformance", _getPerformance, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getStep", _getStep, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getStepCount", _getStepCount, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "hideCursor", _hideCursor, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "openWindow", _openWindow, MRB_ARGS_REQ ( 3 ) );
	state.DefineStaticMethod ( klass, "pauseTimer", _pauseTimer, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "setBoostThreshold", _setBoostThreshold, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "setCpuBudget", _setCpuBudget, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "setGCActive", _setGCActive, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "setGCStep", _setGCStep, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "setListener", &MOAIGlobalEventSource::_setListener < MOAISim >, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "setLongDelayThreshold", _setLongDelayThreshold, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "setLoopFlags", _setLoopFlags, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "setRubyAllocLogEnabled", _setRubyAllocLogEnabled, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "setStep", _setStep, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "setStepMultiplier", _setStepMultiplier, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "setStepSmoothing", _setStepSmoothing, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "setTimerError", _setTimerError, MRB_ARGS_ARG ( 0, 1 ) );
	state.DefineStaticMethod ( klass, "setTraceback", _setTraceback, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "setTextInputRect", _setTextInputRect, MRB_ARGS_REQ ( 4 ) );
	state.DefineStaticMethod ( klass, "showCursor", _showCursor, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "timeToFrames", _timeToFrames, MRB_ARGS_REQ ( 1 ) );
}

//----------------------------------------------------------------//
void MOAISim::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAISim::Resume () {

	if ( this->mLoopState == PAUSED ) {
	
		double skip = ZLDeviceTime::GetTimeInSeconds () - this->mPauseTime;
		MOAIInputMgr::Get ().FlushEvents ( skip );
	
		this->InvokeListener ( EVENT_RESUME );
		this->mLoopState = START;
	}
}

//----------------------------------------------------------------//
void MOAISim::SetStep ( double step ) {

	if ( this->mStep != step ) {

		this->mStep = step;
		if ( this->mSetSimStepFunc ) {
			this->mSetSimStepFunc ( step );
		}
	}
}

//----------------------------------------------------------------//
double MOAISim::SmoothStep ( double step ) {
	
	if ( this->mSmoothBuffer.Size () == 0 ) {
		return step;
	}
	
	size_t size = this->mSmoothBuffer.Size ();
	
	this->mSmoothBuffer [ this->mSmoothIdx++ ] = step;
	this->mSmoothIdx %= size;
	
	u32 count = 0;
	double sum = 0.0;
	for ( size_t i = 0; i < size; ++i ) {
		double dt = this->mSmoothBuffer [ i ];
		
		// Ignore long delay steps
		if ( !( this->mLoopFlags & SIM_LOOP_LONG_DELAY ) || ( dt < this->mStep * this->mLongDelayThreshold )) {
			count++;
			sum += dt;
		}
	}
	return ( count > 0 ) ? sum / count : step;
}

//----------------------------------------------------------------//
double MOAISim::StepSim ( double step, u32 multiplier ) {

	double time = ZLDeviceTime::GetTimeInSeconds ();

	MOAIRubyState state = MOAIRubyRuntime::Get ().State ();

	for ( u32 s = 0; s < multiplier; ++s ) {
		
		//lua_gc ( state, LUA_GCSTOP, 0 );
		
		//MOAITestMgr::Get ().Step ();
		
		this->InvokeListener ( EVENT_STEP );
		
		double t = ZLDeviceTime::GetTimeInSeconds ();
		MOAIInputMgr::Get ().Update ( step );
		this->mActionTree->Update ( step );
		this->mActionTreeTime = this->mActionTreeTime + ZLDeviceTime::GetTimeInSeconds () - t;
		
		t = ZLDeviceTime::GetTimeInSeconds ();
		MOAINodeMgr::Get ().Update ();
		this->mNodeMgrTime = this->mNodeMgrTime + ZLDeviceTime::GetTimeInSeconds () - t;
		
		this->mSimTime += step;
		this->mStepCount++;
		
		if ( this->mGCActive ) {
		
			// empty the userdata cache
			MOAIRubyRuntime::Get ().PurgeUserdataCache ();
		
			// crank the garbage collector
			//lua_gc ( state, LUA_GCSTEP, this->mGCStep );
		}
	}
	return ZLDeviceTime::GetTimeInSeconds () - time;
}

//----------------------------------------------------------------//
void MOAISim::Update () {

	MOAIRubyState state = MOAIRubyRuntime::Get ().State ();

	if ( !this->mRubyGCFunc ) {

		/*lua_getglobal ( state, LUA_GC_FUNC_NAME );
		this->mRubyGCFunc.SetRef ( *this, state, -1 );
		lua_pop ( state, 1 );

		lua_pushcfunction ( state, _collectgarbage );
		lua_setglobal ( state, LUA_GC_FUNC_NAME );*/
	}

	// Measure performance
	double simStartTime = ZLDeviceTime::GetTimeInSeconds ();

	double interval = this->MeasureFrameRate ();
	this->ResetPerformanceTimers ();
	
	MOAIMainThreadTaskSubscriber::Get ().Publish ();
	
	// try to account for timer error
	if ( this->mTimerError != 0.0 ) {
		
		double steps = interval / this->mStep;
		double integer = floor ( steps );
		double decimal = steps - integer;
		
		if ( decimal <= this->mTimerError ) {
			interval = this->mStep * integer;
		}
		else if ( decimal >= ( 1.0 - this->mTimerError )) {
			interval = this->mStep * ( integer + 1.0 );
		}
	}

	interval = this->SmoothStep ( interval );
	
	// actual device time elapsed since starting or restarting the sim
	this->mRealTime += interval;
	
	// bail if we're paused
	if ( this->mLoopState == PAUSED ) {
		return;
	}
	
	// the reset clock flag warps the sim time ahead to match real time just once, then autoclears
	// this means there will be no time deficit or attempted catch-up
	// if spinning is not allowed, also clear prevent any time deficit
	if ( this->mLoopFlags & SIM_LOOP_RESET_CLOCK ) {
	
		this->mLoopState = START;
		this->mLoopFlags &= ~SIM_LOOP_RESET_CLOCK;
	}
	
	// 'budget' will be used to measure the actual CPU time each sim step takes to proces
	// initialize budget to limit time spent updating when the sim has fallen behind realtime
	// this prevents a scenario where the sim falls behind but loops forever when attempting to catch up due to
	// the update itself taking too long and increading the gap between real and target time
	double budget = this->mStep * this->mCpuBudget;

	// reset sim time on start
	if ( this->mLoopState == START ) {
		
		this->mRealTime = this->mSimTime;
		this->mLoopState = RUNNING;
		
		// perform an empty step to initialize the sim
		// subtract the elapsed CPU time from the budget
		budget -= this->StepSim ( 0.0, 1 );
	}

	// 'gap' is the time left to make up between sim time and real time
	// i.e. the time deficit
	double gap = this->mRealTime - this->mSimTime;

	// long delay lets us ignore gaps bigger than a certain threshold
	if (( this->mLoopFlags & SIM_LOOP_LONG_DELAY ) && ( gap > ( this->mStep * this->mLongDelayThreshold ))) {
		budget -= this->StepSim ( this->mStep, 1 );
		gap = 0.0f;
		this->mRealTime = this->mSimTime;
	}

	// boost mode allows the sim to perform a large, variable-sized step to
	// make up the entire time deficit - but only if the sim has fallen behind
	// by a certain threshold (given in number of frames)
	// we only boost if we've fallen behind the number of steps given by boost threshold
	if ( !(( this->mLoopFlags & SIM_LOOP_ALLOW_BOOST ) && ( gap > ( this->mStep * this->mBoostThreshold )))) {
//		budget -= this->StepSim ( gap, 1 );
//		gap = 0.0f;
//	}
//	else {
	
		// we didn't boost, so process steps normally...
	
		// perform a single step only if the time deficit is greater than step time
		// in other words, at least one interval of step time has elapsed in real time
		// so we need to catch up
		if (( this->mLoopFlags & SIM_LOOP_FORCE_STEP ) || (( this->mStep <= gap ) && ( budget > 0.0 ))) {
			budget -= this->StepSim ( this->mStep, this->mStepMultiplier );
			gap -= this->mStep * ( double )this->mStepMultiplier;
		}
		
		// spin mode allows us to attempt to close the time deficit by using our
		// budget to run additional sim steps
		// of course, if the sim takes an excessively long time to process
		// we may never catch up...
		if ( this->mLoopFlags & SIM_LOOP_ALLOW_SPIN ) {
			while (( this->mStep <= gap ) && ( budget > 0.0 )) {
				budget -= this->StepSim ( this->mStep, this->mStepMultiplier );
				gap -= this->mStep * ( double )this->mStepMultiplier;
				
			}
		}

		// Will use up the remaining 'frame' budget, e.g if step size 1 / 30, it will
		// spin/sleep until this time has passed inside this update
		if ( this->mLoopFlags & SIM_LOOP_ALLOW_SOAK ) {
			
			double startTime = ZLDeviceTime::GetTimeInSeconds ();
			double remainingTime = budget - ( this->mStep * ( DEFAULT_CPU_BUDGET - 1 ) );
			
			// using 2ms buffer zone for sleeps
			while ( ( remainingTime - ( ZLDeviceTime::GetTimeInSeconds() - startTime ) > 0.002 )) {

				#ifndef MOAI_OS_WINDOWS
					usleep ( 1000 );
				#else
					// WARNING: sleep on windows is not quite as precise
					Sleep ( 1 );
				#endif
			}
		}
	}

	// if real time is more than a step ahead of sim time (for whatever reason), wait up
	if (( this->mLoopFlags & SIM_LOOP_NO_DEFICIT ) && (( this->mRealTime - this->mSimTime ) >= this->mStep )) {
		this->mRealTime = this->mSimTime;
	}

	// if real time is behind sim time (for whatever reason), catch up
	if (( this->mLoopFlags & SIM_LOOP_NO_SURPLUS ) && ( this->mRealTime < this->mSimTime )) {
		this->mRealTime = this->mSimTime;
	}
	
	// Measure performance
	double simEndTime = ZLDeviceTime::GetTimeInSeconds ();
	this->mSimDuration = simEndTime - simStartTime;
}
