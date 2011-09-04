/*
    Surface class

    An OpenSceneGraph viewer for ANUGA .sww files.
    Copyright (C) 2004, 2009 Geoscience Australia
*/


#ifndef SURFACE_H
#define SURFACE_H


#include <project.h>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateAttribute>



class Surface
{

public:

    Surface();
    virtual osg::Geode* get(){ return _node; }

protected:

    osg::Geode* _node;
    osg::Geometry* _geom;
    osg::StateSet* _stateset;
    osg::Material* _material;
    virtual ~Surface();

};


#endif  // SURFACE_H
