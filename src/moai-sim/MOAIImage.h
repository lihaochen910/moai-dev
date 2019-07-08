// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIIMAGE_H
#define	MOAIIMAGE_H

//================================================================//
// MOAIImageTransform
//================================================================//
namespace MOAIImageTransform {
	enum Flags {
		POW_TWO				= 0x01,
		QUANTIZE			= 0x02,
		TRUECOLOR			= 0x04,
		PREMULTIPLY_ALPHA	= 0x08,
	};
}

//================================================================//
// MOAIImageBlendMode
//================================================================//
class MOAIImageBlendMode :
	public ZLColorBlendFunc {
public:

	//----------------------------------------------------------------//
	void		Clear					();
	void		Init					( MOAIRubyState& state, int idx );
				MOAIImageBlendMode		();
};

//================================================================//
// MOAIImage
//================================================================//
/**	@lua	MOAIImage
	@text	Image/bitmap class.
	
	@const	FILTER_LINEAR
	@const	FILTER_NEAREST
	
	@flag	POW_TWO				Adds padding at the right and bottom to make the image dimensions powers of 2.
	@flag	QUANTIZE			Uses less than 8 bits per channel to reduce memory consumption.
	@flag	TRUECOLOR			Converts palettized color formats to true color.
	@flag	PREMULTIPLY_ALPHA	Premultiplies the pixel colors with their alpha values.
	
	@flag	PIXEL_FMT_TRUECOLOR
	@flag	PIXEL_FMT_INDEX_4
	@flag	PIXEL_FMT_INDEX_8
	
	@flag	COLOR_FMT_A_1			Alpha only, 1 bit per pixel
	@flag	COLOR_FMT_A_4			Alpha only, 4 bits per pixel
	@flag	COLOR_FMT_A_8			Alpha only, 8 bits per pixel
	@flag	COLOR_FMT_LA_8			Grayscale + alpha, 16 bits per pixel
	@flag	COLOR_FMT_RGB_888		RGB, 24 bits per pixel
	@flag	COLOR_FMT_RGB_565		RGB, 16 bits per pixel
	@flag	COLOR_FMT_RGBA_5551		RGBA, 16 bits per pixel (1 bit alpha)
	@flag	COLOR_FMT_RGBA_4444		RGBA, 16 bits per pixel (4 bits per channel)
	@flag	COLOR_FMT_RGBA_8888		RGBA, 32 bits per pixel
*/
class MOAIImage :
	public virtual MOAIRubyObject {
public:

	enum PixelFormat {
		TRUECOLOR,
		INDEX_4,
		INDEX_8,
		PXL_FMT_UNKNOWN,
	};

private:

	friend class MOAIImageFormat;

	PixelFormat				mPixelFormat;
	ZLColor::ColorFormat	mColorFormat;

	u32		mWidth;
	u32		mHeight;
	
	ZLCopyOnWrite	mBitmap;
	ZLCopyOnWrite	mPalette;

	SET ( PixelFormat, PixelFormat, mPixelFormat )
	SET ( ZLColor::ColorFormat, ColorFormat, mColorFormat )

	SET ( u32, Width, mWidth )
	SET ( u32, Height, mHeight )

	//----------------------------------------------------------------//
	static mrb_value		_average					( mrb_state* M, mrb_value context );
	static mrb_value		_bleedRect					( mrb_state* M, mrb_value context );
	static mrb_value		_calculateGaussianKernel	( mrb_state* M, mrb_value context );
	static mrb_value		_compare					( mrb_state* M, mrb_value context );
	static mrb_value		_convert					( mrb_state* M, mrb_value context );
	static mrb_value		_convolve					( mrb_state* M, mrb_value context );
	static mrb_value		_convolve1D					( mrb_state* M, mrb_value context );
	static mrb_value		_copy						( mrb_state* M, mrb_value context );
	static mrb_value		_copyBits					( mrb_state* M, mrb_value context );
	static mrb_value		_copyRect					( mrb_state* M, mrb_value context );
	static mrb_value		_desaturate					( mrb_state* M, mrb_value context );
	static mrb_value		_fillCircle					( mrb_state* M, mrb_value context );
	static mrb_value		_fillEllipse				( mrb_state* M, mrb_value context );
	static mrb_value		_fillRect					( mrb_state* M, mrb_value context );
	static mrb_value		_gammaCorrection			( mrb_state* M, mrb_value context );
	static mrb_value		_generateOutlineFromSDF		( mrb_state* M, mrb_value context );
	static mrb_value		_generateSDF				( mrb_state* M, mrb_value context );
	static mrb_value		_generateSDFAA				( mrb_state* M, mrb_value context );
	static mrb_value		_generateSDFDeadReckoning	( mrb_state* M, mrb_value context );
	static mrb_value		_getColor32					( mrb_state* M, mrb_value context );
	static mrb_value		_getContentRect				( mrb_state* M, mrb_value context );
	static mrb_value		_getData					( mrb_state* M, mrb_value context );
	static mrb_value		_getFormat					( mrb_state* M, mrb_value context );
	static mrb_value		_getRGBA					( mrb_state* M, mrb_value context );
	static mrb_value		_getSize					( mrb_state* M, mrb_value context );
	static mrb_value		_init						( mrb_state* M, mrb_value context );
	static mrb_value		_isOpaque					( mrb_state* M, mrb_value context );
	static mrb_value		_load						( mrb_state* M, mrb_value context );
	static mrb_value		_loadAsync					( mrb_state* M, mrb_value context );
	static mrb_value		_loadFromBuffer				( mrb_state* M, mrb_value context );
	static mrb_value		_mix						( mrb_state* M, mrb_value context );
	static mrb_value		_padToPow2					( mrb_state* M, mrb_value context );
	static mrb_value		_print						( mrb_state* M, mrb_value context );
	static mrb_value		_resize						( mrb_state* M, mrb_value context );
	static mrb_value		_resizeCanvas				( mrb_state* M, mrb_value context );
	static mrb_value		_setColor32					( mrb_state* M, mrb_value context );
	static mrb_value		_setRGBA					( mrb_state* M, mrb_value context );
	static mrb_value		_simpleThreshold			( mrb_state* M, mrb_value context );
	static mrb_value		_subdivideRect				( mrb_state* M, mrb_value context );
	static mrb_value		_write						( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	void			Alloc					();
	void			ComparePixel			( ZLIntVec2D** grid, ZLIntVec2D& p, int x, int y, int offsetX, int offsetY, int width, int height );
	void			CalculateSDF			( ZLIntVec2D** grid, int width, int height );
	const void*		GetRowAddr				( u32 y ) const;
	void*			GetRowAddrMutable		( u32 y );
	virtual void	OnImageStatusChanged	( bool isOK );

public:
	
	DECL_RUBY_FACTORY ( MOAIImage, MOAIRubyObject )
	
	GET_CONST ( PixelFormat, PixelFormat, mPixelFormat )
	GET_CONST ( ZLColor::ColorFormat, ColorFormat, mColorFormat )

	GET_CONST ( u32, Width, mWidth )
	GET_CONST ( u32, Height, mHeight )
	
	GET_CONST ( void*, Bitmap, mBitmap.GetBuffer ())
	GET_CONST ( void*, Palette, mPalette.GetBuffer ())
	
	GET ( ZLCopyOnWrite, BitmapCow, mBitmap )
	GET ( ZLCopyOnWrite, PaletteCow, mPalette )
	
	GET ( ZLSharedConstBuffer*, BitmapBuffer, mBitmap.GetSharedConstBuffer ())
	GET ( ZLSharedConstBuffer*, PaletteBuffer, mBitmap.GetSharedConstBuffer ())
	
	enum {
		FILTER_LINEAR,
		FILTER_NEAREST,
	};
	
	//----------------------------------------------------------------//
	static MOAIImage*		AffirmImage						( MOAIRubyState& state, int idx );
	ZLColorVec				Average							() const;
	void					BleedRect						( ZLIntRect rect );
	void					Blit							( const MOAIImage& image, int srcX, int srcY, int destX, int destY, int width, int height );
	void					Blur							();
	static void				CalculateGaussianKernel			( float radius, float* kernel, size_t kernalWidth );
	static void				CalculateGaussianKernel			( float radius, float sigma, float* kernel, size_t kernalWidth );
	static size_t			CalculateGaussianKernelWidth	( float radius );
	void					Clear							();
	void					ClearBitmap						();
	void					ClearRect						( ZLIntRect rect );
	bool					Compare							( const MOAIImage& image );
	bool					Convert							( const MOAIImage& image, ZLColor::ColorFormat colorFmt, PixelFormat pixelFmt );
	void					Convolve						( const MOAIImage& image, const float* kernel, size_t kernelWidth );
	void					Convolve						( const MOAIImage& image, const float* kernel, size_t kernelWidth, size_t kernelHeight );
	void					Convolve1D						( const MOAIImage& image, const float* kernel, size_t kernelSize, bool horizontal );
	void					Copy							( const MOAIImage& image );
	void					CopyRect						( const MOAIImage& image, ZLIntRect srcRect, ZLIntRect destRect, u32 filter = FILTER_LINEAR );
	void					CopyRect						( const MOAIImage& image, ZLIntRect srcRect, ZLIntRect destRect, u32 filter, const MOAIImageBlendMode& blendMode );
	void					Desaturate						( const MOAIImage& image, float rY, float gY, float bY, float K );
	void					DrawLine						( int p1x, int p1y, int p2x, int p2y, u32 color );
	void					FillCircle						( float x, float y, float xRad, u32 color );
	void					FillEllipse						( int x, int y, int xRad, int yRad, u32 color );
	void					FillRect						( ZLIntRect rect, u32 color );
	void					GammaCorrection					( const MOAIImage& image, float gamma );
	void					GenerateOutlineFromSDF			( ZLIntRect rect, float distMin, float distMax, float r, float g, float b, float a );
	void					GenerateSDF						( ZLIntRect rect );
	void					GenerateSDFAA					( ZLIntRect rect, float sizeInPixels );
	void					GenerateSDFDeadReckoning		( ZLIntRect rect, int threshold );
	size_t					GetBitmapSize					() const;
	ZLIntRect				GetBounds						();
	u32						GetColor						( u32 x, u32 y ) const;
	ZLIntRect				GetContentRect					();
	size_t					GetDataSize						() const;
	static u32				GetMinPowerOfTwo				( u32 size ); // gets the smallest power of two greater than size
	u32						GetPaletteColor					( u32 idx ) const;
	u32						GetPaletteCount					() const;
	size_t					GetPaletteSize					() const;
	u32						GetPixel						( u32 x, u32 y ) const;
	u32						GetPixelDepthInBits				() const;
	u32						GetPixelMask					() const;
	ZLIntRect				GetRect							();
	size_t					GetRowSize						() const;
	void					GetSubImage						( const MOAIImage& image, ZLIntRect rect );
	void					Init							( const MOAIImage& image );
	void					Init							( u32 width, u32 height, ZLColor::ColorFormat colorFmt, PixelFormat pixelFmt );
	void					Init							( const void* bitmap, u32 width, u32 height, ZLColor::ColorFormat colorFmt );
	bool					IsPow2							();
	static bool				IsPow2							( u32 n );
	bool					Load							( cc8* filename, u32 transform = 0 );
	bool					Load							( ZLStream& stream, u32 transform = 0 );
	bool					IsOK							();
	bool					MipReduce						();
	void					Mix								( const MOAIImage& image, const ZLMatrix4x4& mtx, float K );
							MOAIImage						();
							~MOAIImage						();
	void					PadToPow2						( const MOAIImage& image );
	void					PremultiplyAlpha				( const MOAIImage& image );
	void					Print							();
	void					RegisterRubyClass				( MOAIRubyState& state, RClass* klass );
	void					RegisterRubyFuncs				( MOAIRubyState& state, RClass* klass );
	void					ResizeCanvas					( const MOAIImage& image, ZLIntRect rect );
	u32						SampleColor						( float x, float y, u32 filter, bool wrapX = false, bool wrapY = false ) const;
	void					SerializeIn						( MOAIRubyState& state, MOAIDeserializer& serializer );
	void					SerializeOut					( MOAIRubyState& state, MOAISerializer& serializer );
	void					SetColor						( u32 x, u32 y, u32 color );
	void					SetColor						( u32 x, u32 y, u32 color, const MOAIImageBlendMode& blendMode );
	void					SetPaletteColor					( u32 idx, u32 rgba );
	void					SetPixel						( u32 x, u32 y, u32 pixel );
	void					SimpleThreshold					( const MOAIImage& image, float rT, float gT, float bT, float aT );
	void					Take							( MOAIImage& image );
	void					Transform						( u32 transform );
	bool					Write							( ZLStream& stream, cc8* formatName );
	
	//----------------------------------------------------------------//
	// TODO: move this somewhere more appropriate
	static inline double Gaussian ( double x, double c ) {
	
		return Gaussian ( x, GaussianUnitIntegral ( c ), 0.0, c );
	}
	
	//----------------------------------------------------------------//
	// TODO: move this somewhere more appropriate
	static inline double Gaussian ( double x, double b, double c ) {
	
		return Gaussian ( x, GaussianUnitIntegral ( c ), b, c );
	}
	
	//----------------------------------------------------------------//
	// TODO: move this somewhere more appropriate
	static inline double Gaussian ( double x, double a, double b, double c ) {
	
		return a * exp ( -((( x - b ) * ( x - b )) / ( 2.0 * ( c * c ))));
	}
	
	//----------------------------------------------------------------//
	// TODO: move this somewhere more appropriate
	static inline double GaussianUnitIntegral ( double c ) {
	
		return 1.0 / ( c * sqrt ( TWOPI ));
	}
};

#endif
