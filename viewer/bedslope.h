

/*
    BedSlope class

    An OpenSceneGraph viewer for ANUGA .sww files.
    copyright (C) 2004-2005 Geoscience Australia
*/


#ifndef BEDSLOPE_H
#define BEDSLOPE_H


#include <project.h>
#include <swwreader.h>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateAttribute>

#include "meshobject.h"

/**
 * The static geometry in the scene.
 */
class BedSlope : public MeshObject
{

public:

    BedSlope(SWWReader *sww);
    osg::Geode* get(){ return _node; }
    const osg::BoundingBox getBound(){ return _geom->getBound(); }

	/**
	 * Called on mesh data refresh.
	 * Updates the mesh vertices with new data from disk.
	 */
	void onRefreshData();

	/**
	 * Called when the texture state is refreshed.
	 * Updates the texture state and vertex coloring.
	 * @param aIsTextured true if mesh is textured
	 */
	void onRefreshTextured(bool aIsTextured);

protected:

    osg::Material* _material;

    virtual ~BedSlope(){;}
    bool _texture;
	bool _loaded;

};


#endif  // BEDSLOPE_H
