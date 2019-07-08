// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIPartition.h>
#include <moai-sim/MOAIPartitionHolder.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	clear
	@text	Remove all props from the layer's partition.
	
	@in		MOAIPartitionHolder self
	@out	nil
*/
mrb_value MOAIPartitionHolder::_clear ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHolder, "U" )

	if ( self->mPartition ) {
		self->mPartition->Clear ();
	}
	return context;
}

//----------------------------------------------------------------//
/**	@lua	getPartition
	@text	Returns the partition currently attached to this layer.
	
	@in		MOAIPartitionHolder self
	@out	MOAIPartition partition
*/
mrb_value	MOAIPartitionHolder::_getPartition ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHolder, "U" )

	return self->GetPartition ()->PushRubyUserdata ( state );
}

//----------------------------------------------------------------//
/**	@lua	setPartition
	@text	Sets a partition for the holder to use. The holder will automatically
			create a partition when the first hull is added if no partition
			has been set.
	
	@in		MOAIPartitionHolder self
	@in		MOAIPartition partition
	@out	nil
*/
mrb_value MOAIPartitionHolder::_setPartition ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIPartitionHolder, "U" )

	self->mPartition.Set ( *self, state.GetRubyObject < MOAIPartition >( 1, true ));

	return context;
}

//================================================================//
// MOAIPartitionHolder
//================================================================//

//----------------------------------------------------------------//
MOAIPartition* MOAIPartitionHolder::GetPartition () {

	if ( !this->mPartition ) {
		this->mPartition.Set ( *this, MOAIRubyRuntime::Get ().GetMainState ().CreateClassInstance < MOAIPartition >());
	}
	assert ( this->mPartition );
	return this->mPartition;
}

//----------------------------------------------------------------//
MOAIPartitionHolder::MOAIPartitionHolder () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAIRubyObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIPartitionHolder::~MOAIPartitionHolder () {

	this->mPartition.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAIPartitionHolder::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	UNUSED ( state );
	UNUSED ( klass );
}

//----------------------------------------------------------------//
void MOAIPartitionHolder::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "clear", _clear, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getPartition", _getPartition, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setPartition", _setPartition, MRB_ARGS_REQ ( 1 ) );
	
}

//================================================================//
// ::implementation::
//================================================================//
