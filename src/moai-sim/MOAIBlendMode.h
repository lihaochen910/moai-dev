// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAIBLENDMODE_H
#define MOAIBLENDMODE_H

//================================================================//
// MOAIBlendMode
//================================================================//
class MOAIBlendMode {
public:
	
	int				mEquation;
	int				mSourceFactor;
	int				mDestFactor;

	//----------------------------------------------------------------//
	void			Init				( MOAIRubyState& state, int idx );
	void			SetBlend			( int equation, int srcFactor, int dstFactor );
					MOAIBlendMode		();
					MOAIBlendMode		( int equation, int srcFactor, int dstFactor );
					~MOAIBlendMode		();
	mrb_value		Push				( MOAIRubyState& state ) const;
	
	//----------------------------------------------------------------//
	inline bool IsSame ( const MOAIBlendMode& blendMode ) {
		return (
			( this->mSourceFactor == blendMode.mSourceFactor ) &&
			( this->mDestFactor == blendMode.mDestFactor ) &&
			( this->mEquation == blendMode.mEquation )
		);
	}
};

#endif
