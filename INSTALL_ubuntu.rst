
Ubuntu Install
==============


The following instructions will take you through installing Anuga Viewer on a completely fresh Linux distro, step by step.

 *. Install linux distro. I used ubuntu - http://www.ubuntu.com/getubuntu/download.

 *. Install git. For instance using apt-get::
    
          sudo apt-get install git

 3. Get anuga-viewer source from: https://github.com/GeoscienceAustralia/anuga-viewer.git
       check out to ~/anuga_viewer/ (or wherever you want). I.e::
       
         git clone https://github.com/GeoscienceAustralia/anuga-viewer.git
      

 4. Verify that file structure is present as follows::

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
	
  7. Get cppUnit (optional - only for testing)::

	sudo apt-get install libcppunit-dev
	
    8. Get OpenScenGraph::

        sudo apt-get install libopenscenegraph-dev
        
    9. Now, you should be able to build anuga_viewer. Do the following in the anuga_viewer folder::

         make
         sudo make install
         sudo ldconfig
      
      10. Setup Environment variables in your `.bashrc` file::
    	
    	export SWOLLEN_BINDIR=/home/user/anuga-viewer/bin
    	export PATH=$PATH:$SWOLLEN_BINDIR
