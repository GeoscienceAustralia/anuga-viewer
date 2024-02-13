
Installation
~~~~~~~~~~~~

Windows Install (pre-compiled)
===============================

The version on sourceforge seems to still work fine on windows 10 and 11. 

`Here is the link for the Ppecompiled Viewer for Windows <https://sourceforge.net/projects/anuga/files/anuga_viewer_windows/>`_. 

Download the file `anuga_viewer.zip` and extract to a convenient location and associate the `cairns.sww` file 
in the data directory with the executable viewer.exe in the viewer's bin directory.


Ubuntu Install from Source
==========================

For Linux we recommend installation from source:

The following instructions will take you through installing Anuga Viewer on a fresh Linux distro, step by step.

#. To build anuga_viewer, install the following packages via::

    sudo apt-get install git build-essential libgdal-dev libcppunit-dev libopenscenegraph-dev
    
#. Get the anuga-viewer source from github.::
    
    git clone https://github.com/anuga-community/anuga-viewer.git
    
    
    If you are using an old version of OpenSceneGraph (ie the default in Unbuntu 18.04) 
    then clone the older version of the ANUGA Viewer via::

    git clone -b 3.2 https://github.com/anuga-community/anuga-viewer.git

#. Now, you should be able to build anuga-viewer. Do the following in the anuga-viewer folder::

        make
        sudo make install
    
#. Setup Environment variables in your `.bashrc` file::
        
        export SWOLLEN_BINDIR=/home/<<your_ubuntu_user_directory>>/anuga-viewer/bin
        export PATH=$PATH:$SWOLLEN_BINDIR
        
#. From a new terminal, test the install by going to directory `~/anuga-viewer/data` and running the following command  (use escape to get out of the viewer)::

    anuga_viewer cairns.sww



Building from source on Windows
===============================

Installing from source is quite complicated, and indeed has not been tested for many years as we have concentrated on the linux version. As such we recommend install using the old pre-compiled version.

Here is our old build instructions:

#. Get the latest ANUGA viewer source via git.

#. Install OSG. Download OpenSceneGraph-2.8.2 from sourceforge: http://openscenegraph.sourceforge.net. 

   You want the full dev package:

   http://www.openscenegraph.org/downloads/stable_releases/OpenSceneGraph-2.8.2/binaries/Windows/VisualStudio9/openscenegraph-2.8.2-win32-x86-vc90-Release.zip

   Unzip wherever you want and remove the major and minor versions from the path name - ie, rename C:\<3rd party lib folder>\OpenSceneGraph-2.8.2 to C:\<3rd party lib folder>\OpenSceneGraph

   Go to Control Panel/System/Advanced/Environment Variables and add the following variable.  
   
   OSG_ROOT => C:\<3rd party lib folder>

#. Install NETCDF

   These are painful to build; you can download the DLLs here instead: http://www.unidata.ucar.edu/software/netcdf/docs/faq.html#windows_netcdf4

   Set the environment variable NETCDF_DIR to the location of binary netcdf.dll and netcdf.lib.


#. Install GDAL

   You can download source from here: http://download.osgeo.org/gdal/gdal170.zip

   Build it in MSVC. The build should be relatively painless - makegdal90.vcproj is the correct project for MSVC 2008. The build will dump all the build artifacts in the gdal root folder.

   Go to Control Panel/System/Advanced/Environment Variables and add the following variable. GDAL_DIR => C:\<3rd party lib folder>

#. Compilation. Within VisualStudio, Open the solution in viewer visualstudio subdirectory

   Build the solution


#. Running. The binary (viewer.exe on Windows) lives in the distribution's bin directory.  Test the build with any of the sample datasets in the  distribution's data subdirectory, e.g.

   cd bin; ./viewer ../data/laminar.sww

