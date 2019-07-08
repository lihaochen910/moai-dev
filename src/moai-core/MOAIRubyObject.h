// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIRUBYOBJECT_H
#define	MOAIRUBYOBJECT_H

#include <moai-core/MOAIRubyRef.h>

class MOAIDeserializer;
class MOAIRubyClass;
class MOAIRubyObject;
class MOAIRubyState;
class MOAIScopedRubyState;
class MOAISerializer;

// static void mrb_data_free ( mrb_state* M, void* p ) { if ( p != 0 ) mrb_free ( M, p ); }
#define DECL_RUBY_DATA(type)													\
typedef struct mrb_data_t { type* data; } mrb_data_t;							\
static void mrb_data_free ( mrb_state* M, void* p ) { if ( p != 0 ) mrb_free ( M, p ); }			\
constexpr static struct mrb_data_type const mrb_data_type_i = { #type, ##type##::mrb_data_free };	\
static type* GetRubyObject ( mrb_state* M, mrb_value context ) {				\
	mrb_data_t* data;															\
	if ( mrb_nil_p ( context ) ) return 0;										\
	data = ( mrb_data_t* )mrb_data_get_ptr( M, context, &mrb_data_type_i );		\
	return data->data;															\
}


//================================================================//
// MOAIRubyObject
//================================================================//
class MOAIRubyObject :
	public virtual RTTIBase,
	public virtual ZLRefCountedObject {
private:
	
	u32						mActiveUserdataCount;
	RObject*				mPtr;			// ptr to class instance
	MOAIRubyWeakRef			mUserdata;		// ref to userdata (weak)
	MOAIRubyStrongRef		mFinalizer;		// ref to finalizer (strong)
	MOAIRubyWeakRef			mRefTable;		// ref to Ref Hash (weak)

protected:

	//----------------------------------------------------------------//
	static mrb_value		_gc						( mrb_state* M, mrb_value self );
	static mrb_value		_getClassName			( mrb_state* M, mrb_value self );
	static mrb_value		_pin					( mrb_state* M, mrb_value self );
	//static mrb_value		_serializeIn			( mrb_state* M, mrb_value self );
	//static mrb_value		_serializeOut			( mrb_state* M, mrb_value self );
	static mrb_value		_setFinalizer			( mrb_state* M, mrb_value self );
	static mrb_value		_tostring				( mrb_state* M, mrb_value self );
	static mrb_value		_unpin					( mrb_state* M, mrb_value self );

	//----------------------------------------------------------------//
	//static int			InjectAndCall			( lua_CFunction func, MOAIRubyObject* self, lua_State* L );
	void					MakeRubyBinding			( MOAIRubyState& state );
	//bool					PushRefTable			( MOAIRubyState& state );
	//void					SetInterfaceTable		( MOAIRubyState& state, int idx );
	//void					SetMemberTable			( MOAIRubyState& state, int idx );

	//----------------------------------------------------------------//
	void					ZLRefCountedObjectBase_OnRelease				( u32 refCount ) override;

public:

	friend class MOAIRubyCanary;
	friend class MOAIRubyClass;
	friend class MOAIRubyMemberRef;
	friend class MOAIRubyRuntime;
	friend class MOAIDeserializer;
	friend class MOAISerializer;

	DECL_RUBY_DATA ( MOAIRubyObject )

	//----------------------------------------------------------------//
	static mrb_value		_alertNewIsUnsupported		( mrb_state* M, mrb_value self );

	//----------------------------------------------------------------//
	void					BindToRuby					( MOAIRubyState& state );
	mrb_value				GetMRBObject				();
	virtual MOAIRubyClass*	GetRubyClass				();
	virtual MOAIRubyClass*	GetSuperRubyClass			();
	void					GetRef						( MOAIRubyRef& ref, u32 refType );
	bool					IsBound						();
	static bool				IsMoaiUserdata				( MOAIRubyState& state, int idx );
	bool					IsSingleton					();
	void					RubyRelease					();
	void					RubyRelease					( MOAIRubyObject* object );
	void					RubyRetain					();
	void					RubyRetain					( MOAIRubyObject* object );
							MOAIRubyObject				();
	virtual					~MOAIRubyObject				();
	void					PrintTracking				();
	mrb_value				PushRubyUserdata			( MOAIRubyState& state );
	virtual void			RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	virtual void			RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	virtual	void			SerializeIn					( MOAIRubyState& state, MOAIDeserializer& serializer );
	virtual	void			SerializeOut				( MOAIRubyState& state, MOAISerializer& serializer );
	void					SetRObjectPtr				( RObject* ptr );
	cc8*					TypeName					() const;
	
	//----------------------------------------------------------------//
	template < typename TYPE, mrb_func_t FUNC >
	static mrb_value WrapInstanceFuncAsGlobal ( mrb_state* M, mrb_value self ) {
	
		TYPE* type = ZLContextMgr::Get ()->GetGlobal < TYPE >();
		assert ( type );
		//return InjectAndCall ( FUNC, type, M );
		return self;
	}
};

#endif
