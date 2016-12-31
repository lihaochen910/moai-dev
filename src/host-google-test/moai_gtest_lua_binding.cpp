//----------------------------------------------------------------//
// Copyright (c) 2010-2011 Zipline Games, Inc. 
// All Rights Reserved. 
// http://getmoai.com
//----------------------------------------------------------------//

#include <moai-sim/headers.h>
#include "moai_gtest.h"

//================================================================//
// GTESTMoaiLuaBoundClass
//================================================================//
class GTESTMoaiLuaBoundClass :
	public MOAILuaObject {
public:

	typedef STLMap < STLString, GTESTMoaiLuaBoundClass* > MemberTable;
	
	MemberTable*		mMembership;
	STLString			mName;

	DECL_LUA_FACTORY ( GTESTMoaiLuaBoundClass )

	//----------------------------------------------------------------//
	GTESTMoaiLuaBoundClass () :
		mMembership ( 0 ) {
	
		RTTI_SINGLE ( MOAILuaObject )
	}
	
	//----------------------------------------------------------------//
	~GTESTMoaiLuaBoundClass () {
	
		if ( this->mMembership ) {
			this->mMembership->erase ( this->mName );
		}
	}
	
	//----------------------------------------------------------------//
	void Track () {
	
		this->mMembership = 0;
		this->mName.clear ();
	}
	
	//----------------------------------------------------------------//
	void Track ( STLMap < STLString, GTESTMoaiLuaBoundClass* >& membership, cc8* name ) {
	
		this->mMembership = &membership;
		this->mName = name;
		membership [ this->mName ] = this;
	}
};

//----------------------------------------------------------------//
TEST_F ( GTESTMoaiContext, MoaiTestLuaBindings ) {
	ASSERT_TRUE ( this->mContext != 0 );

	REGISTER_LUA_CLASS ( GTESTMoaiLuaBoundClass )

	GTESTMoaiLuaBoundClass::MemberTable memberTable;

	{
		GTESTMoaiLuaBoundClass test;
		test.Track ( memberTable, "test" );
		ASSERT_FALSE ( test.IsBound ());
	}
	ASSERT_FALSE ( memberTable.contains ( "test" ));
	
	{
		GTESTMoaiLuaBoundClass test;
		test.Track ( memberTable, "test" );
		
		MOAIScopedLuaState state = MOAILuaRuntime::Get ().State ();
		state.Push ( &test );
		
		ASSERT_TRUE ( test.IsBound ());
	}
	ASSERT_FALSE ( memberTable.contains ( "test" ));
	
	{
		MOAIScopedLuaState state = MOAILuaRuntime::Get ().State ();
	
		{
			GTESTMoaiLuaBoundClass test;
			test.Track ( memberTable, "test" );
			state.Push ( &test );
		}
		ASSERT_FALSE ( memberTable.contains ( "test" ));
		
		MOAILuaRuntime::Get ().ForceGarbageCollection ();
		
		state.Pop ();
	}
}
