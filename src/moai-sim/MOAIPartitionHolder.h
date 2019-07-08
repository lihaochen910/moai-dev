// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPARTITIONHOLDER_H
#define	MOAIPARTITIONHOLDER_H

class MOAIPartition;

//================================================================//
// MOAIPartitionHolder
//================================================================//
// TODO: doxygen
class MOAIPartitionHolder :
	public virtual MOAIRubyObject {
protected:

	MOAIRubySharedPtr < MOAIPartition >		mPartition;

	//----------------------------------------------------------------//
	static mrb_value			_clear						( mrb_state* M, mrb_value context );
	static mrb_value			_getPartition				( mrb_state* M, mrb_value context );
	static mrb_value			_setPartition				( mrb_state* M, mrb_value context );

public:
	
	//----------------------------------------------------------------//
	MOAIPartition*		GetPartition				();
						MOAIPartitionHolder			();
						~MOAIPartitionHolder		();
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
};

#endif
