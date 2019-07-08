// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIFREETYPEFONTREADER_H
#define	MOAIFREETYPEFONTREADER_H

#if MOAI_WITH_FREETYPE

#include <contrib/moai_utf8.h>

#include <moai-sim/MOAIFontReader.h>

typedef int* MOAIFtLibrary;
typedef int* MOAIFtFace;

//================================================================//
// MOAIFreeTypeFontReader
//================================================================//
/**	@lua	MOAIFreeTypeFontReader
	@text	Implementation of MOAIFontReader that based on FreeType 2.
			Can load and render TTF and OTF font files.
*/
class MOAIFreeTypeFontReader :
	public MOAIFontReader {
private:

	u32				mGlyphCode;
	float			mFaceSize;

	MOAIFtLibrary	mLibrary;
	MOAIFtFace		mFace;
	float			mFaceHeight;

	bool			mAntiAlias;
	ZLColorVec		mPenColor;

	//----------------------------------------------------------------//
	static mrb_value		_enableAntiAliasing			( mrb_state* M, mrb_value context );
	static mrb_value		_extractSystemFont			( mrb_state* M, mrb_value context );
	static mrb_value		_setPenColor				( mrb_state* M, mrb_value context );
	static mrb_value		_strokeGlyph				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	static void		ExtractSystemFont			( cc8* fontName, ZLStream& stream );

public:
	
	DECL_RUBY_FACTORY ( MOAIFreeTypeFontReader, MOAIRubyObject )

	//----------------------------------------------------------------//
	int				CloseFontFile				();
	int				GetFaceMetrics				( MOAIFontFaceMetrics& faceMetrics );
	int				GetGlyphMetrics				( MOAIGlyphMetrics& glyphMetrics );
	int				GetKernVec					( u32 c, MOAIKernVec& kernVec );
	bool			HasKerning					();
					MOAIFreeTypeFontReader		();
					~MOAIFreeTypeFontReader		();
	int				OpenFontFile				( cc8* filename );
	void			RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	int				RenderGlyph					( MOAIImage& image, float x, float y );
	int				SelectFace					( float size );
	int				SelectGlyph					( u32 c );
	void			SerializeIn					( MOAIRubyState& state, MOAIDeserializer& serializer );
	void			SerializeOut				( MOAIRubyState& state, MOAISerializer& serializer );
	int				StrokeGlyph					( MOAIImage& image, float x, float y, float strokeSize, u32 capStyle, u32 joinStyle, float miterLimit );
};

#endif
#endif
