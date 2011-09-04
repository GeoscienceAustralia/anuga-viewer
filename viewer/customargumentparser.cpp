/*
  SWWViewer

  An OpenSceneGraph viewer for ANUGA .sww files.
  copyright (C) 2004-2005 Geoscience Australia
*/

#include "customargumentparser.h"
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <osgDB/FileNameUtils>


#define MAX_LINE_LENGTH 200

using namespace osg;


CustomArgumentParser::CustomArgumentParser(int* argc, char **argv) : ArgumentParser(argc, argv)
{

   // last argument is either .sww or .swm filename
   _filename = new std::string(argv[*argc-1]);

   // macro file?
   if( osgDB::getLowerCaseFileExtension(*_filename) == std::string("swm") )
   {
      _isswm = true;

      // attempt to open the macro file ...
      std::fstream f;
      f.open( _filename->c_str(), std::fstream::in );
      if( f.is_open() )
      {
         char str[MAX_LINE_LENGTH];

         // FIXME: verify this really is the SWM header ...
         f.getline(str,MAX_LINE_LENGTH);

         // read the stored argc count
         f.getline(str,MAX_LINE_LENGTH);
         sscanf( str, "# SWM CLP %d", &_nargs );

         // we will modify the argv array to include the original 
         // argv[1] ... argv[argc-1] (ie., ignore the final argument which is the swm 
         // filename) extended by the stored arguments contained in the macro file.
         char** newargv = new char*[*argc-1+_nargs];

         // current command line params (sans last entry, the .swm file)
         for( int i=0; i<*argc-1; i++ )
            newargv[i] = argv[i];

         // now fill in the stored argv[] from the macro file
         for( int i=0; i<_nargs; i++ )
         {
            f.getline(str,MAX_LINE_LENGTH);
            char *s = new char[strlen(str)+1];
            strcpy(s,str);
            newargv[*argc-1+i] = s;
         }

         // change baseclass private members to the extended argc/argv
         *_argc = *argc - 1 + _nargs;
         _argv = newargv;


         std::cout << "Modified argument list based on SWM contents" << std::endl;
         for( int i=0; i<*_argc; i++ )
            std::cout << "    " << i << ":  " << _argv[i] << std::endl;

      }
      f.close();

   }

   else

   {
      _isswm = false;

      // local copies of current argc, argv[] as we may have to save them on '3' save.
      // note that we don't save argv[0] which is the program name.
      _nargs = *argc - 1;
      for( int i=1; i<*argc; i++ )
         _vargs.push_back( std::string(argv[i]) );
   } 

   SetUsage();
}


void CustomArgumentParser::SetUsage()
{
	// set up the usage document
	std::string appname = getApplicationName();
	osg::ApplicationUsage & usage = *getApplicationUsage();
	usage.setDescription( appname );
	usage.setCommandLineUsage("viewer [options] swwfile ...");
	usage.addCommandLineOption("-help", "Display this information");
	usage.addCommandLineOption("-scale <float>", "Vertical scale factor");
	usage.addCommandLineOption("-tps <rate>", "Timesteps per second");
	usage.addCommandLineOption("-hmin <float>", "Height below which transparency is set to zero");
	usage.addCommandLineOption("-hmax <float>", "Height above which transparency is set to alphamax");
	usage.addCommandLineOption("-alphamin <float 0-1>", "Transparency value at hmin");
	usage.addCommandLineOption("-alphamax <float 0-1>", "Maximum transparency clamp value");
	usage.addCommandLineOption("-lightpos <float>,<float>,<float>", "x,y,z of bedslope directional light (z is up, default is 1, 1, 1)");
	usage.addCommandLineOption("-movie <dirname>", "Save numbered images to named directory and quit");
	usage.addCommandLineOption("-loop", "Repeated (looped) playback of .swm files");
	usage.addCommandLineOption("-nosky", "Omit background sky");
	usage.addCommandLineOption("-cullangle <float angle 0-90>", "Cull triangles steeper than this value");
	usage.addCommandLineOption("-texture <file>", "Image to use for bedslope topography");
	usage.addCommandLineOption("-version", "Revision number and creation (not compile) date");
}


void CustomArgumentParser::write(std::ostream& s)
{
   s << "# SWM CLP " << _nargs << std::endl;

   for( int i=0; i < _nargs; i++ )
      s << _vargs[i] << std::endl;

}
