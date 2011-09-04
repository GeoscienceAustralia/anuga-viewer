/*
	Authored under contract to Geoscience Australia by:

			James Hudson
            Nocturnal Entertainment Australia Pty Ltd

    copyright (C) 2009 Geoscience Australia
*/

#ifndef FILECHANGEDCHECK_H_
#define FILECHANGEDCHECK_H_

// needed to create a .lib file under win32/Visual Studio
#if defined(_MSC_VER)
    #define SWWREADER_EXPORT   __declspec(dllexport)
#else
    	#define SWWREADER_EXPORT
	#define _off_t int
#endif

/**
 * A class to detect if a file has changed.
 * Triggers once when a change has been detected. It will detect a change in the datestamp, or a change in the filesize.
 *  Datestamp can be unreliable if file updates are made too quickly or the app's innerloop updates too quickly, therefore the
 * file size is compared too. It does not do any checksumming, so this class may not be perfectly reliable.
 *
 * Usage
 *
 * FileChangedCheck checker("myfile.txt");
 * while (1)
 * {
 *      if(checker.isChanged())
 *     {
 *        reloadFile("myfile.txt");
 *     }
 * // do something
 * }
 *
 */
class SWWREADER_EXPORT FileChangedCheck
{
	public:

		/**
		 * Default constructor
		 */
		FileChangedCheck();

		/**
		 * Constructor with filename and intialisation.
		 * @param aFilename Filename of file to watch.
		 */
		FileChangedCheck(const std::string & aFilename)	{	watch(aFilename);	}

		/**
		 * Initialisation.
		 * Choose which file to watch.
		 * @param aFilename The name of the filename to watch for changes.
		 * @return false if file could not be watched, ie, it doesn't exist.
		 */
		bool watch(const std::string & aFilename);

		/**
		 * Has file changed.
		 * Triggers once when a change has been detected, then returns false as usual.
		 * Drop this in before any code that requires loading.
		 * @return true if the file has changed between calls.
		 */
		bool isChanged();

	private:
		std::string _filename;	/**< Name of file to watch */
		bool _exists;		/**< Does file exist */
		time_t _modificationTime;	/**< When the file was modified. */
		_off_t _oldLength;	/**< Old length of the file. */

};

#endif // FILECHANGEDCHECK_H_
