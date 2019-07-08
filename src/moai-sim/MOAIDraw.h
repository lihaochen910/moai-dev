// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIDRAW_H
#define	MOAIDRAW_H

#include <moai-sim/MOAIDrawShapeImmediate.h>

class MOAIAnimCurve;
class MOAITextureBase;
class MOAITexture;
class MOAIFont;
class MOAIGlyph;
class MOAIGfxBuffer;
class MOAIVertexFormat;

//================================================================//
// MOAIDraw
//================================================================//
/**	@lua	MOAIDraw
	@text	Singleton for performing immediate mode drawing operations.
			See MOAIDrawDeck.
*/
class MOAIDraw :
	public ZLContextClass < MOAIDraw, MOAIRubyObject >,
	public MOAIDrawShapeImmediate {
private:

	//----------------------------------------------------------------//
	static mrb_value			_bind					( mrb_state* M, mrb_value context );
	static mrb_value			_bindFrameBuffer		( mrb_state* M, mrb_value context );
	static mrb_value			_bindIndexBuffer		( mrb_state* M, mrb_value context );
	static mrb_value			_bindShader				( mrb_state* M, mrb_value context );
	static mrb_value			_bindTexture			( mrb_state* M, mrb_value context );
	static mrb_value			_bindVertexArray		( mrb_state* M, mrb_value context );
	static mrb_value			_bindVertexBuffer		( mrb_state* M, mrb_value context );
	static mrb_value			_bindVertexFormat		( mrb_state* M, mrb_value context );
	static mrb_value			_clear					( mrb_state* M, mrb_value context );
	static mrb_value			_drawAnimCurve			( mrb_state* M, mrb_value context );
	static mrb_value			_drawAxisGrid			( mrb_state* M, mrb_value context );
	static mrb_value			_drawBezierCurve		( mrb_state* M, mrb_value context );
	static mrb_value			_drawBoxOutline			( mrb_state* M, mrb_value context );
	static mrb_value			_drawCircle				( mrb_state* M, mrb_value context );
	static mrb_value			_drawCircleSpokes		( mrb_state* M, mrb_value context );
	static mrb_value			_drawElements			( mrb_state* M, mrb_value context );
	static mrb_value			_drawEllipse			( mrb_state* M, mrb_value context );
	static mrb_value			_drawEllipseSpokes		( mrb_state* M, mrb_value context );
	static mrb_value			_drawGrid				( mrb_state* M, mrb_value context );
	static mrb_value			_drawLine				( mrb_state* M, mrb_value context );
	static mrb_value			_drawPoints				( mrb_state* M, mrb_value context );
	static mrb_value			_drawRay				( mrb_state* M, mrb_value context );
	static mrb_value			_drawRect				( mrb_state* M, mrb_value context );
	static mrb_value			_drawTexture			( mrb_state* M, mrb_value context );
	static mrb_value			_drawText				( mrb_state* M, mrb_value context );
	static mrb_value			_fillCircle				( mrb_state* M, mrb_value context );
	static mrb_value			_fillEllipse			( mrb_state* M, mrb_value context );
	static mrb_value			_fillFan				( mrb_state* M, mrb_value context );
	static mrb_value			_fillRect				( mrb_state* M, mrb_value context );
	static mrb_value			_fillRoundedRect		( mrb_state* M, mrb_value context );
	static mrb_value			_getPenColor			( mrb_state* M, mrb_value context );
	static mrb_value			_popGfxState			( mrb_state* M, mrb_value context );
	static mrb_value			_pushGfxState			( mrb_state* M, mrb_value context );
	static mrb_value			_setBlendMode			( mrb_state* M, mrb_value context );
	static mrb_value			_setClearColor			( mrb_state* M, mrb_value context );
	static mrb_value			_setClearDepth			( mrb_state* M, mrb_value context );
	static mrb_value			_setCullMode			( mrb_state* M, mrb_value context );
	static mrb_value			_setDefaultTexture		( mrb_state* M, mrb_value context );
	static mrb_value			_setDepthFunc			( mrb_state* M, mrb_value context );
	static mrb_value			_setMatrix				( mrb_state* M, mrb_value context );
	static mrb_value			_setPenColor			( mrb_state* M, mrb_value context );
	static mrb_value			_setPenWidth			( mrb_state* M, mrb_value context );
	static mrb_value			_setScissorRect			( mrb_state* M, mrb_value context );
	static mrb_value			_setViewProj			( mrb_state* M, mrb_value context );
	static mrb_value			_setViewRect			( mrb_state* M, mrb_value context );
	static mrb_value			_strokeRoundedRect		( mrb_state* M, mrb_value context );
	
	//----------------------------------------------------------------//
	void			DrawRubyParams			( mrb_state* M, mrb_value context, u32 primType );
	void			DrawRubyArray			( mrb_state* M, mrb_value context, u32 primType );
	
public:

	DECL_RUBY_SINGLETON ( MOAIDraw )

	//----------------------------------------------------------------//
	void			BeginDrawString			( float scale, MOAIFont& font, float fontSize, float shadowOffsetX, float shadowOffsetY );
	bool			Bind					();
	void			DrawAnimCurve			( const MOAIAnimCurve& curve, u32 resolution );
	void			DrawAxisGrid			( ZLVec2D loc, ZLVec2D vec, float size );
	void			DrawBezierCurve			( const ZLCubicBezier2D& bezier );
	void			DrawElements			( MOAIGfxBuffer* vtxBuffer, MOAIVertexFormat* vtxFormat, u32 count );
	void			DrawGrid				( const ZLRect& rect, u32 xCells, u32 yCells );
	void			DrawRay					( float x, float y, float dx, float dy );
	void			DrawString				( cc8* text, float x, float y, float width, float height );
	void			DrawString				( cc8* text, float x, float y, float scale, MOAIFont& font, float fontSize, float shadowOffsetX, float shadowOffsetY, float width, float height );
	void			DrawTexture				( float left, float top, float right, float bottom, MOAITexture* texture );
	void			DrawVertexArray			( const ZLVec3D* verts, u32 count, u32 color, u32 primType );
	void			DrawVertexArray2D		( const float* verts, u32 count, u32 color, u32 primType );
	void			EndDrawString			();
					MOAIDraw				();
					~MOAIDraw				();
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
};

#endif
