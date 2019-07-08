// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPARTITIONVIEWLAYER_H
#define	MOAIPARTITIONVIEWLAYER_H

#include <moai-sim/MOAIFrameBuffer.h>
#include <moai-sim/MOAIGraphicsProp.h>
#include <moai-sim/MOAIViewLayer.h>
#include <moai-sim/MOAIPartitionHolder.h>
#include <moai-sim/MOAILayer.h>
#include <moai-sim/MOAIViewport.h>

//================================================================//
// MOAIPartitionViewLayer
//================================================================//
/**	@lua	MOAIPartitionViewLayer
	@text	Scene controls class.
	
	@const	SORT_NONE
	@const	SORT_ISO
	@const	SORT_PRIORITY_ASCENDING
	@const	SORT_PRIORITY_DESCENDING
	@const	SORT_X_ASCENDING
	@const	SORT_X_DESCENDING
	@const	SORT_Y_ASCENDING
	@const	SORT_Y_DESCENDING
	@const	SORT_Z_ASCENDING
	@const	SORT_Z_DESCENDING
	@const	SORT_VECTOR_ASCENDING
	@const	SORT_VECTOR_DESCENDING
*/
class MOAIPartitionViewLayer :
	public virtual MOAIPartitionHolder,
	public virtual MOAIViewLayer {
private:

	u32			mSortMode;
	bool		mSortInViewSpace;

	float		mSortScale [ 4 ];

	bool		mPartitionCull2D;

	//----------------------------------------------------------------//
	static mrb_value		_getPropViewList		( mrb_state* M, mrb_value context );
	static mrb_value		_getSortMode			( mrb_state* M, mrb_value context );
	static mrb_value		_getSortScale			( mrb_state* M, mrb_value context );
	static mrb_value		_setPartitionCull2D		( mrb_state* M, mrb_value context );
	static mrb_value		_setSortMode			( mrb_state* M, mrb_value context );
	static mrb_value		_setSortScale			( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	void			DrawPartition			( MOAIPartition& partition );
	void			DrawProps				( MOAIPartitionResultBuffer& buffer );
	void			DrawPropsDebug			( MOAIPartitionResultBuffer& buffer);

	//----------------------------------------------------------------//
	void			MOAIViewLayer_Draw		();

public:
		
	DECL_RUBY_FACTORY ( MOAIPartitionViewLayer, MOAILayer )
	
	//----------------------------------------------------------------//
					MOAIPartitionViewLayer		();
					~MOAIPartitionViewLayer		();
	void			RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void			SerializeIn					( MOAIRubyState& state, MOAIDeserializer& serializer );
	void			SerializeOut				( MOAIRubyState& state, MOAISerializer& serializer );
};

#endif
