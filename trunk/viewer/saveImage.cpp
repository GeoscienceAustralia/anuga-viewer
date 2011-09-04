/*
  SWWViewer

  An OpenSceneGraph viewer for ANUGA .sww files.
  Copyright (C) 2004-2005, 2009 Geoscience Australia
*/

#include <iostream>
#include <osg/Image>
#include <osgDB/WriteFile>
#include <osgDB/FileNameUtils>
#include <customviewer.h>


bool saveImage(unsigned int playback_index, CustomViewer viewer, std::string moviedir)
{

   int x, y;
   unsigned int width, height;

   viewer.getCamera(0)->getProjectionRectangle(x, y, width, height);

   osg::ref_ptr<osg::Image> image = new osg::Image;

   image->readPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE);




   if ( osgDB::writeImageFile(*image, "image.jpg") )
   {
      std::cout << "Saved image " << playback_index << std::endl;
      return true;
   }

   return false;

}
