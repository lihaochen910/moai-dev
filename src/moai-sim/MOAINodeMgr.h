// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAINODEMGR_H
#define MOAINODEMGR_H

class MOAINode;

//================================================================//
// MOAINodeMgr
//================================================================//
class MOAINodeMgr :
	public ZLContextClass < MOAINodeMgr, MOAIRubyObject > {
private:

	static const u32 DEFAULT_MAX_ITERATIONS = 3; // arbitrary number

	MOAINode* mUpdateListHead;
	MOAINode* mUpdateListTail;

	bool mScheduled;
	u32 mMaxIterations;

	//----------------------------------------------------------------//
	static mrb_value		_reset				( mrb_state* M, mrb_value context );
	static mrb_value		_setMaxIterations	( mrb_state* M, mrb_value context );
	static mrb_value		_update				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void			InsertAfter			( MOAINode& cursor, MOAINode& node );
	void			InsertBefore		( MOAINode& cursor, MOAINode& node );
	void			PushBack			( MOAINode& node );
	void			PushFront			( MOAINode& node );
	void			Remove				( MOAINode& node );

public:

	friend class MOAINode;

	DECL_RUBY_SINGLETON ( MOAINodeMgr )

	GET_SET ( u32, MaxIterations, mMaxIterations )

	//----------------------------------------------------------------//
	void			Reset				();
					MOAINodeMgr			();
					~MOAINodeMgr		();
	void			RegisterRubyClass	( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs	( MOAIRubyState& state, RClass* klass );
	void			Update				();
};

#endif
