// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIRUBYCLASS_H
#define	MOAIRUBYCLASS_H

#include <moai-core/MOAIRubyObject.h>

class MOAIRubyState;

#define DECL_RUBY_ABSTRACT(type)					\
MOAIRubyClass* GetRubyClass () { return 0; }		\
cc8* TypeName () const { return #type; }

#define DECL_RUBY_FACTORY_ORIGIN(type)													\
MOAIRubyClass* GetRubyClass () { return &MOAIRubyFactoryClass < type >::Get (); }		\
MOAIRubyClass* GetSuperRubyClass () { return &MOAIRubyBasicClass::Get (); }				\
static void RegisterRubyType () { MOAIRubyFactoryClass < type >::Get ().Register (); }	\
cc8* TypeName () const { return #type; }

#define DECL_RUBY_FACTORY(type,super)													\
MOAIRubyClass* GetRubyClass () { return &MOAIRubyFactoryClass < type >::Get (); }		\
MOAIRubyClass* GetSuperRubyClass () { return &MOAIRubyFactoryClass < super >::Get (); }	\
static void RegisterRubyType () { MOAIRubyFactoryClass < type >::Get ().Register (); }	\
cc8* TypeName () const { return #type; }

#define DECL_RUBY_OPAQUE(type)															\
MOAIRubyClass* GetRubyClass () { return &MOAIRubyFactoryClass < type >::Get (); }		\
cc8* TypeName () const { return #type; }

#define DECL_RUBY_SINGLETON(type)														\
MOAIRubyClass* GetRubyClass () { return &MOAIRubySingletonClass < type >::Get (); }			\
static void RegisterRubyType () { MOAIRubySingletonClass < type >::Get ().Register (); }	\
cc8* TypeName () const { return #type; }

#define DECL_RUBY_SINGLETON_INHERIT_FACTORY(type,super)										\
MOAIRubyClass* GetRubyClass () { return &MOAIRubySingletonClass < type >::Get (); }			\
MOAIRubyClass* GetSuperRubyClass () { return &MOAIRubyFactoryClass < super >::Get (); }		\
static void RegisterRubyType () { MOAIRubySingletonClass < type >::Get ().Register (); }	\
cc8* TypeName () const { return #type; }

#define DECL_RUBY_SINGLETON_INHERIT_SINGLETON(type,super)									\
MOAIRubyClass* GetRubyClass () { return &MOAIRubySingletonClass < type >::Get (); }			\
MOAIRubyClass* GetSuperRubyClass () { return &MOAIRubySingletonClass < super >::Get (); }	\
static void RegisterRubyType () { MOAIRubySingletonClass < type >::Get ().Register (); }	\
cc8* TypeName () const { return #type; }

#define REGISTER_RUBY_CLASS(type)	\
type::RegisterRubyType ();

#define RUBY_FACTORY_NEW(type)													\
	type::mrb_data_t* data_t = ( type::mrb_data_t* )mrb_malloc ( M, sizeof ( type::mrb_data_t ) );	\
	data_t->data = data;														\
	DATA_PTR ( self ) = ( void* )data_t;										\
	DATA_TYPE ( self ) = &type::mrb_data_type_i;								\
	data->SetRObjectPtr ( mrb_obj_ptr ( self ) );

#define RUBY_SINGLETON_CTOR(type)												\
	type::mrb_data_t* data_t = ( type::mrb_data_t* )mrb_malloc ( M, sizeof ( type::mrb_data_t ) );	\
	data_t->data = data;														\
	DATA_PTR ( self ) = ( void* )data_t;										\
	DATA_TYPE ( self ) = &type::mrb_data_type_i;								\
	data->SetRObjectPtr ( mrb_obj_ptr ( self ) );								\
	return mrb_singleton_class ( M, self );


//================================================================//
// MOAIRubyClass
//================================================================//
class MOAIRubyClass :
	public ZLContextClassBase {
protected:

	bool				mIsSingleton;
	RClass*				mClass;
	RClass*				mSuperClass;
	
	MOAIRubyStrongRef	mClassTable;			// global factory class for type
	MOAIRubyStrongRef	mInterfaceTable;		// interface shared by all instances of type
	MOAIRubyStrongRef	mRefTable;				// strong ref to member table for singletons
	MOAIRubyStrongRef	mSingletonRef;			// for singletons

	//----------------------------------------------------------------//
	//static int		_extendFactory				( lua_State* L );
	//static int		_extendSingleton			( lua_State* L );
	//static int		_get						( lua_State* L );
	//static int		_getInterfaceTable			( lua_State* L );
	static mrb_value	_new						( mrb_state* M, mrb_value self );

	//----------------------------------------------------------------//
	void				InitRubyFactoryClass		( MOAIRubyObject& data, MOAIRubyState& state );
	void				InitRubySingletonClass		( MOAIRubyObject& data, MOAIRubyState& state );
	//void				PushInterfaceTable			( MOAIRubyState& state );
	//void				PushRefTable				( MOAIRubyState& state );
	virtual void		RegisterRubyClass			( MOAIRubyState& state, RClass* klass ) = 0;

public:

	friend class MOAIRubyObject;

	//----------------------------------------------------------------//
	RClass*					GetBasicRClass			();
	RClass*					GetRClass				();
	virtual MOAIRubyObject*	GetSingleton			();
	RObject*				GetSingletonRObject		();
	bool					IsSingleton				();
	virtual void			Register				() = 0;
							MOAIRubyClass			();
	virtual					~MOAIRubyClass			();
};

//================================================================//
// MOAIRubyFactoryClass
//================================================================//
template < typename TYPE >
class MOAIRubyFactoryClass :
public MOAIRubyClass {
private:
	
	//----------------------------------------------------------------//
	static mrb_value				_getClassName			( mrb_state* M, mrb_value self );
	static mrb_value				_getTypeID				( mrb_state* M, mrb_value self );
	static mrb_value				_new					( mrb_state* M, mrb_value self );
	
	//----------------------------------------------------------------//
	void							RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	
public:
	
	//----------------------------------------------------------------//
	static MOAIRubyFactoryClass&	Get						();
									MOAIRubyFactoryClass	();
	void							Register				();
};

//================================================================//
// MOAIRubySingletonClass
//================================================================//
template < typename TYPE >
class MOAIRubySingletonClass :
public MOAIRubyClass {
private:

	//----------------------------------------------------------------//
	static mrb_value				_getClassName			( mrb_state* M, mrb_value self );
	static mrb_value				_getTypeID				( mrb_state* M, mrb_value self );
	static mrb_value				_new					( mrb_state* M, mrb_value self );
	
	//----------------------------------------------------------------//
	void							RegisterRubyClass		( MOAIRubyState& state, RClass* klass );

	
public:
	
	//----------------------------------------------------------------//
	static MOAIRubySingletonClass&	Get						();
	MOAIRubyObject*					GetSingleton			();
									MOAIRubySingletonClass	();
	void							Register				();

};

//================================================================//
// MOAIRubyBasicClass
//================================================================//
class MOAIRubyBasicClass :
public MOAIRubyClass {
private:

	//----------------------------------------------------------------//
	void							RegisterRubyClass		( MOAIRubyState& state, RClass* klass );

public:
	
	//----------------------------------------------------------------//
	static MOAIRubyBasicClass&		Get						();
									MOAIRubyBasicClass		();
	void							Register				();

};

#endif
