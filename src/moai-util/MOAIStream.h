// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAISTREAM_H
#define MOAISTREAM_H

//================================================================//
// MOAIStream
//================================================================//
/**	@lua	MOAIStream
	@text	Interface for reading/writing binary data.
	
	@const	SEEK_CUR
	@const	SEEK_END
	@const	SEEK_SET
*/
class MOAIStream :
	public virtual ZLStream,
	public virtual MOAIRubyObject {
private:
	
	enum {
		UNKNOWN,
		SIGNED,
		UNSIGNED,
		INT_8,
		INT_16,
		INT_32,
		DOUBLE,
		FLOAT,
		UINT_8,
		UINT_16,
		UINT_32,
	};
	
	//----------------------------------------------------------------//
	static mrb_value		_collapse			( mrb_state* M, mrb_value context );
	static mrb_value		_compact			( mrb_state* M, mrb_value context );
	static mrb_value		_flush				( mrb_state* M, mrb_value context );
	static mrb_value		_getCursor			( mrb_state* M, mrb_value context );
	static mrb_value		_getLength			( mrb_state* M, mrb_value context );
	static mrb_value		_read				( mrb_state* M, mrb_value context );
	static mrb_value		_read8				( mrb_state* M, mrb_value context );
	static mrb_value		_read16				( mrb_state* M, mrb_value context );
	static mrb_value		_read32				( mrb_state* M, mrb_value context );
	static mrb_value		_readBoolean		( mrb_state* M, mrb_value context );
	static mrb_value		_readDouble			( mrb_state* M, mrb_value context );
	static mrb_value		_readFloat			( mrb_state* M, mrb_value context );
	static mrb_value		_readFormat			( mrb_state* M, mrb_value context );
	static mrb_value		_readString			( mrb_state* M, mrb_value context );
	static mrb_value		_readU8				( mrb_state* M, mrb_value context );
	static mrb_value		_readU16			( mrb_state* M, mrb_value context );
	static mrb_value		_readU32			( mrb_state* M, mrb_value context );
	static mrb_value		_sample				( mrb_state* M, mrb_value context );
	static mrb_value		_seek				( mrb_state* M, mrb_value context );
	static mrb_value		_write				( mrb_state* M, mrb_value context );
	static mrb_value		_write8				( mrb_state* M, mrb_value context );
	static mrb_value		_write16			( mrb_state* M, mrb_value context );
	static mrb_value		_write32			( mrb_state* M, mrb_value context );
	static mrb_value		_writeBoolean		( mrb_state* M, mrb_value context );
	static mrb_value		_writeColor32		( mrb_state* M, mrb_value context );
	static mrb_value		_writeDouble		( mrb_state* M, mrb_value context );
	static mrb_value		_writeFloat			( mrb_state* M, mrb_value context );
	static mrb_value		_writeFormat		( mrb_state* M, mrb_value context );
	static mrb_value		_writeStream		( mrb_state* M, mrb_value context );
	static mrb_value		_writeString		( mrb_state* M, mrb_value context );
	static mrb_value		_writeU8			( mrb_state* M, mrb_value context );
	static mrb_value		_writeU16			( mrb_state* M, mrb_value context );
	static mrb_value		_writeU32			( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	static cc8*		ParseTypeToken		( cc8* format, u32& type );
	int				ReadFormat			( MOAIRubyState& state, int idx );
	int				WriteFormat			( MOAIRubyState& state, int idx );

	//----------------------------------------------------------------//
	template < typename TYPE >
	size_t ReadValue ( MOAIRubyState& state ) {
		
		TYPE value;
		size_t size = sizeof ( TYPE );
		size_t bytes = this->ReadBytes ( &value, size );
		if ( bytes == size ) {
			//state.Push ( value );
		}
		else {
			//state.Push ();
		}
		return bytes;
	}

	//----------------------------------------------------------------//
	template < typename TYPE >
	mrb_value ReadValues ( MOAIRubyState& state, int idx ) {
		
		size_t base = this->GetCursor ();
		
		u32 total = state.GetParamValue < u32 >( idx, 1 );
		u32 count = 0;
		mrb_value ary = mrb_ary_new ( state );
		
		for ( u32 i = 0; i < total; ++i ) {
		
			ZLResult < TYPE > result = this->Read < TYPE >();
			
			if ( result.mCode == ZL_OK ) {
				mrb_ary_push ( state, ary, state.ToRValue ( result.mValue ) );
				count++;
			}
			else {
				// TODO: report errors
				break;
			} 
		}
		//state.Push (( u32 )( this->GetCursor () - base )); // TODO: overflow?
		//return count + 1;
		return ary;
	}
	
	//----------------------------------------------------------------//
	template < typename TYPE >
	int WriteValues ( MOAIRubyState& state, int idx ) {
		
		size_t base = this->GetCursor ();
		
		u32 total = state.GetParamsCount ();
		
		for ( u32 i = 0; i < total; ++i ) {
		
			TYPE value = state.GetParamValue < TYPE >( idx + i, 0 );
			ZLSizeResult result = this->Write < TYPE >( value );

			if ( result.mCode != ZL_OK ) {
				// TODO: report errors
				break;
			}
		}
		return ( u32 )( this->GetCursor () - base );
	}

public:

	//----------------------------------------------------------------//
					MOAIStream			();
					~MOAIStream			();
	void			RegisterRubyClass	( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs	( MOAIRubyState& state, RClass* klass );
};

#endif
