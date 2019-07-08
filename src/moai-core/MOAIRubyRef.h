// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIRUBYREF_H
#define MOAIRUBYREF_H

#include <moai-core/MOAIRubyRefTable.h>

class MOAIRubyObject;
class MOAIRubyRuntime;
class MOAIRubyState;
class MOAIScopedRubyState;

//================================================================//
// MOAIRubyRef
//================================================================//
class MOAIRubyRef {
private:

	bool						mOwnsRef;
	u32							mRefType;
	std::shared_ptr<RObject>	mStrongPtr;
	std::weak_ptr<RObject>		mWeakPtr;

public:

	enum {
		MAKE_STRONG,
		MAKE_WEAK,
	};

	enum {
		NONE,
		STRONG,
		WEAK,
	};

	friend class MOAIRubyStrongRef;
	friend class MOAIRubyWeakRef;

	//----------------------------------------------------------------//
	void					Clear			();
	bool					Empty			();
	mrb_value				GetRef			();
	bool					IsNil			();
	void					MakeStrong		();
	void					MakeWeak		();
							MOAIRubyRef		();
							MOAIRubyRef		( const MOAIRubyRef& assign );
	virtual					~MOAIRubyRef	();
	void					SetRef			( MOAIRubyObject* object, u32 type );
	virtual void			SetRef			( mrb_value value );
	void					SetRef			( mrb_value value, u32 type );
	void					Take			( const MOAIRubyRef& assign );

	//----------------------------------------------------------------//
	inline operator mrb_value () {
		return this->GetRef ();
	};

	//----------------------------------------------------------------//
	inline void operator = ( const MOAIRubyRef& assign ) {
		this->Take ( assign );
	}

	//----------------------------------------------------------------//
	inline operator bool () {
		return !this->IsNil ();
	}
};

//================================================================//
// MOAIRubyStrongRef
//================================================================//
class MOAIRubyStrongRef :
	public MOAIRubyRef {
public:

	//----------------------------------------------------------------//
					MOAIRubyStrongRef	();
					MOAIRubyStrongRef	( MOAIRubyObject* object );
					MOAIRubyStrongRef	( mrb_value value );
	void			SetRef				( MOAIRubyObject* object );
	void			SetRef				( mrb_value value );
};

//================================================================//
// MOAIRubyWeakRef
//================================================================//
class MOAIRubyWeakRef :
	public MOAIRubyRef {
public:

	//----------------------------------------------------------------//
					MOAIRubyWeakRef		();
					MOAIRubyWeakRef		( MOAIRubyObject* object );
					MOAIRubyWeakRef		( mrb_value value );
	void			SetRef				( MOAIRubyObject* object );
	void			SetRef				( mrb_value value );
};

//================================================================//
// MOAIRubyMemberRef
//================================================================//
class MOAIRubyMemberRef {
private:

	int					mRefID;
	MOAIRubyObject*		mOwner;

public:
	
	//----------------------------------------------------------------//
	void		Clear					();
				MOAIRubyMemberRef		();
				~MOAIRubyMemberRef		();
	bool		PushRef					( MOAIRubyState& state );
	void		SetRef					( MOAIRubyObject& owner, MOAIRubyState& state, int idx );
	
	//----------------------------------------------------------------//
	inline operator bool () const {
		return this->mRefID != 0;
	}
};

#endif
