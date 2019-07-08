// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIColor.h>
#include <moai-sim/MOAIEaseDriver.h>
#include <moai-sim/MOAISim.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	getColor
	@text	Return the color.
	
	@in		MOAIProp self
	@out	number rDelta
	@out	number gDelta
	@out	number bDelta
	@out	number aDelta
*/
mrb_value MOAIColor::_getColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIColor, "U" )

	mrb_value ret [ 4 ];
	ret [ 0 ] = state.ToRValue ( self->mR );
	ret [ 1 ] = state.ToRValue ( self->mG );
	ret [ 2 ] = state.ToRValue ( self->mB );
	ret [ 3 ] = state.ToRValue ( self->mA );
	
	return mrb_ary_new_from_values ( state, 4, ret );
}

//----------------------------------------------------------------//
/**	@lua	moveColor
	@text	Animate the color by applying a delta. Creates and returns
			a MOAIEaseDriver initialized to apply the delta.
	
	@in		MOAIColor self
	@in		number rDelta		Delta to be added to r.
	@in		number gDelta		Delta to be added to g.
	@in		number bDelta		Delta to be added to b.
	@in		number aDelta		Delta to be added to a.
	@in		number length		Length of animation in seconds.
	@opt	number mode			The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
								MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.

	@out	MOAIEaseDriver easeDriver
*/
mrb_value MOAIColor::_moveColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIColor, "UNNNNN" )

	float delay		= state.GetParamValue < float >( 5, 0.0f );
	
	if ( delay > 0.0f ) {
	
		u32 mode = state.GetParamValue < u32 >( 6, ZLInterpolate::kSmooth );
		
		MOAIEaseDriver* action = state.CreateClassInstance < MOAIEaseDriver >();
		
		action->ParseForMove ( state, 1, self, 3, mode,
			MOAIColorAttr::Pack ( ATTR_R_COL ), 0.0f,
			MOAIColorAttr::Pack ( ATTR_G_COL ), 0.0f,
			MOAIColorAttr::Pack ( ATTR_B_COL ), 0.0f,
			MOAIColorAttr::Pack ( ATTR_A_COL ), 0.0f
		);
		
		action->SetSpan ( delay );
		action->Start ( 0, false );

		return state.ToRValue < MOAIRubyObject* >( action );
	}
	
	if ( !state.CheckVector ( 1, 4, 0, 0 )) {
		self->mR += state.GetParamValue < float >( 1, 0.0f );
		self->mG += state.GetParamValue < float >( 2, 0.0f );
		self->mB += state.GetParamValue < float >( 3, 0.0f );
		self->mA += state.GetParamValue < float >( 4, 0.0f );
		self->ScheduleUpdate ();
	}
	
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIColor::_packRGBA ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_CLASS ( "" )

	float r		= state.GetParamValue < float >( 1, 1.0f );
	float g		= state.GetParamValue < float >( 2, 1.0f );
	float b		= state.GetParamValue < float >( 3, 1.0f );
	float a		= state.GetParamValue < float >( 4, 1.0f );

	return state.ToRValue ( ZLColor::PackRGBA ( r, g, b, a ) );
}

//----------------------------------------------------------------//
/**	@lua	seekColor
	@text	Animate the color by applying a delta. Delta is computed
			given a target value. Creates and returns a MOAIEaseDriver
			initialized to apply the delta.
	
	@in		MOAIColor self
	@in		number rGoal		Desired resulting value for r.
	@in		number gGoal		Desired resulting value for g.
	@in		number bGoal		Desired resulting value for b.
	@in		number aGoal		Desired resulting value for a.
	@in		number length		Length of animation in seconds.
	@opt	number mode			The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
								MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.

	@out	MOAIEaseDriver easeDriver
*/
mrb_value MOAIColor::_seekColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIColor, "UNNNNN" )

	float delay		= state.GetParamValue < float >( 5, 0.0f );
	
	
	if ( delay > 0.0f ) {
	
		u32 mode = state.GetParamValue < u32 >( 6, ZLInterpolate::kSmooth );
		
		MOAIEaseDriver* action = state.CreateClassInstance < MOAIEaseDriver >();
		
		action->ParseForSeek ( state, 1, self, 3, mode,
			MOAIColorAttr::Pack ( ATTR_R_COL ), self->mR, 0.0f,
			MOAIColorAttr::Pack ( ATTR_G_COL ), self->mG, 0.0f,
			MOAIColorAttr::Pack ( ATTR_B_COL ), self->mB, 0.0f,
			MOAIColorAttr::Pack ( ATTR_A_COL ), self->mA, 0.0f
		);
		
		action->SetSpan ( delay );
		action->Start ( 0, false );

		return state.ToRValue < MOAIRubyObject* >( action );
	}
	
	ZLColorVec color = state.GetColor ( 1, 0.0f, 0.0f, 0.0f, 0.0f );
	if ( !color.IsEqual ( *self )) {
		self->Set ( color.mR, color.mG, color.mB, color.mA );
		self->ScheduleUpdate ();
	}
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setColor
	@text	Initialize the color.
	
	@in		MOAIColor self
	@in		number r	Default value is 0.
	@in		number g	Default value is 0.
	@in		number b	Default value is 0.
	@opt	number a	Default value is 1.
	@out	nil
*/
mrb_value MOAIColor::_setColor ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIColor, "UNNN" )

	ZLColorVec color = state.GetColor ( 1, 0.0f, 0.0f, 0.0f, 1.0f );
	if ( !color.IsEqual ( *self )) {
		self->Set ( color.mR, color.mG, color.mB, color.mA );
		self->ScheduleUpdate ();
	}

	return context;
}

//----------------------------------------------------------------//
/**	@lua	setParent
	@text	This method has been deprecated. Use MOAINode setAttrLink instead.
	
	@in		MOAIColor self
	@opt	MOAINode parent		Default value is nil.
	@out	nil
*/
mrb_value MOAIColor::_setParent ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAIColor, "U" )

	MOAINode* parent = state.GetRubyObject < MOAINode >( 1, true );
	
	self->SetAttrLink ( PACK_ATTR ( MOAIColor, INHERIT_COLOR ), parent, PACK_ATTR ( MOAIColor, COLOR_TRAIT ));
	
	//MOAILogF ( state, MOAISTRING_FunctionDeprecated_S, "setParent" );
	
	return context;
}

//----------------------------------------------------------------//
// TODO: doxygen
mrb_value MOAIColor::_unpackRGBA ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_CLASS ( "" )

	u32 rgba = state.GetParamValue < u32 >( 1, 0xffffffff );

	ZLColorVec color;
	color.SetRGBA ( rgba );

	return state.Get ( color );
}

//================================================================//
// MOAIColor
//================================================================//

//----------------------------------------------------------------//
MOAIColor* MOAIColor::AffirmColor ( MOAIRubyState& state, int idx ) {

	MOAIColor* color = 0;
	
	if ( state.ParamIsType ( idx, MRB_TT_DATA )) {
		color = state.GetRubyObject < MOAIColor >( idx, false );
	}
	else {
	
		float r = state.GetParamValue < float >( 1, 0.0f );
		float g = state.GetParamValue < float >( 2, 0.0f );
		float b = state.GetParamValue < float >( 3, 0.0f );
		float a = state.GetParamValue < float >( 4, 1.0f );

		color = state.CreateClassInstance < MOAIColor >();
		color->Set ( r, g, b, a );
		color->ScheduleUpdate ();
	}
	return color;
}

//----------------------------------------------------------------//
ZLColorVec MOAIColor::GetColorTrait () const {

	return this->mColor;
}

//----------------------------------------------------------------//
bool MOAIColor::IsClear () {

	return this->mColor.IsClear ();
}

//----------------------------------------------------------------//
MOAIColor::MOAIColor () {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAINode )
	RTTI_END
	
	this->Set ( 1.0f, 1.0f, 1.0f, 1.0f );
	this->mColor.Set ( 1.0f, 1.0f, 1.0f, 1.0f );
}

//----------------------------------------------------------------//
MOAIColor::~MOAIColor () {
}

//----------------------------------------------------------------//
void MOAIColor::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {
	
	MOAINode::RegisterRubyClass ( state, klass );
	
	state.DefineClassConst ( klass, "ATTR_R_COL", MOAIColorAttr::Pack ( ATTR_R_COL ));
	state.DefineClassConst ( klass, "ATTR_G_COL", MOAIColorAttr::Pack ( ATTR_G_COL ));
	state.DefineClassConst ( klass, "ATTR_B_COL", MOAIColorAttr::Pack ( ATTR_B_COL ));
	state.DefineClassConst ( klass, "ATTR_A_COL", MOAIColorAttr::Pack ( ATTR_A_COL ));
	
	state.DefineClassConst ( klass, "ADD_COLOR", MOAIColorAttr::Pack ( ADD_COLOR ));
	state.DefineClassConst ( klass, "INHERIT_COLOR", MOAIColorAttr::Pack ( INHERIT_COLOR ));
	state.DefineClassConst ( klass, "COLOR_TRAIT", MOAIColorAttr::Pack ( COLOR_TRAIT ));

	state.DefineStaticMethod ( klass, "packRGBA", _packRGBA, MRB_ARGS_REQ ( 4 ) );
	state.DefineStaticMethod ( klass, "unpackRGBA", _unpackRGBA, MRB_ARGS_REQ ( 1 ) );
	
}

//----------------------------------------------------------------//
void MOAIColor::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	MOAINode::RegisterRubyFuncs ( state, klass );

	state.DefineInstanceMethod ( klass, "getColor", _getColor, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "moveColor", _moveColor, MRB_ARGS_REQ ( 4 ) );
	state.DefineInstanceMethod ( klass, "seekColor", _seekColor, MRB_ARGS_ARG ( 5, 1 ) );
	state.DefineInstanceMethod ( klass, "setColor", _setColor, MRB_ARGS_ARG ( 3, 1 ) );
	state.DefineInstanceMethod ( klass, "setParent", _setParent, MRB_ARGS_ARG ( 0, 1 ) );
	
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAIColor::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	if ( MOAIColorAttr::Check ( attrID )) {

		switch ( UNPACK_ATTR ( attrID )) {
		
			case ATTR_R_COL:
				this->mR = ZLFloat::Clamp ( attr.Apply ( this->mR, op, MOAIAttribute::ATTR_READ_WRITE ), 0.0f, 1.0f );
				return true;
				
			case ATTR_G_COL:
				this->mG = ZLFloat::Clamp ( attr.Apply ( this->mG, op, MOAIAttribute::ATTR_READ_WRITE ), 0.0f, 1.0f );
				return true;
				
			case ATTR_B_COL:
				this->mB = ZLFloat::Clamp ( attr.Apply ( this->mB, op, MOAIAttribute::ATTR_READ_WRITE ), 0.0f, 1.0f );
				return true;
				
			case ATTR_A_COL:
				this->mA = ZLFloat::Clamp ( attr.Apply ( this->mA, op, MOAIAttribute::ATTR_READ_WRITE ), 0.0f, 1.0f );
				return true;
				
			case COLOR_TRAIT:
				attr.ApplyNoAdd ( this->mColor, op, MOAIAttribute::ATTR_READ );
				return true;
		}
	}
	return false;
}

//----------------------------------------------------------------//
void MOAIColor::MOAINode_Update () {

	this->mColor = *this;
	
	MOAIAttribute attr;
	if ( this->PullLinkedAttr ( MOAIColorAttr::Pack ( INHERIT_COLOR ), attr )) {
		this->mColor.Modulate ( attr.GetValue ( ZLColorVec::WHITE ));
	}
	
	if ( this->PullLinkedAttr ( MOAIColorAttr::Pack ( ADD_COLOR ), attr )) {
		this->mColor.Add ( attr.GetValue ( ZLColorVec::WHITE ));
	}
}

