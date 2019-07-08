// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIRUBYSTATE_IMPL_H
#define MOAIRUBYSTATE_IMPL_H

//================================================================//
// MOAIRubyState
//================================================================//

//----------------------------------------------------------------//
template < typename TYPE >
TYPE* MOAIRubyState::CreateClassInstance () {
	
	TYPE data;
	mrb_value instance = mrb_class_new_instance ( this->mState, 0, 0, mrb_class_get ( this->mState, data.TypeName () ) );

	return this->GetRubyObject < TYPE >( instance, true );
}

//----------------------------------------------------------------//
template < typename TYPE >
TYPE MOAIRubyState::GetFieldValue ( int idx, int key, TYPE value ) {

	//this->PushField ( idx, key );
	value = this->GetParamValue < TYPE >( idx, value );
	//this->Pop ( 1 );
	
	return value;
}

//----------------------------------------------------------------//
template < typename TYPE >
TYPE MOAIRubyState::GetFieldValue ( mrb_value obj, cc8* key, TYPE value ) {

	value = mrb_iv_get ( this->mState, obj, mrb_intern_cstr ( key ) );
	
	return value;
}

//----------------------------------------------------------------//
/**	@text	获取Ruby绑定类的C++实例，

	@opt	MOAIDataBuffer self
	@opt	string data				The string data to encode.  You must either provide either a MOAIDataBuffer (via a :hexEncode type call) or string data (via a .hexEncode type call), but not both.
	@out	string output			If passed a string, returns either a string or nil depending on whether it could be encoded.  Otherwise the encoding occurs inline on the existing data buffer in this object, and nil is returned.
*/
template < typename TYPE >
TYPE* MOAIRubyState::GetRubyObject ( mrb_value context, bool verbose ) {

	if ( !this->IsType ( context, MRB_TT_DATA ) ) return 0;

	MOAIRubyObject* rubyData = MOAIRubyObject::GetRubyObject ( this->mState, context );
	if ( rubyData ) {
		TYPE* type = rubyData->AsType < TYPE > ();
		if ( ( type == 0 ) && verbose ) {
			this->ReportBadCast ( -1, rubyData->TypeName () );
		}
		return type;
	}
	return 0;
}

//----------------------------------------------------------------//
template < typename TYPE >
TYPE* MOAIRubyState::GetRubyObject ( int idx, bool verbose ) {

	mrb_value v = this->GetParamValue ( idx );
	
	if ( !this->IsType ( v, MRB_TT_DATA ) ) return 0;
	
	MOAIRubyObject* rubyData = TYPE::GetRubyObject ( this->mState, v );
	if ( rubyData ) {
		TYPE* type = rubyData->AsType < TYPE >();
		if ( ( type == 0 ) && verbose ) {
			this->ReportBadCast ( idx, rubyData->TypeName () );
		}
		return type;
	}
	return 0;
}

//----------------------------------------------------------------//
template < typename TYPE >
TYPE* MOAIRubyState::GetRubyObject ( int idx, cc8* name, bool verbose ) {
	
	//if ( this->PushFieldWithType ( idx, name, LUA_TUSERDATA ) ) {
	//	
	//	MOAIRubyObject* rubyData = ( MOAIRubyObject* )this->GetPtrUserData ( -1 );
	//	//this->Pop ( 1 );
	//	
	//	if ( rubyData ) {
	//		TYPE* type = rubyData->AsType < TYPE >();
	//		if (( type == 0 ) && verbose ) {
	//			//this->ReportBadCast ( this->AbsIndex ( -1 ), rubyData->TypeName () );
	//		}
	//		return type;
	//	}
	//}
	return 0;
}

//----------------------------------------------------------------//
//template < typename TYPE >
//STLSet < TYPE* > MOAIRubyState::GetRubyDataSet ( int idx ) {
//	
//	idx = this->AbsIndex ( idx );
//	
//	STLSet < TYPE* > luaDataSet;
//	
//	this->PushTableItr ( idx );
//	while ( this->TableItrNext ( idx )) {
//		
//		TYPE* type = this->GetRubyObject < TYPE >( -1 );
//		if ( type ) {
//			luaDataSet.Affirm ( type );
//		}
//	}
//	
//	return luaDataSet;
//}

//----------------------------------------------------------------//
template < typename TYPE >
ZLMetaRect < TYPE > MOAIRubyState::GetRect ( int idx, TYPE value ) {

	ZLMetaRect < TYPE > rect;
	
	rect.mXMin = this->GetParamValue < TYPE >( idx++, value );
	rect.mYMin = this->GetParamValue < TYPE >( idx++, value );
	rect.mXMax = this->GetParamValue < TYPE >( idx++, value );
	rect.mYMax = this->GetParamValue < TYPE >( idx, value );
	
	return rect;
}

//----------------------------------------------------------------//
template < typename TYPE >
TYPE MOAIRubyState::GetParamValue ( int idx, TYPE value ) {
	UNUSED ( idx );

	assert ( false ); // unsupported type
	return value;
}

//----------------------------------------------------------------//
template < typename TYPE >
ZLMetaVec2D < TYPE > MOAIRubyState::GetVec2D ( int idx, TYPE value ) {

	ZLMetaVec2D < TYPE > vec;
	
	vec.mX = this->GetParamValue < TYPE >( idx++, value );
	vec.mY = this->GetParamValue < TYPE >( idx, value );
	
	return vec;
}

//----------------------------------------------------------------//
template < typename TYPE >
ZLMetaVec3D < TYPE > MOAIRubyState::GetVec3D ( int idx, TYPE value ) {

	ZLMetaVec3D < TYPE > vec;
	
	vec.mX = this->GetParamValue < TYPE >( idx++, value );
	vec.mY = this->GetParamValue < TYPE >( idx++, value );
	vec.mZ = this->GetParamValue < TYPE >( idx, value );
	
	return vec;
}

//----------------------------------------------------------------//
template < typename TYPE >
TYPE MOAIRubyState::PopValue ( TYPE value ) {

	value = this->GetParamValue < TYPE >( -1, value );
	//this->Pop ( 1 );
	return value;
}

//----------------------------------------------------------------//
//template < typename TYPE >
//void MOAIRubyState::Push ( ZLMetaRect < TYPE >& rect ) {
//
//	this->Push ( rect.mXMin );
//	this->Push ( rect.mYMin );
//	this->Push ( rect.mXMax );
//	this->Push ( rect.mYMax );
//}

//----------------------------------------------------------------//
template < typename TYPE >
void MOAIRubyState::ReadArray ( int size, TYPE* values, TYPE value ) {

	for ( int i = 0; i < size; ++i ) {
		values [ i ] = this->GetFieldValue ( -1, i + 1, value );
	}
}

//----------------------------------------------------------------//
//template < typename TYPE >
//void MOAIRubyState::SetClassField ( RClass* klass, cc8* key, TYPE value ) {
//	
//	//mrb_define_const ( this->mState, klass, key, ToRValue < TYPE >( key ) );
//}

//----------------------------------------------------------------//
template < typename TYPE >
void MOAIRubyState::SetField ( int idx, cc8* key, TYPE value ) {
	
	/*if ( this->IsTableOrUserdata ( idx )) {
		idx = this->AbsIndex ( idx );
		this->Push ( value );
		lua_setfield ( this->mState, idx, key );
	}*/
}

//----------------------------------------------------------------//
template < typename TYPE >
void MOAIRubyState::SetFieldByIndex ( int idx, int key, TYPE value ) {
	
	/*if ( this->IsTableOrUserdata ( idx )) {
		idx = this->AbsIndex ( idx );
		this->Push ( key );
		this->Push ( value );
		lua_settable ( this->mState, idx );
	}*/
}

//----------------------------------------------------------------//
template < typename TYPE >
void MOAIRubyState::SetGlobal ( cc8* key, TYPE value ) {

	/*this->Push ( value );
	lua_setglobal ( this->mState, key );*/
}

//----------------------------------------------------------------//
template < typename TYPE >
void MOAIRubyState::WriteArray ( int size, TYPE* values ) {

	for ( int i = 0; i < size; ++i ) {
		this->SetFieldByIndex ( -1, i + 1, values [ i ]);
	}
}

#endif
