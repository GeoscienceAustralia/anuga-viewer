Usage
=====

Application to view `ANUGA <https://github.com/anuga-community/anuga_core>`_ sww output 
files. 




Controls
--------

Hold the left mouse button and drag to spin the model.
Hold the right mouse button and drag to change the zoom distance.
Hold both mouse buttons down or hold the middle button to slide around the model.
Hold down shift and click on the water with the left mouse button to show a timeseries plot. The data shown depends on the view mode.
Click on something that is not water, or click without holding shift to hide the timeseries plot.


Applying Textures
-----------------

Applying images (ie textures) to the bedslope mesh can be done with the --texture command line option. For example:

viewer.exe -texture ..\images\bedslope.jpg ..\data\cairns.sww

There are two possible ways the texture is mapped onto the bedslope mesh, based on the texture format.

   1. If the texture file contains GDAL geodata, this will be used to map the texture onto the mesh.

   2. Otherwise, the texture will be projected directly from above, in a rectangle that exactly bounds the bedslope.

In summary, if you load a GDAL texture, it will map the texture onto the mesh using the data in the texture file. 
Otherwise (if you are using a jpeg, tiff, etc.) it will naively map the texture onto the mesh, 
as if a projector beam was pointing directly downwards, situated so that the 
image would just cover every corner of the bedslope mesh.



How to Make a Movie
-------------------

   1. Anuga viewer can export a movie in a format that is only viewable with the viewer. 
      Press 1 to begin recording, and 3 to save the movie in Anuga's /bin folder as "movie.sww".

   2. To export the movie as an AVI that can be viewed by anyone, run anuga on the command line like so::

         anuga_viewer -movie <mymoviename> movie.swm

      The movie will be saved as a series of JPEG stills in a folder <mymoviename>.

   3a. <WINDOWS ONLY> To stitch these frames into a single AVI movie, 
        get the `VirtualDub program <http://www.virtualdub.org>`_. It is a video processing 
        tool which can load JPEG stills as a movie, and then save them as a standard video format.

	    a. select "File/Open video file..." from the menu. 
               Select frame_0_0.jpg in the folder ./Anuga Viewer/bin/<mymoviename>.

	    b. go to "Video" in the menu. Here you can set the codec format under "Compression..." 
               and resize or process the output with "Filters...".

	    c. select "File/Save as AVI..." to save the file as an AVI.

   3b. <LINUX ONLY> Use the MEncoder program to stitch the jpegs into a movie. 
       `Instructions here <http://www.mplayerhq.hu/DOCS/HTML/en/encoding-guide.html>`_

   4. You can now view this .avi file on any computer, or upload to a video site, etc.



Lighting
--------

By default, there is a single light in the ANUGA Viewer scene at a default position. 
To change its position you can use the following command line parameter::

   -lightpos <float>,<float>,<float> - x,y,z of bedslope directional light (z is up)

To remove lighting altogether and just have flat texturing, press 'l' to toggle lighting.




Troubleshooting
---------------

Q: I can't load TIF images.
A: Try opening the file in Gimp, or some other image viewer, then re-save it. Some TIF files seem to have a strange format that OSG/ANUGA can't read. Loading very large image files also seems to be dependent on the video card - you could try shrinking the image to 4096x4096 pixels or smaller.

Q: I have 2 monitors, and the viewer opens a window in each. How do I make it only show in a single monitor?
A: You need an extra command line option or environment setting to tell it which screen to open into::

   -- screen <screen num>

ie::

  anuga_viewer.exe --window 64 64 1024 768 --screen 0 -scale 1.5 -texture ..\images\bedslope.jpg ..\data\cairns.sww


