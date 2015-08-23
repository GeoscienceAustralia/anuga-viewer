
Ubuntu Install
==============


The following instructions will take you through installing Anuga Viewer on a completely fresh Linux distro, step by step.

    1. Install linux distro. I used ubuntu - http://www.ubuntu.com/getubuntu/download. Used VirtualBox to run it inside a VM.

    2. Install git. 

    3. Get anuga-viewer source from: https://github.com/GeoscienceAustralia/anuga-viewer.git
       check out to ~/anuga_viewer/ (or wherever you want).

    4. Verify that file structure is present as so::

        ~/anuga_viewer/
	    + viewer
		main.cpp
		...
	    + include
		swwreader.h
		...
	    + swwreader
		swwreader.cpp
		...
	    Doxyfile
	    Makefile
	    INSTALL.rst
	    ...

       To build anuga_viewer, you will need the following packages. 
       Some of these may be installed already. 
       If there are still packages missing, google is your friend.
	
    5. Get standard GNU build tools::

	  sudo apt-get install build-essential 
	
    6. Get GDAL (Geospatial Data Abstraction Library)::

	sudo apt-get install libgdal-dev
	
    7. Get git::

	sudo apt-get install git

    8. Get cppUnit (optional - only for testing)::

	sudo apt-get install libcppunit-dev
	
    9. Get OpenScenGraph::

        sudo apt-get install libopenscenegraph-dev  


   
    10. Now, you should be able to build anuga_viewer. Do the following in the anuga_viewer folder::

         make
         sudo make install
         sudo ldconfig

      the executable will be created in the anuga_viewer/bin folder. You might want to add that directory to your path environment variable.
