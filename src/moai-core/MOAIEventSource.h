// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIEVENTSOURCE_H
#define	MOAIEVENTSOURCE_H

#include <moai-core/MOAIRubyObject.h>
#include <moai-core/MOAIRubyState.h>

#define LISTENER_TABLE "listener_table"

//================================================================//
// MOAIEventSource
//================================================================//
/**	@ruby	MOAIEventSource
	@text	Base class for all Ruby-bound Moai objects that emit events
			and have an event table.
*/
class MOAIEventSource :
	public virtual MOAIRubyObject {
protected:

	//----------------------------------------------------------------//
	virtual void		AffirmListenerTable			( MOAIRubyState& state ) = 0;
	
public:
	
	//----------------------------------------------------------------//
	virtual void		InvokeListener				( u32 eventID, mrb_int argc = 0, const mrb_value* args = 0 ) = 0;
						MOAIEventSource				();
	virtual				~MOAIEventSource			();
};

//================================================================//
// MOAIInstanceEventSource
//================================================================//
/**	@ruby	MOAIInstanceEventSource
	@text	Derivation of MOAIEventSource for non-global Ruby objects.
*/
class MOAIInstanceEventSource :
	public virtual MOAIEventSource {
private:

	MOAIRubyStrongRef		mListenerTable;

	//----------------------------------------------------------------//
	static mrb_value	_getListener				( mrb_state* M, mrb_value context );
	static mrb_value 	_setListener				( mrb_state* M, mrb_value context );

protected:

	//----------------------------------------------------------------//
	void				AffirmListenerTable			( MOAIRubyState& state );
	void				SetListener					( mrb_state* M, u32 eventID, mrb_value handler );

public:

	//----------------------------------------------------------------//
	mrb_value			GetListener					( mrb_state* M, u32 eventID );
	void				InvokeListener				( u32 eventID, mrb_int argc = 0, const mrb_value* args = 0 );
	void				InvokeListenerWithSelf		( u32 eventID );
						MOAIInstanceEventSource		();
	virtual				~MOAIInstanceEventSource	();
	void				RegisterRubyFuncs			( MOAIRubyState& state, RClass* klass );
};

//================================================================//
// MOAIGlobalEventSource
//================================================================//
/**	@ruby	MOAIGlobalEventSource
	@text	Derivation of MOAIEventSource for global Ruby objects.
*/
class MOAIGlobalEventSource :
	public virtual MOAIEventSource {
private:

	MOAIRubyStrongRef	mListenerTable;

protected:

	//----------------------------------------------------------------//
	/**	@ruby	getListener
		@text	Gets the listener callback for a given event ID.

		@in		number eventID				The ID of the event.
		@out	function					The listener callback.
	*/
	template < typename TYPE >
	static mrb_value _getListener ( mrb_state* M, mrb_value context ) {
		MOAI_RUBY_SETUP ( TYPE, "N" );

		TYPE& global = TYPE::Get ();
		return global.GetListener ( M, state.GetParamValue < u32 >( 0, 0 ) );
	}

	//----------------------------------------------------------------//
	/**	@ruby	setListener
		@text	Sets a listener callback for a given event ID. It is up
				to individual classes to declare their event IDs.
			
		@in		number eventID				The ID of the event.
		@opt	function callback			The callback to be called when the object emits the event. Default value is nil.
		@out	nil
	*/
	template < typename TYPE >
	static mrb_value _setListener ( mrb_state* M, mrb_value context ) {
		MOAI_RUBY_SETUP ( TYPE, "N" );
	
		TYPE& global = TYPE::Get ();
		global.SetListener ( M, state.GetParamValue < u32 > ( 0, 0 ), state.GetParamValue ( 1 ) );
		return context;
	}

	//----------------------------------------------------------------//
	void			AffirmListenerTable			( MOAIRubyState& state );
	//bool			PushListenerTable			( MOAIRubyState& state );
	void			SetListener					( mrb_state* M, u32 eventID, mrb_value handler );

public:

	//----------------------------------------------------------------//
	mrb_value		GetListener					( mrb_state* M, u32 eventID );
	void			InvokeListener				( u32 eventID, mrb_int argc = 0, const mrb_value* args = 0 );
					MOAIGlobalEventSource		();
	virtual			~MOAIGlobalEventSource		();
};

#endif
