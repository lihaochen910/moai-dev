// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/host.h>
#include <moai-sim/headers.h>

//================================================================//
// aku
//================================================================//

//----------------------------------------------------------------//
void AKUDetectFramebuffer () {

	 MOAIGfxMgr::Get ().DetectFramebuffer ();
}

//----------------------------------------------------------------//
void AKUDetectGfxContext () {

	 MOAIGfxMgr::Get ().DetectContext ();
}

//----------------------------------------------------------------//
void AKUDiscardGfxResources () {

	 MOAIGfxMgr::Get ().mResourceMgr.DiscardResources ();
}

//----------------------------------------------------------------//
void AKUDisplayListBeginPhase ( int phase ) {

	 MOAIGfxMgr::Get ().mPipelineMgr.BeginPhase ( phase );
}

//----------------------------------------------------------------//
void AKUDisplayListEnable ( int list ) {

	 MOAIGfxMgr::Get ().mPipelineMgr.EnablePipeline ( list );
}

//----------------------------------------------------------------//
void AKUDisplayListEndPhase ( int phase ) {

	 MOAIGfxMgr::Get ().mPipelineMgr.EndPhase ( phase );
}

//----------------------------------------------------------------//
bool AKUDisplayListHasContent ( int list ) {

	return MOAIGfxMgr::Get ().mPipelineMgr.HasContent ( list );
}

//----------------------------------------------------------------//
bool AKUDisplayListIsEnabled ( int list ) {

	return MOAIGfxMgr::Get ().mPipelineMgr.IsPipelineEnabled ( list );
}

//----------------------------------------------------------------//
void AKUDisplayListProcess ( int list ) {

	MOAIGfxMgr::Get ().mPipelineMgr.ProcessPipeline ( list );
}

//----------------------------------------------------------------//
void AKUDisplayListPublishAndReset () {

	MOAIGfxMgr::Get ().mPipelineMgr.PublishAndReset ( MOAIGfxPipelineClerk::LOADING_PIPELINE );
	MOAIGfxMgr::Get ().mPipelineMgr.PublishAndReset ( MOAIGfxPipelineClerk::DRAWING_PIPELINE );
}

//----------------------------------------------------------------//
void AKUEnqueueButtonEvent ( int deviceID, int sensorID, bool down ) {

	MOAIButtonSensor::EnqueueButtonEvent (( u8 )deviceID, ( u8 )sensorID, down );
}

//----------------------------------------------------------------//
void AKUEnqueueCompassEvent ( int deviceID, int sensorID, float heading ) {

	MOAICompassSensor::EnqueueCompassEvent (( u8 )deviceID, ( u8 )sensorID, heading );
}

//----------------------------------------------------------------//
void AKUEnqueueJoystickEvent( int deviceID, int sensorID, float x, float y ) {

	MOAIJoystickSensor::EnqueueJoystickEvent (( u8 )deviceID, ( u8 )sensorID, x, y );
}

//----------------------------------------------------------------//
void AKUEnqueueKeyboardCharEvent ( int deviceID, int sensorID, int unicodeChar ) {

	MOAIKeyboardSensor::EnqueueKeyboardCharEvent (( u8 )deviceID, ( u8 )sensorID, unicodeChar );
}

//----------------------------------------------------------------//
void AKUEnqueueKeyboardEditEvent ( int deviceID, int sensorID, char const* text, int start, int editLength, int maxLength) {
	
	MOAIKeyboardSensor::EnqueueKeyboardEditEvent (( u8 )deviceID, ( u8 )sensorID, text, ( u32 )start, ( u32 )editLength, ( u32 )maxLength );
}

//----------------------------------------------------------------//
void AKUEnqueueKeyboardKeyEvent ( int deviceID, int sensorID, int keyID, bool down ) {

	MOAIKeyboardSensor::EnqueueKeyboardKeyEvent (( u8 )deviceID, ( u8 )sensorID, keyID, down );
}

//----------------------------------------------------------------//
void AKUEnqueueKeyboardTextEvent ( int deviceID, int sensorID, const char* text ) {

	MOAIKeyboardSensor::EnqueueKeyboardTextEvent (( u8 )deviceID, ( u8 )sensorID, text );
}

//----------------------------------------------------------------//
void AKUEnqueueLevelEvent ( int deviceID, int sensorID, float x, float y, float z ) {

	MOAIMotionSensor::EnqueueLevelEvent (( u8 )deviceID, ( u8 )sensorID, x, y, z );
}

//----------------------------------------------------------------//
void AKUEnqueueLocationEvent ( int deviceID, int sensorID, double longitude, double latitude, double altitude, float hAccuracy, float vAccuracy, float speed ) {

	MOAILocationSensor::EnqueueLocationEvent (( u8 )deviceID, ( u8 )sensorID, longitude, latitude, altitude, hAccuracy, vAccuracy, speed );
}

//----------------------------------------------------------------//
void AKUEnqueuePointerEvent ( int deviceID, int sensorID, int x, int y ) {

	MOAIPointerSensor::EnqueuePointerEvent (( u8 )deviceID, ( u8 )sensorID, x, y );
}

//----------------------------------------------------------------//
void AKUEnqueueTouchEvent ( int deviceID, int sensorID, int touchID, bool down, float x, float y ) {

	MOAITouchSensor::EnqueueTouchEvent (( u8 )deviceID, ( u8 )sensorID, ( u32 )touchID, down, x, y );
}

//----------------------------------------------------------------//
void AKUEnqueueTouchEventCancel ( int deviceID, int sensorID ) {

	MOAITouchSensor::EnqueueTouchEventCancel (( u8 )deviceID, ( u8 )sensorID );
}

//----------------------------------------------------------------//
void AKUEnqueueVectorEvent ( int deviceID, int sensorID, float x, float y, float z ) {

	MOAIVectorSensor::EnqueueVectorEvent (( u8 )deviceID, ( u8 )sensorID, x, y, z );
}

//----------------------------------------------------------------//
void AKUEnqueueWheelEvent ( int deviceID, int sensorID, float value ) {

	MOAIWheelSensor::EnqueueWheelEvent (( u8 )deviceID, ( u8 )sensorID, value );
}

//----------------------------------------------------------------//
double AKUGetSimStep () {

	return MOAISim::Get ().GetStep ();
}

//----------------------------------------------------------------//
void AKUPause ( bool pause ) {

	if ( pause ) {
		MOAISim::Get().Pause ();
	}
	else {
		MOAISim::Get().Resume ();
	}
}

//----------------------------------------------------------------//
void AKURender () {

	 MOAIRenderMgr::Get ().Render ();
}

//----------------------------------------------------------------//
void AKUSetFunc_HideCursor ( AKUHideCursorFunc func ) {

	MOAISim::Get ().SetHideCursorFunc ( func );
}

//----------------------------------------------------------------//
void AKUReserveInputDevices ( int total ) {

	 MOAIInputMgr::Get ().ReserveDevices (( u8 )total );
}

//----------------------------------------------------------------//
void AKUReserveInputDeviceSensors ( int deviceID, int total ) {

	 MOAIInputMgr::Get ().ReserveSensors (( u8 )deviceID, ( u8 )total );
}

//----------------------------------------------------------------//
void AKUSetFunc_EnterFullscreenMode ( AKUEnterFullscreenModeFunc func ) {

	MOAISim::Get ().SetEnterFullscreenModeFunc ( func );
}

//----------------------------------------------------------------//
void AKUSetFunc_ExitFullscreenMode ( AKUExitFullscreenModeFunc func ) {

	MOAISim::Get ().SetExitFullscreenModeFunc ( func );
}

//----------------------------------------------------------------//
void AKUSetFunc_OpenWindow ( AKUOpenWindowFunc func ) {

	MOAISim::Get ().SetOpenWindowFunc ( func );
}

//----------------------------------------------------------------//
void AKUSetFunc_SetSimStep ( AKUSetSimStepFunc func ) {

	MOAISim::Get ().SetSetSimStepFunc ( func );
}

//----------------------------------------------------------------//
void AKUSetInputAutoTimestamp ( bool autotimestamp ) {

	 MOAIInputMgr::Get ().SetAutotimestamp ( autotimestamp );
}

//----------------------------------------------------------------//
void AKUSetInputConfigurationName ( char const* name ) {

	 MOAIInputMgr::Get ().SetConfigurationName ( name );
}

//----------------------------------------------------------------//
void AKUSetInputDevice ( int deviceID, char const* name ) {

	 MOAIInputMgr::Get ().SetDevice (( u8 )deviceID, name );
}

//----------------------------------------------------------------//
void AKUSetInputDeviceHardwareInfo ( int deviceID, char const* hardwareInfo ) {

	 MOAIInputMgr::Get ().SetDeviceHardwareInfo (( u8 )deviceID, hardwareInfo );
}

//----------------------------------------------------------------//
void AKUSetInputDeviceActive ( int deviceID, bool active ) {

	 MOAIInputMgr::Get ().SetDeviceActive (( u8 )deviceID, active );
}

//----------------------------------------------------------------//
void AKUSetInputDeviceButton ( int deviceID, int sensorID, char const* name ) {

	 MOAIInputMgr::Get ().SetSensor < MOAIButtonSensor >(( u8 )deviceID, ( u8 )sensorID, name );
}

//----------------------------------------------------------------//
void AKUSetInputDeviceCompass ( int deviceID, int sensorID, char const* name ) {

	 MOAIInputMgr::Get ().SetSensor < MOAICompassSensor >(( u8 )deviceID, ( u8 )sensorID, name );
}

//----------------------------------------------------------------//
void AKUSetInputDeviceKeyboard ( int deviceID, int sensorID, char const* name ) {

	 MOAIInputMgr::Get ().SetSensor < MOAIKeyboardSensor >(( u8 )deviceID, ( u8 )sensorID, name );
}

//----------------------------------------------------------------//
void AKUSetInputDeviceJoystick ( int deviceID, int sensorID, char const* name ) {

	 MOAIInputMgr::Get ().SetSensor < MOAIJoystickSensor >(( u8 )deviceID, ( u8 )sensorID, name );
}

//----------------------------------------------------------------//
void AKUSetInputDeviceLevel ( int deviceID, int sensorID, char const* name ) {

	 MOAIInputMgr::Get ().SetSensor < MOAIMotionSensor >(( u8 )deviceID, ( u8 )sensorID, name );
}

//----------------------------------------------------------------//
void AKUSetInputDeviceLocation ( int deviceID, int sensorID, char const* name ) {

	 MOAIInputMgr::Get ().SetSensor < MOAILocationSensor >(( u8 )deviceID, ( u8 )sensorID, name );
}

//----------------------------------------------------------------//
void AKUSetInputDevicePointer ( int deviceID, int sensorID, char const* name ) {

	 MOAIInputMgr::Get ().SetSensor < MOAIPointerSensor >(( u8 )deviceID, ( u8 )sensorID, name );
}

//----------------------------------------------------------------//
void AKUSetInputDeviceTouch ( int deviceID, int sensorID, char const* name ) {

	 MOAIInputMgr::Get ().SetSensor < MOAITouchSensor >(( u8 )deviceID, ( u8 )sensorID, name );
}

//----------------------------------------------------------------//
void AKUSetInputDeviceVector ( int deviceID, int sensorID, char const* name ) {

	 MOAIInputMgr::Get ().SetSensor < MOAIVectorSensor >(( u8 )deviceID, ( u8 )sensorID, name );
}

//----------------------------------------------------------------//
void AKUSetInputDeviceWheel ( int deviceID, int sensorID, char const* name ) {

	 MOAIInputMgr::Get ().SetSensor < MOAIWheelSensor >(( u8 )deviceID, ( u8 )sensorID, name );
}

//----------------------------------------------------------------//
void AKUSetInputTimebase ( double timebase ) {

	 MOAIInputMgr::Get ().SetTimebase ( timebase );
}

//----------------------------------------------------------------//
void AKUSetInputTimestamp ( double timestamp ) {

	 MOAIInputMgr::Get ().SetTimestamp ( timestamp );
}

//----------------------------------------------------------------//
void AKUSetOrientation ( int orientation ) {

	 MOAIGfxMgr::Get ().mGfxState.GetDefaultFrameBuffer ()->SetLandscape ( orientation == AKU_ORIENTATION_LANDSCAPE );
}

//----------------------------------------------------------------//	
void AKUSetScreenDpi ( int dpi ) {

	 MOAIEnvironment::Get ().SetValue ( MOAI_ENV_screenDpi, dpi );
}

//----------------------------------------------------------------//
void AKUSetScreenSize ( int width, int height) {

	MOAIEnvironment::Get ().SetValue ( MOAI_ENV_horizontalResolution, width );
	MOAIEnvironment::Get ().SetValue ( MOAI_ENV_verticalResolution, height );
}

//----------------------------------------------------------------//
void AKUSetViewSize ( int width, int height ) {
	
	 MOAIGfxMgr& gfxMgr= MOAIGfxMgr::Get ();
	
	 u32 currentWidth = gfxMgr.mGfxState.GetBufferWidth ();
	 u32 currentHeight = gfxMgr.mGfxState.GetBufferHeight ();
	
	 if (( currentWidth != ( u32 )width ) || ( currentHeight != ( u32 )height )) {
	
	 	MOAIGfxMgr::Get ().SetBufferSize ( width, height );
		
		/*MOAIScopedLuaState state = MOAILuaRuntime::Get ().State ();
		if ( gfxMgr.PushListener ( MOAIGfxMgr::EVENT_RESIZE, state )) {
			lua_pushnumber ( state, width );
			lua_pushnumber ( state, height );
			state.DebugCall ( 2, 0 );
		}*/
	 }
}

//----------------------------------------------------------------//
void AKUSetFunc_ShowCursor ( AKUShowCursorFunc func ) {

	MOAISim::Get ().SetShowCursorFunc ( func );
}

//----------------------------------------------------------------//
void AKUSetFunc_SetTextInputRect ( AKUSetTextInputRectFunc func ) {
	
	MOAISim::Get ().SetSetTextInputRectFunc ( func );
}

//----------------------------------------------------------------//
void AKUSimAppFinalize () {
}

//----------------------------------------------------------------//
void AKUSimAppInitialize () {
}

//----------------------------------------------------------------//
void AKUSimContextInitialize () {

	ZLBitBuffer::Test ();

	//ZLLog_DebugF ( ZLLog::CONSOLE, "AKUSimContextInitialize Mgr Affirm start!\n" );

	MOAINodeMgr::Affirm ();
	MOAIActionStackMgr::Affirm ();

	//MOAIProfiler::Affirm ();
	//MOAIGfxMgr::Affirm ();
	MOAIMaterialMgr::Affirm ();
	MOAIImageFormatMgr::Affirm ();
	MOAIVertexFormatMgr::Affirm ();
	MOAIShaderMgr::Affirm ();
	MOAIDraw::Affirm ();
	MOAIDebugLinesMgr::Affirm ();
	MOAIPartitionResultMgr::Affirm ();
	MOAINodeMgr::Affirm ();
	MOAIInputMgr::Affirm ();
	//MOAISim::Affirm ();
	MOAIRenderMgr::Affirm ();

	//ZLLog_DebugF ( ZLLog::CONSOLE, "AKUSimContextInitialize Mgr Affirm ok!\n" );

	// MOAI
	REGISTER_RUBY_CLASS ( MOAIAction )
	REGISTER_RUBY_CLASS ( MOAIActionTree )
	REGISTER_RUBY_CLASS ( MOAIAnim )
	REGISTER_RUBY_CLASS ( MOAIAnimCurve )
	REGISTER_RUBY_CLASS ( MOAIAnimCurveBone )
	REGISTER_RUBY_CLASS ( MOAIAnimCurveQuat )
	REGISTER_RUBY_CLASS ( MOAIAnimCurveVec )
	//REGISTER_LUA_CLASS ( MOAIBspBuilder2D )
	REGISTER_RUBY_CLASS ( MOAIButtonSensor )
	REGISTER_RUBY_CLASS ( MOAICamera )
	REGISTER_RUBY_CLASS ( MOAICameraAnchor2D )
	REGISTER_RUBY_CLASS ( MOAICameraFitter2D )
	REGISTER_RUBY_CLASS ( MOAICollisionDeck )
	REGISTER_RUBY_CLASS ( MOAICollisionProp )
	REGISTER_RUBY_CLASS ( MOAICollisionWorld )
	REGISTER_RUBY_CLASS ( MOAIColor )
	REGISTER_RUBY_CLASS ( MOAICompassSensor )
	REGISTER_RUBY_CLASS ( MOAICoroutine )
	REGISTER_RUBY_CLASS ( MOAIDebugLinesMgr )
	REGISTER_RUBY_CLASS ( MOAIDeckRemapper )
	REGISTER_RUBY_CLASS ( MOAIDraw )
	REGISTER_RUBY_CLASS ( MOAIDrawDeck )
	REGISTER_RUBY_CLASS ( MOAIEaseDriver )
	REGISTER_RUBY_CLASS ( MOAIEaseType )
	REGISTER_RUBY_CLASS ( MOAIFancyGrid )
	REGISTER_RUBY_CLASS ( MOAIFrameBuffer )
	//REGISTER_RUBY_CLASS ( MOAIFrameBufferTexture )
	REGISTER_RUBY_CLASS ( MOAIGeometryWriter )

	REGISTER_RUBY_CLASS ( MOAIGfxMgr )
	REGISTER_RUBY_CLASS ( MOAIGraphicsProp )
	REGISTER_RUBY_CLASS ( MOAIGraphicsGridProp )
	REGISTER_RUBY_CLASS ( MOAIGrid )
	REGISTER_RUBY_CLASS ( MOAIGridSpace )
	REGISTER_RUBY_CLASS ( MOAIGridPathGraph )
	REGISTER_RUBY_CLASS ( MOAIImage )
	REGISTER_RUBY_CLASS ( MOAIImageTexture )
	REGISTER_RUBY_CLASS ( MOAIIndexBuffer )
	REGISTER_RUBY_CLASS ( MOAIInputDevice )
	REGISTER_RUBY_CLASS ( MOAIInputMgr )
	REGISTER_RUBY_CLASS ( MOAIJoystickSensor )
	REGISTER_RUBY_CLASS ( MOAIKeyCode )
	REGISTER_RUBY_CLASS ( MOAIKeyboardSensor )
	REGISTER_RUBY_CLASS ( MOAILayer )
	REGISTER_RUBY_CLASS ( MOAILayoutFrame )
	REGISTER_RUBY_CLASS ( MOAILight )
	REGISTER_RUBY_CLASS ( MOAILightFormat )
	REGISTER_RUBY_CLASS ( MOAILocationSensor )
	REGISTER_RUBY_CLASS ( MOAIMaterialBatch )
	REGISTER_RUBY_CLASS ( MOAIMatrix )
	REGISTER_RUBY_CLASS ( MOAIMesh )
	REGISTER_RUBY_CLASS ( MOAIMetaTileDeck2D )
	REGISTER_RUBY_CLASS ( MOAIMotionSensor )
	REGISTER_RUBY_CLASS ( MOAINode )
	REGISTER_RUBY_CLASS ( MOAINodeMgr )
	REGISTER_RUBY_CLASS ( MOAIParticleCallbackPlugin )
	REGISTER_RUBY_CLASS ( MOAIParticleDistanceEmitter )
	REGISTER_RUBY_CLASS ( MOAIParticleForce )
	REGISTER_RUBY_CLASS ( MOAIParticleScript )
	REGISTER_RUBY_CLASS ( MOAIParticleState )
	REGISTER_RUBY_CLASS ( MOAIParticleSystem )
	REGISTER_RUBY_CLASS ( MOAIParticleTimedEmitter )
	REGISTER_RUBY_CLASS ( MOAIPartition )
	REGISTER_RUBY_CLASS ( MOAIPartitionViewLayer )
	REGISTER_RUBY_CLASS ( MOAIPath )
	REGISTER_RUBY_CLASS ( MOAIPathFinder )
	REGISTER_RUBY_CLASS ( MOAIPathStepper )
	REGISTER_RUBY_CLASS ( MOAIPathTerrainDeck )
	REGISTER_RUBY_CLASS ( MOAIPinTransform )
	REGISTER_RUBY_CLASS ( MOAIPointerSensor )
	//REGISTER_LUA_CLASS ( MOAIProfilerReportBox )
	REGISTER_RUBY_CLASS ( MOAIRegion )
	REGISTER_RUBY_CLASS ( MOAIRenderMgr )
	REGISTER_RUBY_CLASS ( MOAIScissorRect )
	REGISTER_RUBY_CLASS ( MOAIScriptNode )
	REGISTER_RUBY_CLASS ( MOAISelectionMesh )
	REGISTER_RUBY_CLASS ( MOAIShader )
	REGISTER_RUBY_CLASS ( MOAIShaderMgr )
	REGISTER_RUBY_CLASS ( MOAIShaderProgram )
	REGISTER_RUBY_CLASS ( MOAISim )
	REGISTER_RUBY_CLASS ( MOAISpriteDeck2D )
	REGISTER_RUBY_CLASS ( MOAIStretchPatch2D )
	//REGISTER_LUA_CLASS ( MOAISurfaceDeck2D )
	REGISTER_RUBY_CLASS ( MOAITableLayer )
	REGISTER_RUBY_CLASS ( MOAITableViewLayer )
	REGISTER_RUBY_CLASS ( MOAITexture )
	REGISTER_RUBY_CLASS ( MOAITileDeck2D )
	REGISTER_RUBY_CLASS ( MOAITimer )
	REGISTER_RUBY_CLASS ( MOAITouchSensor )
	REGISTER_RUBY_CLASS ( MOAITransform )
	REGISTER_RUBY_CLASS ( MOAIVecPathGraph )
	REGISTER_RUBY_CLASS ( MOAIVectorTesselator )
	REGISTER_RUBY_CLASS ( MOAIVertexArray )
	REGISTER_RUBY_CLASS ( MOAIVertexBuffer )
	REGISTER_RUBY_CLASS ( MOAIVertexFormat )
	REGISTER_RUBY_CLASS ( MOAIVertexFormatMgr )
	REGISTER_RUBY_CLASS ( MOAIViewport )
	REGISTER_RUBY_CLASS ( MOAIWheelSensor )
	
	// text stuff here for now
	REGISTER_RUBY_CLASS ( MOAIBitmapFontReader )
	REGISTER_RUBY_CLASS ( MOAIDynamicGlyphCache )
	REGISTER_RUBY_CLASS ( MOAIFont )
	REGISTER_RUBY_CLASS ( MOAIStaticGlyphCache )
	REGISTER_RUBY_CLASS ( MOAITextBundle )
	REGISTER_RUBY_CLASS ( MOAITextLabel )
	REGISTER_RUBY_CLASS ( MOAITextStyle )
	
	#if MOAI_WITH_TINYXML
		REGISTER_RUBY_CLASS ( MOAIParticlePexPlugin )
	#endif
	
	#if MOAI_WITH_FREETYPE
		REGISTER_RUBY_CLASS ( MOAIFreeTypeFontReader )
	#endif

	MOAIGfxMgr::Affirm ();
	MOAIGfxMgr::Get ().mGfxState.InitBuffers ();
	MOAISim::Affirm ();

	REGISTER_RUBY_CLASS ( MOAIFrameBufferTexture )
}

//----------------------------------------------------------------//
void AKUUpdate () {

	MOAISim::Get ().Update ();
}
