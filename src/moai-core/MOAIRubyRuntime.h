// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIRUBYRUNTIME_H
#define MOAIRUBYRUNTIME_H

#include <moai-core/MOAIRubyClass.h>
#include <moai-core/MOAIRubyObject.h>
#include <moai-core/MOAIRubyRef.h>
#include <moai-core/MOAIRubyState.h>
//#include <moai-core/MOAIScopedRubyState.h>

class MOAIRubyObject;

//================================================================//
// MOAIRubyObjectInfo
//================================================================//
class MOAIRubyObjectInfo {
private:

	friend class MOAIRubyRuntime;
	
	STLString	mTrackingGroup;
	STLString	mStackTrace;
};

//================================================================//
// MOAIRubyTraversalState
//================================================================//
class MOAIRubyTraversalState {
private:

	friend class MOAIRubyRuntime;
	
	typedef STLSet < STLString >							StringSet;
	typedef StringSet::iterator								StringSetIt;

	typedef STLSet < const void* >							TraversalSet;
	typedef TraversalSet::iterator							TraversalSetIt;

	typedef STLMap < const MOAIRubyObject*, StringSet >		ObjectPathMap;
	typedef ObjectPathMap::iterator							ObjectPathMapIt;

	TraversalSet		mTraversalStack; // to hold traversed objects
	TraversalSet		mTraversalSet; // all traversed objects
	TraversalSet		mIgnoreSet; // to hold ignored objects
	ObjectPathMap		mPathMap; // sets of Ruby reference paths to objects
	
	bool				mIgnoreTraversed;
};

//================================================================//
// MOAIRubyRuntime
//================================================================//
class MOAIRubyRuntime :
	public ZLContextClass < MOAIRubyRuntime, MOAIRubyObject > {
public:

	typedef void ( *TracebackFunc ) ( const char* message, struct mrb_state* M, int level );

private:

	static const u32 WEAK_REF_BIT	= 0x80000000;
	static const u32 REF_MASK		= 0x7fffffff;

	typedef STLSet < MOAIRubyObject* >						ObjectSet;
	typedef ObjectSet::iterator								ObjectSetIt;
	typedef ObjectSet::const_iterator						ObjectSetConstIt;

	typedef STLMap < MOAIRubyObject*, MOAIRubyObjectInfo >	TrackingMap;
	typedef TrackingMap::iterator							TrackingMapIt;
	typedef TrackingMap::const_iterator						TrackingMapConstIt;
	
	typedef STLArray < MOAIRubyObject* >					LeakPtrList;
	typedef LeakPtrList::iterator							LeakPtrListIt;
	
	typedef STLMap < STLString, ObjectSet >					LeakStackMap;
	typedef LeakStackMap::iterator							LeakStackMapIt;
	
	typedef STLMap < STLString, size_t >					HistMap;

	STLString			mTrackingGroup;
	u32					mTrackingFlags;
	TrackingMap			mTrackingMap;

	MOAIRubyRefTable	mStrongRefs;
	MOAIRubyRefTable	mWeakRefs;

	// this is to handle the edge case where an object gets created and bound to Ruby, but is not
	// passed back to Ruby immediately. the binding's mUserdata is a weak ref, so if additional
	// work is done in Ruby, there is a slight chance it may get tagged for garbage collection
	// in an indeterminate way.
	
	// the cache gets thrown out every update by MOAISim.
	MOAIRubyStrongRef	mUserdataCache;

	TracebackFunc		mTracebackFunc;			// Custom traceback func
	MOAIRubyStrongRef	mTracebackRef;			// Custom traceback ref
	MOAIRubyState		mState;

	size_t				mTotalBytes;
	size_t				mObjectCount;			// All MOAIObjects, whether or not bound to Ruby

	bool				mAllocLogEnabled;
	bool				mReportGC;

	//----------------------------------------------------------------//
	static mrb_value		_clearRef				( mrb_state* M, mrb_value self );
	static mrb_value		_debugCall				( mrb_state* M, mrb_value self );
	static mrb_value		_deref					( mrb_state* M, mrb_value self );
	static mrb_value		_dofile					( mrb_state* M, mrb_value self );
	static mrb_value		_dump					( mrb_state* M, mrb_value self );
	static mrb_value		_dumpStack				( mrb_state* M, mrb_value self );
	static mrb_value		_forceGC				( mrb_state* M, mrb_value self );
	static mrb_value		_getHistogram			( mrb_state* M, mrb_value self );
	static mrb_value		_getRef					( mrb_state* M, mrb_value self );
	static mrb_value		_panic					( mrb_state* M, mrb_value self );
	static mrb_value		_reportGC				( mrb_state* M, mrb_value self );
	static mrb_value		_reportHistogram		( mrb_state* M, mrb_value self );
	static mrb_value		_reportLeaks			( mrb_state* M, mrb_value self );
	static mrb_value		_setTrackingFlags		( mrb_state* M, mrb_value self );
	static void*			_trackingAlloc			( mrb_state* mrb, void* ptr, size_t size, void* ud );
	static mrb_value		_traceback				( mrb_state* M, mrb_value self );

	//----------------------------------------------------------------//
	void					BuildHistogram			( HistMap& histogram, cc8* trackingGroup );
	void					DeregisterObject		( MOAIRubyObject& object );
	void					FindRubyRefs			( mrb_state* M, FILE* file, cc8* trackingGroup, MOAIRubyTraversalState& traversalState );
	void					FindRubyRefs			( mrb_state* M, FILE* file, STLString path, cc8* trackingGroup, MOAIRubyTraversalState& traversalState );
	void					FindRubyRefs			( mrb_state* M, int idx, FILE* file, STLString path, cc8* trackingGroup, MOAIRubyTraversalState& traversalState );
	//MOAIRubyRef&			GetRef					( MOAIRubyState& state, const mrb_value& v, u32 type );
	static bool				IsRubyIdentifier		( const char *str );
	int						MakeStrong				( int refID );
	int						MakeWeak				( int refID );
	void					OnGlobalsFinalize		();
	void					Ref						( const mrb_value& v );
	void					RegisterObject			( MOAIRubyObject& object );
	void					RegisterObject			( MOAIRubyState& state, MOAIRubyObject& object );
	void					Unref					( const mrb_value& v );

public:

	enum {
		TRACK_OBJECTS				= 0x01,
		TRACK_OBJECTS_STACK_TRACE	= 0x02,
	};

	friend class MOAIRubyObject;
	friend class MOAIRubyRef;
	friend class MOAIRubyStrongRef;
	friend class MOAIRubyWeakRef;
	friend class MOAIRubyState;
	template < typename U > friend class MOAIRubyLocal;

	DECL_RUBY_SINGLETON ( MOAIRubyRuntime )

	GET ( size_t, ObjectCount, mObjectCount )
	GET ( MOAIRubyStrongRef&, TracebackRef, mTracebackRef )
	GET ( u32, TrackingFlags, mTrackingFlags )
	
	GET_SET ( bool, AllocLogEnabled, mAllocLogEnabled )
	GET_SET ( TracebackFunc, TracebackFunc, mTracebackFunc )

	//----------------------------------------------------------------//
	void					CacheUserdata				( MOAIRubyState& state, int idx );
	void					ClearRef					( const mrb_value& v, u32 refType );
	void					Close						();
	void					ForceGarbageCollection		();
	size_t					GetMemoryUsage				();
	MOAIRubyState&			GetMainState				();
	bool					IsMainThread				( mrb_state* mrb );
	bool					IsOpen						();
	void					LoadLibs					();
							MOAIRubyRuntime				();
							~MOAIRubyRuntime			();
	MOAIRubyState&			Open						();
	void					PurgeUserdata				( MOAIRubyState& state, int idx );
	void					PurgeUserdataCache			();
	void					PushHistogram				( MOAIRubyState& state, cc8* trackingGroup );
	void					PrintTracking				( MOAIRubyObject& object );
	void					RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void					RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void					RegisterModule				( cc8* name, lua_CFunction loader, bool autoLoad );
	void					ReportHistogram				( cc8* filename, cc8* trackingGroup );
	void					ReportLeaksFormatted		( cc8* filename, cc8* trackingGroup );
	void					ReportLeaksRaw				( cc8* filename, cc8* trackingGroup );
	void					ResetTracking				();
	void					SetPath						( cc8* path );
	void					SetTrackingFlags			( u32 flags );
	void					SetTrackingGroup			();
	void					SetTrackingGroup			( const STLString& trackingGroup );
	//MOAIScopedRubyState	State						();
	MOAIRubyState			State						();
};

#endif
