/*
  SWWReader

    A C++ class for reading ANUGA .sww files from within
    OpenSceneGraph.

    An SWW file is the visualization output of the ANUGA
    Shallow Water Wave equation solver. SWW files contain bedslope
    geometry and a sequence of stages at known timesteps.  The
    internal format is NetCDF. Use ncdump testfile.sww > out.txt to view.

    Authored under contract to Geoscience Australia by:

            Darran Edmundson
            ANU Supercomputer Facility Vizlab
            Australian National University
            Canberra, ACT 2600

    Copyright (C) 2004, 2009 Geoscience Australia
*/

#ifndef SWWREADER_H
#define SWWREADER_H

#include <string>
#include <project.h>
#include <iostream>
#include <osg/Geometry>

#include <filechangedcheck.h>


// needed to create a .lib file under win32/Visual Studio
#if defined(_MSC_VER)
    #define SWWREADER_EXPORT   __declspec(dllexport)
#else
    #define SWWREADER_EXPORT
#endif


typedef std::vector<unsigned int> triangle_list;

/**
 * Reader for an SWW file
 */
class SWWREADER_EXPORT SWWReader
{

public:

	/**
	 * Type of timeseries data represented by a 1D FloatArray
	 */
	enum TimeSeriesType
	{
		TSTYPE_MOMENTUM_MAGNITUDE = 0,	/**< Magnitude of momentum */
		TSTYPE_STAGE,	/** < Water height in absolute metres. */
	};


    SWWReader(const std::string& filename);

    virtual bool isValid() {return _valid;}

	virtual bool isElevationAnimated() {	return _elevationAnimated;	}

    // bedslope
    virtual osg::ref_ptr<osg::Vec3Array> getBedslopeVertexArray() {return _bedslopevertices;}
    virtual osg::ref_ptr<osg::Vec3Array> getBedslopeNormalArray() {return _bedslopenormals;}
    virtual osg::ref_ptr<osg::DrawElementsUInt> getBedslopeIndexArray() {return _bedslopeindices;}
    virtual osg::ref_ptr<osg::Vec3Array> getBedslopeCentroidArray() {return _bedslopecentroids;}
    virtual osg::ref_ptr<osg::Vec2Array> getBedslopeTextureCoords();

    virtual bool hasBedslopeTexture() {return (_state.bedslopetexturefilename != NULL);}
    virtual void setBedslopeTexture( std::string filename );
    virtual osg::Image* getBedslopeTexture();

	/**
	 * Load the bedslope at the given timestep.
	 * Note that the bedslope may deform dynamically at each timestep (ie a dam burst),
	 * or it may remain static.
	 * @param aIndex Timestep to load
	 * @return true if no error
	 */
	virtual bool loadBedslopeVertexArray(unsigned int aIndex);

    // stage
    virtual bool loadStageVertexArray(unsigned int index);
	virtual osg::ref_ptr<osg::Vec3Array> getStageVertexArray() {	return _stagevertices;	}
    virtual osg::ref_ptr<osg::Vec3Array> getStageVertexNormalArray() {return _stagevertexnormals;}
    virtual osg::ref_ptr<osg::Vec4Array> getStageColorArray() {return _stagecolors;}

	/**
	 * Given a polygon index, return the stage/momentum timeseries data at that point.
	 */
	virtual bool getTimeSeries(unsigned int aPolyIndex, TimeSeriesType aPlotType, osg::ref_ptr<osg::FloatArray> aData);
	/**
	 * Get the actual simulation time when this timestep occurred.
	 * @return the time that this timestep occurred in seconds
	 */
	virtual float getTime(unsigned int index);
    virtual size_t getNumberOfVertices() {return _npoints;}
    virtual unsigned int getNumberOfTimesteps() {return _ntimesteps;}

    virtual float getAlphaMin() {return _state.alphamin;}
    virtual float getAlphaMax() {return _state.alphamax;}
    virtual float getHeightMin() {return _state.heightmin;}
    virtual float getHeightMax() {return _state.heightmax;}
    virtual float getCullAngle() {return _state.cullangle;}

    virtual void setAlphaMin( float value ) {_state.alphamin = value;}
    virtual void setAlphaMax( float value ) {_state.alphamax = value;}
    virtual void setHeightMin( float value ) {_state.heightmin = value;}
    virtual void setHeightMax( float value ) {_state.heightmax = value;}

    virtual void setCullAngle( float value ) {_state.cullangle = value;}
    virtual void toggleCulling() {_state.culling = _state.culling ? false : true;}
    virtual bool getCulling() {return _state.culling;}
    virtual void setCulling(bool value) {_state.culling = value;}
    
    virtual triangle_list getConnectivity(unsigned int index) {return _connectivity.at(index);}

    const std::string getSwollenDir() {return *(_state.swollendirectory);}
    virtual void setSwollenDir(const std::string path) {_state.swollendirectory = new std::string(path);}

	virtual bool refresh();


protected:

    virtual ~SWWReader();

	/**
	 * Free all resources associated with this loaded file.
	 */
	void clear();

	/**
	 * Reload all the data from this file.
	 */
	bool load();

	/**
	 * Get the bounding volume of the bedslope mesh.
	 * @param aZData pointer to z data for the bedslope mesh.
	 */
	void getBedslopeBoundingVolume(const float * aZData);

private:
	/**
	 * Access the bedslope heights.
	 * sww file must be open or method will fail
	 * @param aTimestep timestep index
	 * @return pointer to bedslope height data
	 */
	float * loadBedslopeZ(unsigned int aTimestep);

protected:

    // state contains all the info needed to serialize
    struct
    {
          float alphamax;  // define alpha (transparency) function
          float alphamin;
          float heightmax;
          float heightmin;

          float cullangle;  // cull triangles with steepness angle above this value
          bool culling;   // culling is on or off

          std::string* swwfilename;
          std::string* bedslopetexturefilename;
          std::string* swollendirectory;

    } _state;


    // constructor determines SWW validity (netcdf + proper structure)
    bool _valid;

    // netCDF file id
    int _ncid;

    // netcdf dimension ids
    int _nvolumesid, _nverticesid, _npointsid, _ntimestepsid;

    // netcdf dimension values
    size_t _nvolumes, _nvertices, _npoints, _ntimesteps;

    // netcdf variable ids
    int _xid, _yid, _zid, _volumesid, _timeid, _stageid, _xmomentumid, _ymomentumid;

    // netcdf variable values (allocated in constructor)
    float *_px, *_py, *_pz, *_ptime, *_pstage, *_pxmomentum, *_pymomentum;
    unsigned int *_pvolumes;

    // fixed geometry - this is the land and other objects which are static
    osg::ref_ptr<osg::DrawElementsUInt> _bedslopeindices;
    osg::ref_ptr<osg::Vec2Array> _bedslopetexcoords;

	// land geometry that can change per timestep
    osg::ref_ptr<osg::Vec3Array> _bedslopevertices;
    osg::ref_ptr<osg::Vec3Array> _bedslopenormals;
	osg::ref_ptr<osg::Vec3Array> _bedslopecentroids;

    // water geometry that changes per timestep
    osg::ref_ptr<osg::Vec3Array> _stagevertices;
    osg::ref_ptr<osg::Vec3Array> _stageprimitivenormals;
    osg::ref_ptr<osg::Vec3Array> _stagevertexnormals;
    osg::ref_ptr<osg::Vec4Array> _stagecolors;

    // optional geodata for bedslope texture map
    struct
    {
        bool hasData;
        int xresolution, yresolution;
        float xorigin, yorigin;
        float xpixel, ypixel;
        float rotation;
    } _bedslopegeodata;


	// bedslope vertex scale and shift factors (for normalizing to unit cube)
	float _xscale, _yscale, _zscale, _scale;
	float _xoffset, _yoffset, _zoffset;
	float _xcenter, _ycenter, _zcenter;
	
	// sww file can contain optional global offset attributes
	float _xllcorner, _yllcorner;
	
	// stack of return values from netcdf function calls
	std::vector<int> _status;

	bool _elevationAnimated;	/**< True if the elevation data is animated */

	// error checker (iterates through _status stack)
	bool _statusHasError();
	
	// triangle connectivity, list (indexed by vertex number) of 
	// lists (indices of triangles sharing this vertex)
	std::vector<triangle_list> _connectivity;
	
	FileChangedCheck _fileChanged;	/**< Monitor this file for disk changes. */
};

#endif  // SWWREADER_H
