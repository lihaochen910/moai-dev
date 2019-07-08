// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIDATABUFFER_H
#define MOAIDATABUFFER_H

#include <moai-util/MOAIDataBuffer.h>
#include <moai-util/MOAIMutex.h>

class MOAIDataIOTask;

//================================================================//
// MOAIDataBuffer
//================================================================//
/**	@lua	MOAIDataBuffer
	@text	Buffer for loading and holding data. Data operations may be
			performed without additional penalty of marshalling buffers
			between Ruby and C.
*/
class MOAIDataBuffer :
	public virtual MOAIRubyObject {
private:
	
	MOAIMutex			mMutex;
	ZLLeanArray < u8 >	mBytes;
	
	//----------------------------------------------------------------//
	static mrb_value		_base64Decode		( mrb_state* M, mrb_value context );
	static mrb_value		_base64Encode		( mrb_state* M, mrb_value context );
	static mrb_value		_clear				( mrb_state* M, mrb_value context );
	static mrb_value		_deflate			( mrb_state* M, mrb_value context );
	static mrb_value		_getSize			( mrb_state* M, mrb_value context );
	static mrb_value		_getString			( mrb_state* M, mrb_value context );
	static mrb_value		_hexDecode			( mrb_state* M, mrb_value context );
	static mrb_value		_hexEncode			( mrb_state* M, mrb_value context );
	static mrb_value		_inflate			( mrb_state* M, mrb_value context );
	static mrb_value		_load				( mrb_state* M, mrb_value context );
	static mrb_value		_loadAsync			( mrb_state* M, mrb_value context );
	static mrb_value		_save				( mrb_state* M, mrb_value context );
	static mrb_value		_saveAsync			( mrb_state* M, mrb_value context );
	static mrb_value		_setString			( mrb_state* M, mrb_value context );
	static mrb_value		_toCppHeader		( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	bool			Decode				( ZLStreamAdapter& reader );
	bool			Encode				( ZLStreamAdapter& writer );
	static bool		IsZipFilename		( cc8* filename );

public:
	
	DECL_RUBY_FACTORY ( MOAIDataBuffer, MOAIRubyObject )

	enum {
		NO_INFLATE,
		FORCE_INFLATE,
		INFLATE_ON_EXT,
	};

	//----------------------------------------------------------------//
	bool			Base64Decode			();
	bool			Base64Encode			();
	void			Clear					();
	bool			Deflate					( int level = ZLDeflateWriter::DEFAULT_LEVEL, int windowBits = ZLDeflateWriter::DEFAULT_WBITS );
	void*			GetBuffer				(); // NOTE: unsafe; bypasses the mutex
	bool			HexDecode				();
	bool			HexEncode				();
	bool			Inflate					( int windowBits = ZLDeflateWriter::DEFAULT_WBITS );
	bool			Load					( cc8* filename );
	void			Load					( void* bytes, size_t size );
	void			Load					( MOAIRubyState& state, int idx );
	void			Lock					( void** bytes, size_t* size );
					MOAIDataBuffer			();
					~MOAIDataBuffer			();
	mrb_value		PushString				( MOAIRubyState& state );
	size_t			Read					( void* buffer, size_t size );
	void			RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs		( MOAIRubyState& state, RClass* klass );
	size_t			Size					();
	bool			Save					( cc8* filename );
	void			Unlock					();
};

//================================================================//
// MOAIDataBufferScopedLock
//================================================================//
class MOAIDataBufferScopedLock {
private:

	MOAIDataBuffer&		mDataBuffer;
	void*				mBytes;
	size_t				mSize;

public:

	GET ( void*, Bytes, mBytes )
	GET ( size_t, Size, mSize )

	//----------------------------------------------------------------//
	MOAIDataBufferScopedLock ( MOAIDataBuffer& buffer ) :
		mDataBuffer ( buffer ) {
		
		this->mDataBuffer.Lock ( &this->mBytes, &this->mSize );
	}
	
	//----------------------------------------------------------------//
	MOAIDataBufferScopedLock ( MOAIDataBuffer& buffer, void** bytes, size_t* size ) :
		mDataBuffer ( buffer ) {
		
		this->mDataBuffer.Lock ( &this->mBytes, &this->mSize );
		
		if ( bytes ) {
			*bytes = this->mBytes;
		}
		
		if ( size ) {
			*size = this->mSize;
		}
	}
	
	//----------------------------------------------------------------//
	~MOAIDataBufferScopedLock () {
	
		this->mDataBuffer.Unlock ();
	}
};

#endif
