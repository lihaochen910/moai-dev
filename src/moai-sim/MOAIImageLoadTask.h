// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIIMAGELOADTASK_H
#define MOAIIMAGELOADTASK_H

#include <moai-sim/MOAIImage.h>

class MOAIDataBuffer;

//================================================================//
// MOAIImageLoadTask
//================================================================//
class MOAIImageLoadTask :
	public MOAITask {
private:
	
	MOAIImage				mImage;

	STLString				mFilename;
	MOAIDataBuffer*			mDataBuffer;
	MOAIImage*				mTarget;
	MOAIRubyStrongRef		mOnFinish;
	u32						mTransform;

	//----------------------------------------------------------------//
	void		Execute				();
	void		Publish				();

public:

	//----------------------------------------------------------------//
	void		Init				( cc8* filename, MOAIImage& target, u32 transform );
	void		Init				( MOAIDataBuffer& data, MOAIImage& target, u32 transform );
				MOAIImageLoadTask	();
				~MOAIImageLoadTask	();
	void		RegisterRubyClass	( MOAIRubyState& state, RClass* klass );
	void		RegisterRubyFuncs	( MOAIRubyState& state, RClass* klass );
	void		SetCallback			( mrb_state* M, int idx );
};

#endif
