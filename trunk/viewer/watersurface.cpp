/*
  WaterSurface class

  An OpenSceneGraph viewer for ANUGA .sww files.
  Copyright (C) 2004, 2009 Geoscience Australia
*/


#include <watersurface.h>
#include <osg/AlphaFunc>
#include <osg/BlendFunc>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osg/TexEnv>
#include <osg/TexGen>

#include <osgDB/ReadFile>

#define DEF_ALPHA_THRESHOLD 0.05


// constructor
WaterSurface::WaterSurface(SWWReader* sww)
	: MeshObject("watersurface")
{
   // persistent
   _sww = sww;

   // environment map
   osg::Texture2D* texture = new osg::Texture2D;
   texture->setDataVariance(osg::Object::DYNAMIC);
   texture->setBorderColor(osg::Vec4(1.0f,1.0f,1.0f,0.5f));
   texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
   std::string* envmap = new std::string( _sww->getSwollenDir() + std::string("/../images/") + std::string("envmap.jpg") );
   texture->setImage(osgDB::readImageFile( envmap->c_str() ));
   _stateset->setTextureAttributeAndModes( 1, texture, osg::StateAttribute::ON );
   _stateset->setMode( GL_LIGHTING, osg::StateAttribute::ON );

   // surface transparency
   osg::BlendFunc* osgBlendFunc = new osg::BlendFunc();
   osgBlendFunc->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
   _stateset->setAttribute(osgBlendFunc);
   _stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
   _stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

   // discard pixels with an alpha value below threshold
   osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
   alphaFunc->setFunction( osg::AlphaFunc::GREATER, DEF_ALPHA_THRESHOLD );
   _stateset->setAttributeAndModes( alphaFunc, osg::StateAttribute::ON );

   // automatically generate texture coords
   osg::TexGen* texgen = new osg::TexGen;
   texgen->setMode( osg::TexGen::SPHERE_MAP );

   osg::TexEnv* texenv = new osg::TexEnv;
   texenv->setMode( osg::TexEnv::DECAL );
   //texenv->setMode( osg::TexEnv::BLEND );
   texenv->setColor( osg::Vec4(0.6f,0.6f,0.6f,0.2f) );
   _stateset->setTextureAttributeAndModes( 1, texgen, osg::StateAttribute::ON );
   _stateset->setTextureAttribute( 1, texenv );

}



WaterSurface::~WaterSurface()
{
}


void WaterSurface::onRefreshData()
{
	// delete if exists
	if( _geom->getNumPrimitiveSets() )
	{
		_geom->removePrimitiveSet(0);  // reference counting does actual delete
	}

	// refresh data if file on disk has changed
	if ((_sww->refresh() == false) || (_sww->loadStageVertexArray(_timestep) == false))
	{
		// error: could not reload file
		return;
	}

	// local reference to raw height field data
	osg::ref_ptr<osg::Vec3Array> vertices = _sww->getStageVertexArray();
	osg::ref_ptr<osg::Vec3Array> vertexnormals = _sww->getStageVertexNormalArray();
	osg::ref_ptr<osg::Vec4Array> colors = _sww->getStageColorArray();

	// geometry
	_geom->setVertexArray( vertices.get() );
	_geom->addPrimitiveSet( _sww->getBedslopeIndexArray().get() );

	// per vertex colors (we only modulate the alpha for transparency)
	_geom->setColorArray( colors.get() );
	_geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	// normals
	// Performance warning: OpenGL has no concept of per-primitive normals, so if we try to use
	// BIND_PER_PRIMITIVE, it will revert to glBegin/glEnd mode instead of display lists. This is SLOW!
	_geom->setNormalArray( vertexnormals.get() );
	_geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	// water surface corresponding to _timestep is now (re)loaded ...
}
