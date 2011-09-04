
#include <math.h>
#include <directionallight.h>
#include <osg/Texture2D>
// #include <osg/ShapeDrawable>
// #include <osgDB/ReadFile>

#include <iostream>

// #define DEF_DEFAULT_CYLINDER_RADIUS 0.25
// #define DEF_DEFAULT_CYLINDER_HEIGHT 0.50



DirectionalLight::DirectionalLight(osg::StateSet* rootStateSet, int num)
{
    // positioning container
    _transform = new osg::MatrixTransform;
    _transform->setCullingActive(false);

    // OpenGL light
    _light = new osg::Light;
    _light->setLightNum(num);

    // homogeneous coordinates (x,y,z,w), w=0 indicates position at infinity
    _light->setPosition( osg::Vec4( 0, 0, 1, 0 ) );

    _light->setAmbient( osg::Vec4( 0.7, 0.7, 0.7, 1.0 ) );
    _light->setDiffuse( osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
    _light->setSpecular( osg::Vec4( 1.0, 1.0, 1.0, 1.0) );

    // Scenegraph node
    _source = new osg::LightSource;
    _source->setReferenceFrame( osg::LightSource::RELATIVE_RF );
    _source->setLight( _light );
    _source->setLocalStateSetModes( osg::StateAttribute::ON ); 
    _source->setStateSetModes( *rootStateSet, osg::StateAttribute::ON );
    _transform->addChild( _source );

    // marker geometry, texture and state
    //_marker = osgDB::readNodeFile( "light.osg" );
    //_transform->addChild( _marker );

    setPosition( osg::Vec3(0,0,1) );  // default overhead
}



void DirectionalLight::setPosition(osg::Vec3 v)
{

    //std::cout << "[DirectionalLight::setPosition] vector = " << v << std::endl;
    osg::Vec3 origindir;
    osg::Quat quat;
    origindir.set( -v );
    origindir.normalize();
    quat.makeRotate( osg::Vec3(0,0,-1), origindir );

    _transform->setMatrix( osg::Matrix::rotate( quat ) * osg::Matrix::translate(v) );
}



DirectionalLight::~DirectionalLight()
{
}
