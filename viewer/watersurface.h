
/*
    WaterSurface class

    An OpenSceneGraph viewer for ANUGA .sww files.
    Copyright (C) 2004, 2009 Geoscience Australia
*/


#ifndef WATERSURFACE_H
#define WATERSURFACE_H


#include <project.h>
#include <swwreader.h>
#include <osg/Geode>
#include <osg/StateAttribute>

#include "meshobject.h"

/**
 * An animating water surface mesh.
 */
class WaterSurface : public MeshObject
{

public:

    WaterSurface(SWWReader *sww);

protected:

    virtual ~WaterSurface();
	
	void onRefreshData();


};


#endif  // WATERSURFACE_H

