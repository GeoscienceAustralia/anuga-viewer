
#include <sstream>
#include <iomanip>
#include <stdio.h>

#include <osg/io_utils>
#include <osg/Export>
#include <osg/MatrixTransform>
#include <osgViewer/Renderer>
#include <osgViewer/Viewer>

#include <osg/PolygonMode>
#include <osg/Geometry>
#include <osgText/Text>

#include "project.h"
#include "hud.h"
#include "linegraph.h"

static const char * FONT_NAME = "fonts/arial.ttf";

static const float MAXMIN_ROUNDING = 10.0f;	//Rounding of Y scale 1 is to 0dp, 10 is to 1dp
static const char *PRECISION = "%0.2f m";	// Formatting string for precision

LineGraph::LineGraph():
	_geode(NULL)
{
}


osg::Geometry* LineGraph::createBackgroundRectangle(const osg::Vec3& pos, const float width, const float height, osg::Vec4& color)
{
    osg::StateSet *ss = new osg::StateSet;

    osg::Geometry* geometry = new osg::Geometry;

    geometry->setUseDisplayList(true);
    geometry->setStateSet(ss);

    osg::Vec3Array* vertices = new osg::Vec3Array;
    geometry->setVertexArray(vertices);

    vertices->push_back(osg::Vec3(pos.x(), pos.y(), 0));
    vertices->push_back(osg::Vec3(pos.x(), pos.y()-height,0));
    vertices->push_back(osg::Vec3(pos.x()+width, pos.y()-height,0));
    vertices->push_back(osg::Vec3(pos.x()+width, pos.y(),0));

    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(color);
    geometry->setColorArray(colors);
    geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

    osg::DrawElementsUInt *base =  new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS,0);
    base->push_back(0);
    base->push_back(1);
    base->push_back(2);
    base->push_back(3);

    geometry->addPrimitiveSet(base);

    return geometry;
}


osg::Geode * LineGraph::setUpScene(const std::string & aLabel, const osg::FloatArray * aData, float aTimeLength, const osg::Vec3 & aPos, const osg::Vec2 & aSize)
{
	assert(!_geode);

    const float characterSize = 20.0f;

    // rescale to put the data to be graphed between 0 and 1
    osg::FloatArray *normalised_data = NULL; 
    
    if (aData)
    {
        normalised_data = new osg::FloatArray(*aData, osg::CopyOp::DEEP_COPY_ALL);
    }

	_geode = new osg::Geode;

	osg::Vec3 pos = aPos;
    osg::Vec4 backgroundColor(1.0, 1.0, 1.0f, 1.0);
    osg::Vec4 graphColour(0.0, 0.0, 0.0f, 1.0);
	osg::Vec4 lineColour(0.2, 0.2, 0.5f, 1.0);

	// --- label
    {
		osg::ref_ptr<osgText::Text> graphLabel = new osgText::Text;
        _geode->addDrawable( graphLabel.get() );

        graphLabel->setColor(graphColour);
        graphLabel->setFont(FONT_NAME);
        graphLabel->setCharacterSize(characterSize);
        graphLabel->setPosition(pos);
        graphLabel->setText(aLabel);

        pos.y() -= characterSize*1.5f;
    }

    float backgroundMargin = 12;

	if (aData)
	{
		_geode->addDrawable(createBackgroundRectangle(pos + osg::Vec3(-backgroundMargin, backgroundMargin+characterSize*3.0f, 0),
				aSize.x(), aSize.y()+ characterSize*4.5f, backgroundColor) );
	}

	float max_y = 0;
	float min_y = 99999;

    if (aData)
	{
		for(unsigned int i=0; i<aData->size(); ++i)
		{
			float v = ceil(aData->at(i)*MAXMIN_ROUNDING)/MAXMIN_ROUNDING;
			max_y = osg::maximum(v, max_y);

			float low = floor(aData->at(i)*MAXMIN_ROUNDING)/MAXMIN_ROUNDING;
			min_y = osg::minimum(low, min_y);
		}

        float range_y = osg::maximum(max_y-min_y, 0.0001f);

        for(unsigned int i=0; i<aData->size(); ++i)
        {
            // Normalise the data between min and max
            normalised_data->at(i) = (aData->at(i)-min_y)/range_y;   
            assert((normalised_data->at(i) >=0.0f) && (normalised_data->at(i) <=1.0f));
        }


		// lines
		_geode->addDrawable(createGraphGrid(pos + osg::Vec3(0, 0, 0),
				aSize.x() - backgroundMargin*2, aSize.y() - backgroundMargin*2, lineColour, 20, (max_y-min_y)*MAXMIN_ROUNDING*2) );


		// graph labels
		osg::ref_ptr<osgText::Text> graphLabel = new osgText::Text;
        _geode->addDrawable( graphLabel.get() );

		char label[32];
		sprintf(label, PRECISION, min_y + range_y);

        graphLabel->setColor(graphColour);
        graphLabel->setFont(FONT_NAME);
        graphLabel->setCharacterSize(characterSize/1.5f);
        graphLabel->setPosition(pos + osg::Vec3(0, 5.0f, 0));
        graphLabel->setText(label);
    }
	else
	{
		// lines
		_geode->addDrawable(createGraphGrid(pos + osg::Vec3(0, 0, 0),
				aSize.x() - backgroundMargin*2, aSize.y() - backgroundMargin*2, lineColour, 20, 20) );
	}

    {
		// graph label for min y
		char label[32];
		sprintf(label, PRECISION, min_y);
        
		osg::ref_ptr<osgText::Text> graphLabel = new osgText::Text;
        _geode->addDrawable( graphLabel.get() );

        graphLabel->setColor(graphColour);
        graphLabel->setFont(FONT_NAME);
        graphLabel->setCharacterSize(characterSize/1.5f);
        graphLabel->setPosition(pos + osg::Vec3(0, - aSize.y(), 0));
        graphLabel->setText(label);
    }

    {
		// graph labels
		osg::ref_ptr<osgText::Text> graphLabel = new osgText::Text;
        _geode->addDrawable( graphLabel.get() );

		char label[32];
		sprintf(label, "%0.2f sec", aTimeLength);

        graphLabel->setColor(graphColour);
        graphLabel->setFont(FONT_NAME);
        graphLabel->setCharacterSize(characterSize/1.5f);
        graphLabel->setPosition(pos + osg::Vec3(aSize.x() - backgroundMargin*2 - 80, -aSize.y(), 0));
        graphLabel->setText(label);
    }

	if (aData)
	{
		// graph
		_geode->addDrawable(createGraphGeometry(pos + osg::Vec3(0, 0, 0),
				aSize.x() - backgroundMargin*2, aSize.y() - backgroundMargin*2, graphColour, normalised_data) );
	}

	_geode->setName("linegraph");

	return _geode;
}


osg::Geometry* LineGraph::createGraphGrid(const osg::Vec3& pos, const float width, const float height, osg::Vec4& color, float aXCount, float aYCount)
{
    osg::Geometry* geometry = new osg::Geometry;
    osg::Vec3Array* vertices = new osg::Vec3Array;

    geometry->setVertexArray(vertices);
    vertices->reserve(aXCount*2 + aYCount*2);

	for(unsigned int i=0; i<=aXCount; ++i)
    {
		float xpos = (float(i)/float(aXCount))*width;

        vertices->push_back(pos+osg::Vec3(xpos, 0, 0.0));
        vertices->push_back(pos+osg::Vec3(xpos, -height, 0.0));
    }

	for(unsigned int i=0; i<=aYCount; ++i)
    {
		float ypos = -(float(i)/float(aYCount))*height;

        vertices->push_back(pos+osg::Vec3(0, ypos, 0.0));
        vertices->push_back(pos+osg::Vec3(width, ypos, 0.0));
    }

    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(color);
    geometry->setColorArray(colors);
    geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

    geometry->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, vertices->size()));

    return geometry;
}


osg::Geometry* LineGraph::createGraphGeometry(const osg::Vec3& pos, float width, float height, const osg::Vec4& colour, const osg::FloatArray * aData)
{
	assert(aData);

	osg::Geometry* geometry = new osg::Geometry;
    osg::Vec3Array* vertices = new osg::Vec3Array;

    geometry->setVertexArray(vertices);
    vertices->reserve(aData->size());

	float step = width/float(aData->size()-1);

	for(unsigned int i=0; i<aData->size(); ++i)
    {
        vertices->push_back(pos+osg::Vec3(step*float(i), aData->at(i)*height-height, 0.0));
    }

    osg::Vec4Array* colours = new osg::Vec4Array;
    colours->push_back(colour);
    geometry->setColorArray(colours);
    geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

    geometry->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP, 0, aData->size()));

    return geometry;
}


osg::Node * Axes_Create(const osg::BoundingBox & aBB)
{
	osg::Group * gr = new osg::Group;
	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	osg::MatrixTransform * mt_top = new osg::MatrixTransform;
	LineGraph lg;
	LineGraph lg2;

	assert(aBB.valid());

	osg::Matrix mat, mat2;
	mat.postMultScale(osg::Vec3(0.001, 0.001, 0.001));

	osg::Vec3 scale = aBB._max - aBB._min;
	scale._v[2] = osg::maximum(scale._v[2], 0.1f);

	mat.postMultScale(scale);
	mat.postMultTranslate(aBB.corner(2));
	mt_top->setMatrix(mat);

	mat = mat.rotate(osg::PI_2, 1,0,0);
	mat.postMultTranslate(osg::Vec3d(0, 0, scale._v[2]*1025));
	mt->setMatrix(mat);
	mt->addChild(lg2.setUpScene("Elevation", NULL, 0, osg::Vec3(0,0,0), osg::Vec2(1000,1000)));

	gr->addChild(mt);
	gr->addChild(lg.setUpScene("Position", NULL, 0, osg::Vec3(0,0,0), osg::Vec2(1000,1000)));

	mt_top->addChild(gr);

	// --- new renderstate
	osg::StateSet* stateset = new osg::StateSet;
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	mt_top->setStateSet( stateset );


	return mt_top;
}


osg::Geometry* IntensityBar_Create(const osg::Vec3& pos, const float width, const float height)
{
    osg::StateSet *ss = new osg::StateSet;

    osg::Geometry* geometry = new osg::Geometry;

    geometry->setUseDisplayList(true);
    geometry->setStateSet(ss);

    osg::Vec3Array* vertices = new osg::Vec3Array;
    geometry->setVertexArray(vertices);

    vertices->push_back(osg::Vec3(pos.x(), pos.y(), 0));
    vertices->push_back(osg::Vec3(pos.x(), pos.y()-height/2,0));
    vertices->push_back(osg::Vec3(pos.x()+width, pos.y()-height/2,0));
    vertices->push_back(osg::Vec3(pos.x()+width, pos.y(),0));
    vertices->push_back(osg::Vec3(pos.x(), pos.y()-height/2, 0));
    vertices->push_back(osg::Vec3(pos.x(), pos.y()-height,0));
    vertices->push_back(osg::Vec3(pos.x()+width, pos.y()-height,0));
    vertices->push_back(osg::Vec3(pos.x()+width, pos.y()-height/2,0));

	osg::Vec4 r(1,0,0,1);
	osg::Vec4 g(0,1,0,1);
	osg::Vec4 b(0,0,1,1);

    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(b);
    colors->push_back(g);
    colors->push_back(g);
    colors->push_back(b);
    colors->push_back(g);
    colors->push_back(r);
    colors->push_back(r);
    colors->push_back(g);

    geometry->setColorArray(colors);
	geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    osg::DrawElementsUInt *base =  new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS,0);
	for (int i=0; i<8; i++)
	{
		base->push_back(i);
	}

    geometry->addPrimitiveSet(base);

    return geometry;
}
