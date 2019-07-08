// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIRENDERMGR_H
#define	MOAIRENDERMGR_H

//================================================================//
// MOAIRenderMgr
//================================================================//
// TODO: doxygen
class MOAIRenderMgr :
	public ZLContextClass < MOAIRenderMgr, MOAIRubyObject > {
private:

	u32				mRenderCounter;	// increments every render
	double			mRenderDuration;
	double			mRenderTime;
	
	MOAIRubyStrongRef	mRenderRoot;
	
	//----------------------------------------------------------------//
	static mrb_value		_getRenderCount				( mrb_state* M, mrb_value context );
	static mrb_value		_getRender					( mrb_state* M, mrb_value context );
	static mrb_value		_setRender					( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void			RenderTable					( MOAIRubyState& state, int idx );

public:

	DECL_RUBY_SINGLETON ( MOAIRenderMgr )
	
	GET ( u32, RenderCounter, mRenderCounter )
	GET ( double, RenderDuration, mRenderDuration )
	GET ( double, RenderTime, mRenderTime )
	
	//----------------------------------------------------------------//
					MOAIRenderMgr				();
					~MOAIRenderMgr				();
	void			PushDrawable				( MOAIRubyObject* drawable );
	void			RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void			Render						();
};

#endif
