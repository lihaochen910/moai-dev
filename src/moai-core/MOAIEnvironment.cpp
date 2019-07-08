// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-core/MOAIEnvironment.h>

#ifdef _WIN32
	#include <shlobj.h>
	typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
#elif defined( MOAI_OS_LINUX )
	#include <sys/utsname.h>
#endif

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@ruby	generateGUID
	@text	Generates a globally unique identifier. This method will be
			moved to MOAIUnique in a future release.

	@out	string GUID
*/
mrb_value MOAIEnvironment::_generateGUID ( mrb_state* M, mrb_value self ) {

	MOAIRubyState state ( M );

	STLString guid = ZLUnique::GetGUID ();
	return state.ToRValue ( guid.c_str () );
}

//----------------------------------------------------------------//
/**	@lua	getMACAddress
	@text	Finds and returns the primary MAC Address
 
	@out	string MAC
*/

mrb_value MOAIEnvironment::_getMACAddress ( mrb_state* M, mrb_value self ) {
	MOAIRubyState state ( M );
	
	STLString macAddress = ZLAdapterInfo::GetMACAddress ();
	return state.ToRValue ( macAddress.c_str () );
}

//----------------------------------------------------------------//
/**	@lua	setValue
	@text	Sets an environment value and also triggers the listener
			callback (if any).

	@in		string key
	@opt	variant value		Default value is nil.
	@out	nil
*/
mrb_value MOAIEnvironment::_setValue ( mrb_state* M, mrb_value self ) {
	MOAIRubyState state ( M );

	if ( state.CheckParams ( 1, "S*" ) ) {
		MOAIEnvironment& environment = MOAIEnvironment::Get ();
		environment.SetValue ( state.GetParamValue < cc8* >( 1, 0 ), state.GetParamValue ( 2 ) );
	}
	
	return self;
}

//================================================================//
// MOAIEnvironment
//================================================================//

//----------------------------------------------------------------//
void MOAIEnvironment::DetectEnvironment () {
	
	#if defined( MOAI_OS_WINDOWS )
	
		//printf ( "Env Windows\n" );
		this->SetValue ( MOAI_ENV_osBrand, "Windows" );
		
		UUID uuid;
		UuidCreateSequential ( &uuid );
		
		// For now, we'll just use the MAC address which is the last 6 bytes of the uuid.
		char buf[13];
		sprintf ( buf, "%02X%02X%02X%02X%02X%02X", uuid.Data4[2], uuid.Data4[3], uuid.Data4[4], uuid.Data4[5], uuid.Data4[6], uuid.Data4[7]);
		this->SetValue ( MOAI_ENV_udid, buf );
		
		char path[MAX_PATH];
		//HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path);
		SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path);
		this->SetValue ( MOAI_ENV_documentDirectory, path );
		
		const int BUFSIZE = 256;
		TCHAR pszOS[BUFSIZE];

		OSVERSIONINFOEX osvi;
		SYSTEM_INFO si;
		PGNSI pGNSI;				

		ZeroMemory(&si, sizeof(SYSTEM_INFO));
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		GetVersionEx((OSVERSIONINFO*) &osvi);
		
		pGNSI = (PGNSI) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
		if(NULL != pGNSI) {
			pGNSI(&si);
		}
		else {
			GetSystemInfo(&si);
		}

		if ( VER_PLATFORM_WIN32_NT==osvi.dwPlatformId && osvi.dwMajorVersion > 4 ) {
		
			strcpy ( pszOS, TEXT ( "Win" ));			
			if ( osvi.dwMajorVersion == 6 ) {
				if ( osvi.dwMinorVersion == 1 ) {
					if ( osvi.wProductType == VER_NT_WORKSTATION )
						strcat ( pszOS, TEXT ( "7" ));
					else strcat ( pszOS, TEXT ( "2008R2" ));
				}
				else if( osvi.dwMinorVersion == 0 ) {
					if( osvi.wProductType == VER_NT_WORKSTATION )
						strcat ( pszOS, TEXT ( "Vista" ));
					else strcat ( pszOS, TEXT ( "Server2008" ));
				}
			}
			else if ( osvi.dwMajorVersion == 5 ) {
				if (osvi.dwMinorVersion == 2) {				
					if( osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64) {
						strcat ( pszOS, TEXT ( "XPx64" ));
					}
					else {
						strcat ( pszOS, TEXT ( "Server2003" ));
					}
				}
				else if ( osvi.dwMinorVersion == 1 ) {
					strcat ( pszOS, TEXT ( "XP" ));
				}
				else if ( osvi.dwMinorVersion == 0 ) {
					strcat ( pszOS, TEXT ( "2000" ));
				}
			}
			
			this->SetValue ( MOAI_ENV_osVersion, pszOS );
		}
		else {
			this->SetValue ( MOAI_ENV_osVersion, "WinUnknown" );
		}
		
	#elif defined( MOAI_OS_LINUX )
		
		utsname sysID;

		if ( 0 == uname( &sysID ) ) {
			
			this->SetValue ( MOAI_ENV_osBrand, sysID.sysname );
			this->SetValue ( MOAI_ENV_osVersion, sysID.release );
		}
		else {
			this->SetValue ( MOAI_ENV_osBrand, "Linux" );
			this->SetValue ( MOAI_ENV_osVersion, "Unknown" );
		}

	#elif defined ( MOAI_OS_OSX )
	
		//printf ( "Env OSX\n" );
		this->SetValue ( MOAI_ENV_osBrand, "OSX" );
	  #if 0 /* doesn't compile yet */
		// OS Version
		SInt32 majorVersion,minorVersion,bugFixVersion;

		Gestalt(gestaltSystemVersionMajor, &majorVersion);
		Gestalt(gestaltSystemVersionMinor, &minorVersion);
		Gestalt(gestaltSystemVersionBugFix, &bugFixVersion);

		char buffer[256];
		sprintf(buffer, "%d.%d.%d",majorVersion,minorVersion,bugFixVersion);
		this->SetValue ( MOAI_ENV_osVersion, buffer );
	  #endif
	#else
		//printf ( "No environment detected\n" );
	#endif
}

//----------------------------------------------------------------//
MOAIEnvironment::MOAIEnvironment () {

	RTTI_SINGLE ( MOAIGlobalEventSource )
}

//----------------------------------------------------------------//
MOAIEnvironment::~MOAIEnvironment () {
}

//----------------------------------------------------------------//
void MOAIEnvironment::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineClassConst ( klass, "EVENT_VALUE_CHANGED", ( u32 )EVENT_VALUE_CHANGED );

	state.DefineClassConst ( klass, "CONNECTION_TYPE_NONE", ( u32 )CONNECTION_TYPE_NONE );
	state.DefineClassConst ( klass, "CONNECTION_TYPE_WIFI", ( u32 )CONNECTION_TYPE_WIFI );
	state.DefineClassConst ( klass, "CONNECTION_TYPE_WWAN", ( u32 )CONNECTION_TYPE_WWAN );
	
	state.DefineClassConst ( klass, "OS_BRAND_AMAZON", OS_BRAND_AMAZON );
	state.DefineClassConst ( klass, "OS_BRAND_ANDROID", OS_BRAND_ANDROID );
	state.DefineClassConst ( klass, "OS_BRAND_IOS", OS_BRAND_IOS );
	state.DefineClassConst ( klass, "OS_BRAND_LINUX", OS_BRAND_LINUX );
	state.DefineClassConst ( klass, "OS_BRAND_NOOK", OS_BRAND_NOOK );
	state.DefineClassConst ( klass, "OS_BRAND_OSX", OS_BRAND_OSX );
	state.DefineClassConst ( klass, "OS_BRAND_WINDOWS", OS_BRAND_WINDOWS );
	state.DefineClassConst ( klass, "OS_BRAND_UNAVAILABLE", OS_BRAND_UNAVAILABLE );

	state.DefineStaticMethod ( klass, "generateGUID", _generateGUID, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "getListener", MOAIGlobalEventSource::_getListener < MOAIEnvironment >, MRB_ARGS_REQ ( 1 ) );
	state.DefineStaticMethod ( klass, "getMACAddress", _getMACAddress, MRB_ARGS_NONE () );
	state.DefineStaticMethod ( klass, "setListener", MOAIGlobalEventSource::_setListener < MOAIEnvironment >, MRB_ARGS_REQ ( 2 ) );
	state.DefineStaticMethod ( klass, "setValue", _setValue, MRB_ARGS_REQ ( 2 ) );
}

//----------------------------------------------------------------//
void MOAIEnvironment::SetValue ( cc8* key ) {

	MOAIRubyState state = MOAIRubyRuntime::Get ().State ();
	state.SetClassField ( this->GetRubyClass ()->GetRClass (), key, mrb_nil_value () );
}
