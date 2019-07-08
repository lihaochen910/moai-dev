// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIVIEWPORT_H
#define	MOAIVIEWPORT_H

//================================================================//
// MOAIViewport
//================================================================//
/**	@lua	MOAIViewport
	@text	Viewport object.
*/
class MOAIViewport :
	public virtual MOAIRubyObject,
	public ZLRect {
private:

	bool		mXScale;
	bool		mYScale;
	
	ZLVec2D		mScale;
	ZLVec2D		mOffset;
	float		mRotation;

	//----------------------------------------------------------------//
	static mrb_value		_getFrame		( mrb_state* M, mrb_value context );
	static mrb_value		_getSize		( mrb_state* M, mrb_value context );
	static mrb_value		_setOffset		( mrb_state* M, mrb_value context );
	static mrb_value		_setRotation	( mrb_state* M, mrb_value context );
	static mrb_value		_setScale		( mrb_state* M, mrb_value context );
	static mrb_value		_setSize		( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_FACTORY ( MOAIViewport, MOAIRubyObject )
	
	friend class MOAICamera;
	
	//----------------------------------------------------------------//
	float			GetAspect				() const;
	float			GetInvAspect			() const;
	ZLMatrix4x4		GetNormToWndMtx			() const;
	ZLMatrix4x4		GetProjMtx				() const;
	ZLMatrix4x4		GetProjMtxInv			() const;
	ZLRect			GetRect					() const;
	ZLVec2D			GetScale				() const;
	ZLVec2D			GetUnits				() const;
	ZLMatrix4x4		GetWndToNormMtx			() const;
					MOAIViewport			();
					~MOAIViewport			();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			SetOffset				( float xOffset, float yOffset );
	void			SetRotation				( float degrees );
	void			SetScale				( float xScale, float yScale );
};

#endif
