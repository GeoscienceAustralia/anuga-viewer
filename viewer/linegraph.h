#ifndef LINEGRAPH_H_
#define LINEGRAPH_H_

/**
 * Class to show a line graph of a 1D array.
 */
class LineGraph
{
public:
	LineGraph();

	/**
	 * Setup the linegraph geometry description
     * @aMinY the minimum Y value on the graph - this will be subtracted from
     *        all data
	 */
	osg::Geode * setUpScene(const std::string & aLabel, const osg::FloatArray * aData, float aTimeLength, const osg::Vec3 & aPos, const osg::Vec2 & aSize);

	/**
	 * Get the linegraph's geode
	 */
	osg::Geode * getGeode()	{	return _geode;	}

protected:

	osg::Geometry* createBackgroundRectangle(const osg::Vec3& pos, const float width, const float height, osg::Vec4& color);
	osg::Geometry* createGraphGrid(const osg::Vec3& pos, const float width, const float height, osg::Vec4& color, float aXCount, float aYCount);
    
    /**
     *  Create the actual graphed data.
     */
	osg::Geometry* createGraphGeometry(const osg::Vec3& pos, float width, float height, const osg::Vec4& colour, const osg::FloatArray * aData);

protected:
	osg::Geode * _geode;
};

/**
 * Create axes
 */
osg::Node * Axes_Create(const osg::BoundingBox & aBB);

osg::Geometry* IntensityBar_Create(const osg::Vec3& pos, const float width, const float height);

#endif	// LINEGRAPH_H_
