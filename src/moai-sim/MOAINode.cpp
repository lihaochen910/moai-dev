// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIEaseDriver.h>
#include <moai-sim/MOAINode.h>
#include <moai-sim/MOAINodeMgr.h>
#include <moai-sim/MOAISim.h>
#include <moai-sim/strings.h>

//================================================================//
// MOAIDepLink
//================================================================//
class MOAIDepLink {
private:

	friend class MOAINode;

	// don't need smart pointers; either node's destructor will delete link
	MOAINode*					mSourceNode;
	MOAINode*					mDestNode;

	// sibling pointers for the two singly linked lists
	MOAIDepLink*				mNextInSource;
	MOAIDepLink*				mNextInDest;

	// the attribute mapping
	u32							mSourceAttrID;
	u32							mDestAttrID;

	// cached flag indicating it's safe to pull from source to dest (attribute flags match)
	bool						mPullable;

	//----------------------------------------------------------------//
	MOAIDepLink () :
		mSourceNode ( 0 ),
		mDestNode ( 0 ),
		mNextInSource ( 0 ),
		mNextInDest ( 0 ),
		mSourceAttrID ( MOAIAttribute::NULL_ATTR ),
		mDestAttrID ( MOAIAttribute::NULL_ATTR ),
		mPullable ( false ) {
	}

	//----------------------------------------------------------------//
	~MOAIDepLink () {
	}

	//----------------------------------------------------------------//
	void Update () {
		this->mPullable =
			( mSourceAttrID & MOAIAttribute::ATTR_READ ) &&
			( mDestAttrID & MOAIAttribute::ATTR_WRITE ) &&
			( mSourceAttrID != MOAIAttribute::NULL_ATTR );
	}
};

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	clearAttrLink
	@text	Clears an attribute *pull* link - call this from the node
			receiving the attribute value.
	
	@in		MOAINode self
	@in		number attrID
	@out	nil
*/
mrb_value MOAINode::_clearAttrLink ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAINode, "UN" );

	u32 attrID = state.GetParamValue < u32 >( 1, 0 );
	self->ClearAttrLink ( attrID );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	clearNodeLink
	@text	Clears a dependency on a foreign node.
	
	@in		MOAINode self
	@in		MOAINode sourceNode
	@out	nil
*/
mrb_value MOAINode::_clearNodeLink ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAINode, "UU" );

	MOAINode* srcNode = state.GetRubyObject < MOAINode >( 1, true );
	if ( !srcNode ) return context;
	
	self->ClearNodeLink ( *srcNode );

	return context;
}

//----------------------------------------------------------------//
/**	@lua	forceUpdate
	@text	Evaluates the dependency graph for this node. Typically,
			the entire active dependency graph is evaluated once per
			frame, but in some cases it may be desirable to force
			evaluation of a node to make sure source dependencies
			are propagated to it immediately.
	
	@in		MOAINode self
	@out	nil
*/
mrb_value MOAINode::_forceUpdate ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAINode, "U" );

	self->ForceUpdate ();

	return context;
}

//----------------------------------------------------------------//
/**	@lua	getAttr
	@text	Returns the value of the attribute if it exists or nil if it doesn't.
	
	@in		MOAINode self
	@in		number attrID
	@out	number value
*/
mrb_value MOAINode::_getAttr ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAINode, "UN" );

	u32 attrID = state.GetParamValue < u32 >( 1, 0 );

	MOAIAttribute getter;
	self->ApplyAttrOp ( attrID, getter, MOAIAttribute::GET );
	
	if ( getter.IsValid () ) {
		return state.ToRValue ( getter.GetValue ( 0.0f ) );
	}
	
	MOAILogF ( M, ZLLog::LOG_ERROR, MOAISTRING_MOAINode_AttributeNotFound );
	return state.ToRValue ( 0.0f );
}

//----------------------------------------------------------------//
/**	@lua	getAttrLink
	@text	Returns the link if it exists or nil if it doesn't.
	
	@in		MOAINode self
	@in		number attrID
	@out	MOAINode sourceNode
	@out	number sourceAttrID
*/
mrb_value MOAINode::_getAttrLink ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAINode, "UN" );

	u32 attrID = state.GetParamValue < u32 >( 1, 0 );

	MOAIDepLink* link = self->FindAttrLink ( attrID );
	if ( link && link->mSourceNode ) {

		if ( link->mSourceAttrID != MOAIAttribute::NULL_ATTR ) {
			mrb_value args [ 2 ];
			args [ 0 ] = state.ToRValue < MOAIRubyObject* >( link->mSourceNode );
			args [ 1 ] = state.ToRValue ( link->mSourceAttrID );
			return mrb_ary_new_from_values ( state, 2, args );
		}
		return state.ToRValue < MOAIRubyObject* >( link->mSourceNode );
	}
	return mrb_nil_value ();
}

//----------------------------------------------------------------//
/**	@lua	getNodeState
	@text	Returns the current state of the node (for debugging purposes).
	
	@in		MOAINode self
	@out	number state			One of MOAINode.STATE_IDLE, MOAINode.STATE_ACTIVE, MOAINode.STATE_SCHEDULED, MOAINode.STATE_UPDATING.
*/
mrb_value MOAINode::_getNodeState ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAINode, "U" );

	return state.ToRValue( self->mState );
}

//----------------------------------------------------------------//
/**	@lua	moveAttr
	@text	Animate the attribute by applying a delta. Creates and returns
			a MOAIEaseDriver initialized to apply the delta.
	
	@in		MOAINode self
	@in		number attrID	ID of the attribute to animate.
	@in		number delta	Total change to be added to attribute.
	@in		number length	Length of animation in seconds.
	@opt	number mode		The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
							MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.

	@out	MOAIEaseDriver easeDriver
*/
mrb_value MOAINode::_moveAttr ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAINode, "UNNN" )

	MOAIEaseDriver* action = MOAIRubyRuntime::Get ().GetMainState ().CreateClassInstance < MOAIEaseDriver >();
	action->ReserveLinks ( 1 );
	
	u32 attrID		= state.GetParamValue < u32 >( 1, 0 );
	float value		= state.GetParamValue < float >( 2, 0.0f );
	float length	= state.GetParamValue < float >( 3, 0.0f );
	u32 mode		= state.GetParamValue < u32 >( 4, ZLInterpolate::kSmooth );
	
	if ( self->CheckAttrExists ( attrID )) {
	
		action->SetLink ( 0, self, attrID, value, mode );
		action->SetSpan ( length );
		action->Start ( 0, false );

		return state.ToRValue < MOAIRubyObject* >( action );
	}
	
	MOAILogF ( M, ZLLog::LOG_ERROR, MOAISTRING_MOAINode_AttributeNotFound );
	return state.ToRValue < MOAIRubyObject* >( action );
}

//----------------------------------------------------------------//
/**	@lua	scheduleUpdate
	@text	Schedule the node for an update next time the dependency graph
			is processed. Any dependent nodes will also be updated.
	
	@in		MOAINode self
	@out	nil
*/
mrb_value MOAINode::_scheduleUpdate ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAINode, "U" );

	self->ScheduleUpdate ();
	return context;
}

//----------------------------------------------------------------//
/**	@lua	seekAttr
	@text	Animate the attribute by applying a delta. Delta is computed
			given a target value. Creates and returns a MOAIEaseDriver
			initialized to apply the delta.
	
	@in		MOAINode self
	@in		number attrID	ID of the attribute to animate.
	@in		number goal		Desired resulting value for attribute.
	@in		number length	Length of animation in seconds.
	@opt	number mode		The ease mode. One of MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT MOAIEaseType.LINEAR,
							MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN, MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH. Defaults to MOAIEaseType.SMOOTH.

	@out	MOAIEaseDriver easeDriver
*/
mrb_value MOAINode::_seekAttr ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAINode, "UNNN" )

	MOAIEaseDriver* action = state.CreateClassInstance < MOAIEaseDriver >();
	action->ReserveLinks ( 1 );
	
	u32 attrID = state.GetParamValue < u32 >( 1, 0 );
	if ( self->CheckAttrExists ( attrID )) {
	
		MOAIAttribute getter;
		self->ApplyAttrOp ( attrID, getter, MOAIAttribute::GET );
		if ( !getter.IsValid ()) return state.ToRValue < MOAIRubyObject* >( action );
		
		float value		= state.GetParamValue < float >( 2, 0.0f );
		float delay		= state.GetParamValue < float >( 3, 0.0f );
		u32 mode		= state.GetParamValue < u32 >( 4, ZLInterpolate::kSmooth );
		
		action->SetLink ( 0, self, attrID, value - getter.GetValue ( 0.0f ), mode );
		
		action->SetSpan ( delay );
		action->Start ( 0, false );

		return state.ToRValue < MOAIRubyObject* >( action );
	}
	
	MOAILogF ( M, ZLLog::LOG_ERROR, MOAISTRING_MOAINode_AttributeNotFound );
	return state.ToRValue < MOAIRubyObject* >( action );
}

//----------------------------------------------------------------//
/**	@lua	setAttr
	@text	Sets the value of an attribute.
	
	@in		MOAINode self
	@in		number attrID
	@in		number value
	@out	nil
*/
mrb_value MOAINode::_setAttr ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAINode, "UNN" );
	
	u32 attrID = state.GetParamValue < u32 >( 1, 0 );
	float value = state.GetParamValue < float >( 2, 0.0f );
	
	if ( self->CheckAttrExists ( attrID )) {
	
		MOAIAttribute setter;
		setter.SetValue ( value );
	
		self->ClearAttrLink ( attrID );
		self->ApplyAttrOp ( attrID, setter, MOAIAttribute::SET );
		self->ScheduleUpdate ();
	}
	else {
		MOAILogF ( M, ZLLog::LOG_ERROR, MOAISTRING_MOAINode_AttributeNotFound );
	}
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setAttrLink
	@text	Sets a *pull* attribute connecting an attribute in the
			node to an attribute in a foreign node.
	
	@in		MOAINode self
	@in		number attrID				ID of attribute to become dependent of foreign node.
	@in		MOAINode sourceNode			Foreign node.
	@opt	number sourceAttrID			Attribute in foreign node to control value of attribue. Default value is attrID.
	@out	nil
*/
mrb_value MOAINode::_setAttrLink ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAINode, "UNU" );
	
	u32 attrID = state.GetParamValue < u32 >( 1, 0 );
	
	MOAINode* srcNode = state.GetRubyObject < MOAINode >( 2, true );
	if ( !srcNode ) return context;

	u32 srcAttrID = state.GetParamValue < u32 >( 3, attrID );
	
	if ( srcNode->CheckAttrExists ( srcAttrID )) {
		self->SetAttrLink ( attrID, srcNode, srcAttrID );
		self->ScheduleUpdate ();
		return context;
	}
	
	MOAILogF ( M, ZLLog::LOG_ERROR, MOAISTRING_MOAINode_AttributeNotFound );
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setNodeLink
	@text	Creates a dependency between the node and a foreign node
			without the use of attributes; if the foreign node is updated,
			the dependent node will be updated after.
	
	@in		MOAINode self
	@in		MOAINode sourceNode
	@out	nil
*/
mrb_value MOAINode::_setNodeLink ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP ( MOAINode, "UU" );
	
	MOAINode* srcNode = state.GetRubyObject < MOAINode >( 1, true );
	if ( !srcNode ) return context;
	
	self->SetNodeLink ( *srcNode );
	
	return context;
}

//================================================================//
// MOAINode
//================================================================//

//----------------------------------------------------------------//
void MOAINode::Activate ( MOAINode& activator ) {

	// bail if we're already in the list
	if ( this->mState != STATE_IDLE ) return;

	// insert before activator
	MOAINodeMgr::Get ().InsertBefore ( activator, *this );
	this->mState = STATE_ACTIVE;

	// activate source nodes
	MOAIDepLink* link = this->mPullLinks;
	for ( ; link ; link = link->mNextInDest ) {
		link->mSourceNode->Activate ( *this );
	}
}

//----------------------------------------------------------------//
bool MOAINode::ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {

	return this->MOAINode_ApplyAttrOp ( attrID, attr, op );
}

//----------------------------------------------------------------//
void MOAINode::ActivateOnLink ( MOAINode& srcNode ) {

	if ( this->mState != STATE_IDLE ) {
	
		if ( srcNode.mState == STATE_IDLE ) {
			srcNode.Activate ( *this );
		}
		else {
		
			MOAINodeMgr& depNodeMgr = MOAINodeMgr::Get ();
			
			if ( srcNode.IsNodeUpstream ( this ) && this->mState != STATE_UPDATING ) {
				
				depNodeMgr.Remove ( *this );
				depNodeMgr.InsertAfter ( srcNode, *this );
			}
		}
	}
}

//----------------------------------------------------------------//
bool MOAINode::CheckAttrExists ( u32 attrID ) {

	if ( attrID == MOAIAttribute::NULL_ATTR ) return false;

	MOAIAttribute getter;
	this->ApplyAttrOp ( attrID, getter, MOAIAttribute::CHECK );
	return getter.IsValid ();
}

//----------------------------------------------------------------//
void MOAINode::ClearAttrLink ( int attrID ) {

	MOAIDepLink* cursor = this->mPullLinks;
	this->mPullLinks = 0;

	while ( cursor ) {
		MOAIDepLink* link = cursor;
		cursor = cursor->mNextInDest;
		
		if (( link->mDestAttrID & ~MOAIAttribute::ATTR_FLAGS_MASK ) == ( u32 )attrID ) {
			link->mSourceNode->RemoveDepLink ( *link );
			delete link;
			this->ScheduleUpdate ();
		}
		else {
			link->mNextInDest = this->mPullLinks;
			this->mPullLinks = link;
		}
	}
}

//----------------------------------------------------------------//
void MOAINode::ClearNodeLink ( MOAINode& srcNode ) {
	
	MOAIDepLink* cursor = this->mPullLinks;
	this->mPullLinks = 0;

	while ( cursor ) {
		MOAIDepLink* link = cursor;
		cursor = cursor->mNextInDest;
		
		if (( link->mDestAttrID == MOAIAttribute::NULL_ATTR ) && ( link->mSourceNode == &srcNode )) {
			link->mSourceNode->RemoveDepLink ( *link );
			delete link;
		}
		else {
			link->mNextInDest = this->mPullLinks;
			this->mPullLinks = link;
		}
	}
}

//----------------------------------------------------------------//
void MOAINode::DepNodeUpdate () {
	
	if ( this->mState == STATE_SCHEDULED ) {
	
		this->mState = STATE_UPDATING;
		this->PullAttributes ();

		InvokeListener ( EVENT_NODE_PRE_UPDATE );
		this->MOAINode_Update ();
		InvokeListener ( EVENT_NODE_POST_UPDATE );
		
		this->mState = STATE_ACTIVE;
	}
}

//----------------------------------------------------------------//
void MOAINode::ExtendUpdate () {

	MOAIDepLink* link = this->mPushLinks;
	for ( ; link ; link = link->mNextInSource ) {
		link->mDestNode->ScheduleUpdate ();
	}
}

//----------------------------------------------------------------//
MOAIDepLink* MOAINode::FindAttrLink ( int attrID ) {

	attrID = attrID & ~MOAIAttribute::ATTR_FLAGS_MASK;

	MOAIDepLink* link = this->mPullLinks;
	for ( ; link; link = link->mNextInDest ) {
		if ( link->mDestAttrID == ( u32 )attrID ) break;
	}
	return link;
}

//----------------------------------------------------------------//
MOAIDepLink* MOAINode::FindNodeLink ( MOAINode& srcNode ) {

	MOAIDepLink* link = this->mPullLinks;
	for ( ; link; link = link->mNextInDest ) {
		if (( link->mSourceNode == &srcNode ) && ( link->mDestAttrID == MOAIAttribute::NULL_ATTR )) break;
	}
	return link;
}

//----------------------------------------------------------------//
void MOAINode::ForceUpdate () {

	this->ScheduleUpdate ();
	this->DepNodeUpdate ();
}

//----------------------------------------------------------------//
u32 MOAINode::GetAttrFlags ( u32 attrID ) {

	MOAIAttribute attr;
	this->ApplyAttrOp ( attrID, attr, MOAIAttribute::CHECK );
	return attr.GetFlags ();
}

//----------------------------------------------------------------//
bool MOAINode::IsNodeUpstream ( MOAINode* node ) {

	MOAINode* cursor = this->mPrev;
	for ( ; cursor; cursor = cursor->mPrev ) {
		if ( cursor == node ) return true;
	}
	return false;
}

//----------------------------------------------------------------//
MOAINode::MOAINode () :
	mPullLinks ( 0 ),
	mPushLinks ( 0 ),
	mState ( STATE_IDLE ),
	mPrev ( 0 ),
	mNext ( 0 ) {
	
	RTTI_SINGLE ( MOAIInstanceEventSource )
}

//----------------------------------------------------------------//
MOAINode::~MOAINode () {
	
	while ( this->mPullLinks ) {
		MOAIDepLink* link = this->mPullLinks;
		this->mPullLinks = link->mNextInDest;
		link->mSourceNode->RemoveDepLink ( *link );
		delete link;
	}
	
	while ( this->mPushLinks ) {
		MOAIDepLink* link = this->mPushLinks;
		this->mPushLinks = link->mNextInSource;
		link->mDestNode->RemoveDepLink ( *link );
		delete link;
	}

	if (( this->mState != STATE_IDLE ) && ( MOAINodeMgr::IsValid ())) {
		MOAINodeMgr::Get ().Remove ( *this );
	}
}

//----------------------------------------------------------------//
void MOAINode::PullAttributes () {

	MOAIAttribute attr;

	MOAIDepLink* link = this->mPullLinks;	
	for ( ; link ; link = link->mNextInDest ) {
		
		if ( link->mSourceNode->mState == STATE_SCHEDULED ) {
			link->mSourceNode->DepNodeUpdate ();
		}

		if ( link->mPullable ) {
			link->mSourceNode->ApplyAttrOp ( link->mSourceAttrID, attr, MOAIAttribute::GET );
			this->ApplyAttrOp ( link->mDestAttrID, attr, MOAIAttribute::SET );
		}
	}
}

//----------------------------------------------------------------//
bool MOAINode::PullLinkedAttr ( u32 attrID, MOAIAttribute& attr ) {

	MOAIDepLink* link = this->mPullLinks;	
	for ( ; link ; link = link->mNextInDest ) {
		if ((( link->mDestAttrID & ~MOAIAttribute::ATTR_FLAGS_MASK ) == attrID ) && ( link->mSourceAttrID & MOAIAttribute::ATTR_READ )) {
			link->mSourceNode->ApplyAttrOp ( link->mSourceAttrID, attr, MOAIAttribute::GET );
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------//
void MOAINode::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass ,"EVENT_UPDATE",				( u32 )EVENT_NODE_PRE_UPDATE ); // TODO: deprecate
	
	state.DefineClassConst ( klass, "EVENT_NODE_PRE_UPDATE",	( u32 )EVENT_NODE_PRE_UPDATE );
	state.DefineClassConst ( klass, "EVENT_NODE_POST_UPDATE",	( u32 )EVENT_NODE_POST_UPDATE );
	
	state.DefineClassConst ( klass, "STATE_IDLE",				( u32 )STATE_IDLE );
	state.DefineClassConst ( klass, "STATE_ACTIVE",				( u32 )STATE_ACTIVE );
	state.DefineClassConst ( klass, "STATE_SCHEDULED",			( u32 )STATE_SCHEDULED );
	state.DefineClassConst ( klass, "STATE_UPDATING",			( u32 )STATE_UPDATING );
}

//----------------------------------------------------------------//
void MOAINode::RegisterRubyFuncs ( MOAIRubyState& state, RClass* klass ) {
	
	state.DefineInstanceMethod ( klass, "clearAttrLink", _clearAttrLink, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "clearNodeLink", _clearNodeLink, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "forceUpdate", _forceUpdate, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getAttr", _getAttr, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getAttrLink", _getAttrLink, MRB_ARGS_REQ ( 1 ) );
	state.DefineInstanceMethod ( klass, "getNodeState", _getNodeState, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "moveAttr", _moveAttr, MRB_ARGS_REQ ( 3 ) | MRB_ARGS_OPT ( 1 ) );
	state.DefineInstanceMethod ( klass, "scheduleUpdate", _scheduleUpdate, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "seekAttr", _seekAttr, MRB_ARGS_REQ ( 3 ) | MRB_ARGS_OPT ( 1 ) );
	state.DefineInstanceMethod ( klass, "setAttr", _setAttr, MRB_ARGS_REQ ( 2 ) );
	state.DefineInstanceMethod ( klass, "setAttrLink", _setAttrLink, MRB_ARGS_REQ ( 2 ) | MRB_ARGS_OPT ( 1 ) );
	state.DefineInstanceMethod ( klass, "setNodeLink", _setNodeLink, MRB_ARGS_REQ ( 1 ) );

}

//----------------------------------------------------------------//
void MOAINode::RemoveDepLink ( MOAIDepLink& link ) {

	if ( link.mSourceNode == this ) {
	
		MOAIDepLink* cursor = this->mPushLinks;
		this->mPushLinks = 0;
		
		while ( cursor ) {
			MOAIDepLink* next = cursor->mNextInSource;
			if ( cursor != &link ) {
				cursor->mNextInSource = this->mPushLinks;
				this->mPushLinks = cursor;
			}
			cursor = next;
		}
		link.mNextInSource = 0;
	}
	else {
	
		MOAIDepLink* cursor = this->mPullLinks;
		this->mPullLinks = 0;
		
		while ( cursor ) {
			MOAIDepLink* next = cursor->mNextInDest;
			if ( cursor != &link ) {
				cursor->mNextInDest = this->mPullLinks;
				this->mPullLinks = cursor;
			}
			cursor = next;
		}
		link.mNextInDest = 0;
	}
	link.Update ();
}

//----------------------------------------------------------------//
void MOAINode::ScheduleUpdate () {
	
	MOAINodeMgr& nodeMgr = MOAINodeMgr::Get ();
	
	if ( nodeMgr.IsValid ()) {
	
		nodeMgr.mScheduled = true;
		
		if (( this->mState == STATE_IDLE ) || ( this->mState == STATE_ACTIVE )) {
		
			// add to the list if not already in it
			if ( this->mState == STATE_IDLE ) {
			
				this->mState = STATE_SCHEDULED;

				// push us at the end of the list
				nodeMgr.PushBack ( *this );
				
				// activate source nodes
				MOAIDepLink* link = this->mPullLinks;
				for ( ; link ; link = link->mNextInDest ) {
					link->mSourceNode->Activate ( *this );
				}
			}
			
			this->mState = STATE_SCHEDULED;
			
			this->ExtendUpdate ();
		}
	}
}

//----------------------------------------------------------------//
void MOAINode::SetAttrLink ( int attrID, MOAINode* srcNode, int srcAttrID ) {
	
	if ( attrID == ( int )MOAIAttribute::NULL_ATTR ) return;
	
	if (( srcNode ) && ( !srcNode->CheckAttrExists ( srcAttrID ))) {
		srcNode = 0;
	}
	
	if (( !srcNode ) || ( srcAttrID == ( int )MOAIAttribute::NULL_ATTR )) {
		attrID |= this->GetAttrFlags ( attrID );
		this->ClearAttrLink ( attrID );
		return;
	}
	
	attrID |= this->GetAttrFlags ( attrID );
	srcAttrID |= srcNode->GetAttrFlags ( srcAttrID );
	
	MOAIDepLink* link = this->FindAttrLink ( attrID );
	
	if ( link ) {
		if ( link->mSourceNode != srcNode ) {
			link->mSourceNode->RemoveDepLink ( *link );
			link->mNextInSource = srcNode->mPushLinks;
			srcNode->mPushLinks = link;
		}
	}
	else {
		link = new MOAIDepLink ();
		
		link->mDestNode = this;
		link->mDestAttrID = attrID;
		
		link->mNextInSource = srcNode->mPushLinks;
		srcNode->mPushLinks = link;
		
		link->mNextInDest = this->mPullLinks;
		this->mPullLinks = link;
	}

	link->mSourceNode = srcNode;
	link->mSourceAttrID = srcAttrID;
	link->Update ();
	
	this->ScheduleUpdate ();
	this->ActivateOnLink ( *srcNode );
}

//----------------------------------------------------------------//
void MOAINode::SetNodeLink ( MOAINode& srcNode ) {
	
	if ( &srcNode == this ) return;
	
	MOAIDepLink* link = this->FindNodeLink ( srcNode );
	
	if ( !link ) {
	
		link = new MOAIDepLink ();

		link->mSourceNode = &srcNode;
		link->mDestNode = this;
		
		link->mNextInSource = srcNode.mPushLinks;
		srcNode.mPushLinks = link;
		
		link->mNextInDest = this->mPullLinks;
		this->mPullLinks = link;
		link->mPullable = true;
		
		this->ScheduleUpdate ();
		this->ActivateOnLink ( srcNode );
		this->ScheduleUpdate ();
	}
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//
bool MOAINode::MOAINode_ApplyAttrOp ( u32 attrID, MOAIAttribute& attr, u32 op ) {
	UNUSED ( attrID );
	UNUSED ( attr );
	UNUSED ( op );
	
	return false;
}

//----------------------------------------------------------------//
void MOAINode::MOAINode_Update () {
}
