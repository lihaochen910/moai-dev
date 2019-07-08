// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moai-util/MOAIFileSystem.h>
#include <zl-vfs/ZLVfsFileSystem.h>
#include <zl-vfs/ZLVfsVirtualPath.h>
#include <zl-vfs/ZLVfsZipArchive.h>

// TODO: these are getting reintroduced somewhere; find them and kill them

#ifdef DeleteFile
	#undef DeleteFile
#endif

#ifdef RemoveDirectory
	#undef RemoveDirectory
#endif

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	affirmPath
	@text	Creates a folder at 'path' if none exists.

	@in		string path
	@out	nil
*/
mrb_value MOAIFileSystem::_affirmPath ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	cc8* path = state.GetParamValue < cc8* >( 1, "" );
	ZLFileSys::AffirmPath ( path );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	checkFileExists
	@text	Check for the existence of a file.

	@in		string filename
	@out	boolean exists
*/
mrb_value MOAIFileSystem::_checkFileExists ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	cc8* filename = state.GetParamValue < cc8* >( 1, "" );
	bool result = ZLFileSys::CheckFileExists ( filename );
	
	return state.ToRValue ( result );
}

//----------------------------------------------------------------//
/**	@lua	checkPathExists
	@text	Check for the existence of a path.

	@in		string path
	@out	boolean exists
*/
mrb_value MOAIFileSystem::_checkPathExists ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	cc8* path = state.GetParamValue < cc8* >( 1, "" );
	bool result = ZLFileSys::CheckPathExists ( path );
	
	return state.ToRValue ( result );
}

//----------------------------------------------------------------//
/**	@lua	copy
	@text	Copy a file or directory to a new location.

	@in		string srcPath
	@in		string destPath
	@out	boolean result
*/
mrb_value MOAIFileSystem::_copy ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	cc8* srcPath	= state.GetParamValue < cc8* >( 1, "" );
	cc8* destPath	= state.GetParamValue < cc8* >( 2, "" );
	
	bool result = ZLFileSys::Copy ( srcPath, destPath );
	
	return state.ToRValue ( result );
}

//----------------------------------------------------------------//
/**	@lua	deleteDirectory
	@text	Deletes a directory and all of its contents.

	@in		string path
	@opt	boolean recursive		If true, the directory and all contents beneath it will be purged.
									Otherwise, the directory will only be removed if empty.
	@out	boolean success
*/
mrb_value MOAIFileSystem::_deleteDirectory ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	cc8* path = state.GetParamValue < cc8* >( 1, "" );
	bool recursive = state.GetParamValue < bool >( 2, false );
	bool result = ZLFileSys::DeleteDirectory ( path, recursive, recursive );
	
	return state.ToRValue ( result );
}

//----------------------------------------------------------------//
/**	@lua	deleteFile
	@text	Deletes a file.

	@in		string filename
	@out	boolean success
*/
mrb_value MOAIFileSystem::_deleteFile ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	cc8* filename = state.GetParamValue < cc8* >( 1, "" );
	bool result = ZLFileSys::DeleteFile ( filename );
	
	return state.ToRValue ( result );
}

//----------------------------------------------------------------//
/**	@lua	getAbsoluteDirectoryPath
	@text	Returns the absolute path given a relative path.

	@in		string path
	@out	string absolute
*/
mrb_value MOAIFileSystem::_getAbsoluteDirectoryPath ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	cc8* path = state.GetParamValue < cc8* >( 1, "" );
	STLString result = ZLFileSys::GetAbsoluteDirPath ( path );
	
	return state.ToRValue ( result.c_str () );
}

//----------------------------------------------------------------//
/**	@lua	getAbsoluteFilePath
	@text	Returns the absolute path to a file. Result includes the
			file name.

	@in		string filename
	@out	string absolute
*/
mrb_value MOAIFileSystem::_getAbsoluteFilePath ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	cc8* filename = state.GetParamValue < cc8* >( 1, "" );
	STLString result = ZLFileSys::GetAbsoluteFilePath ( filename );
	
	return state.ToRValue ( result.c_str () );
}

//----------------------------------------------------------------//
/**	@lua	getRelativePath
	@text	Given an absolute path returns the relative path
			in relation to the current working directory or a
			user supplied 'base' directory.

	@in		string path
	@opt	string base
	@out	string path
*/
mrb_value MOAIFileSystem::_getRelativePath ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	cc8* path = state.GetParamValue < cc8* >( 1, "" );
	cc8* base = state.GetParamValue < cc8* >( 2, 0 );
	
	STLString result = ZLFileSys::GetRelativePath ( path, base );
	
	return state.ToRValue ( result.c_str () );
}

//----------------------------------------------------------------//
/**	@lua	getWorkingDirectory
	@text	Returns the path to current working directory.

	@out	string path
*/
mrb_value MOAIFileSystem::_getWorkingDirectory ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	STLString result = ZLFileSys::GetCurrentPath ();
	
	return state.ToRValue ( result.c_str () );
}

//----------------------------------------------------------------//
/**	@lua	getVirtualPathInfo
	@text	Given a virtual path, return the path to the archive and, if
			the path is to a file, the offset to the file record within
			the archive.
 
	@in		string path					A virtual path.
	@out	string pathToArchive		The archive to which the path is mapped.
	@out	string localPath			The relative portion of the path.
	@opt	string offsetToHeader		Offset to the file record header in the archive, or nil if not a file path.
	@opt	number uncompressedSize		Uncompressed size of the file or nil if not a file path.
	@opt	number compressedSize		Compressed size of the file or nil if not compressed or not a file path.
*/
mrb_value MOAIFileSystem::_getVirtualPathInfo ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	cc8* path = state.GetParamValue < cc8* >( 1, "" );

	ZLVfsVirtualPathInfo info = ZLVfsFileSystem::Get ().GetVirtualPathInfo ( path );

	/*int top = state.GetTop ();

	if ( info.mIsVirtual ) {
	
		state.Push ( info.mPathToArchive.c_str ());
		state.Push ( info.mLocalPath.c_str ());
		
		if ( info.mIsFile ) {
			state.Push (( u32 )info.mOffsetToHeader );
			state.Push (( u32 )info.mUncompressedSize );
			
			if ( info.mIsCompressed ) {
				state.Push (( u32 )info.mCompressedSize );
			}
		}
	}
	return state.GetTop () - top;*/
	return context;
}

//----------------------------------------------------------------//
/**	@lua	listDirectories
	@text	Lists the sub-directories contained in a directory, sorted alphabetically.
			Special directories '.' and '..' are omitted.
 
	@opt	string path				Path to search. Default is current directory.
	@out	table directories		A table of directory names (or nil if the path is invalid)
*/
mrb_value MOAIFileSystem::_listDirectories ( mrb_state* M, mrb_value context ) {
	UNUSED ( M );
	
	//STLString oldPath = ZLFileSys::GetCurrentPath ();
	//
	//if ( lua_type ( L, 1 ) == LUA_TSTRING ) {
	//	cc8* dir = lua_tostring ( L, 1 );
	//	if ( !ZLFileSys::SetCurrentPath ( dir )) {
	//		return context;
	//	}
	//}
	//
	//STLSet < STLString > sortedSet;
	//
	//ZLDirectoryItr dirItr;
	//dirItr.Start ();
	//while ( dirItr.NextDirectory ()) {
	//	sortedSet.insert ( dirItr.Current ());
	//}
	//ZLFileSys::SetCurrentPath ( oldPath );
	//
	//lua_newtable ( M );
	//
	//STLSet < STLString >::iterator sortedSetIt = sortedSet.begin ();
	//for ( int n = 1; sortedSetIt != sortedSet.end (); ++sortedSetIt, ++n ) {
	//
	//	lua_pushstring ( L, sortedSetIt->c_str ());
	//	#ifdef luaL_setn
	//		luaL_setn ( L, -2, n );  // new size
	//	#endif
	//	lua_rawseti ( L, -2, n );  // t[pos] = v
	//}
	//
	//return 1;
	return context;
}

//----------------------------------------------------------------//
/**	@lua	listFiles
	@text	Lists the files contained in a directory, sorted alphabetically.
 
	@opt	string path		Path to search. Default is current directory.
	@out	table files		A table of filenames (or nil if the path is invalid)
*/
mrb_value MOAIFileSystem::_listFiles ( mrb_state* M, mrb_value context ) {
	UNUSED ( M );
	
	//STLString oldPath = ZLFileSys::GetCurrentPath ();
	//
	//if ( lua_type ( L, 1 ) == LUA_TSTRING ) {
	//	cc8* dir = lua_tostring ( L, 1 );
	//	if( !ZLFileSys::SetCurrentPath ( dir )) {
	//		return context;
	//	}
	//}

	//STLSet < STLString > sortedSet;
	//
	//ZLDirectoryItr dirItr;
	//dirItr.Start ();
	//while ( dirItr.NextFile ()) {
	//	sortedSet.insert ( dirItr.Current ());
	//}
	//ZLFileSys::SetCurrentPath ( oldPath );
	//
	//lua_newtable ( M );
	//
	//STLSet < STLString >::iterator sortedSetIt = sortedSet.begin ();
	//for ( int n = 1; sortedSetIt != sortedSet.end (); ++sortedSetIt, ++n ) {
	//
	//	lua_pushstring ( L, sortedSetIt->c_str ());
	//	#ifdef luaL_setn
	//		luaL_setn ( L, -2, n );  // new size
	//	#endif
	//	lua_rawseti ( L, -2, n );  // t[pos] = v
	//}
	//
	//return 1;
	return context;
}

//----------------------------------------------------------------//
/**	@lua	loadFile
	@text	Open and read the entirity of a file into a string.

	@in		string filename
	@out	string contents		Returns empty string if file length is 0. Returns nil is no such file.
*/
mrb_value MOAIFileSystem::_loadFile ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIFileSystem, "S" )

	/*cc8* filename = state.GetParamValue < cc8* >( 1, 0 );

	if ( filename && ZLFileSys::CheckFileExists ( filename )) {
	
		ZLFileStream stream;
		if ( stream.OpenRead ( filename )) {
		
			size_t len = stream.GetLength ();
			if ( len > 0 ) {
			
				void* buffer = malloc ( len );
				stream.ReadBytes ( buffer, len );
				lua_pushlstring ( state, ( cc8* )buffer, len );
				free ( buffer );
			}
			else {
				state.Push ( "" );
			}
			stream.Close ();
			return 1;
		}
	}*/
	return context;
}

//----------------------------------------------------------------//
/**	@lua	mountVirtualDirectory
	@text	Mount an archive as a virtual filesystem directory.

	@in		string path			Virtual path.
	@opt	string archive		Name of archive file to mount. Default value is nil.
	@out	boolean success
*/
mrb_value MOAIFileSystem::_mountVirtualDirectory ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	cc8* path		= state.GetParamValue < cc8* >( 1, "" );
	cc8* archive	= state.GetParamValue < cc8* >( 2, 0 );
	
	bool result = ZLFileSys::MountVirtualDirectory ( path, archive );
	
	return state.ToRValue ( result );
}

//----------------------------------------------------------------//
// TODO: deprecate
mrb_value MOAIFileSystem::_pathFromRef ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	cc8* ref = state.GetParamValue < cc8* >( 1, "" );
	return state.ToRValue ( ZLFileSys::PathFromRef ( ref ).c_str () );
}

//----------------------------------------------------------------//
// TODO: deprecate
mrb_value MOAIFileSystem::_pathToRef ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	cc8* ref = state.GetParamValue < cc8* >( 1, "" );
	return state.ToRValue ( ZLFileSys::PathToRef ( ref ).c_str () );
}

//----------------------------------------------------------------//
/**	@lua	rename
	@text	Renames a file or folder.

	@in		string oldPath
	@in		string newPath
	@out	boolean success
*/
mrb_value MOAIFileSystem::_rename ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	cc8* oldPath = state.GetParamValue < cc8* >( 1, "" );
	cc8* newPath = state.GetParamValue < cc8* >( 2, "" );
	
	bool result = ZLFileSys::Rename ( oldPath, newPath );
	
	return state.ToRValue ( result );
}

//----------------------------------------------------------------//
/**	@lua	saveFile
	@text	Create or overwrite a file to contain the contents of a given string.

	@in		string filename
	@opt	string contents
	@out	nil
*/
mrb_value MOAIFileSystem::_saveFile ( mrb_state* M, mrb_value context ) {
	MOAI_RUBY_SETUP_SINGLE ( MOAIFileSystem, "S" )

	/*cc8* filename = state.GetParamValue < cc8* >( 1, 0 );
	
	ZLFileStream stream;
	if ( filename && stream.Open ( filename, ZLFileStream::READ_WRITE_NEW )) {
	
		if ( state.IsType ( 2, LUA_TSTRING )) {
			size_t len;
			cc8* str = lua_tolstring ( state, 2, &len );
			stream.WriteBytes ( str, len );
		}
		stream.Close ();
	}*/
	return context;
}

//----------------------------------------------------------------//
// TODO: deprecate
mrb_value MOAIFileSystem::_setPathRef ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	cc8* ref = state.GetParamValue < cc8* >( 1, "" );
	cc8* path = state.GetParamValue < cc8* >( 2, 0 );
	
	ZLFileSys::SetPathRef ( ref, path );
	
	return context;
}

//----------------------------------------------------------------//
/**	@lua	setWorkingDirectory
	@text	Sets the current working directory.

	@in		string path
	@out	boolean success
*/
mrb_value MOAIFileSystem::_setWorkingDirectory ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );
	
	cc8* path = state.GetParamValue < cc8* >( 1, "" );
	bool result = ZLFileSys::SetCurrentPath ( path );
	
	return state.ToRValue ( result );
}

//----------------------------------------------------------------//
/**	@lua	stripPKZipTimestamps
	@text	Strip the timestamp from a pkzip file. Useful in systems
			where periodically generated pkzip files are compared against
			each other using hashes. Once timestamps are stripeed, two
			pkzip files with the same contents should produce identical
			hashes.

	@in		string infilename
	@in		string outfilename
	@out	boolean success
*/
mrb_value MOAIFileSystem::_stripPKZipTimestamps ( mrb_state* M, mrb_value context ) {
	MOAIRubyState state ( M );

	cc8* infilename = state.GetParamValue < cc8* >( 1, "" );
	cc8* outfilename = state.GetParamValue < cc8* >( 2, "" );
	bool result = ZLFileSys::StripPKZipTimestamps ( infilename, outfilename );
	
	return state.ToRValue ( result );
}

//================================================================//
// MOAIFileSystem
//================================================================//

//----------------------------------------------------------------//
void MOAIFileSystem::RegisterRubyClass ( MOAIRubyState& state, RClass* klass ) {

	state.DefineInstanceMethod ( klass, "affirmPath", _affirmPath, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "checkFileExists", _checkFileExists, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "checkPathExists", _checkPathExists, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "copy", _copy, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "deleteDirectory", _deleteDirectory, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "deleteFile", _deleteFile, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getAbsoluteFilePath", _getAbsoluteFilePath, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getAbsoluteDirectoryPath", _getAbsoluteDirectoryPath, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getRelativePath", _getRelativePath, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getWorkingDirectory", _getWorkingDirectory, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "getVirtualPathInfo", _getVirtualPathInfo, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "listDirectories", _listDirectories, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "listFiles", _listFiles, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "loadFile", _loadFile, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "mountVirtualDirectory", _mountVirtualDirectory, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "pathFromRef", _pathFromRef, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "pathToRef", _pathToRef, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "rename", _rename, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "saveFile", _saveFile, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setPathRef", _setPathRef, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "setWorkingDirectory", _setWorkingDirectory, MRB_ARGS_NONE () );
	state.DefineInstanceMethod ( klass, "stripPKZipTimestamps", _stripPKZipTimestamps, MRB_ARGS_NONE () );

}
