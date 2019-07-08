// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIRUBYSTATE_H
#define MOAIRUBYSTATE_H

class MOAIRubyMemberRef;
class MOAIRubyObject;
class MOAIRubyRef;

//================================================================//
// MOAIRubyState
//================================================================//
class MOAIRubyState {
private:

	friend class MOAIScopedRubyState;

	mrb_state*		mState;

	//----------------------------------------------------------------//
	bool			Decode					( int idx, ZLStreamAdapter& reader );
	bool			Encode					( int idx, ZLStreamAdapter& writer );
	void			ReportBadCast			( int idx, cc8* typeName );

public:

	enum {
		THREAD_UNKNOWN		= 0x00,
		THREAD_DEAD			= 0x01,
		THREAD_ERROR		= 0x02,
		THREAD_NORMAL		= 0x04,
		THREAD_RUNNING		= 0x08,
		THREAD_SUSPENDED	= 0x10,
	};
	
	static const int THREAD_VALID_MASK = THREAD_SUSPENDED | THREAD_NORMAL | THREAD_RUNNING;

	//----------------------------------------------------------------//
	int				AbsIndex					( int idx );
	bool			Base64Decode				( int idx );
	bool			Base64Encode				( int idx );
	bool			CheckParams					( int idx, cc8* format, bool verbose = true ); // "BCFLNOPSTU"
	void			ClearField					( mrb_value obj, cc8* key );
	bool			CheckVector					( int idx, u32 n, mrb_float match, mrb_float fallback );
	bool			CheckVector					( int idx, u32 n, mrb_float min, mrb_float max, mrb_float fallback );
	void			DefineClassConst			( struct RClass* klass, cc8* name, mrb_value value );
	void			DefineInstanceMethod		( struct RClass* klass, cc8* name, mrb_func_t func, mrb_aspec aspec );
	void			DefineSingletonMethod		( struct RObject* obj, cc8* name, mrb_func_t func, mrb_aspec aspec );
	void			DefineStaticMethod			( struct RClass* klass, cc8* name, mrb_func_t func, mrb_aspec aspec );
	int				DebugCall					( int nArgs, int nResults );
	bool			Deflate						( int idx, int level, int windowBits );
//	bool			DumpChunk					( int idx, ZLStream& stream );
	mrb_value		Get							( const ZLBox& value );
	mrb_value		Get							( const ZLColorVec& value );
	mrb_value		Get							( const ZLAffine2D& value );
	mrb_value		Get							( const ZLAffine3D& value );
	mrb_value		Get							( const ZLMatrix3x3& value );
	mrb_value		Get							( const ZLMatrix4x4& value );
	mrb_value		Get							( const ZLQuad& value );
	mrb_value		Get							( const ZLRect& value );
	mrb_value		Get							( const ZLVec2D& value );
	mrb_value		Get							( const ZLVec3D& value );
	mrb_value		Get							( const ZLVec4D& value );
	ZLBox			GetBox						( int idx );
	ZLColorVec		GetColor					( int idx, float r, float g, float b, float a );
	u32				GetColor32					( int idx, float r, float g, float b, float a );
	bool			FiberAlive					( mrb_value fib );
	mrb_value		FiberResume					( mrb_value fib, mrb_int argc = 0, const mrb_value* args = 0 );
	mrb_value		FiberYield					( mrb_int argc = 0, const mrb_value* args = 0 );
	mrb_value		FuncCall					( mrb_value obj, cc8* funcName, mrb_int argc = 0, const mrb_value* args = 0 );
	mrb_value		FuncCallWithBlock			( mrb_value obj, cc8* funcName, mrb_int argc, const mrb_value* args, mrb_value block );

//	STLString		GetField					( int idx, cc8* key, cc8* value );
//	STLString		GetField					( int idx, int key, cc8* value );
//	STLString		GetField					( int idx, cc8* key, const STLString& value );
//	STLString		GetField					( int idx, int key, const STLString& value );
	
	mrb_int			GetArraySize				( mrb_value ary );
	mrb_value		GetField					( mrb_value obj, cc8* name );
	mrb_int			GetHashSize					( mrb_value hash );
	static cc8*		GetRubyTypeName				( mrb_vtype type );
	mrb_int			GetParamsCount				();
	ZLMatrix4x4		GetParamMatrix				( int idx, size_t size );
	mrb_vtype		GetParamType				( int idx );
	ZLQuad			GetParamQuad				( int idx );
	void*			GetPtrUserData				( int idx );
	STLString		GetStackDump				();
	STLString		GetStackTrace				( cc8* title, int level );
	void*			GetUserData					( int idx, void* value );
	void*			GetUserData					( int idx, cc8* name, void* value );
	STLString		GetValue					( int idx, cc8* value );
	mrb_value		GetParamValue				( int idx );
	mrb_value		GetRObjectField				( RObject* obj, cc8* key );
	mrb_vtype		GetType						( mrb_value value );
	bool			HasField					( mrb_value klass, cc8* name );
	bool			HasFieldWithType			( mrb_value klass, cc8* name, mrb_vtype type );
	bool			HexDecode					( int idx );
	bool			HexEncode					( int idx );
	bool			Inflate						( int idx, int windowBits );
	bool			IsNil						( mrb_value value );
	bool			IsNilOrUndef				( mrb_value value );
	bool			IsType						( mrb_value value, mrb_vtype type );
	bool			IsValid						();
	
	bool			LogErrors					( u32 level, FILE* file, int status );
	void			LogStackDump				( u32 level, FILE* file );
	void			LogStackTrace				( u32 level, FILE* file, cc8* title, int stackLevel );
	
					MOAIRubyState				();
					MOAIRubyState				( mrb_state* state );
	virtual			~MOAIRubyState				();

	bool			ParamIsType					( int idx, mrb_vtype type );
	int				RealParamIndex				( int idx );
	mrb_value		Run							( void* data, size_t size, int nArgs, int nResults );
	void			SetInstanceField			( mrb_value obj, cc8* key, mrb_value value, bool force = true );
	void			SetRObjectField				( RObject* obj, cc8* key, mrb_value value, bool force = true );
	void			SetClassField				( RClass* klass, cc8* key, mrb_value value, bool force = true );
	void			SetPath						( cc8* path );
	mrb_value		ToSymbol					( cc8* str );
	mrb_value		YieldThread					( int nResults );
	
	//----------------------------------------------------------------//
	inline mrb_state* operator -> () const {
		return mState;
	};

	//----------------------------------------------------------------//
	inline mrb_state& operator * () const {
		return *mState;
	};

	//----------------------------------------------------------------//
	inline operator mrb_state* () {
		return mState;
	};

	//----------------------------------------------------------------//
	inline operator bool () const {
		return ( this->mState != 0 );
	}
	
	//----------------------------------------------------------------//
	template < typename TYPE > TYPE*					CreateClassInstance	();
	template < typename TYPE > TYPE						GetFieldValue		( int idx, int key, TYPE value );
	template < typename TYPE > TYPE						GetFieldValue		( mrb_value obj, cc8* key, TYPE value );
	template < typename TYPE > TYPE*					GetRubyObject 		( mrb_value context, bool verbose = true );
	template < typename TYPE > TYPE*					GetRubyObject		( int idx, bool verbose );
	template < typename TYPE > TYPE*					GetRubyObject		( int idx, cc8* name, bool verbose );
	template < typename TYPE > ZLMetaRect < TYPE >		GetRect				( int idx, TYPE value = 0 );
	template < typename TYPE > TYPE						GetParamValue		( int idx, TYPE value );
	template < typename TYPE > ZLMetaVec2D < TYPE >		GetVec2D			( int idx, TYPE value = 0 );
	template < typename TYPE > ZLMetaVec3D < TYPE >		GetVec3D			( int idx, TYPE value = 0 );
	template < typename TYPE > TYPE						PopValue			( TYPE value );
	template < typename TYPE > void						Push				( ZLMetaRect < TYPE >& rect );
	template < typename TYPE > void						ReadArray			( int size, TYPE* values, TYPE value );
	//template < typename TYPE > void					SetClassField		( RClass* klass, cc8* key, TYPE value );
	template < typename TYPE > void						SetField			( int idx, cc8* key, TYPE value );
	template < typename TYPE > void						SetFieldByIndex		( int idx, int key, TYPE value );
	template < typename TYPE > void						SetGlobal			( cc8* key, TYPE value );
	template < typename TYPE > TYPE						ToCValue			( const mrb_value& value );
	template < typename TYPE > mrb_value				ToRValue			( TYPE value );
	template < typename TYPE > void						WriteArray			( int size, TYPE* values );

	//----------------------------------------------------------------//
	template < typename TYPE >
	void DefineClassConst ( struct RClass* klass, cc8* name, TYPE value ) {

		mrb_define_const ( this->mState, klass, name, this->ToRValue ( value ) );
	}
	
//	//----------------------------------------------------------------//
//	template < typename TYPE >
//	TYPE GetFieldValue ( int idx, cc8* key, TYPE value ) {
//	
//		this->PushField ( idx, key );
//		TYPE result = this->GetValue < TYPE >( -1, value );
//		this->Pop ();
//		
//		return result;
//	}
//	
//	//----------------------------------------------------------------//
//	template < typename TYPE >
//	TYPE GetFieldValue ( int idx, int key, TYPE value ) {
//	
//		this->PushField ( idx, key );
//		TYPE result = this->GetValue < TYPE >( -1, value );
//		this->Pop ();
//		
//		return result;
//	}
};

//----------------------------------------------------------------//
template <> bool			MOAIRubyState::ToCValue < bool >				( const mrb_value& value );
template <> cc8*			MOAIRubyState::ToCValue < cc8* >				( const mrb_value& value );
template <> double			MOAIRubyState::ToCValue < double >				( const mrb_value& value );
template <> float			MOAIRubyState::ToCValue < float >				( const mrb_value& value );
template <> s8				MOAIRubyState::ToCValue < s8 >					( const mrb_value& value );
template <> s16				MOAIRubyState::ToCValue < s16 >					( const mrb_value& value );
template <> s32				MOAIRubyState::ToCValue < s32 >					( const mrb_value& value );
template <> s64				MOAIRubyState::ToCValue < s64 >					( const mrb_value& value );
template <> u8				MOAIRubyState::ToCValue < u8 >					( const mrb_value& value );
template <> u16				MOAIRubyState::ToCValue < u16 >					( const mrb_value& value );
template <> u32				MOAIRubyState::ToCValue < u32 >					( const mrb_value& value );
template <> u64				MOAIRubyState::ToCValue < u64 >					( const mrb_value& value );
template <> const void*		MOAIRubyState::ToCValue < const void* >			( const mrb_value& value );

//----------------------------------------------------------------//
template <> mrb_value		MOAIRubyState::ToRValue < bool >				( bool value );
template <> mrb_value		MOAIRubyState::ToRValue < cc8* >				( cc8* value );
template <> mrb_value		MOAIRubyState::ToRValue < double >				( double value );
template <> mrb_value		MOAIRubyState::ToRValue < float >				( float value );
template <> mrb_value		MOAIRubyState::ToRValue < s8 >					( s8 value );
template <> mrb_value		MOAIRubyState::ToRValue < s16 >					( s16 value );
template <> mrb_value		MOAIRubyState::ToRValue < s32 >					( s32 value );
template <> mrb_value		MOAIRubyState::ToRValue < s64 >					( s64 value );
template <> mrb_value		MOAIRubyState::ToRValue < u8 >					( u8 value );
template <> mrb_value		MOAIRubyState::ToRValue < u16 >					( u16 value );
template <> mrb_value		MOAIRubyState::ToRValue < u32 >					( u32 value );
template <> mrb_value		MOAIRubyState::ToRValue < u64 >					( u64 value );
template <> mrb_value		MOAIRubyState::ToRValue < void* >				( void* value );
template <> mrb_value		MOAIRubyState::ToRValue < MOAIRubyObject* >		( MOAIRubyObject* value );
template <> mrb_value		MOAIRubyState::ToRValue < MOAIRubyObject& >		( MOAIRubyObject& value );
template <> mrb_value		MOAIRubyState::ToRValue < STLString >			( STLString value );
template <> mrb_value		MOAIRubyState::ToRValue < ZLBox >				( ZLBox value );
template <> mrb_value		MOAIRubyState::ToRValue < ZLColorVec >			( ZLColorVec value );
template <> mrb_value		MOAIRubyState::ToRValue < ZLAffine2D >			( ZLAffine2D value );
template <> mrb_value		MOAIRubyState::ToRValue < ZLAffine3D >			( ZLAffine3D value );
template <> mrb_value		MOAIRubyState::ToRValue < ZLMatrix3x3 >			( ZLMatrix3x3 value );
template <> mrb_value		MOAIRubyState::ToRValue < ZLMatrix4x4 >			( ZLMatrix4x4 value );
template <> mrb_value		MOAIRubyState::ToRValue < ZLQuad >				( ZLQuad value );
template <> mrb_value		MOAIRubyState::ToRValue < ZLRect >				( ZLRect value );
template <> mrb_value		MOAIRubyState::ToRValue < ZLVec2D >				( ZLVec2D value );
template <> mrb_value		MOAIRubyState::ToRValue < ZLVec3D >				( ZLVec3D value );
template <> mrb_value		MOAIRubyState::ToRValue < ZLVec4D >				( ZLVec4D value );

//----------------------------------------------------------------//
template <> bool			MOAIRubyState::GetParamValue < bool >			( int idx, const bool value );
template <> cc8*			MOAIRubyState::GetParamValue < cc8* >			( int idx, const cc8* value );
template <> double			MOAIRubyState::GetParamValue < double >			( int idx, const double value );
template <> float			MOAIRubyState::GetParamValue < float >			( int idx, const float value );
template <> s8				MOAIRubyState::GetParamValue < s8 >				( int idx, const s8 value );
template <> s16				MOAIRubyState::GetParamValue < s16 >			( int idx, const s16 value );
template <> s32				MOAIRubyState::GetParamValue < s32 >			( int idx, const s32 value );
template <> s64				MOAIRubyState::GetParamValue < s64 >			( int idx, const s64 value );
template <> u8				MOAIRubyState::GetParamValue < u8 >				( int idx, const u8 value );
template <> u16				MOAIRubyState::GetParamValue < u16 >			( int idx, const u16 value );
template <> u32				MOAIRubyState::GetParamValue < u32 >			( int idx, const u32 value );
template <> u64				MOAIRubyState::GetParamValue < u64 >			( int idx, const u64 value );
template <> const void*		MOAIRubyState::GetParamValue < const void* >	( int idx, const void* value );
template <> ZLBox			MOAIRubyState::GetParamValue < ZLBox >			( int idx, const ZLBox value );
template <> ZLColorVec		MOAIRubyState::GetParamValue < ZLColorVec >		( int idx, const ZLColorVec value );
template <> ZLAffine2D		MOAIRubyState::GetParamValue < ZLAffine2D >		( int idx, const ZLAffine2D value );
template <> ZLAffine3D		MOAIRubyState::GetParamValue < ZLAffine3D >		( int idx, const ZLAffine3D value );
template <> ZLMatrix3x3		MOAIRubyState::GetParamValue < ZLMatrix3x3 >	( int idx, const ZLMatrix3x3 value );
template <> ZLMatrix4x4		MOAIRubyState::GetParamValue < ZLMatrix4x4 >	( int idx, const ZLMatrix4x4 value );
template <> ZLQuaternion	MOAIRubyState::GetParamValue < ZLQuaternion >	( int idx, const ZLQuaternion value );
template <> ZLRect			MOAIRubyState::GetParamValue < ZLRect >			( int idx, const ZLRect value );
template <> ZLVec2D			MOAIRubyState::GetParamValue < ZLVec2D >		( int idx, const ZLVec2D value );
template <> ZLVec3D			MOAIRubyState::GetParamValue < ZLVec3D >		( int idx, const ZLVec3D value );
template <> ZLVec4D			MOAIRubyState::GetParamValue < ZLVec4D >		( int idx, const ZLVec4D value );

#endif
