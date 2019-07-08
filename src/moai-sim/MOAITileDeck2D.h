// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAITILEDECK2D_H
#define	MOAITILEDECK2D_H

#include <moai-sim/MOAIDeck.h>
#include <moai-sim/MOAIMaterialBatchHolder.h>
#include <moai-sim/MOAIQuadBrush.h>

//================================================================//
// MOAITileDeck2D
//================================================================//
/**	@lua	MOAITileDeck2D
	@text	Subdivides a single texture into uniform tiles enumerated
			from the texture's left top to right bottom.
*/
class MOAITileDeck2D :
	public MOAIDeck,
	public MOAIMaterialBatchHolder,
	public MOAIGridSpace {
private:
	
	MOAIQuadBrush mQuad;
	
	//----------------------------------------------------------------//
	static mrb_value		_setQuad				( mrb_state* M, mrb_value context );
	static mrb_value		_setRect				( mrb_state* M, mrb_value context );
	static mrb_value		_setUVQuad				( mrb_state* M, mrb_value context );
	static mrb_value		_setUVRect				( mrb_state* M, mrb_value context );
	static mrb_value		_setSize				( mrb_state* M, mrb_value context );
	static mrb_value		_transform				( mrb_state* M, mrb_value context );
	static mrb_value		_transformUV			( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	ZLBounds				MOAIDeck_ComputeMaxBounds		();
	void					MOAIDeck_Draw					( u32 idx );
	ZLBounds				MOAIDeck_GetBounds				( u32 idx );
	MOAICollisionShape*		MOAIDeck_GetCollisionShape		( u32 idx );
	bool					MOAIDeck_Overlap				( u32 idx, const ZLVec2D& vec, u32 granularity, ZLBounds* result );
	bool					MOAIDeck_Overlap				( u32 idx, const ZLVec3D& vec, u32 granularity, ZLBounds* result );
	
public:
	
	DECL_RUBY_FACTORY ( MOAITileDeck2D, MOAIDeck )
	
	//----------------------------------------------------------------//
					MOAITileDeck2D			();
					~MOAITileDeck2D			();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			SerializeIn				( MOAIRubyState& state, MOAIDeserializer& serializer );
	void			SerializeOut			( MOAIRubyState& state, MOAISerializer& serializer );
	void			Transform				( const ZLAffine3D& mtx );
	void			TransformUV				( const ZLAffine3D& mtx );
};

#endif
