#include "pch.h"
#include <moai-sim/MOAIKeyCode.h>
#include <moai-sim/MOAIKeyCodeEnum.h>

//================================================================//
// MOAIKeyCode
//================================================================//

//----------------------------------------------------------------//
void MOAIKeyCode::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	state.DefineClassConst ( klass, "BACKSPACE",		( u32 )MOAI_KEY_BACKSPACE );
	state.DefineClassConst ( klass, "TAB",				( u32 )MOAI_KEY_TAB );
	state.DefineClassConst ( klass, "RETURN",			( u32 )MOAI_KEY_RETURN );
	state.DefineClassConst ( klass, "SHIFT",			( u32 )MOAI_KEY_SHIFT );
	state.DefineClassConst ( klass, "CONTROL",			( u32 )MOAI_KEY_CONTROL );
	state.DefineClassConst ( klass, "ALT",				( u32 )MOAI_KEY_ALT );
	state.DefineClassConst ( klass, "PAUSE",			( u32 )MOAI_KEY_PAUSE );
	state.DefineClassConst ( klass, "CAPS_LOCK",		( u32 )MOAI_KEY_CAPS_LOCK );
	state.DefineClassConst ( klass, "ESCAPE",			( u32 )MOAI_KEY_ESCAPE );
	state.DefineClassConst ( klass, "SPACE",			( u32 )MOAI_KEY_SPACE );
	state.DefineClassConst ( klass, "PAGE_UP",			( u32 )MOAI_KEY_PAGE_UP );
	state.DefineClassConst ( klass, "PAGE_DOWN",		( u32 )MOAI_KEY_PAGE_DOWN );
	state.DefineClassConst ( klass, "END",				( u32 )MOAI_KEY_END );
	state.DefineClassConst ( klass, "HOME",				( u32 )MOAI_KEY_HOME );
	state.DefineClassConst ( klass, "LEFT",				( u32 )MOAI_KEY_LEFT );
	state.DefineClassConst ( klass, "UP",				( u32 )MOAI_KEY_UP );
	state.DefineClassConst ( klass, "RIGHT",			( u32 )MOAI_KEY_RIGHT );
	state.DefineClassConst ( klass, "DOWN",				( u32 )MOAI_KEY_DOWN );
	state.DefineClassConst ( klass, "PRINT_SCREEN",		( u32 )MOAI_KEY_PRINT_SCREEN );
	state.DefineClassConst ( klass, "INSERT",			( u32 )MOAI_KEY_INSERT );
	state.DefineClassConst ( klass, "DELETE",			( u32 )MOAI_KEY_DELETE );
	state.DefineClassConst ( klass, "DIGIT_0",			( u32 )MOAI_KEY_DIGIT_0 );
	state.DefineClassConst ( klass, "DIGIT_1",			( u32 )MOAI_KEY_DIGIT_1 );
	state.DefineClassConst ( klass, "DIGIT_2",			( u32 )MOAI_KEY_DIGIT_2 );
	state.DefineClassConst ( klass, "DIGIT_3",			( u32 )MOAI_KEY_DIGIT_3 );
	state.DefineClassConst ( klass, "DIGIT_4",			( u32 )MOAI_KEY_DIGIT_4 );
	state.DefineClassConst ( klass, "DIGIT_5",			( u32 )MOAI_KEY_DIGIT_5 );
	state.DefineClassConst ( klass, "DIGIT_6",			( u32 )MOAI_KEY_DIGIT_6 );
	state.DefineClassConst ( klass, "DIGIT_7",			( u32 )MOAI_KEY_DIGIT_7 );
	state.DefineClassConst ( klass, "DIGIT_8",			( u32 )MOAI_KEY_DIGIT_8 );
	state.DefineClassConst ( klass, "DIGIT_9",			( u32 )MOAI_KEY_DIGIT_9 );
	state.DefineClassConst ( klass, "A",				( u32 )MOAI_KEY_A );
	state.DefineClassConst ( klass, "B",				( u32 )MOAI_KEY_B );
	state.DefineClassConst ( klass, "C",				( u32 )MOAI_KEY_C );
	state.DefineClassConst ( klass, "D",				( u32 )MOAI_KEY_D );
	state.DefineClassConst ( klass, "E",				( u32 )MOAI_KEY_E );
	state.DefineClassConst ( klass, "F",				( u32 )MOAI_KEY_F );
	state.DefineClassConst ( klass, "G",				( u32 )MOAI_KEY_G );
	state.DefineClassConst ( klass, "H",				( u32 )MOAI_KEY_H );
	state.DefineClassConst ( klass, "I",				( u32 )MOAI_KEY_I );
	state.DefineClassConst ( klass, "J",				( u32 )MOAI_KEY_J );
	state.DefineClassConst ( klass, "K",				( u32 )MOAI_KEY_K );
	state.DefineClassConst ( klass, "L",				( u32 )MOAI_KEY_L );
	state.DefineClassConst ( klass, "M",				( u32 )MOAI_KEY_M );
	state.DefineClassConst ( klass, "N",				( u32 )MOAI_KEY_N );
	state.DefineClassConst ( klass, "O",				( u32 )MOAI_KEY_O );
	state.DefineClassConst ( klass, "P",				( u32 )MOAI_KEY_P );
	state.DefineClassConst ( klass, "Q",				( u32 )MOAI_KEY_Q );
	state.DefineClassConst ( klass, "R",				( u32 )MOAI_KEY_R );
	state.DefineClassConst ( klass, "S",				( u32 )MOAI_KEY_S );
	state.DefineClassConst ( klass, "T",				( u32 )MOAI_KEY_T );
	state.DefineClassConst ( klass, "U",				( u32 )MOAI_KEY_U );
	state.DefineClassConst ( klass, "V",				( u32 )MOAI_KEY_V );
	state.DefineClassConst ( klass, "W",				( u32 )MOAI_KEY_W );
	state.DefineClassConst ( klass, "X",				( u32 )MOAI_KEY_X );
	state.DefineClassConst ( klass, "Y",				( u32 )MOAI_KEY_Y );
	state.DefineClassConst ( klass, "Z",				( u32 )MOAI_KEY_Z );
	state.DefineClassConst ( klass, "GUI",				( u32 )MOAI_KEY_GUI );
	state.DefineClassConst ( klass, "APPLICATION",		( u32 )MOAI_KEY_APPLICATION );
	state.DefineClassConst ( klass, "NUM_0",			( u32 )MOAI_KEY_NUM_0 );
	state.DefineClassConst ( klass, "NUM_1",			( u32 )MOAI_KEY_NUM_1 );
	state.DefineClassConst ( klass, "NUM_2",			( u32 )MOAI_KEY_NUM_2 );
	state.DefineClassConst ( klass, "NUM_3",			( u32 )MOAI_KEY_NUM_3 );
	state.DefineClassConst ( klass, "NUM_4",			( u32 )MOAI_KEY_NUM_4 );
	state.DefineClassConst ( klass, "NUM_5",			( u32 )MOAI_KEY_NUM_5 );
	state.DefineClassConst ( klass, "NUM_6",			( u32 )MOAI_KEY_NUM_6 );
	state.DefineClassConst ( klass, "NUM_7",			( u32 )MOAI_KEY_NUM_7 );
	state.DefineClassConst ( klass, "NUM_8",			( u32 )MOAI_KEY_NUM_8 );
	state.DefineClassConst ( klass, "NUM_9",			( u32 )MOAI_KEY_NUM_9 );
	state.DefineClassConst ( klass, "NUM_MULTIPLY",		( u32 )MOAI_KEY_NUM_MULTIPLY );
	state.DefineClassConst ( klass, "NUM_PLUS",			( u32 )MOAI_KEY_NUM_PLUS );
	state.DefineClassConst ( klass, "NUM_MINUS",		( u32 )MOAI_KEY_NUM_MINUS );
	state.DefineClassConst ( klass, "NUM_DECIMAL",		( u32 )MOAI_KEY_NUM_DECIMAL );
	state.DefineClassConst ( klass, "NUM_DIVIDE",		( u32 )MOAI_KEY_NUM_DIVIDE );
	state.DefineClassConst ( klass, "F1",				( u32 )MOAI_KEY_F1 );
	state.DefineClassConst ( klass, "F2",				( u32 )MOAI_KEY_F2 );
	state.DefineClassConst ( klass, "F3",				( u32 )MOAI_KEY_F3 );
	state.DefineClassConst ( klass, "F4",				( u32 )MOAI_KEY_F4 );
	state.DefineClassConst ( klass, "F5",				( u32 )MOAI_KEY_F5 );
	state.DefineClassConst ( klass, "F6",				( u32 )MOAI_KEY_F6 );
	state.DefineClassConst ( klass, "F7",				( u32 )MOAI_KEY_F7 );
	state.DefineClassConst ( klass, "F8",				( u32 )MOAI_KEY_F8 );
	state.DefineClassConst ( klass, "F9",				( u32 )MOAI_KEY_F9 );
	state.DefineClassConst ( klass, "F10",				( u32 )MOAI_KEY_F10 );
	state.DefineClassConst ( klass, "F11",				( u32 )MOAI_KEY_F11 );
	state.DefineClassConst ( klass, "F12",				( u32 )MOAI_KEY_F12 );
	state.DefineClassConst ( klass, "NUM_LOCK",			( u32 )MOAI_KEY_NUM_LOCK );
	state.DefineClassConst ( klass, "SCROLL_LOCK",		( u32 )MOAI_KEY_SCROLL_LOCK );
	state.DefineClassConst ( klass, "OEM_1",			( u32 )MOAI_KEY_OEM_1 );
	state.DefineClassConst ( klass, "OEM_PLUS",			( u32 )MOAI_KEY_OEM_PLUS );
	state.DefineClassConst ( klass, "OEM_COMMA",		( u32 )MOAI_KEY_OEM_COMMA );
	state.DefineClassConst ( klass, "OEM_MINUS",		( u32 )MOAI_KEY_OEM_MINUS );
	state.DefineClassConst ( klass, "OEM_PERIOD",		( u32 )MOAI_KEY_OEM_PERIOD );
	state.DefineClassConst ( klass, "OEM_2",			( u32 )MOAI_KEY_OEM_2 );
	state.DefineClassConst ( klass, "OEM_3",			( u32 )MOAI_KEY_OEM_3 );
	state.DefineClassConst ( klass, "OEM_4",			( u32 )MOAI_KEY_OEM_4 );
	state.DefineClassConst ( klass, "OEM_5",			( u32 )MOAI_KEY_OEM_5 );
	state.DefineClassConst ( klass, "OEM_6",			( u32 )MOAI_KEY_OEM_6 );
	state.DefineClassConst ( klass, "OEM_7",			( u32 )MOAI_KEY_OEM_7 );
	state.DefineClassConst ( klass, "OEM_8",			( u32 )MOAI_KEY_OEM_8 );
	state.DefineClassConst ( klass, "OEM_102",			( u32 )MOAI_KEY_OEM_102 );
}
