// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIFRAMEBUFFER_H
#define	MOAIFRAMEBUFFER_H

class MOAIColor;
class MOAIImage;

//================================================================//
// MOAIFrameBuffer
//================================================================//
/**	@lua	MOAIFrameBuffer
	@text	MOAIFrameBuffer is responsible for drawing a list of MOAIRenderable
			objects. MOAIRenderable is the base class for any object that can be
			drawn. This includes MOAIProp and MOAIPartitionViewLayer. To use MOAIFrameBuffer
			pass a table of MOAIRenderable objects to setRenderTable ().
			The table will usually be a stack of MOAIPartitionViewLayer objects. The contents of
			the table will be rendered the next time a frame is drawn. Note that the
			table must be an array starting with index 1. Objects will be rendered
			counting from the base index until 'nil' is encountered. The render
			table may include other tables as entries. These must also be arrays
			indexed from 1.
*/
class MOAIFrameBuffer :
	public virtual MOAIRubyObject,
	public virtual ZLGfxListener {
protected:
	
	friend class MOAIGfxMgr;
	friend class MOAIGfxStateGPUCache;
	
	u32					mBufferWidth;
	u32					mBufferHeight;
	float				mBufferScale;
	bool				mLandscape;
	
	bool				mNeedsClear;
	
	ZLGfxHandle			mGLFrameBuffer;

	bool								mGrabNextFrame;
	MOAIRubyMemberRef					mOnFrameFinish;
	MOAIRubySharedPtr < MOAIImage >		mFrameImage;

	//----------------------------------------------------------------//
	static mrb_value			_getGrabbedImage			( mrb_state* M, mrb_value context );
	static mrb_value			_grabNextFrame				( mrb_state* M, mrb_value context );
	static mrb_value			_isPendingGrab				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void				AffirmBuffers				();
	void				OnReadPixels				( const ZLCopyOnWrite& buffer, void* userdata );
	void				RenderTable					( MOAIRubyState& state, int idx );

	//----------------------------------------------------------------//
	virtual void		MOAIFrameBuffer_AffirmBuffers		();

public:
	
	DECL_RUBY_FACTORY ( MOAIFrameBuffer, MOAIRubyObject )
	
	GET_CONST	( u32, BufferWidth, mBufferWidth )
	GET_CONST	( u32, BufferHeight, mBufferHeight )
	GET_SET		( float, BufferScale, mBufferScale )
	GET_SET		( bool, Landscape, mLandscape )
	
	//----------------------------------------------------------------//
	void				DetectGLFrameBufferID		();
	ZLRect				GetBufferRect				() const;
	void				GrabImage					( MOAIImage* image );
						MOAIFrameBuffer				();
						~MOAIFrameBuffer			();
	bool				NeedsClear					() const;
	void				NeedsClear					( bool needsClear );
	void				SetBufferSize				( u32 width, u32 height );
	void				SetGLFrameBuffer			( const ZLGfxHandle& frameBuffer );
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	ZLRect				WndRectToDevice				( ZLRect rect ) const;
};

#endif
