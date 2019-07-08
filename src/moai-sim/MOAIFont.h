// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIFONT_H
#define	MOAIFONT_H

#include <moai-sim/MOAIGlyphSet.h>
#include <moai-sim/MOAISpanList.h>

class MOAIFontReader;
class MOAIGlyph;
class MOAIGlyphCache;
class MOAIShader;
class MOAITextureBase;
class MOAITexture;

#define DPI 72
#define POINTS_TO_PIXELS(points,dpi) (( points * dpi ) / DPI )
#define PIXELS_TO_POINTS(pixels,dpi) (( pixels * DPI ) / dpi )

//================================================================//
// MOAIFont
//================================================================//
/**	@lua	MOAIFont
	@text	<p>MOAIFont is the top level object for managing sets of
			glyphs associated with a single font face. An instance of
			MOAIFont may contain glyph sets for multiple sizes of the font.
			Alternatively, a separate instance of MOAIFont may be used for each
			font size. Using a single font object for each size of a font face
			can make it easier to unload font sizes that are no longer needed.</p>
			
			<p>An instance of MOAIFont may represent a dynamic or static font. Dynamic fonts
			are used to retrieve glyphs from a font file format on an as-needed basis.
			Static fonts have no associated font file format and therefore contain a fixed
			set of glyphs at runtime. For languages demanding very large character sets
			(such as Chinese), dynamic fonts are typically used. For languages where it is
			feasible to pre-render a full set of glyphs to texture (or bitmap fonts), static
			fonts may be used.</p>
			
			<p>MOAIFont orchestrates objects derived from MOAIFontReader and MOAIGlyphCache
			to render glyphs into glyph sets. MOAIFontReader is responsible for interpreting
			the font file format (if any), retrieving glyph metrics (including kerning)
			and rendering glyphs to texture. MOAIDynamicGlyphCache is responsible for
			allocating textures to hold glyphs and for managing glyph placement within
			textures. For dynamic fonts, the typical setup uses MOAIFreeTypeFontReader
			and MOAIDynamicGlyphCache. For static fonts, there is usually no font reader;
			MOAIStaticGlyphCache is loaded directly from a serialized file and its texture
			memory is initialized with MOAIFont's setImage () command.</p>
			
			<p>As mentioned, a single MOAIFont may be used to render multiple sizes of a font
			face. When glyphs need to be laid out or rendered, the font object will return
			a set of glyphs matching whatever size was requested. It is also possible to specify
			a default size that will be used if no size is requested for rendering or if no
			matching size is found. If no default size is set by the user, it will be set
			automatically the first time a specific size is requested.</p>
			
			<p>MOAIFont can also control how or if kerning tables are loaded when glyphs are
			being rendered. The default behavior is to load kerning information automatically.
			It is possible to prevent kerning information from being loaded. In this case,
			kerning tables may be loaded manually if so desired.</p>
	
	@const	FONT_AUTOLOAD_KERNING
	@const	DEFAULT_FLAGS
*/
class MOAIFont :
	public virtual MOAIRubyObject,
	public MOAIInstanceEventSource {
protected:

	STLString mFilename;
	u32 mFlags;
	
	MOAIRubySharedPtr < MOAIFontReader > mReader;
	MOAIRubySharedPtr < MOAIGlyphCache > mCache;
	MOAIRubySharedPtr < MOAIShader > mShader;
	
	// for now
	typedef STLMap < float, MOAIGlyphSet >::iterator GlyphSetsIt;
	STLMap < float, MOAIGlyphSet > mGlyphSets;

	float mDefaultSize;

	int	mMinFilter;
	int	mMagFilter;

	//----------------------------------------------------------------//
	static mrb_value			_getCache				( mrb_state* M, mrb_value context );
	static mrb_value			_getDefaultSize         ( mrb_state* M, mrb_value context );
	static mrb_value			_getFilename			( mrb_state* M, mrb_value context );
	static mrb_value			_getFlags				( mrb_state* M, mrb_value context );
	static mrb_value			_getImage				( mrb_state* M, mrb_value context );
	static mrb_value			_getReader				( mrb_state* M, mrb_value context );
	static mrb_value			_load					( mrb_state* M, mrb_value context );
	static mrb_value			_loadFromBMFont			( mrb_state* M, mrb_value context );
	static mrb_value			_preloadGlyphs			( mrb_state* M, mrb_value context );
	static mrb_value			_rebuildKerningTables	( mrb_state* M, mrb_value context );
	static mrb_value			_setCache				( mrb_state* M, mrb_value context );
	static mrb_value			_setDefaultSize			( mrb_state* M, mrb_value context );
	static mrb_value			_setFilter				( mrb_state* M, mrb_value context );
	static mrb_value			_setFlags				( mrb_state* M, mrb_value context );
	static mrb_value			_setImage				( mrb_state* M, mrb_value context );
	static mrb_value			_setReader				( mrb_state* M, mrb_value context );
	static mrb_value			_setShader				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	#ifdef DOXYGEN
		static mrb_value		_loadFromTTF			( mrb_state* M, mrb_value context );
	#endif

	//----------------------------------------------------------------//
	void				BuildKerning			( MOAIGlyph* glyphs, MOAIGlyph* pendingGlyphs );
	void				RebuildKerning			( MOAIGlyphSet& glyphSet );
	void				RenderGlyph				( MOAIGlyph& glyph );

public:
	
	DECL_RUBY_FACTORY ( MOAIFont, MOAIInstanceEventSource )
	
	GET ( cc8*, Filename, mFilename );
	GET ( MOAIGlyphCache*, Cache, mCache );

	GET ( int, MinFilter, mMinFilter );
	GET ( int, MagFilter, mMagFilter );

	GET ( MOAIShader*, Shader, mShader );

	GET_SET ( float, DefaultSize, mDefaultSize );
	
	enum {
		FONT_AUTOLOAD_KERNING		= 0x01,
	};
	
	enum {
		EVENT_RENDER_GLYPH,
	};
	
	static const u32 DEFAULT_FLAGS = FONT_AUTOLOAD_KERNING;
	
	//----------------------------------------------------------------//
	void				AffirmGlyph				( float size, u32 c );
    MOAIGlyphSet&		AffirmGlyphSet			( float size );
	MOAIGlyphSet*		GetGlyphSet				( float size );
	MOAITextureBase*	GetGlyphTexture			( MOAIGlyph& glyph );
	void				Init					( cc8* filename );
	void				InitWithBMFont			( cc8* filename, const u32 numPreloadedTextures, MOAITexture** preloadedTextures );
	static bool			IsControl				( u32 c );
	static bool			IsWhitespace			( u32 c );
						MOAIFont				();
						~MOAIFont				();
	void				ProcessGlyphs			();
	void				RebuildKerning			();
	void				RebuildKerning			( float size );
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	void				ResizePageList			( u32 size );
	void				SerializeIn				( MOAIRubyState& state, MOAIDeserializer& serializer );
	void				SerializeOut			( MOAIRubyState& state, MOAISerializer& serializer );
};

#endif
