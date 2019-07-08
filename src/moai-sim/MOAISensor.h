// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAISENSOR_H
#define MOAISENSOR_H

class MOAIInputMgr;

//================================================================//
// MOAISensor
//================================================================//
/**	@lua	MOAISensor
	@text	Base class for sensors.
*/
class MOAISensor :
	public MOAIRubyObject {
protected:

	u32			mType;
	STLString	mName;
	double		mTimestamp;

	//----------------------------------------------------------------//
	static mrb_value	_getTimestamp			( mrb_state* M, mrb_value context );

	SET ( u32, Type, mType );

public:

	friend class MOAIInputContext;
	friend class MOAIInputDevice;
	friend class MOAIInputMgr;

	//----------------------------------------------------------------//
	virtual void	ClearState			() {}
	virtual void	ParseEvent			( ZLStream& eventStream ) = 0;
					MOAISensor			();
					~MOAISensor			();
	void			RegisterRubyClass	( MOAIRubyState& state, RClass* klass );
	void			RegisterRubyFuncs	( MOAIRubyState& state, RClass* klass );
	virtual void	ResetState			() {}
};

#endif
