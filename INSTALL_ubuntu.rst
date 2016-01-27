
Ubuntu Install
==============


The following instructions will take you through installing Anuga Viewer on a completely fresh Linux distro, step by step.

#. Install linux distro. I used ubuntu - http://www.ubuntu.com/getubuntu/download.

#. To build anuga_viewer, install the following packages via::

	  sudo apt-get install git build-essential libgdal-dev libcppunit-dev libopenscenegraph-dev
	  
#. Get the anuga-viewer source from github.::
       
         git clone https://github.com/GeoscienceAustralia/anuga-viewer.git

#. Now, you should be able to build anuga-viewer. Do the following in the anuga-viewer folder::

         make
         sudo make install
      
#. Setup Environment variables in your `.bashrc` file::
    	
    	export SWOLLEN_BINDIR=/home/user/anuga-viewer/bin
    	export PATH=$PATH:$SWOLLEN_BINDIR
    	
#. From a new terminal, test the install by going to directory `~/anuga-viewer/data` and running the following command  (use escape to get out of the viewer)::

	anuga_viewer cairns.sww
