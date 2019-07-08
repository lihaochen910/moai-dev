// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIDrawable.h>
#include <moai-sim/MOAIGfxMgr.h>
#include <moai-sim/MOAIPartition.h>
#include <moai-sim/MOAIPartitionResultMgr.h>

//================================================================//
// MOAIDrawable
//================================================================//

//----------------------------------------------------------------//
void MOAIDrawable::Draw ( MOAIRubyMemberRef& ref, bool debug ) {

	if ( ref ) {
		MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
		//state.Push ( ref );
		MOAIDrawable::Draw ( state, -1, debug );
		//state.Pop ( 1 );
	}
}

//----------------------------------------------------------------//
void MOAIDrawable::Draw ( MOAIRubyState& state, int idx, bool debug ) {

	idx = state.AbsIndex ( idx );
	mrb_vtype valType = state.GetParamType ( idx );
	
	switch ( valType ) {
	
		case MRB_TT_DATA: {
		
			/*MOAIDrawable* drawable = state.GetRubyObject < MOAIDrawable >( idx, false );
			
			if ( drawable ) {
				if ( debug ) {
					drawable->DrawDebug ();
				}
				else {
					drawable->Draw ();
				}
			}*/
			break;
		}
		
		case MRB_TT_ARRAY: {
		
			/*size_t tableSize = state.GetArraySize ( state.GetParamValue ( idx ) );
			for ( size_t i = 0; i < tableSize; ++i ) {
				lua_rawgeti ( state, idx, i + 1 );
				MOAIDrawable::Draw ( state, -1, debug );
				lua_pop ( state, 1 );
			}*/
			break;
		}
		
		case LUA_TFUNCTION: {
		
			//state.CopyToTop ( idx ); // copy the function to the top
			//state.DebugCall ( 0, 0 );
			break;
		}
	}
}

//----------------------------------------------------------------//
void MOAIDrawable::Draw ( MOAIRubyState& state, mrb_value val, bool debug ) {

	mrb_vtype valType = state.GetType ( val );

	switch ( valType ) {

		case MRB_TT_DATA: {

			MOAIDrawable* drawable = state.GetRubyObject < MOAIDrawable >( val, false );

			if ( drawable ) {
				if ( debug ) {
					drawable->DrawDebug ();
				}
				else {
					drawable->Draw ();
				}
			}
			break;
		}

		case MRB_TT_ARRAY: {

			mrb_int arraySize = state.GetArraySize ( val );
			for ( mrb_int i = 0; i < arraySize; ++i ) {
				mrb_value ref = mrb_ary_ref ( state, val, i );
				MOAIDrawable::Draw ( state, ref, debug );
			}
			break;
		}

		case MRB_TT_PROC: {

			state.FuncCall ( val, "call" );
			break;
		}
	}
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
void MOAIDrawable::MOAIDrawable_Draw ( int subPrimID ) {
	UNUSED ( subPrimID );
}

//----------------------------------------------------------------//
void MOAIDrawable::MOAIDrawable_DrawDebug ( int subPrimID ) {
	UNUSED ( subPrimID );
}
