#include <iostream>
#include <sys/stat.h>

#include "filechangedcheck.h"

FileChangedCheck::FileChangedCheck()	:
	_exists(false),
	_modificationTime(0),
	_oldLength(0)
{
}

bool FileChangedCheck::watch(const std::string & aFilename)
{
	struct stat buf;

	// Check file exists
	if (stat(aFilename.c_str(), &buf) != 0)
	{
		_exists = false;
		return false;
	}

	_filename = aFilename;
	_modificationTime = buf.st_mtime;
	_oldLength = buf.st_size;
	_exists = true;

	return true;
}


bool FileChangedCheck::isChanged()
{
	struct stat buf;

	// Check file exists
	if (stat(_filename.c_str(), &buf) != 0)
	{
		if (_exists == true)
		{
			// File existed before but has been deleted - this is a change
			_exists = false;
			return true;
		}

		return false;
	}

	if (_exists == false)
	{
		// File was newly created
		return true;
	}

	if ((_modificationTime != buf.st_mtime) || (_oldLength != buf.st_size))
	{
		_modificationTime = buf.st_mtime;
		_oldLength = buf.st_size;
		return true;
	}

	return false;
}
