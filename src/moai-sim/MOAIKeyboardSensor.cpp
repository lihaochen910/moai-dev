// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIKeyboardSensor.h>
#include <moai-sim/MOAIInputMgr.h>
#include <contrib/moai_utf8.h>

namespace KeyboardEventType {
	enum {
		KEY,
		CHAR,
		EDIT,
		INVALID
	};
}

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@lua	keyDown
	@text	Checks to see if one or more keys were pressed during the last iteration.

	@in		MOAIKeyboardSensor self
	@in		... keys					Keys to be checked against the input table.
										Each key can be specified using a MOAIKeyboardSensor.* constant or the corresponding string.
										Multiple strings can be combined: 'wasd' is equivalent to 'w', 'a', 's', 'd'.
	@out	boolean... down				For every specified key, indicates whether this key was pressed during the last iteration.
*/
mrb_value MOAIKeyboardSensor::_keyDown ( mrb_state* M, mrb_value context ) {
	return CheckKeys ( M, context, &MOAIKeyboardSensor::KeyDown );
}

//----------------------------------------------------------------//
/**	@lua	keyIsDown
	@text	Checks to see if one or more keys are currently pressed.

	@in		MOAIKeyboardSensor self
	@in		... keys					Keys to be checked against the input table.
										Each key can be specified using a MOAIKeyboardSensor.* constant or the corresponding string.
										Multiple strings can be combined: 'wasd' is equivalent to 'w', 'a', 's', 'd'.
	@out	boolean... isDown			For every specified key, indicates whether this key is currently pressed.
*/
mrb_value MOAIKeyboardSensor::_keyIsDown ( mrb_state* M, mrb_value context ) {
	return CheckKeys ( M, context, &MOAIKeyboardSensor::KeyIsDown );
}

//----------------------------------------------------------------//
/**	@lua	keyIsUp
	@text	Checks to see if one or more keys are currently up.

	@in		MOAIKeyboardSensor self
	@in		... keys					Keys to be checked against the input table.
										Each key can be specified using a MOAIKeyboardSensor.* constant or the corresponding string.
										Multiple strings can be combined: 'wasd' is equivalent to 'w', 'a', 's', 'd'.
	@out	boolean... isUp				For every specified key, indicates whether this key is currently up.
*/
mrb_value MOAIKeyboardSensor::_keyIsUp ( mrb_state* M, mrb_value context ) {
	return CheckKeys ( M, context, &MOAIKeyboardSensor::KeyIsUp );
}

//----------------------------------------------------------------//
/**	@lua	keyUp
	@text	Checks to see if one or more keys were released during the last iteration.

	@in		MOAIKeyboardSensor self
	@in		... keys					Keys to be checked against the input table.
										Each key can be specified using a MOAIKeyboardSensor.* constant or the corresponding string.
										Multiple strings can be combined: 'wasd' is equivalent to 'w', 'a', 's', 'd'.
	@out	boolean... up				For every specified key, indicates whether this key was released during the last iteration.
*/
mrb_value MOAIKeyboardSensor::_keyUp ( mrb_state* M, mrb_value context ) {
	return CheckKeys ( M, context, &MOAIKeyboardSensor::KeyUp );
}

//----------------------------------------------------------------//
/**	@lua	setCallback
	@text	This method has been deprecated. Use setKeyCallback instead.

	@in		MOAIKeyboardSensor self
	@opt	function callback			A callback function with the signature "void callback(number keyCode, bool down)".
										Default value is nil.
	@out	nil
*/
mrb_value MOAIKeyboardSensor::_setCallback ( mrb_state* M, mrb_value context ) {
	return _setKeyCallback ( M, context );
}

//----------------------------------------------------------------//
/**	@lua	setCharCallback
	@text	Sets or clears the callback to be issued when a character is typed.

	@in		MOAIKeyboardSensor self
	@opt	function callback			A callback function with the signature "void callback(string character)".
										Note that for non-ASCII characters, the string argument will be a multibyte UTF-8 character.
										Default value is nil.
	@out	nil
*/
mrb_value MOAIKeyboardSensor::_setCharCallback ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIKeyboardSensor, "U" )
	
	self->mOnChar.SetRef ( state.GetParamValue ( 1 ) );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setEditCallback
	@text	Sets or clears the callback to be issued when a character is editing.
 
	@in		MOAIKeyboardSensor self
	@opt	function callback			A callback function with the signature "void callback(string text, number start, number length)".
										Note that for non-ASCII characters, the string argument will be a multibyte UTF-8 character.
										Default value is nil.
	@out	nil
 */
mrb_value MOAIKeyboardSensor::_setEditCallback ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIKeyboardSensor, "U" )
	
	self->mOnEdit.SetRef ( state.GetParamValue ( 1 ) );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setKeyCallback
	@text	Sets or clears the callback to be issued when a key is pressed or released.

	@in		MOAIKeyboardSensor self
	@opt	function callback			A callback function with the signature "void callback(number keyCode, bool down)".
										Default value is nil.
	@out	nil
*/
mrb_value MOAIKeyboardSensor::_setKeyCallback ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIKeyboardSensor, "U" )
	
	self->mOnKey.SetRef ( state.GetParamValue ( 1 ) );
	return context;
}

//================================================================//
// MOAIKeyboardSensor
//================================================================//

//----------------------------------------------------------------//
void MOAIKeyboardSensor::ClearState () {

	// Clear the DOWN and UP flags
	for ( u32 i = 0; i < this->mClearCount; ++i ) {
		u32 keyCode = this->mClearQueue [ i ];
		this->mState [ keyCode ] = 0;
	}
	this->mClearCount = 0;
}

//----------------------------------------------------------------//
void MOAIKeyboardSensor::EnqueueKeyboardCharEvent ( u8 deviceID, u8 sensorID, u32 unicodeChar ) {

	// Don't allow non-printable characters
	if ( unicodeChar < ' ' ) return;

	MOAIInputMgr& inputMgr = MOAIInputMgr::Get ();
	if ( inputMgr.WriteEventHeader < MOAIKeyboardSensor >( deviceID, sensorID )) {
		inputMgr.Write < u32 >( KeyboardEventType::CHAR );
		inputMgr.Write < u32 >( unicodeChar );
	}
}

//----------------------------------------------------------------//
void MOAIKeyboardSensor::EnqueueKeyboardEditEvent ( u8 deviceID, u8 sensorID, char const* text, u32 start, u32 editLength, u32 maxLength ) {
	
	MOAIInputMgr& inputMgr = MOAIInputMgr::Get ();
	if ( inputMgr.WriteEventHeader < MOAIKeyboardSensor >( deviceID, sensorID )) {
		inputMgr.Write < u32 >( KeyboardEventType::EDIT );
		inputMgr.Write < u32 >( start );
		inputMgr.Write < u32 >( editLength );
		inputMgr.Write < u32 >( maxLength );
		
		for ( u32 i = 0; i < maxLength; i++ ) {
			inputMgr.Write < char >( text[i] );
		}
	}
}

//----------------------------------------------------------------//
void MOAIKeyboardSensor::EnqueueKeyboardKeyEvent ( u8 deviceID, u8 sensorID, u32 keyID, bool down ) {

	if ( keyID >= MOAI_KEY_TOTAL ) return;

	MOAIInputMgr& inputMgr = MOAIInputMgr::Get ();
	if ( inputMgr.WriteEventHeader < MOAIKeyboardSensor >( deviceID, sensorID )) {
		inputMgr.Write < u32 >( KeyboardEventType::KEY );
		inputMgr.Write < u32 >( keyID );
		inputMgr.Write < bool >( down );
	}
}

//----------------------------------------------------------------//
void MOAIKeyboardSensor::EnqueueKeyboardTextEvent ( u8 deviceID, u8 sensorID, cc8* text ) {
	
	int i = 0;
	while ( text [ i ]) {
		u_int32_t uc = moai_u8_nextchar ( text, &i );
		MOAIKeyboardSensor::EnqueueKeyboardCharEvent ( deviceID, sensorID, uc );
	}
}

//----------------------------------------------------------------//
// For each key, returns whether the specified predicate returns true.
// Expects self and a number of keycodes or strings on the stack.
mrb_value MOAIKeyboardSensor::CheckKeys ( mrb_state* M, mrb_value context, bool ( MOAIKeyboardSensor::* predicate )( u32 keyCode )) {
	MOAI_RUBY_SETUP ( MOAIKeyboardSensor, "U" )

	u32 argCount = state.GetParamsCount ();
	mrb_value ary = mrb_ary_new ( state );

	for ( u32 arg = 1; arg <= argCount; ++arg ) {
		if ( state.ParamIsType ( arg, MRB_TT_FIXNUM )) {
			// Argument is number. Treat as single key code.
			u32 keyCode = state.GetParamValue < u32 > ( arg, 0 );
			bool result = ( self->*predicate )( keyCode );
			mrb_ary_push ( state, ary, state.ToRValue ( result ) );
		}
		else if ( state.ParamIsType ( arg, MRB_TT_STRING ) ) {
			// Argument is string. Treat as list of key codes.
			cc8* str = state.GetParamValue ( arg, "" );
			for ( u32 i = 0; str [ i ]; ++i ) {
				u32 keyCode = str [ i ];
				bool result = ( self->*predicate )( keyCode );
				mrb_ary_push ( state, ary, state.ToRValue ( result ) );
			}
		}
		else {
			// Argument is invalid. Push nil to return the expected number of results
			mrb_ary_push ( state, ary, mrb_nil_value () );
		}
	}

	return ary;
}

//----------------------------------------------------------------//
bool MOAIKeyboardSensor::KeyDown ( u32 keyID ) {

	return (( this->mState [ keyID ] & DOWN ) == DOWN );
}

//----------------------------------------------------------------//
bool MOAIKeyboardSensor::KeyIsDown ( u32 keyID ) {

	return (( this->mState [ keyID ] & IS_DOWN ) == IS_DOWN );
}

//----------------------------------------------------------------//
bool MOAIKeyboardSensor::KeyIsUp ( u32 keyID ) {

	return (( this->mState [ keyID ] & IS_DOWN ) == 0 );
}

//----------------------------------------------------------------//
bool MOAIKeyboardSensor::KeyUp ( u32 keyID ) {

	return (( this->mState [ keyID ] & UP ) == UP );
}

//----------------------------------------------------------------//
MOAIKeyboardSensor::MOAIKeyboardSensor () :
	mClearCount ( 0 ) {
	
	RTTI_SINGLE ( MOAISensor )
	
	memset ( this->mState, 0, MOAI_KEY_TOTAL * sizeof ( u32 ));
}

//----------------------------------------------------------------//
MOAIKeyboardSensor::~MOAIKeyboardSensor () {
}

//----------------------------------------------------------------//
void MOAIKeyboardSensor::ParseEvent ( ZLStream& eventStream ) {
	
	u32 eventType = eventStream.Read < u32 >( KeyboardEventType::INVALID );
	if ( eventType == KeyboardEventType::KEY ) {

		u32 keyCode = eventStream.Read < u32 >( 0 );
		bool down = eventStream.Read < bool >( false );
	
		// Ignore redundant events
		if ( down == KeyIsDown ( keyCode )) return;

		bool alreadyInClearQueue = ( this->mState [ keyCode ] & ( DOWN | UP )) != 0;
	
		if ( down ) {
			this->mState [ keyCode ] |= IS_DOWN | DOWN;
		}
		else {
			this->mState [ keyCode ] &= ~IS_DOWN;
			this->mState [ keyCode ] |= UP;
		}
	
		if ( this->mOnKey ) {
			MOAIRubyState& state = MOAIRubyRuntime::Get ().State ();

			mrb_value argv [ 2 ];
			argv [ 0 ] = state.ToRValue ( keyCode );
			argv [ 1 ] = state.ToRValue ( down );

			state.FuncCall ( this->mOnKey, "call", 2, argv );
		}
	
		if ( !alreadyInClearQueue ) {
			this->mClearQueue [ this->mClearCount ] = keyCode;
			this->mClearCount++;
		}
	} else if ( eventType == KeyboardEventType::CHAR ) {

		u32 unicodeChar = eventStream.Read < u32 >( 0xffffffff );
		
		if ( this->mOnChar ) {
			// Convert to UTF-8 and zero-terminate
			char utf8Sequence [ 5 ];
			int byteCount = moai_u8_wc_toutf8 ( utf8Sequence, unicodeChar );
			utf8Sequence [ byteCount ] = 0;

			if ( byteCount ) {
				MOAIRubyState& state = MOAIRubyRuntime::Get ().State ();

				mrb_value argv [ 1 ];
				argv [ 0 ] = state.ToRValue < cc8* >( utf8Sequence );

				state.FuncCall ( this->mOnChar, "call", 1, argv );
			}
		}
	} else if ( eventType == KeyboardEventType::EDIT ) {
		
		u32 start = eventStream.Read < u32 >( 0 );
		u32 editLength = eventStream.Read < u32 >( 0 );
		u32 maxLength = eventStream.Read < u32 >( 0 );
		
		if ( this->mOnEdit ) {
			char *text = (char*)malloc(maxLength);
			eventStream.ReadBytes(text, ( size_t )maxLength);
			
			MOAIRubyState& state = MOAIRubyRuntime::Get ().State ();

			mrb_value argv [ 3 ];
			argv [ 0 ] = state.ToRValue < cc8* >( text );
			argv [ 1 ] = state.ToRValue ( start );
			argv [ 2 ] = state.ToRValue ( editLength );

			state.FuncCall ( this->mOnEdit, "call", 3, argv );
			
			free(text);
		} else {
			eventStream.SetCursor(eventStream.GetCursor() + maxLength);
		}
	}
}

//----------------------------------------------------------------//
void MOAIKeyboardSensor::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	MOAISensor::RegisterRubyClass ( state, klass );
}

//----------------------------------------------------------------//
void MOAIKeyboardSensor::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	MOAISensor::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "keyDown", _keyDown, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "keyIsDown", _keyIsDown, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "keyIsUp", _keyIsUp, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "keyUp", _keyUp, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setCallback", _setCallback, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setCharCallback", _setCharCallback, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setEditCallback", _setEditCallback, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "setKeyCallback", _setKeyCallback, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAIKeyboardSensor::ResetState () {

	// Clear the DOWN and UP flags
	for ( u32 i = 0; i < this->mClearCount; ++i ) {
		u32 keyCode = this->mClearQueue [ i ];
		this->mState [ keyCode ] &= ~( DOWN | UP );	
	}
	this->mClearCount = 0;
}
