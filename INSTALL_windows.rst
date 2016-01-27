Building from source
=====================

Get Source
-----------

Get the latest ANUGA viewer source via git.

INSTALL OSG
-----------

Download OpenSceneGraph-2.8.2 from sourceforge: http://openscenegraph.sourceforge.net. You want the full dev package: http://www.openscenegraph.org/downloads/stable_releases/OpenSceneGraph-2.8.2/binaries/Windows/VisualStudio9/openscenegraph-2.8.2-win32-x86-vc90-Release.zip

Unzip wherever you want and remove the major and minor versions from the path name - ie, rename C:\<3rd party lib folder>\OpenSceneGraph-2.8.2 to C:\<3rd party lib folder>\OpenSceneGraph

Go to Control Panel/System/Advanced/Environment Variables and add the following variable. OSG_ROOT => C:\<3rd party lib folder>

INSTALL NETCDF
--------------

These are painful to build; you can download the DLLs here instead: http://www.unidata.ucar.edu/software/netcdf/docs/faq.html#windows_netcdf4

Set the environment variable NETCDF_DIR to the location of binary netcdf.dll and netcdf.lib.


INSTALL GDAL
------------
You can download source from here: http://download.osgeo.org/gdal/gdal170.zip

Build it in MSVC. The build should be relatively painless - makegdal90.vcproj is the correct project for MSVC 2008. The build will dump all the build artifacts in the gdal root folder.

Go to Control Panel/System/Advanced/Environment Variables and add the following variable. GDAL_DIR => C:\<3rd party lib folder>

COMPILATION
-----------

Within VisualStudio, Open the solution in viewer visualstudio subdirectory

Build the solution


RUNNING
-------

The binary (viewer.exe on Windows) lives in the distribution's bin directory.  Test the build with any of the sample datasets in the  distribution's data subdirectory, e.g.::

   cd bin; ./viewer ../data/laminar.sww
