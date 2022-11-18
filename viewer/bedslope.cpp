

#include <bedslope.h>

#include <osg/Texture>
#include <osg/Texture2D>
#include <osg/PolygonMode>

#include <osgUtil/SmoothingVisitor>

// Bedslope colour when there is no texture
#define DEF_BEDSLOPE_COLOUR     (225.0f/255.0f), (190.0f/255.0f), (90.0f/255.0f), 1     // R, G, B, Alpha (brown)


// constructor
BedSlope::BedSlope(SWWReader* sww)
	: MeshObject("bedslope"),
	_loaded(false)
{
    // persistent
    _sww = sww;

	osg::Texture2D* texture = NULL;

    // bedslope texture
	_texture = false;
	if( sww->hasBedslopeTexture() )
	{
	    _texture = true;
		texture = new osg::Texture2D;
		texture->setDataVariance( osg::Object::DYNAMIC );
		texture->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP );
		texture->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP );
		texture->setImage( _sww->getBedslopeTexture() );
	}

	// material
	_material = new osg::Material();
	_material->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4(0.2, 0.15, 0.06, 1.0) );
	_material->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(0.6, 0.5, 0.2, 1.0) );
	_material->setSpecular( osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0, 0.0, 1.0) );
	_material->setEmission( osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0, 0.0, 1.0) );

	_material->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0, 0.0, 1.0) );

	_material->setSpecular( osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0, 0.0, 1.0) );
	_material->setEmission( osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0, 0.0, 1.0) );

	// state
	_stateset->setAttributeAndModes( _material, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
	_stateset->setMode( GL_BLEND, osg::StateAttribute::ON );
	_stateset->setMode( GL_LIGHTING, osg::StateAttribute::ON );
	if (texture)
	{
		_stateset->setTextureAttributeAndModes( 0, texture, osg::StateAttribute::ON );
	}

	onRefreshTextured(texture!=NULL);
}


void BedSlope::onRefreshData()
{
	if (!_sww->isElevationAnimated() && _loaded)
	{
		// it is already loaded and has a static mesh, so do nothing
		return;
	}

	// delete if exists
	if( _geom->getNumPrimitiveSets() )
	{
		_geom->removePrimitiveSet(0);  // reference counting does actual delete
	}

	// refresh data if file on disk has changed
	if ((_sww->refresh() == false) || (_sww->loadBedslopeVertexArray(_timestep) == false))
	{
		// error: could not reload file
		return;
	}

    // geometry from sww file
    osg::Vec3Array* vertices = _sww->getBedslopeVertexArray().get();
    _geom->setVertexArray( vertices );
    _geom->addPrimitiveSet( _sww->getBedslopeIndexArray().get() );

    osg::Vec4Array* color = new osg::Vec4Array(1);
    (*color)[0] = osg::Vec4( DEF_BEDSLOPE_COLOUR );
    _geom->setColorArray( color );
    _geom->setColorBinding( osg::Geometry::BIND_OVERALL );

    // Calculate per-vertex normals
    osgUtil::SmoothingVisitor* visitor = new osgUtil::SmoothingVisitor();
    visitor->smooth( *_geom );

	_loaded = true;
}

void BedSlope::onRefreshTextured(bool aIsTextured)
{
	if (aIsTextured && _texture)
	{
		// Textured mesh has a different state set
		_material->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(1.0, 1.0, 1.0, 1.0) );
		_geom->setTexCoordArray( 0, _sww->getBedslopeTextureCoords().get() );
	}
	else
	{
		_material->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(DEF_BEDSLOPE_COLOUR) );
	}
}