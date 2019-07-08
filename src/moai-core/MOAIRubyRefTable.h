// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIRUBYREFTABLE_H
#define MOAIRUBYREFTABLE_H

class MOAIRubyState;

//================================================================//
// MOAIRubyRefTable
//================================================================//
class MOAIRubyRefTable {
private:

	friend class MOAIRubyRef;

	//typedef STLMap < void*, u32 >						RefMap;

	//RefMap					mRefMap;
	mrb_value				mTable;
	u32						mType;
	
	static const u32		MAX_REF_ID = 0xffffffff;
	static const u32		NO_REF = 0xffffffff;

	static const u32		REFID_CHUNK_SIZE = 1024;
	ZLLeanArray < u32 >		mRefIDStack;
	u32						mRefIDStackTop;

	//----------------------------------------------------------------//
	void					ReleaseRefID		( int refID );
	int						ReserveRefID		();

public:

	enum {
		STRONG,
		WEAK,
	};

	//----------------------------------------------------------------//
	void					Clear				();
	void					InitStrong			();
	void					InitWeak			();
							MOAIRubyRefTable	();
							~MOAIRubyRefTable	();
	void					Ref					( MOAIRubyState& state, const mrb_value& v );
	void					Unref				( const mrb_value& v );
};

#endif
