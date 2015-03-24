

Install
=======

Note:: - this readme will be deprecated in favour of install guides for each platform. 

BUILDING FROM SOURCE
--------------------

1) The ANUGA viewer has several dependencies. 

   OpenThreads, OpenSceneGraph
		
	- http://openscenegraph.sourceforge.net
	- This version of anuga viewer works with OpenSceneGraph-2.8.2.
        - Windows: OSG has many dependencies - it may be easier to download 
	   	  the DLL's and libs than build everything
         - Linux: OSG should build easily - just download the source and run the 
	   	  configure/make/make install scripts. 
                  You may need to apt-get various components, such as the OpenGL libs.
         - ensure you can run the examples that come with OSG
                  prior to building the viewer (these live in the 
                  OpenSceneGraph/bin directory on Windows).

   NetCDF   
   
         - a self-describing binary file format I/O library
                 - current version is 4.0.1

         - Windows: http://my.unidata.ucar.edu/content/software/netcdf/index.html
         - Linux: use sudo apt-get install libnetcdf-dev
         - require the header file netcdf.h and the library proper to
           link against

   cppUnit

         - a unit testing framework for C++
         - http://cppunit.sourceforge.net
                 - used for regression testing and verification of new platforms
         - this is optional


2) Set environment variables:
   These may be automatically set by package installs, but please check your environment 
   variables for their presence before proceeding::

       OSG_ROOT :  root of your OpenSceneGraph installation
                   conforming to following layout

          OSG_ROOT
              /OpenThreads
                 /lib
                 /include
             /OpenSceneGraph
                 /lib
                 /include

      NETCDF_DIR :  location of binary netcdf.dll and netcdf.lib

      CPPUNIT :  root of c++ unit testing framework with needed subdirs 
                 /lib and /include


3) Compilation


   WINDOWS

     (i) Within VisualStudio, Open the solution in viewer visualstudio subdirectory

     (ii) Open Menu Tools|Options|Projects|VC++Directories and add a
          path to $(NETCDF_DIR)/bin and $(OSG_ROOT)/bin.

     (iii) Build the solution


   Mac OSX / Linux

        (i)   Run 'make' in top-level anuga_viewer directory.
	
        (ii)  Run 'sudo make install' in same directory to install the shared libraries.


4) The binary (viewer.exe on Windows) lives in the distribution's
   bin directory.  Test the build with any of the sample datasets in the
   distribution's data subdirectory, e.g.:: 
         
      cd bin; ./viewer ../data/laminar.sww
	 

	 	 
RUNNING anuga_viewer
====================

You can use standard OpenSceneGraph viewer parameters to set screen attributes, as well as some custom parameters.
Type -help as a parameter to see the full parameter list.




STEREO
======

Invoke through the commandline parameter --stereo
e.g.::
 
   anuga_viewer --stereo lwru2_variable_mesh.sww 

or::
  
  anuga_viewer --stereo HORIZONTAL_SPLIT lwru2_variable_mesh.sww 

See::

  anuga_viewer --help 

for more info  
  
  
Minor tweaks to re-enable stereo functionality. 
I have verified that this works on the vizlab active stereo system. 
For the GA passive stereo system, you'll need to set the following environment variables.

Environment Variable Name Value Description 

OSG_STEREO_MODE HORIZONTAL_SPLIT Use horizontal split stereo mode when in stereo 

OSG_SCREEN_DISTANCE 0.50 Set the distance the viewer is from screen in metres (default shown) 

OSG_SCREEN_HEIGHT 0.26 Set the height of image on the screen in metres (default shown) 

OSG_SCREEN_WIDTH 0.325 Set the width of image on the screen in metres (default shown) 

OSG_EYE_SEPARATION 0.06 Set the eye separation  interoccular distance (default shown.) 

OSG_SPLIT_STEREO_HORIZONTAL_SEPARATION 42 Set the number of pixels between the left and right viewports (default shown) 


The default physical dimensions are for a computer monitor. 
These need to be adjusted for the larger setup. 
Typical values might be 2m viewing distance from screen, screen dimensions of 2m x 1.5m. 
Eye separation obviously stays the same. Then just start swollen with the "dash dash stereo" flag.

The GA edge:

OSG_STEREO_MODE QUAD_BUFFER

OSG_SCREEN_DISTANCE 3.0 

OSG_SCREEN_HEIGHT 2.0

OSG_SCREEN_WIDTH 2.66

OSG_EYE_SEPARATION 0.06 

OSG_SPLIT_STEREO_HORIZONTAL_SEPARATION 0

	 
DEBUG
=====

set OSG_NOTIFY_LEVEL=DEBUG


