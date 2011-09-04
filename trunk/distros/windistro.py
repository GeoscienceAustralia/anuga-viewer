#
# Python script to create a windows distribution,
# currently a gzipped tar file. 
#


# convenience function for building file list
import os
def DLLFiles( directory, list ):
    return [ os.sep.join( [directory, name+'.dll'] ) for name in list ]


#---- NAME ------------------------------------------------------------

from datetime import datetime
distro = datetime.today().strftime('distro%Y%m%d.tgz')


  
#---- LOCAL DEPENDENCIES ----------------------------------------------

# local directories
localrootdir = '..'
localbindir = localrootdir + os.sep + 'bin'
localimagedir = localrootdir + os.sep + 'images'
localdatadir = localrootdir + os.sep + 'data'

# local files
localdlls = DLLFiles( localbindir, ['swwreader', 'gdal17', 'szlibdll', 'zlib1'] )
localfiles = [ 
               os.sep.join( [localimagedir, f] ) for f in \
               ['sky_small.jpg',   # sky texture
                'envmap.jpg',      # water surface environment map
                'bedslope.jpg']]   # surface texture
localfiles += [os.sep.join( [localbindir, f] ) for f in \
               ['viewer.exe',      # the executable
                'viewer_run.bat',  # helper script to set up a default view with a test file
				'fonts'+os.sep+'arial.ttf']]	# font file
localfiles += [os.sep.join( [localdatadir, f] ) for f in \
               ['cairns.sww']]     # a default test file
localfiles += [os.sep.join( [localrootdir, f] ) for f in \
               ['readme.txt']]
			   

# sample sww file for testing
localtestdir = localrootdir + os.sep + 'tests'
#localfiles.append(localtestdir + os.sep + 'cylinders.sww')
#localfiles.append(localtestdir + os.sep + 'cylinders.tif')


#----- NETCDF DEPENDENCIES --------------------------------------------

# netcdf directory
netcdfdir = os.environ['NETCDF_DIR'] + os.sep + "lib"
hd5dir = os.environ['HD5_LIB_PATH'] # used by netcdf - you should make this point to your hd5 install path

# netcdf DLLs
netcdfdlls = DLLFiles(netcdfdir, ['netcdf'])
netcdfdlls = netcdfdlls + DLLFiles(hd5dir, ['hdf5_hldll', 'hdf5dll'])


#----- OSG DEPENDENCIES -----------------------------------------------

# OSG directories
osgrootdir = os.environ['OSG_ROOT']
osgdir = os.sep.join( [osgrootdir,'OpenSceneGraph','bin'] )
osgplugindir = os.sep.join( [osgdir,'osgPlugins-2.8.2'] )

# OSG/Producer/OpenThreads DLLs
osgdllnames = [ 'osg55-'+f for f in ['osg', 'osgdb', 'osgGA', 'osgViewer', 'osgText', 'osgUtil'] ]
osgdlls = DLLFiles( osgdir, osgdllnames)
otdlls = DLLFiles( osgdir, ['ot11-OpenThreads'])

# supported database loaders (osgdb_XXX.dll)
osgimgformats = [ 'osgdb_'+f for f in ['jpeg', 'png', 'tiff', 'freetype'] ]
osgimgdlls = DLLFiles( osgplugindir, osgimgformats )

# All OSG DLLs
osgdlls = osgdlls + osgimgdlls + otdlls



#------ ARCHIVE -------------------------------------------------------

# all files above
files = localfiles + localdlls + osgdlls + netcdfdlls

files.sort()

import tarfile
import os.path
print "creating tgz archive %s" % distro
t = tarfile.open( distro, 'w:gz' )
for f in files:
    if f.find('..') == False:
        archivename = f[3:]     #local files get to keep their folders
    else:
        archivename = os.sep.join(["bin", os.path.basename(f)]) #global files are dumped in the bin folder

    print "\tadding ", f, " as ", archivename, ""
    t.add( f, archivename )
print "Archive file contents:"
t.list(True)
t.close()

cmd = "scp %s dee900@anusf.anu.edu.au:public_html/Projects/Swollen/Releases" % distro
#os.system(cmd)
