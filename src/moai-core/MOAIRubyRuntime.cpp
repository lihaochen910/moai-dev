// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <moai-core/MOAILogMgr.h>
#include <moai-core/MOAIRubyObject.h>
#include <moai-core/MOAIRubyState.h>
//#include <moai-core/MOAIScopedRubyState.h>
#include <moai-core/MOAIRubyRuntime.h>
#include <moai-core/MOAIRubyRef.h>

#include <moai-core/MOAIRubyState-impl.h>
#include <moai-core/MOAIRubyClass-impl.h>

#define DUMP_FORMAT "%p <%s> %s"

//typedef STLSet < struct Table* > TableSet;

//================================================================//
// local
//================================================================//

//#if LUA_VERSION_NUM < 502
//
//	//----------------------------------------------------------------//
//	// TODO: move into MOAIRubyState
//	static void lua_pushglobaltable ( lua_State* l ) {
//		lua_pushvalue ( l, LUA_GLOBALSINDEX );
//	}
//
//#endif
//
//#if ( LUA_VERSION_NUM < 502 ) && ( !MOAI_WITH_LUAJIT )
//
//	//----------------------------------------------------------------//
//	static void _dumpType ( lua_State* L, int idx, const char *name, bool verbose, TableSet& foundTables ) {
//
//		MOAIRubyState state ( L );
//
//		const char *format = DUMP_FORMAT;
//
//		idx = state.AbsIndex( idx );
//		StkId tvalue = state->base + idx - 1;
//
//		switch ( lua_type ( state, idx )) {
//
//			case LUA_TBOOLEAN:
//
//				ZLLogF ( ZLLog::CONSOLE, format, tvalue, "bool", name );
//				ZLLogF ( ZLLog::CONSOLE, " = %s", lua_toboolean ( state, idx ) ? "true" : "false" );
//				break;
//
//			case LUA_TFUNCTION: {
//
//				const char *funcType = iscfunction ( tvalue ) ? "C function" : "Ruby function";
//
//				ZLLogF ( ZLLog::CONSOLE, format, clvalue ( tvalue ), funcType, name );
//				break;
//			}
//
//			case LUA_TLIGHTUSERDATA:
//
//				ZLLogF ( ZLLog::CONSOLE, format, pvalue ( tvalue ), "pointer", name );
//				break;
//
//			case LUA_TNIL:
//
//				ZLLogF ( ZLLog::CONSOLE, format, tvalue, "nil", name );
//				break;
//
//			case LUA_TNONE:
//				 // Intentionally do nothing--not even the line break.
//				return;
//
//			case LUA_TNUMBER:
//
//				ZLLogF ( ZLLog::CONSOLE, format, tvalue, "number", name );
//				ZLLogF ( ZLLog::CONSOLE, " = %f", lua_tonumber ( state, idx ));
//				break;
//
//			case LUA_TSTRING:
//
//				ZLLogF ( ZLLog::CONSOLE, format, rawtsvalue( tvalue ), "string", name );
//				ZLLogF ( ZLLog::CONSOLE, " = \"%s\"", lua_tostring ( state, idx ));
//				break;
//
//			case LUA_TTABLE: {
//
//				struct Table* htable = hvalue( tvalue );
//
//				if ( foundTables.contains ( htable )) {
//
//					ZLLogF ( ZLLog::CONSOLE, DUMP_FORMAT " (see above)", htable, "table", name );
//					break;
//				}
//				else {
//
//					foundTables.insert ( htable );
//
//					ZLLogF ( ZLLog::CONSOLE, format, htable, "table", name );
//
//					if ( verbose ) {
//
//						ZLLogF ( ZLLog::CONSOLE, "\n" );
//						lua_pushnil ( state );
//
//						while ( lua_next ( state, idx ) ) {
//
//							STLString elementName( name );
//							elementName.append ( "." );
//							elementName.append ( lua_tostring ( state, -2 ));
//							_dumpType ( state, -1, elementName.c_str (), verbose, foundTables );
//							lua_pop ( state, 1 );
//						}
//					}
//				}
//			}
//				return; // suppress newline
//
//			case LUA_TTHREAD:
//
//				ZLLogF ( ZLLog::CONSOLE, format, thvalue( tvalue ), "thread", name );
//				break;
//
//			case LUA_TUSERDATA:
//
//				if ( lua_islightuserdata ( state, idx ) ) {
//					
//					ZLLogF ( ZLLog::CONSOLE, format, lua_topointer ( state, idx ) , "light userdata", name );
//				}
//				else {
//
//					ZLLogF ( ZLLog::CONSOLE, format, lua_topointer( state, idx ), "userdata", name );
//
//					if ( verbose ) {
//
//						lua_getglobal ( state, "tostring" );
//						lua_pushvalue ( state, idx );
//						
//						lua_pcall ( state, 1, 1, 0 );
//
//						ZLLogF ( ZLLog::CONSOLE, "\n\t%s", lua_tostring ( state, -1 ));
//						state.Pop ( 1 );
//					}
//				}
//				break;
//
//			default:
//				ZLLogF ( ZLLog::CONSOLE, "*** Unexpected type: %d ***", lua_type ( state, idx ));
//		}
//
//		ZLLogF ( ZLLog::CONSOLE, "\n" );
//	}
//
//	//----------------------------------------------------------------//
//	static void _dumpTypeByAddress ( lua_State* L, TValue* tvalue, const char *name, bool verbose, TableSet& foundTables ) {
//
//		MOAIRubyState state ( L );
//		
//		lua_lock ( L );
//		setobj2s ( L, L->top, tvalue );
//		L->top++;
//		lua_unlock ( L );
//
//		_dumpType ( L, -1, name, verbose, foundTables );
//		lua_pop ( L, 1 );
//	}
//
//#endif

//================================================================//
// MOAIRubyRuntime Ruby API
//================================================================//

//----------------------------------------------------------------//
mrb_value MOAIRubyRuntime::_clearRef ( mrb_state* M, mrb_value self ) {
	MOAIRubyState state ( M );

	mrb_value ref = state.GetParamValue ( 1 );
	u32 refType = state.GetParamValue <u32>( 2, MOAIRubyRefTable::WEAK );
	MOAIRubyRuntime::Get ().ClearRef ( ref, refType );

	return self;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyRuntime::_debugCall ( mrb_state* M, mrb_value self ) {

	MOAIRubyState state ( M );

	state.DebugCall ( 1, 0 );

	return self;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyRuntime::_deref ( mrb_state* M, mrb_value self ) {
	/*MOAIRubyState state ( M );

	int ref = state.GetParamValue < int >( 2, false );
	MOAIRubyRuntime::Get ().PushRef ( state, ref );*/

	return self;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyRuntime::_dofile ( mrb_state* M, mrb_value self ) {
	MOAIRubyState state ( M );

	cc8* filename = state.GetParamValue < cc8* >( 1, "" );
	
	if ( !ZLFileSys::CheckFileExists ( filename ) ) {
		ZLLog_ErrorF ( ZLLog::CONSOLE, "Could not find file %s \n", filename );
		return mrb_nil_value ();
	}

	mrb_value ret;
	mrbc_context* context = mrbc_context_new ( M );

	mrbc_filename ( M, context, filename );

	ret = mrb_load_file_cxt ( M, ( ZLFILE* )zl_fgethandle ( zl_fopen ( filename, "rb" ) ), context );
	ret = mrb_load_file ( M, ( ZLFILE* )zl_fgethandle ( zl_fopen ( filename, "rb" ) ) );

	if ( M->exc ) {

		if ( mrb_undef_p ( ret ) ) {
			mrb_p ( M, mrb_obj_value ( M->exc ) );
		}
		else {
			mrb_print_error ( M );
		}

		mrbc_context_free ( M, context );

		return mrb_nil_value ();
	}

	mrbc_context_free ( M, context );

	return ret;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyRuntime::_dump ( mrb_state* M, mrb_value self ) {
	UNUSED ( M );

	/*#if ( LUA_VERSION_NUM < 502 ) && ( !MOAI_WITH_LUAJIT )

		MOAIRubyState state ( L );

		if ( !state.CheckParams ( 1, "S." )) return 0;

		cc8* name = state.GetParamValue < cc8* >( 1, "" );
		bool verbose = state.GetParamValue < bool >( 3, true );

		TableSet foundTables;
		_dumpType ( state, 2, name, verbose, foundTables );

	#endif*/

	return self;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyRuntime::_dumpStack ( mrb_state* M, mrb_value self ) {
	UNUSED ( M );

	/*#if ( LUA_VERSION_NUM < 502 ) && ( !MOAI_WITH_LUAJIT )

		MOAIRubyState state ( L );

		bool verbose = state.GetParamValue < bool >( 1, true );
		int idx = 0;

		TableSet foundTables;
		for ( TValue* tvalue = state->stack; tvalue < state->top; ++tvalue ) {

			ZLLogF ( ZLLog::CONSOLE, "stack [ %d ] ", idx++ );
			_dumpTypeByAddress ( state, tvalue, "", verbose, foundTables );
		}
	#endif*/
	
	return self;
}

//----------------------------------------------------------------//
/**	@lua	forceGC
	@text	Runs the garbage collector repeatedly until no more MOAIObjects
			can be collected.

	@out	nil
*/
mrb_value MOAIRubyRuntime::_forceGC ( mrb_state* M, mrb_value self ) {
	UNUSED ( M );
	MOAIRubyRuntime::Get ().ForceGarbageCollection ();
	return self;
}

//----------------------------------------------------------------//
/**	@lua	getHistogram
	@text	Generates a histogram of active MOAIObjects and returns it
			in a table containing object tallies indexed by object
			class names.

	@opt	string trackingGroup
	@out	table histogram
*/
mrb_value MOAIRubyRuntime::_getHistogram ( mrb_state* M, mrb_value self ) {
	MOAIRubyState state ( M );
	
	cc8* trackingGroup = state.GetParamValue < cc8* >( 1, 0 );
	MOAIRubyRuntime::Get ().PushHistogram ( state, trackingGroup );
	return self;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyRuntime::_getRef ( mrb_state* M, mrb_value self ) {
	MOAIRubyState state ( M );

	u32 type = state.GetParamValue < bool >( 2, false ) ? MOAIRubyRef::MAKE_WEAK : MOAIRubyRef::MAKE_STRONG;
	//state.Push ( MOAIRubyRuntime::Get ().GetRef ( state, 1, type ) );

	return self;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyRuntime::_panic ( mrb_state* M, mrb_value self ) {

	MOAIRubyState state ( M );
	state.LogStackTrace ( ZLLog::LOG_FATAL, ZLLog::CONSOLE, NULL, 1 );
	ZLLog_FatalF ( ZLLog::CONSOLE, "PANIC: unprotected error in call to Ruby API (%s)\n", state.GetParamValue < cc8* >( 0, 0 ) );
	
	return self;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyRuntime::_reportGC ( mrb_state* M, mrb_value self ) {

	MOAIRubyState state ( M );
	MOAIRubyRuntime::Get ().mReportGC = state.GetParamValue < bool >( 1, false );
	return self;
}

//----------------------------------------------------------------//
/**	@lua	reportHistogram
	@text	Generates a histogram of active MOAIObjects.

	@opt	string filename
	@opt	string trackingGroup
	@out	nil
*/
mrb_value MOAIRubyRuntime::_reportHistogram ( mrb_state* M, mrb_value self ) {
	MOAIRubyState state ( M );

	cc8* filename			= state.GetParamValue < cc8* >( 1, 0 );
	cc8* trackingGroup		= state.GetParamValue < cc8* >( 2, 0 );
	
	MOAIRubyRuntime::Get ().ReportHistogram ( filename, trackingGroup );
	
	return self;
}

//----------------------------------------------------------------//
/**	@lua	reportLeaks
	@text	Analyze the currently allocated MOAI objects and create a textual
			report of where they were declared, and what Ruby references (if any)
			can be found. NOTE: This is incredibly slow, so only use to debug
			leaking memory issues.
 
			This will also trigger a full garbage collection before performing
			the required report. (Equivalent of collectgarbage("collect").)
 
	@opt	string filename
	@opt	string trackingGroup
	@out	nil
*/
mrb_value MOAIRubyRuntime::_reportLeaks ( mrb_state* M, mrb_value self ) {
	MOAIRubyState state ( M );
	
	cc8* filename			= state.GetParamValue < cc8* >( 1, 0 );
	cc8* trackingGroup		= state.GetParamValue < cc8* >( 2, 0 );
	
	MOAIRubyRuntime::Get ().ReportLeaksFormatted ( filename, trackingGroup );
	
	return self;
}

//----------------------------------------------------------------//
/**	@lua	setTrackingFlags
	@text	Enable extra memory book-keeping measures that allow all MOAI objects to be
			tracked back to their point of allocation (in Ruby). Use together with
			MOAIRubyRuntime.reportLeaks() to determine exactly where your memory usage is
			being created. NOTE: This is expensive in terms of both CPU and
			the extra memory associated with the stack info book-keeping. Use only
			when tracking down leaks.
 
	@opt	number flags		Bitwise combination of TRACK_OBJECTS and TRACK_OBJECTS_STACK_TRACE. Default value is 0.
	@out	nil
*/
mrb_value MOAIRubyRuntime::_setTrackingFlags ( mrb_state* M, mrb_value self ) {

	MOAIRubyState state ( M );
	MOAIRubyRuntime::Get ().SetTrackingFlags ( state.GetParamValue < u32 >( 1, 0 ) );
	return self;
}

//----------------------------------------------------------------//
mrb_value MOAIRubyRuntime::_traceback ( mrb_state* M, mrb_value self ) {
	
	MOAIRubyRuntime& runtime = MOAIRubyRuntime::Get ();
	MOAIRubyState state ( M );
	
	//cc8* msg = NULL;
	//if ( lua_isstring ( L, 1 )) {  // 'message' a string?
	//	msg = lua_tostring ( L, 1 );
	//}
	//
	//if ( runtime.mTracebackFunc ) {
	//	runtime.mTracebackFunc ( msg, L, 0 );
	//}
	//
	//if ( runtime.mTracebackRef ) {
	//	
	//	if ( runtime.mTracebackRef.PushRef ( state )) {
	//		
	//		lua_pushvalue ( state, 1 );
	//		int result = lua_pcall ( state, 1, 0, 2 );

	//		if ( !result ) return 0;
	//		
	//		ZLLog_ErrorF ( ZLLog::CONSOLE, "error in user supplied traceback func\n" );
	//		ZLLog_ErrorF ( ZLLog::CONSOLE, "falling back on default error handler:\n" );
	//	}
	//}

	//if ( msg ) {
	//	ZLLogF ( ZLLog::CONSOLE, "%s\n", msg );
	//}
	state.LogStackTrace ( ZLLog::LOG_ERROR, ZLLog::CONSOLE, NULL, 1 );

	return self;
}

//----------------------------------------------------------------//
void* MOAIRubyRuntime::_trackingAlloc ( mrb_state* mrb, void* ptr, size_t size, void* ud ) {
	UNUSED ( ud );
	
	if ( MOAIRubyRuntime::IsValid ()) {
		MOAIRubyRuntime& self = MOAIRubyRuntime::Get ();
		
		if ( size == 0 ) {
			//self.mTotalBytes -= osize;
			free ( ptr );
			return NULL;
		}

		if ( self.mAllocLogEnabled ) {
			printf ( "Ruby alloc: %d\n", ( int )size );
		}

		//self.mTotalBytes -= osize;
		self.mTotalBytes += size;
		return realloc ( ptr, size );
	}
	
	if ( size == 0 ) {
		free ( ptr );
		return NULL;
	}

	return realloc ( ptr, size );
}

//================================================================//
// MOAIRubyRuntime
//================================================================//

//----------------------------------------------------------------//
void MOAIRubyRuntime::BuildHistogram ( HistMap& histogram, cc8* trackingGroup ) {

	TrackingMapIt trackingMapIt = this->mTrackingMap.begin ();
	for ( ; trackingMapIt != this->mTrackingMap.end (); ++trackingMapIt ) {
	
		MOAIRubyObjectInfo& info = trackingMapIt->second;
		if ( trackingGroup && ( info.mTrackingGroup.compare ( trackingGroup ) != 0 )) continue;
	
		MOAIRubyObject* obj = trackingMapIt->first;
		cc8* name = obj->TypeName ();
	
		if ( !histogram.contains ( name )) {
			histogram [ name ] = 1;
		}
		else {
			histogram [ name ]++;
		}
	}
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::CacheUserdata ( MOAIRubyState& state, int idx ) {

	/*idx = state.AbsIndex ( idx );

	if ( this->mUserdataCache ) {
		if ( !this->mUserdataCache.PushRef ( state )) {
			state.Pop ();
		}
	}

	if ( !this->mUserdataCache ) {
		lua_newtable ( state );
		this->mUserdataCache.SetRef ( state, -1 );
	}

	lua_pushvalue ( state, idx );
	lua_pushvalue ( state, idx );
	lua_settable ( state, -3 );

	state.Pop ();*/
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::ClearRef ( const mrb_value& v, u32 refType ) {

	if ( !mrb_nil_p ( v ) ) {
		if ( refType == MOAIRubyRefTable::WEAK ) {
			this->mWeakRefs.Unref ( v );
		}
		else {
			this->mStrongRefs.Unref ( v );
		}
	}
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::Close () {

	if ( this->mState ) {
		mrb_close ( this->mState );
		this->mState = 0;
	}
	this->InvalidateContext ();
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::DeregisterObject ( MOAIRubyObject& object ) {

	if ( this != &object ) {
		this->mObjectCount--;
		this->mTrackingMap.erase ( &object );
	}
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::FindRubyRefs ( mrb_state* M, FILE* file, cc8* trackingGroup, MOAIRubyTraversalState& traversalState ) {

	//MOAIRubyState state ( M );

	//traversalState.mTraversalStack.clear ();
	//traversalState.mTraversalSet.clear ();
	//traversalState.mIgnoreSet.clear ();
	//traversalState.mPathMap.clear ();

	//traversalState.mIgnoreTraversed = false;
	//
	//// check all the globals
	//lua_pushglobaltable ( state );
	//this->FindRubyRefs ( state, -1, file, "_G", trackingGroup, traversalState );
	//state.Pop ( 1 );
	//
	//traversalState.mIgnoreTraversed = true;
	//
	//// check the stack
	//this->FindRubyRefs ( state, file, "<caller>", trackingGroup, traversalState );
	//
	//// print the strong refs
	//this->mStrongRefs.PushRefTable ( state );
	//this->FindRubyRefs ( state, -1, file, "<STRONG REFS>", trackingGroup, traversalState );
	//state.Pop ( 1 );
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::FindRubyRefs ( mrb_state* M, FILE* file, STLString path, cc8* trackingGroup, MOAIRubyTraversalState& traversalState ) {

	/*MOAIRubyState state ( M );

	lua_Debug ar;
	for ( int level = 0; lua_getstack ( state, level, &ar ) != 0; ++level ) {

		STLString stackPath = STLString::build ( "%s[stack: %d]", path.c_str (), level );

		cc8* localname;
		for ( int i = 1; ( localname = lua_getlocal ( state, &ar, i )) != NULL; ++i ) {
			STLString localPath = STLString::build ( "%s.%s", stackPath.c_str (), localname );
			this->FindRubyRefs ( state, -1, file, localPath.c_str (), trackingGroup, traversalState );
			state.Pop ( 1 );
		}

		lua_getinfo ( state, "f", &ar );
		this->FindRubyRefs ( state, -1, file, STLString::build ( "%s.<func>", path.c_str ()), trackingGroup, traversalState );
		state.Pop ( 1 );
	}*/
}

//----------------------------------------------------------------//
// This monstrosity will walk through all tables and functions accessible in the
// current lua state and update the reference line for each one found to help
// track who is pointing to it.
void MOAIRubyRuntime::FindRubyRefs ( mrb_state* M, int idx, FILE* file, STLString path, cc8* trackingGroup, MOAIRubyTraversalState& traversalState ) {

	//M = M ? M : ( mrb_state* )this->mState;
	//MOAIRubyState state ( M );
	//idx = state.AbsIndex ( idx );
	//
	//int type = lua_type ( state, idx );
	//
	//switch ( type ) {
	//	case LUA_TNIL:
	//	case LUA_TBOOLEAN:
	//	case LUA_TLIGHTUSERDATA:
	//	case LUA_TNUMBER:
	//	case LUA_TSTRING:
	//		return;
	//	default:
	//		break;
	//}
	//
	//lua_checkstack ( state, 255 );
	//
	//if ( type == LUA_TUSERDATA ) {
	//	MOAIRubyObject* object = MOAIRubyObject::IsMoaiUserdata ( state, idx ) ? ( MOAIRubyObject* )state.GetPtrUserData ( idx ) : NULL;
	//	if ( object && this->mTrackingMap.contains ( object )) {
	//		MOAIRubyObjectInfo& objectInfo = this->mTrackingMap [ object ];
	//		if ( !trackingGroup || ( objectInfo.mTrackingGroup.compare ( trackingGroup ) != 0 )) {
	//			MOAIRubyTraversalState::StringSet& pathSet = traversalState.mPathMap [ object ];
	//			pathSet.affirm ( path );
	//			//printf ( "found: %s\n", path.c_str ());
	//		}
	//	}
	//}
	//
	//// bail if we've already iterated into this item
	//const void* addr = lua_topointer ( state, idx );
	//
	//if ( traversalState.mIgnoreSet.contains ( addr )) return; // always ignore
	//if ( traversalState.mTraversalStack.contains ( addr )) return;
	//if ( traversalState.mIgnoreTraversed && ( traversalState.mTraversalSet.contains ( addr ))) return;
	//
	//traversalState.mTraversalStack.affirm ( addr );
	//traversalState.mTraversalSet.affirm ( addr );
	//
	////printf ( "%d %s\n", idx, path.c_str ());
	//
	//switch ( type ) {
	//
	//	// how to deal with weak tables?
	//	case LUA_TTABLE: {
	//		
	//		u32 itr = state.PushTableItr ( idx );
	//		while ( state.TableItrNext ( itr )) {
	//			
	//			STLString keyPath = path;
	//			
	//			int keyIdx = state.AbsIndex ( -2 );
	//			int valIdx = state.AbsIndex ( -1 );

	//			int keyType = lua_type ( state, keyIdx );
	//			cc8* keyName = lua_tostring ( state, keyIdx );
	//			
	//			// update the path and follow the keys (if iterable)
	//			switch ( keyType ) {
	//				
	//				case LUA_TLIGHTUSERDATA:
	//					keyPath.write ( "[%p]", lua_topointer ( state, keyIdx ));
	//					break;
	//				
	//				case LUA_TBOOLEAN:
	//				case LUA_TNUMBER:
	//					keyPath.write ( "[%s]", keyName );
	//					break;
	//				
	//				case LUA_TSTRING:
	//					if ( MOAIRubyRuntime::IsRubyIdentifier ( keyName )) {
	//						keyPath.write ( ".%s", keyName );
	//					}
	//					else {
	//						keyPath.write ( "[\'%s\']", keyName );
	//					}
	//					break;
	//				
	//				default: {
	//					
	//					STLString keyTypeName;
	//					const void* keyAddr = lua_topointer ( state, keyIdx );
	//				
	//					switch ( keyType ) {
	//					
	//						case LUA_TFUNCTION:
	//							keyTypeName = "function";
	//							break;
	//						
	//						case LUA_TTABLE:
	//							keyTypeName = "table";
	//							break;
	//						
	//						case LUA_TTHREAD:
	//							keyTypeName = "thread";
	//							break;
	//						
	//						case LUA_TUSERDATA:
	//							
	//							if ( MOAIRubyObject::IsMoaiUserdata ( state, keyIdx )) {
	//								MOAIRubyObject* object = state.GetRubyObject < MOAIRubyObject >( keyIdx, false );
	//								keyTypeName = object->TypeName ();
	//								keyAddr = object;
	//							}
	//							else {
	//								keyTypeName = "userdata";
	//							}
	//							break;
	//					}
	//					
	//					keyPath.write ( "[%s: %p]", keyTypeName.c_str (), keyAddr );
	//					this->FindRubyRefs ( state, keyIdx, file, STLString::build ( "%s(as key)", keyPath.c_str ()), trackingGroup, traversalState );
	//				}
	//			}
	//			
	//			// follow the values
	//			this->FindRubyRefs ( state, valIdx, file, keyPath, trackingGroup, traversalState );
	//		}
	//		break;
	//	}
	//	
	//	case LUA_TFUNCTION: {
	//		
	//		cc8* upname;
	//		for ( int i = 1; ( upname = lua_getupvalue ( state, idx, i )) != NULL; ++i ) {
	//			STLString upPath = STLString::build ( "%s.<upvalue: ", path.c_str ());
	//			
	//			if ( upname [ 0 ]) {
	//				upPath.write ( "%s>", upname );
	//			}
	//			else {
	//				upPath.write ( "%d>", i );
	//			}
	//			
	//			this->FindRubyRefs ( state, -1, file, upPath.c_str (), trackingGroup, traversalState );
	//			state.Pop ( 1 );
	//		}
	//		
	//		lua_getfenv ( state, idx );
	//		this->FindRubyRefs ( state, -1, file, STLString::build ( "%s.<env: %p>", path.c_str (), lua_topointer ( state, -1 )), trackingGroup, traversalState );
	//		traversalState.mIgnoreSet.affirm ( lua_topointer ( state, -1 ));
	//		state.Pop ( 1 );
	//		break;
	//	}
	//	
	//	case LUA_TTHREAD: {
	//		
	//		STLString threadPath = STLString::build ( "%s.<thread>", path.c_str ());
	//		MOAIRubyState threadState ( lua_tothread ( state, idx ));
	//		this->FindRubyRefs ( threadState, file, threadPath, trackingGroup, traversalState );
	//		break;
	//	}
	//}
	//
	//// follow the metatable
	//if (( type == LUA_TTABLE ) || ( type == LUA_TUSERDATA )) {
	//
	//	MOAIRubyObject* object = MOAIRubyObject::IsMoaiUserdata ( state, idx ) ? ( MOAIRubyObject* )state.GetPtrUserData ( idx ) : NULL;
	//	
	//	if ( object ) {
	//		
	//		// iterate the member table *first*
	//		object->PushMemberTable ( state );
	//		this->FindRubyRefs ( state, -1, file, STLString::build ( "%s.<member>", path.c_str ()), trackingGroup, traversalState );
	//		state.Pop ( 1 );
	//		
	//		object->PushRefTable ( state );
	//		this->FindRubyRefs ( state, -1, file, STLString::build ( "%s.<ref>", path.c_str ()), trackingGroup, traversalState );
	//		state.Pop ( 1 );
	//	}
	//	else {
	//		if ( lua_getmetatable ( state, idx )) {
	//			this->FindRubyRefs ( state, -1, file, STLString::build ( "%s.<meta: 0x%p>", path.c_str (), lua_topointer ( state, -1 )), trackingGroup, traversalState );
	//			state.Pop ( 1 );
	//		}
	//	}
	//}
	//
	//traversalState.mTraversalStack.erase ( addr );
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::ForceGarbageCollection () {

	this->PurgeUserdataCache ();

	mrb_state* mrb = this->mState;

	// Make sure that anything that can be collected, is. Note: we collect
	// more than once because of this scary snippet:
	//   "When Ruby collects a full userdata with a gc metamethod, Ruby
	//    calls the metamethod and marks the userdata as finalized. When
	//    this userdata is collected again then Ruby frees its corresponding
	//    memory."
	
	// collect until no more changes in memory use or object count are registered
	bool more = true;
	while ( more ) {
	
		size_t b0 = this->mTotalBytes;
		size_t c0 = this->mObjectCount;
		
		mrb_garbage_collect ( mrb );
		
		size_t b1 = this->mTotalBytes;
		size_t c1 = this->mObjectCount;
		
		/*if (( b0 == b1 ) && ( c0 == c1 )) {
			more = false;
		}*/

		more = false;
	}
}

//----------------------------------------------------------------//
size_t MOAIRubyRuntime::GetMemoryUsage() {
	return this->mTotalBytes;
}

//----------------------------------------------------------------//
bool MOAIRubyRuntime::IsMainThread ( mrb_state* mrb ) {

	return ( this->mState == mrb );
}

//----------------------------------------------------------------//
MOAIRubyState& MOAIRubyRuntime::GetMainState () {
	return this->mState;
}

//----------------------------------------------------------------//
//MOAIRubyRef& MOAIRubyRuntime::GetRef ( MOAIRubyState& state, const mrb_value& v, u32 type ) {
//	
//	if ( state.IsNil ( v ) ) return MOAIRubyRef ();
//	return ( type == MOAIRubyRefTable::WEAK ) ? this->mWeakRefs.Ref ( state, v ) : this->mStrongRefs.Ref ( state, v );
//}

//----------------------------------------------------------------//
bool MOAIRubyRuntime::IsRubyIdentifier ( const char *str ) {
	const char *p = str;
	while ( *p != '\0' && ( isalnum(*p) || *p == '_' )) {
		p++;
	}
	return p > str && *p == '\0';
}

//----------------------------------------------------------------//
bool MOAIRubyRuntime::IsOpen () {

	return this->mState;
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::LoadLibs () {
}

//----------------------------------------------------------------//
int MOAIRubyRuntime::MakeStrong ( int refID ) {

	/*if ( refID == LUA_NOREF ) return LUA_NOREF;
	if ( !( refID & WEAK_REF_BIT )) return refID;

	this->mWeakRefs.PushRef ( this->mState, refID );
	this->mWeakRefs.Unref ( refID & REF_MASK );

	refID = this->GetRef ( this->mState, refID, false );
	this->mState.Pop ();*/
	
	return refID;
}

//----------------------------------------------------------------//
int MOAIRubyRuntime::MakeWeak ( int refID ) {

	/*if ( refID == LUA_NOREF ) return LUA_NOREF;
	if ( refID & WEAK_REF_BIT ) return refID;

	this->mStrongRefs.PushRef ( this->mState, refID );
	this->mStrongRefs.Unref ( refID & REF_MASK );

	refID = this->GetRef ( this->mState, refID, true );
	this->mState.Pop ();*/
	
	return refID;
}

//----------------------------------------------------------------//
MOAIRubyRuntime::MOAIRubyRuntime () :
	mTrackingFlags ( 0 ),
	mTracebackFunc ( 0 ),
	mTotalBytes ( 0 ),
	mObjectCount ( 0 ),
	mAllocLogEnabled ( false ),
	mReportGC ( false ) {
}

//----------------------------------------------------------------//
MOAIRubyRuntime::~MOAIRubyRuntime () {
	this->Close ();
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::OnGlobalsFinalize () {
}

//----------------------------------------------------------------//
MOAIRubyState& MOAIRubyRuntime::Open () {

	if ( this->IsOpen () ) {
		this->Close ();
	}

	// open the main state
	#if ( MOAI_WITH_RUBYJIT && (defined(__x86_64 ) || defined(__amd64)) )
		this->mState = mrb_open ();  //jit doesn't support custom allocs on 64bit
	#else
		this->mState = mrb_open_allocf ( _trackingAlloc, NULL );
	#endif
	
	// TODO: mruby at fatal function.
	//lua_atpanic ( this->mState, &_panic );

	// set up the ref tables
	this->mStrongRefs.InitStrong ();
	this->mWeakRefs.InitWeak ();
	
	return this->mState;
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::PurgeUserdata ( MOAIRubyState& state, int idx ) {

	/*idx = state.AbsIndex ( idx );

	if ( this->mUserdataCache.PushRef ( state )) {

		lua_pushvalue ( state, idx );
		lua_pushnil ( state );
		lua_settable ( state, -3 );
	}
	state.Pop ();*/
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::PurgeUserdataCache () {

	this->mUserdataCache.Clear ();
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::PushHistogram ( MOAIRubyState& state, cc8* trackingGroup  ) {
	
	/*lua_newtable ( state );

	HistMap histogram;
	this->BuildHistogram ( histogram, trackingGroup );

	HistMap::iterator histogramIt = histogram.begin ();
	for ( ; histogramIt != histogram.end (); ++histogramIt ) {

		const STLString& name = histogramIt->first;
		size_t count = histogramIt->second;

		lua_pushstring ( state, name );
		lua_pushnumber ( state, ( lua_Number )count );
		lua_settable ( state, -3 );
	}*/
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::PrintTracking ( MOAIRubyObject& object ) {

	if ( this->mTrackingMap.contains ( &object )) {
	
		ZLLogF ( ZLLog::CONSOLE, "Object <%p> created at:\n", &object );
		ZLLogF ( ZLLog::CONSOLE, "%s", this->mTrackingMap [ &object ].mStackTrace.c_str ());
	}
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "TRACK_OBJECTS",				( u32 )TRACK_OBJECTS );
	state.DefineClassConst ( klass, "TRACK_OBJECTS_STACK_TRACE",	( u32 )TRACK_OBJECTS_STACK_TRACE );

	state.DefineStaticMethod ( klass, "clearRef", _clearRef, MRB_ARGS_NONE () );
	//state.DefineStaticMethod ( klass, "debugCall", _debugCall, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "deref", _deref, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "dump", _dump, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "dumpStack", _dumpStack, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "forceGC", _forceGC, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getHistogram", _getHistogram, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getRef", _getRef, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "reportGC", _reportGC, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "reportHistogram", _reportHistogram, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "reportLeaks", _reportLeaks, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "setTrackingFlags", _setTrackingFlags, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "traceback", _traceback, MRB_ARGS_NONE () );

	mrb_define_method ( state, state->kernel_module, "dofile", _dofile, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAIRubyRuntime::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::RegisterModule ( cc8* name, lua_CFunction loader, bool autoLoad ) {

	//this->mState.RegisterModule ( loader, name, autoLoad );
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::Ref ( const mrb_value& v ) {

	if ( this->mState.IsNil ( v ) ) return;
	this->mStrongRefs.Ref ( this->mState, v );
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::RegisterObject ( MOAIRubyObject& object ) {

	if ( this != &object ) {

		this->mObjectCount++;
		
		if ( this->mTrackingFlags & TRACK_OBJECTS ) {
		
			MOAIRubyObjectInfo& info = this->mTrackingMap [ &object ];
			info.mTrackingGroup = this->mTrackingGroup;
		}
	}
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::RegisterObject ( MOAIRubyState& state, MOAIRubyObject& object ) {

	if ( this != &object ) {
	
		if ( this->mTrackingFlags & TRACK_OBJECTS_STACK_TRACE ) {
		
			MOAIRubyObjectInfo& info = this->mTrackingMap [ &object ];
			info.mStackTrace = state.GetStackTrace ( "object allocation:", 1 );
		}
	}
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::ReportHistogram ( cc8* filename, cc8* trackingGroup ) {
	
	FILE* file = 0;
	if ( filename ) {
		file = fopen ( filename, "w" );
		assert ( file );
	}
	
	size_t totalTracked = this->mTrackingMap.size ();
	ZLLogF ( file, "tracking %d of %d allocated MOAIObjects\n", ( int )totalTracked, ( int )this->mObjectCount );
	
	HistMap histogram;
	this->BuildHistogram ( histogram, trackingGroup );
	
	HistMap::iterator histogramIt = histogram.begin ();
	for ( ; histogramIt != histogram.end (); ++histogramIt ) {
	
		const STLString& name = histogramIt->first;
		size_t count = histogramIt->second;
		float percent = (( float )count / ( float )totalTracked ) * 100.0f;
	
		ZLLogF ( file, "%-32.32s %d (%.2f%% of %d)\n", name.str (), ( int )count, percent, ( int )totalTracked );
	}
	
	if ( file ) {
		fclose ( file );
	}
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::ReportLeaksFormatted ( cc8* filename, cc8* trackingGroup ) {

	FILE* file = 0;
	if ( filename ) {
		file = fopen ( filename, "w" );
		assert ( file );
	}

	MOAIRubyState& state = this->mState;
	
	// Update Ruby references for all tracked objects
	MOAIRubyTraversalState traversalState;
	//this->FindRubyRefs ( state, file, trackingGroup, traversalState );
	
	// Correlate leaks by identical stack traces.
	LeakStackMap stacks;
	
	for ( TrackingMapConstIt i = this->mTrackingMap.begin (); i != this->mTrackingMap.end (); ++i ) {
		const MOAIRubyObjectInfo& info = i->second;
		if ( trackingGroup && ( info.mTrackingGroup.compare ( trackingGroup ) != 0 )) continue;
		
		ObjectSet& objectSet = stacks [ info.mStackTrace ];
		objectSet.affirm ( i->first );
	}
	
	ZLLogF ( file, "------------------------------------------------\n" );
	ZLLogF ( file, "-- BEGIN LUA OBJECT LEAKS --\n" );
	
	// Then, print out each unique allocation spot along with all references
	// (including multiple references) followed by the alloction stack
	//int top = state.GetTop ();
	int top = -1;
	UNUSED ( top );
	
	for ( LeakStackMapIt i = stacks.begin (); i != stacks.end (); ++i ) {
		
		const ObjectSet& objectSet = i->second;
		ZLLogF ( file, "Allocation: %lu\n", objectSet.size ());
		
		for ( ObjectSetConstIt objectSetIt = objectSet.begin (); objectSetIt != objectSet.end (); ++objectSetIt ) {
			const MOAIRubyObject* object = *objectSetIt;
			ZLLogF ( file, "<%s> %p\n", object->TypeName (), object );
			
			if ( traversalState.mPathMap.contains ( object )) {
				MOAIRubyTraversalState::StringSet& pathSet = traversalState.mPathMap [ object ];
				for ( MOAIRubyTraversalState::StringSetIt j = pathSet.begin (); j != pathSet.end (); ++j ) {
					ZLLogF ( file, "path: %s\n", j->c_str ());
				}
			}
		}
		
		// print the stack trace
		ZLLogF ( file, i->first.c_str () );
		ZLLogF ( file, "\n" );
	}
	
	//assert ( top == state.GetTop () );
	
	ZLLogF ( file, "-- END LUA LEAKS --\n" );
	
	if ( file ) {
		fclose ( file );
	}
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::ReportLeaksRaw ( cc8* filename, cc8* trackingGroup ) {

	FILE* file = 0;
	if ( filename ) {
		file = fopen ( filename, "w" );
		assert ( file );
	}

	this->ForceGarbageCollection ();
	
	ZLLogF ( file, "-- LUA OBJECT LEAK REPORT ------------\n" );
	u32 count = 0;
	
	for ( TrackingMap::const_iterator i = this->mTrackingMap.begin () ; i != this->mTrackingMap.end (); ++i ) {
		const MOAIRubyObjectInfo& info = i->second;
		if ( trackingGroup && ( info.mTrackingGroup.compare ( trackingGroup ) != 0 )) continue;
		ZLLogF ( file, info.mStackTrace.c_str ());
		count++;
	}
	ZLLogF ( file, "-- END LEAK REPORT (Total Objects: %d) ---------\n", count );
	
	if ( file ) {
		fclose ( file );
	}
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::ResetTracking () {

	this->mTrackingMap.clear ();
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::SetPath ( cc8* path ) {

	/*int top = lua_gettop ( this->mState );

	lua_getglobal ( this->mState, "package" );
	int packageIdx = lua_gettop ( this->mState );

	lua_pushstring ( this->mState, "path" );
	lua_pushstring ( this->mState, path );
	lua_settable ( this->mState, packageIdx );

	lua_settop ( this->mState, top );*/

	// TODO: Set $LOAD_PATH

}

//----------------------------------------------------------------//
void MOAIRubyRuntime::SetTrackingFlags ( u32 flags ) {

	this->mTrackingFlags = flags;
	
	if ( !flags ) {
		this->ResetTracking ();
	}
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::SetTrackingGroup () {

	this->mTrackingGroup.clear ();
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::SetTrackingGroup ( const STLString& trackingGroup ) {

	this->mTrackingGroup = trackingGroup;
}

//----------------------------------------------------------------//
MOAIRubyState MOAIRubyRuntime::State () {

	return this->mState;
}

//----------------------------------------------------------------//
void MOAIRubyRuntime::Unref ( const mrb_value& v ) {

	if ( this->mState.IsNil ( v ) ) return;
	this->mStrongRefs.Unref ( v );
}