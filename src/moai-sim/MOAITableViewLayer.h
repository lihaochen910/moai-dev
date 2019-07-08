// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAITABLEVIEWLAYER_H
#define	MOAITABLEVIEWLAYER_H

#include <moai-sim/MOAITableLayer.h>
#include <moai-sim/MOAIViewLayer.h>

//================================================================//
// MOAITableViewLayer
//================================================================//
// TODO: doxygen
class MOAITableViewLayer :
	public virtual MOAITableLayer,
	public virtual MOAIViewLayer {
private:

	//----------------------------------------------------------------//
	void				MOAIDrawable_Draw			( int subPrimID );
	void				MOAIViewLayer_Draw			();

public:
	
	DECL_RUBY_FACTORY ( MOAITableViewLayer, MOAILayer )
	
	//----------------------------------------------------------------//
						MOAITableViewLayer			();
						~MOAITableViewLayer			();
	void				RegisterRubyClass			( MOAIRubyState& state, RClass* klass );
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
	void				SerializeIn					( MOAIRubyState& state, MOAIDeserializer& serializer );
	void				SerializeOut				( MOAIRubyState& state, MOAISerializer& serializer );
};

#endif
