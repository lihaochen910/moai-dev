// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIUNTZSOUND_H
#define	MOAIUNTZSOUND_H

#include <moai-sim/headers.h>
#include <moai-untz/MOAIUntzSound.h>
#include <UntzSound.h>

class MOAIDataBuffer;

//================================================================//
// MOAIUntzSound
//================================================================//
/**	@lua	MOAIUntzSound
	@text	Untz sound object.
	
	@attr	ATTR_VOLUME
*/
class MOAIUntzSound :
	public virtual MOAINode {
private:

	UNTZ::Sound* mSound;
	STLString mFilename;
	bool mInMemory;

	//----------------------------------------------------------------//
	static mrb_value		_getLength			( mrb_state* M, mrb_value context );
	static mrb_value		_getPosition		( mrb_state* M, mrb_value context );
	static mrb_value		_getVolume			( mrb_state* M, mrb_value context );
	static mrb_value		_getFilename		( mrb_state* M, mrb_value context );
	static mrb_value		_isLooping			( mrb_state* M, mrb_value context );
	static mrb_value		_isPaused			( mrb_state* M, mrb_value context );
	static mrb_value		_isPlaying			( mrb_state* M, mrb_value context );
	static mrb_value		_load				( mrb_state* M, mrb_value context );
	static mrb_value		_moveVolume			( mrb_state* M, mrb_value context );
	static mrb_value		_pause				( mrb_state* M, mrb_value context );
	static mrb_value		_play				( mrb_state* M, mrb_value context );
	static mrb_value		_seekVolume			( mrb_state* M, mrb_value context );
	static mrb_value		_setLooping			( mrb_state* M, mrb_value context );
	static mrb_value		_setLoopPoints		( mrb_state* M, mrb_value context );
	static mrb_value		_setPosition		( mrb_state* M, mrb_value context );
	static mrb_value		_setVolume			( mrb_state* M, mrb_value context );
	static mrb_value		_stop				( mrb_state* M, mrb_value context );

	//----------------------------------------------------------------//
	bool			MOAINode_ApplyAttrOp	( u32 attrID, MOAIAttribute& attr, u32 op );

public:

	DECL_RUBY_FACTORY ( MOAIUntzSound, MOAINode )
	DECL_ATTR_HELPER ( MOAIUntzSound )

	enum {
		ATTR_VOLUME,
		TOTAL_ATTR,
	};

	//----------------------------------------------------------------//
					MOAIUntzSound		();
					~MOAIUntzSound		();
	void			RegisterRubyClass	( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs	( MOAIRubyState& state, RClass* klass );		
};

#endif
