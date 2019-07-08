// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-core/MOAIRubyClass.h>
#include <moai-core/MOAIRubyRefTable.h>
#include <moai-core/MOAIRubyRuntime.h>

//================================================================//
// MOAIRubyRefTable
//================================================================//

//----------------------------------------------------------------//
void MOAIRubyRefTable::Clear () {

	if ( !mrb_nil_p ( this->mTable ) ) {

		MOAIRubyState& state = MOAIRubyRuntime::Get ().GetMainState ();

		mrb_hash_clear ( state, this->mTable );
	}

	this->mRefIDStack.Clear ();
	this->mRefIDStackTop = 0;
}

//----------------------------------------------------------------//
void MOAIRubyRefTable::InitStrong () {

	MOAIRubyState& state = MOAIRubyRuntime::Get ().GetMainState ();

	// create the hash
	this->mType = STRONG;
	this->mTable = mrb_hash_new ( state );

	mrb_gv_set ( state, mrb_intern_cstr ( state, "$MOAIRubyStrongRefTable" ), this->mTable );
}

//----------------------------------------------------------------//
void MOAIRubyRefTable::InitWeak () {

	MOAIRubyState& state = MOAIRubyRuntime::Get ().GetMainState ();

	// create the hash
	this->mType = WEAK;
	this->mTable = mrb_hash_new ( state );

	mrb_gv_set ( state, mrb_intern_cstr ( state, "$MOAIRubyWeakRefTable" ), this->mTable );
}

//----------------------------------------------------------------//
MOAIRubyRefTable::MOAIRubyRefTable () :
	mRefIDStackTop ( 0 ) {
}

//----------------------------------------------------------------//
MOAIRubyRefTable::~MOAIRubyRefTable () {

	/*if ( !mrb_nil_p ( this->mTable ) ) {

		MOAIRubyState& state = MOAIRubyRuntime::Get ().GetMainState ();

		mrb_gc_unregister ( state, this->mTable );
	}*/
}

//----------------------------------------------------------------//
void MOAIRubyRefTable::Ref ( MOAIRubyState& state, const mrb_value& value ) {

	//assert ( !mrb_nil_p ( this->mTable ) );
	
	//idx = state.AbsIndex ( idx );
	//int refID = this->ReserveRefID ();

	// hash[mrb_value] = idx
	if ( this->mType == STRONG ) {

		// look up the old count using the object as a key
		mrb_value v = mrb_hash_fetch ( state, this->mTable, value, state.ToRValue < u32 >( 0 ) );

		u32 count = state.ToCValue < u32 > ( v );

		v = state.ToRValue < u32 > ( count + 1 );

		// store the new count in the ref table
		mrb_hash_set ( state, this->mTable, value, v );
	}

	if ( this->mType == WEAK ) {

		/*if ( this->mObjectMap.contains ( ptr ) ) {
			this->mRefMap [ this->mObjectMap [ ptr ] ]++;
		}
		else {
			MOAIRubyWeakRef* weak_ref = new MOAIRubyWeakRef ( v );
			this->mObjectMap [ ptr ] = weak_ref;
			this->mRefMap [ this->mObjectMap [ ptr ] ] = 1;
		}

		return *this->mObjectMap [ ptr ];*/
	}
}

//----------------------------------------------------------------//
void MOAIRubyRefTable::ReleaseRefID ( int refID ) {

	this->mRefIDStack [ this->mRefIDStackTop++ ] = refID;
}

//----------------------------------------------------------------//
int MOAIRubyRefTable::ReserveRefID () {

	if ( !this->mRefIDStackTop ) {

		// TODO: 64-bit
		u32 currentSize = ( u32 )this->mRefIDStack.Size ();

		assert ( currentSize <= ( MAX_REF_ID - REFID_CHUNK_SIZE ));

		u32 size = currentSize + REFID_CHUNK_SIZE;
		this->mRefIDStack.Init ( size );

		for ( u32 i = 0; i < REFID_CHUNK_SIZE; ++i ) {
			this->mRefIDStack [ i ] = size--;
		}
		this->mRefIDStackTop = REFID_CHUNK_SIZE;
	}

	assert ( this->mRefIDStackTop );

	return this->mRefIDStack [ --this->mRefIDStackTop ];
}

//----------------------------------------------------------------//
void MOAIRubyRefTable::Unref ( const mrb_value& value ) {

	//assert ( !mrb_nil_p ( this->mTable ) );

	MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
	
	// look up the old count using the object as a key
	mrb_value v = mrb_hash_fetch ( state, this->mTable, value, mrb_nil_value () );

	if ( mrb_nil_p ( v ) ) {
		return;
	}

	// get the count
	u32 count = state.ToCValue < u32 > ( v );

	if ( count == 0 || count == 1 ) {
		mrb_hash_delete_key ( state, this->mTable, value );
		return; // do nothing
	}

	if ( count > 1 ) {
		v = state.ToRValue < u32 > ( count - 1 ); // push the new count
		mrb_hash_set ( state, this->mTable, value, v );
	}

	//this->ReleaseRefID ( refID );
}
