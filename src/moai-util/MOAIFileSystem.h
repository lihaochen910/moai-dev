// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIOFILESYSTEM_H
#define	MOAIOFILESYSTEM_H

//================================================================//
// MOAIFileSystem
//================================================================//
/**	@lua	MOAIFileSystem
	@text	Functions for manipulating the file system.
*/
class MOAIFileSystem :
	public ZLContextClass < MOAIFileSystem, MOAIRubyObject > {
private:

	//----------------------------------------------------------------//
	static mrb_value		_affirmPath					( mrb_state* M, mrb_value context );
	static mrb_value		_checkFileExists			( mrb_state* M, mrb_value context );
	static mrb_value		_checkPathExists			( mrb_state* M, mrb_value context );
	static mrb_value		_copy						( mrb_state* M, mrb_value context );
	static mrb_value		_deleteDirectory			( mrb_state* M, mrb_value context );
	static mrb_value		_deleteFile					( mrb_state* M, mrb_value context );
	static mrb_value		_getAbsoluteDirectoryPath	( mrb_state* M, mrb_value context );
	static mrb_value		_getAbsoluteFilePath		( mrb_state* M, mrb_value context );
	static mrb_value		_getRelativePath			( mrb_state* M, mrb_value context );
	static mrb_value		_getWorkingDirectory		( mrb_state* M, mrb_value context );
	static mrb_value		_getVirtualPathInfo			( mrb_state* M, mrb_value context );
	static mrb_value		_listDirectories			( mrb_state* M, mrb_value context );
	static mrb_value		_listFiles					( mrb_state* M, mrb_value context );
	static mrb_value		_loadFile					( mrb_state* M, mrb_value context );
	static mrb_value		_mountVirtualDirectory		( mrb_state* M, mrb_value context );
	static mrb_value		_pathFromRef				( mrb_state* M, mrb_value context );
	static mrb_value		_pathToRef					( mrb_state* M, mrb_value context );
	static mrb_value		_rename						( mrb_state* M, mrb_value context );
	static mrb_value		_saveFile					( mrb_state* M, mrb_value context );
	static mrb_value		_setPathRef					( mrb_state* M, mrb_value context );
	static mrb_value		_setWorkingDirectory		( mrb_state* M, mrb_value context );
	static mrb_value		_stripPKZipTimestamps		( mrb_state* M, mrb_value context );

public:
	
	DECL_RUBY_SINGLETON ( MOAIFileSystem )
	
	//----------------------------------------------------------------//
	void				RegisterRubyClass		( MOAIRubyState& state, RClass* klass );
};

#endif

