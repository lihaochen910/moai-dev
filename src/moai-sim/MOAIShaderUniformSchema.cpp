// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-sim/MOAIAttribute.h>
#include <moai-sim/MOAIShaderUniformSchema.h>

//================================================================//
// MOAIShaderUniformSchema
//================================================================//

//----------------------------------------------------------------//
bool MOAIShaderUniformSchema::ApplyAttrOp ( void* buffer, u32 attrID, MOAIAttribute& attr, u32 op ) const {
			
	MOAIShaderUniformHandle uniform = this->GetUniformHandleForAttributeID ( buffer, attrID );
	
	if ( uniform.IsValid ()) {
	
		switch ( op ) {

			case MOAIAttribute::CHECK:
				attr.SetFlags ( MOAIAttribute::ATTR_WRITE );
				return true;

			case MOAIAttribute::SET:
				uniform.SetValue ( attr );
				return true;

			case MOAIAttribute::ADD:
				uniform.AddValue ( attr );
				return true;
		}
	}
	return false;
}

//----------------------------------------------------------------//
u32 MOAIShaderUniformSchema::GetAttributeID ( u32 uniformID, u32 index ) const {

	return ( uniformID * MAX_UNIFORM_ARRAY_SIZE ) + index;
}

//----------------------------------------------------------------//
MOAIShaderUniformHandle MOAIShaderUniformSchema::GetUniformHandle ( void* buffer, u32 uniformID ) const {

	return this->MOAIShaderUniformSchema_GetUniformHandle ( buffer, uniformID );
}

//----------------------------------------------------------------//
MOAIShaderUniformHandle MOAIShaderUniformSchema::GetUniformHandle ( void* buffer, u32 uniformID, u32 index ) const {

	MOAIShaderUniformHandle uniform = this->GetUniformHandle ( buffer, uniformID );
	return uniform.IsValid () ? uniform.Offset ( index ) : uniform;
}

//----------------------------------------------------------------//
MOAIShaderUniformHandle MOAIShaderUniformSchema::GetUniformHandleForAttributeID ( void* buffer, u32 attrID ) const {

	attrID &= MOAIAttribute::ATTR_ID_MASK;

	u32 uniformID = attrID / MAX_UNIFORM_ARRAY_SIZE;
	u32 index = attrID - ( uniformID * MAX_UNIFORM_ARRAY_SIZE );
	
	return this->GetUniformHandle ( buffer, uniformID, index );
}

//----------------------------------------------------------------//
MOAIShaderUniformSchema::MOAIShaderUniformSchema () {
}

//----------------------------------------------------------------//
MOAIShaderUniformSchema::~MOAIShaderUniformSchema () {
}

//----------------------------------------------------------------//
void MOAIShaderUniformSchema::SetUniform ( mrb_state* M, int idx, void* buffer, u32 uniformID, u32 index ) const {

	MOAIRubyState state ( M );

	MOAIShaderUniformHandle uniform = this->GetUniformHandle ( buffer, uniformID, index );
	
	if ( uniform.IsValid ()) {
		
		int size = state.GetParamsCount ();
		
		if ( size <= 4 ) {
		
			ZLVec4D vec = state.GetParamValue < ZLVec4D >( idx, ZLVec4D::ZERO );
			
			uniform.SetValue ( vec );
		}
		else {

			ZLMatrix4x4 mtx = ZLMatrix4x4::IDENT;

			switch ( size ) {
			
				case 6:
					uniform.SetValue ( state.GetParamValue < ZLAffine2D >( idx, ZLAffine2D::IDENT ));
					break;
				
				case 9:
					uniform.SetValue ( state.GetParamValue < ZLMatrix3x3 >( idx, ZLMatrix3x3::IDENT ));
					break;
				
				case 12:
					uniform.SetValue ( state.GetParamValue < ZLAffine3D >( idx, ZLAffine3D::IDENT ));
					break;
				
				case 16:
					uniform.SetValue ( state.GetParamValue < ZLMatrix4x4 >( idx, ZLMatrix4x4::IDENT ));
					break;
				
				default:
					MOAILogF ( M, ZLLog::LOG_ERROR, "Error setting matrix. Did not recognize number of paramaters (6, 9, 12 or 16)." );
			}
		}
	}
}

//================================================================//
// ::implementation::
//================================================================//

//----------------------------------------------------------------//

