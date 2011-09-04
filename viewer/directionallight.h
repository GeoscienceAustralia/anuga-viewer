
/*
    DirectionalLight class

    An OpenSceneGraph viewer for ANUGA .sww files.
    Copyright (C) 2004, 2009 Geoscience Australia
*/


#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H


#include <project.h>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/MatrixTransform>
#include <osg/StateAttribute>
#include <osg/StateSet>
#include <osg/Vec3>



class DirectionalLight
{

public:

    DirectionalLight(osg::StateSet* rootStateSet, int num=0);
    virtual osg::Group* get(){ return _transform; }
    virtual void setPosition(osg::Vec3 v);
    virtual osg::Vec3f* getPosition(){ return _position; }
    void setMatrix( osg::Matrixf matrix ){ _transform->setMatrix( matrix ); }
    void setAmbient( osg::Vec3f v ){ _light->setAmbient(osg::Vec4(v,1)); }
    void setDiffuse( osg::Vec3f v ){ _light->setDiffuse(osg::Vec4(v,1)); }
    const osg::Matrix getMatrix(){ return _transform->getMatrix(); }
    // void show(){ _marker->setNodeMask(1); }
    // void hide(){ _marker->setNodeMask(0); }


protected:

    osg::MatrixTransform* _transform;
    osg::Vec3* _position;
    osg::Vec3* _target;
    osg::Light* _light;
    osg::LightSource* _source;

    // osg::Node* _marker;
    // osg::Cylinder* _shape;
    // osg::Geometry* _geom;
    // osg::StateSet* _stateset;

    virtual ~DirectionalLight();

};


#endif  // DIRECTIONALLIGHT_H
