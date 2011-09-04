
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
//#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osg/Transform>
#include <osg/PolygonOffset>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgUtil/CullVisitor>

#include "project.h"

/**
 * A transform to move the skybox relative to the camera, to always keep it
 * a fixed distance from the camera.
 */
class MoveHorizonWithEyePointTransform : public osg::Transform
{

public:

	// Get the transformation matrix which moves from local coords to world coords
	virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const 
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.preMult(osg::Matrix::translate(eyePointLocal));
		}
		return true;
	}

	// Get transformation matrix which moves from world coords to local coords
	virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (cv)
		{
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.postMult(osg::Matrix::translate(-eyePointLocal));
		}
		return true;
	}
};


/**
 * A callback to prevent the skybox being included in the cull near/far computation.
 * This is needed to prevent the skybox bounding volume from degrading the zbuffer
 * precision and causing "z fighting".
 * OSG as of v2.8.2 does not include an API to do this neatly, so we need to manually
 * turn off the calculation for this geode.
 * Based on a fix detailed on osg-users@lists.openscenegraph.org
 */
struct DoNotIncludeInNearFarComputationCallback : public osg::NodeCallback
{
	 virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	 {
		osgUtil::CullVisitor *cv = dynamic_cast< osgUtil::CullVisitor*>( nv );

		if( cv )
		{
			osg::CullSettings::ComputeNearFarMode oldMode = osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES;
			oldMode = cv->getComputeNearFarMode();
			cv->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
			traverse(node, nv);
			cv->setComputeNearFarMode(oldMode);
		}
		else
		{
			traverse(node, nv);
		}
	 }
};


/**
 * The second part of this workaround is to override the near/far
 * frustum settings so that the skybox is not clipped by
 * whatever the far plane happens to be.
 * A custom frustum is created to allow the skybox, and then the
 * old frustum is restored.
 */
struct OverrideNearFarValuesCallback : public osg::Drawable::DrawCallback
{
	OverrideNearFarValuesCallback(float aRadius) :
		_radius(double(aRadius))
	{
	}

	 virtual void drawImplementation(osg::RenderInfo& renderInfo, const osg::Drawable* drawable) const
	 {
		 osg::Camera* currentCamera = renderInfo.getCurrentCamera();

		 if (currentCamera)
		 {
			 // Save old values.
			 osg::ref_ptr<osg::RefMatrixd> oldProjectionMatrix = new osg::RefMatrix;
			 oldProjectionMatrix->set(renderInfo.getState()->getProjectionMatrix());

			 // Get the individual values
			 double fovy, aspect_ratio, zNear, zFar;
			 bool result = oldProjectionMatrix->getPerspective(fovy, aspect_ratio, zNear, zFar);
			 assert(result && "Not a perspective xform matrix!");

			 // create a new matrix that includes the skybox
			 osg::ref_ptr<osg::RefMatrixd> projectionMatrix = new osg::RefMatrix;
			 projectionMatrix->makePerspective(fovy, aspect_ratio, 0.01, _radius*10);
			 renderInfo.getState()->applyProjectionMatrix(projectionMatrix.get());

			 // --- draw the drawable
			 drawable->drawImplementation(renderInfo);

			 // Reset the far plane to the old value.
			 renderInfo.getState()->applyProjectionMatrix(oldProjectionMatrix.get());
		 }
		 else
		 {
			 drawable->drawImplementation(renderInfo);
		 }
	 }

	 double _radius;
};


osg::Transform* Skybox_Create(float radius, const std::string filename)
{
	static const float rings = 20;
	static const float segments = 10;
	static const float zscale = -1.0f;	// Turn sphere inside out since we will be inside it

	osg::Geode* geode = new osg::Geode;
 
	// set up the texture state.    
	osg::Texture2D* texture = new osg::Texture2D;
	texture->setDataVariance(osg::Object::DYNAMIC);  // protect from being optimized away as static state.
	texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	texture->setImage(osgDB::readImageFile(filename.c_str()));
	
	osg::StateSet* stateset = new osg::StateSet;
	stateset->setTextureAttributeAndModes( 0, texture, osg::StateAttribute::ON );
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

	// to prevent being able to push the model back through the sky, we
	// do two things (i) ensure sky is drawn first, and (ii) skip writing
	// to the depth buffer
	stateset->setRenderBinDetails( -1, "RenderBin" );

	const float drho = osg::PI / (float) rings;		// angle change latitudinally with each ring
	const float dtheta = 2.0*osg::PI / (float) segments;	// angle change longitudinally with each segment

	const float du = 1.0f / segments;	// delta of u texcoord
	const float dv = 1.0f / rings;	// delta of v texcoord
	float v = 1.0f;	// v texcoord

	for (int i = 0; i < rings; i++ )
	{
		// build the rings
		osg::Geometry* geometry = new osg::Geometry;
		osg::Vec3Array* vl = new osg::Vec3Array;
		osg::Vec2Array* tl = new osg::Vec2Array;

		float rho = i * drho;	// current ring latitude
		float u = 0.0f;			// u texcoord
		for (int j = 0; j <= segments; j++ )
		{
			// build a ring slice by slice
			double theta = (j == segments) ? 0.0 : j * dtheta;
			double neg_sin_t = -sin(theta);
			double cos_t = cos(theta);

			double x = neg_sin_t * sin(rho);
			double y = cos_t * sin(rho);
			double z = cos(rho);

			double x2 = neg_sin_t * sin(rho+drho);
			double y2 = cos_t * sin(rho+drho);
			double z2 = cos(rho+drho);
             
			vl->push_back(osg::Vec3(x*radius, y*radius, z*radius*zscale));
			vl->push_back(osg::Vec3(x2*radius, y2*radius, z2*radius*zscale));

			// The uv's at the top and bottom of the sphere will have gaps because every second tri will have area 0
			tl->push_back(osg::Vec2(u, v));
			tl->push_back(osg::Vec2(u, v-dv));

			u += du;
		}

		assert( vl->size() == tl->size() );

		osg::Vec4Array* cl = new osg::Vec4Array;
		cl->push_back(osg::Vec4(1, 1, 1, 1));

		geometry->setVertexArray(vl);
		geometry->setTexCoordArray(0, tl);
		geometry->setColorArray(cl);
		geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
		geometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_STRIP, 0, vl->size()));

		geometry->setStateSet( stateset );
		geometry->setUseDisplayList( false );
		geometry->setDrawCallback(new OverrideNearFarValuesCallback(radius));
		  
		geode->addDrawable(geometry);
 
         v -= dv;
	}
	
	osg::Transform* transform = new MoveHorizonWithEyePointTransform;
	transform->addChild( geode );
	transform->setCullCallback(new DoNotIncludeInNearFarComputationCallback);

	return transform;
}
