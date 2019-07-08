// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIVERTEXARRAY_H
#define	MOAIVERTEXARRAY_H

class MOAIVertexArray;
class MOAIVertexBuffer;
class MOAIVertexBufferWithFormat;
class MOAIVertexFormat;

//================================================================//
// MOAIVertexArray
//================================================================//
// TODO: doxygen
class MOAIVertexArray :
	public MOAIGfxResource {
protected:

	ZLLeanArray < ZLGfxHandle >						mVAOs; // vertex array objects to bind all the vertex and buffer state
	u32												mCurrentVAO;

	ZLLeanArray < MOAIVertexBufferWithFormat >		mVertexBuffers;

	bool				mUseVAOs;

	//----------------------------------------------------------------//
	static mrb_value			_reserveVAOs				( mrb_state* M, mrb_value context );
	static mrb_value			_reserveVertexBuffers		( mrb_state* M, mrb_value context );
	static mrb_value			_setVertexBuffer			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	bool				AffirmVertexBuffers			( u32 idx );
	void				BindVertexArrayItems		();
	bool				OnCPUCreate					(); // load or initialize any CPU-side resources required to create the GPU-side resource
	void				OnCPUDestroy				(); // clear any CPU-side memory used by class
	void				OnGPUBind					(); // select GPU-side resource on device for use
	bool				OnGPUCreate					(); // create GPU-side resource
	void				OnGPUDeleteOrDiscard		( bool shouldDelete ); // delete or discard GPU resource
	void				OnGPUUnbind					(); // unbind GPU-side resource
	bool				OnGPUUpdate					();
	void				UnbindVertexArrayItems		();

public:
	
	DECL_RUBY_FACTORY ( MOAIVertexArray, MOAIInstanceEventSource )
	
	IS ( UsingVAOs, mUseVAOs, true )
	
	//----------------------------------------------------------------//
	MOAIVertexBuffer*	GetVertexBuffer				( u32 idx );
	MOAIVertexFormat*	GetVertexFormat				( u32 idx );
						MOAIVertexArray				();
						~MOAIVertexArray			();
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void				ReserveVAOs					( u32 total );
	void				ReserveVertexBuffers		( u32 total );
	void				SerializeIn					( MOAIRubyState& state, MOAIDeserializer& serializer );
	void				SerializeOut				( MOAIRubyState& state, MOAISerializer& serializer );
	void				SetVertexBuffer				( u32 idx, MOAIVertexBuffer* vtxBuffer, MOAIVertexFormat* vtxFormat );
};

#endif
