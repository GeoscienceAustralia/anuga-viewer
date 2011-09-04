#include <osg/PolygonMode>
#include <osg/StateSet>
#include <osg/Geode>
#include <osg/Geometry>

#include <swwreader.h>

#include "meshobject.h"


MeshObject::MeshObject(std::string aName) :
	_timestep(0),
	_wireframe(false),
	_dirtywireframe(true)  // will force wireframe refresh
{
   _dirtydata = true;  // will force load of watersurface

   // default is steep culling on
   _culling = true;
   _dirtyculling = true;  // will force culling ...

	_stateset = new osg::StateSet;
	_node = new osg::Geode;
	_geom = new osg::Geometry;

	// construct local scenegraph hierarchy
	_node->setName(aName);
	_node->addDrawable(_geom);
	_node->setStateSet(_stateset);

    _geom->setUseDisplayList( true );
}


void MeshObject::setCulling(bool value)
{
   if( value != _culling )
   {
      _culling = value;
      _dirtyculling = true;
      _dirtydata = true;
   }
}

void MeshObject::setTimeStep(unsigned int ts)
{
   if( ts != _timestep )
   {
      _timestep = ts;
      _dirtydata = true;
   }
}


void MeshObject::setWireframe(bool value)
{
   if( value != _wireframe )
   {
      _wireframe = value;
      _dirtywireframe = true;
   }
}


void MeshObject::update()
{
	// --- update culling state
   if( _dirtyculling )
   {
      _sww->setCulling( _culling );
      _dirtyculling = false;
   }

   // --- update mesh data
   if( _dirtydata )
   {
		onRefreshData();
		_dirtydata = false;
   }

   // --- update wireframe
	if( _dirtywireframe )
   {
      osg::PolygonMode* polyModeObj = 
         dynamic_cast<osg::PolygonMode*>(_stateset->getAttribute(osg::StateAttribute::POLYGONMODE));

      if (!polyModeObj) 
      {
         polyModeObj = new osg::PolygonMode;
         _stateset->setAttribute(polyModeObj);
      }

      switch( _wireframe )
      {
         case true :
            polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
            break;

         case false :
            polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
            break;
      }

      // desired wireframe mode now loaded ...
      _dirtywireframe = false;
   }


}
