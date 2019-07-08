// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIGRIDPROPBASE_H
#define	MOAIGRIDPROPBASE_H

#include <moai-sim/MOAIBlendMode.h>
#include <moai-sim/MOAIColor.h>
#include <moai-sim/MOAIDeckPropBase.h>
#include <moai-sim/MOAIDrawable.h>
#include <moai-sim/MOAIPartitionHull.h>
#include <moai-sim/MOAITransform.h>
#include <moai-core/MOAIRubySharedPtr.h>

class MOAICellCoord;
class MOAICollisionProp;
class MOAIDeck;
class MOAIGrid;
class MOAILayoutFrame;
class MOAIOverlapPrim2D;
class MOAIPartition;
class MOAIPartitionCell;
class MOAIPartitionLevel;
class MOAIPartitionResultBuffer;
class MOAISurfaceSampler2D;

class MOAIGridPropBase;

//================================================================//
// MOAIGridPropBase
//================================================================//
/**	@lua	MOAIGridPropBase
	@text	Base class for props.
	
	@attr	ATTR_INDEX
	@attr	ATTR_PARTITION
*/
class MOAIGridPropBase :
	public virtual MOAIDeckPropBase {
private:
	
	//----------------------------------------------------------------//
	static mrb_value			_getGrid					( mrb_state* M, mrb_value context );
	static mrb_value			_setGrid					( mrb_state* M, mrb_value context );
	static mrb_value			_setGridScale				( mrb_state* M, mrb_value context );

protected:
	
	MOAIRubySharedPtr < MOAIGrid >			mGrid;
	ZLVec2D									mGridScale;

public:

	//----------------------------------------------------------------//
	void				GetGridBoundsInView		( const ZLAffine3D& worldToLocalMtx, MOAICellCoord& c0, MOAICellCoord& c1 ); // TODO: this shoudln't be here
						MOAIGridPropBase		();
	virtual				~MOAIGridPropBase		();
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void				SerializeIn				( MOAIRubyState& state, MOAIDeserializer& serializer );
	void				SerializeOut			( MOAIRubyState& state, MOAISerializer& serializer );
};

#endif
