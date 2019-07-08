// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAITEXTSTYLE_H
#define	MOAITEXTSTYLE_H

#include <moai-sim/MOAINode.h>

class MOAIFont;
class MOAIShader;

//================================================================//
// MOAITextStyleState
//================================================================//
class MOAITextStyleState {
protected:

	friend class MOAITextLabel;
	friend class MOAITextLayoutEngine;
	friend class MOAITextLayout;
	friend class MOAITextStyle;
	friend class MOAITextStyleParser;
	friend class MOAITextStyleCache;
	friend class MOAITextStyleMap;

	MOAIFont*		mFont;
	MOAIShader*		mShader;
	float			mSize;
	ZLVec2D			mScale;
	u32				mColor;
	ZLRect			mPadding;

public:

	GET ( MOAIFont*, Font, mFont );
	GET_SET ( float, Size, mSize );
	GET_SET ( u32, Color, mColor );

	//----------------------------------------------------------------//
	void				AffirmGlyph				( u32 c );
	virtual void		Init					( MOAITextStyleState& style );
						MOAITextStyleState		();
	virtual				~MOAITextStyleState		();
	bool				NeedsLayout				( const MOAITextStyleState& compare ) const;
	virtual void		SetFont					( MOAIFont* font );
	virtual void		SetShader				( MOAIShader* shader );
};

//================================================================//
// MOAITextStyle
//================================================================//
/**	@lua	MOAITextStyle
	@text	Represents a style that may be applied to a text box or a
			section of text in a text box using a style escape.
*/
class MOAITextStyle :
	public virtual MOAINode,
	public MOAITextStyleState {
private:

	friend class MOAITextLabel;
	friend class MOAITextLayoutEngine;
	friend class MOAITextLayout;
	friend class MOAITextStyleParser;
	friend class MOAITextStyleCache;
	friend class MOAITextStyleMap;

	//----------------------------------------------------------------//
	static mrb_value		_getColor				( mrb_state* M, mrb_value context );
	static mrb_value		_getFont				( mrb_state* M, mrb_value context );
	static mrb_value		_getScale				( mrb_state* M, mrb_value context );
	static mrb_value		_getSize				( mrb_state* M, mrb_value context );
	static mrb_value		_setColor				( mrb_state* M, mrb_value context );
	static mrb_value		_setFont				( mrb_state* M, mrb_value context );
	static mrb_value		_setShader				( mrb_state* M, mrb_value context );
	static mrb_value		_setPadding				( mrb_state* M, mrb_value context );
	static mrb_value		_setScale				( mrb_state* M, mrb_value context );
	static mrb_value		_setSize				( mrb_state* M, mrb_value context );
	
public:

	DECL_RUBY_FACTORY ( MOAITextStyle, MOAINode )
	
	//----------------------------------------------------------------//
	void			Init					( MOAITextStyleState& style );
					MOAITextStyle			();
					~MOAITextStyle			();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void			SerializeIn				( MOAIRubyState& state, MOAIDeserializer& serializer );
	void			SerializeOut			( MOAIRubyState& state, MOAISerializer& serializer );
	void			SetFont					( MOAIFont* font );
	void			SetShader				( MOAIShader* shader );
};

#endif
