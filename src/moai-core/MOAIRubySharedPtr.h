// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIRUBYSHAREDPTR_H
#define	MOAIRUBYSHAREDPTR_H

#include <moai-core/MOAIRubyObject.h>

//================================================================//
// MOAIRubySharedPtr
//================================================================//
template < typename TYPE >
class MOAIRubySharedPtr {
protected:

	TYPE*			mObject;
	
	//----------------------------------------------------------------//
	inline TYPE* Get () {
		return this->mObject;
	}
	
	//----------------------------------------------------------------//
	inline const TYPE* Get () const {
		return this->mObject;
	}

public:

	//----------------------------------------------------------------//
	inline operator bool () const {
		return this->Get () != 0;
	};

	//----------------------------------------------------------------//
	inline TYPE& operator * () {
		return *this->Get ();
	};

	//----------------------------------------------------------------//
	inline const TYPE& operator * () const {
		return *this->Get ();
	};

	//----------------------------------------------------------------//
	inline TYPE* operator -> () {
		return this->Get ();
	};

	//----------------------------------------------------------------//
	inline const TYPE* operator -> () const {
		return this->Get ();
	};

	//----------------------------------------------------------------//
	inline operator TYPE* () {
		return this->Get ();
	};

	//----------------------------------------------------------------//
	inline operator const TYPE* () const {
		return this->Get ();
	};
	
	//----------------------------------------------------------------//
	inline void operator = ( const MOAIRubySharedPtr < TYPE >& assign ) {
		assert ( !this->mObject );
		this->mObject = assign.mObject;
		(( MOAIRubySharedPtr < TYPE >& )assign ).mObject = 0;
	};

	//----------------------------------------------------------------//
	inline bool operator == ( const MOAIRubySharedPtr < TYPE >& other ) const {
		return this->mObject == other.mObject;
	}
	
	//----------------------------------------------------------------//
	inline bool operator != ( const MOAIRubySharedPtr < TYPE >& other ) const {
		return this->mObject != other.mObject;
	}

	//----------------------------------------------------------------//
	/*inline bool PushRef ( MOAIRubyState& state ) {

		if ( this->mObject ) {
			state.Push ( this->mObject );
		}
		else {
			lua_pushnil ( state );
		}
		return !lua_isnil ( state, -1 );
	}*/

	//----------------------------------------------------------------//
	inline void Set ( MOAIRubyObject& owner, TYPE* assign ) {

		if ( this->mObject != assign ) {
			owner.RubyRetain ( assign );
			owner.RubyRelease ( this->mObject );
			this->mObject = assign;
		}
	}

	//----------------------------------------------------------------//
	MOAIRubySharedPtr () :
		mObject ( 0 ) {
	}
	
	//----------------------------------------------------------------//
	~MOAIRubySharedPtr () {
		assert ( !this->mObject ); // must be manually cleared before destruction; use Set ( owner, 0 )
	}
	
	//----------------------------------------------------------------//
	MOAIRubySharedPtr ( const MOAIRubySharedPtr < TYPE >& assign ) :
		mObject ( assign.mObject ) {
		(( MOAIRubySharedPtr < TYPE >& )assign ).mObject = 0;
	};
};

#endif
