/*
  SWWReader

  Reader of SWW files from within OpenSceneGraph.

  copyright (C) 2004-2005 Geoscience Australia
*/

#include <swwreader.h>
#include <cstdlib>
#include <string>
#include <fstream>
#include <netcdf.h>
#include <osg/Notify>

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/FileNameUtils>

#include <stdlib.h>
#include <stdio.h>
#include <gdal_priv.h>

#define SAFE_DELETE_ARRAY(x) {if (x) { delete[](x); x=NULL;	}}

// Define this to use a fast square root algorithm - actual speed increases may depend on architecture
#define USE_FAST_SQRT

// compile time defaults
#define DEFAULT_CULLANGLE 85.0
#define DEFAULT_ALPHAMIN 0.8
#define DEFAULT_ALPHAMAX 1.0
#define DEFAULT_HEIGHTMIN 0.0
#define DEFAULT_HEIGHTMAX 1.0
#define DEFAULT_BEDSLOPEOFFSET 0.0
#define DEFAULT_CULLONSTART false

#ifndef min
#define min(x, y) ((x<y) ? x:y)
#endif

#ifndef max
#define max(x, y) ((x>y) ? x:y)
#endif

#define getRange(aMin, aMax, x) { aMin = min(aMin, x);	aMax = max(aMax, x);	} 

#if 0
#include <windows.h>
#define PROFILE_BEGIN				\
	LARGE_INTEGER ticksPerSecond;	\
	LARGE_INTEGER tick_start;		\
	LARGE_INTEGER tick_end;			\
	QueryPerformanceFrequency(&ticksPerSecond);	\
	QueryPerformanceCounter(&tick_start);

#define PROFILE_END		\
	QueryPerformanceCounter(&tick_end);	\
	tick_end.QuadPart -= tick_start.QuadPart;	\
	tick_end.QuadPart /= ticksPerSecond.QuadPart/1000;	\
	printf("PROFILE(prepareTimestepImpl) %dms\n", tick_end.LowPart);
#else
#define PROFILE_BEGIN
#define PROFILE_END
#endif


#ifdef USE_FAST_SQRT
inline float Math_InvSqrtFast(float x)
{
	 float xhalf = 0.5f*x;
	 int i = *(int*)&x;
	 i = 0x5f3759df - (i>>1); // "magic number" that allows us to shift the exponent.
	 x = *(float*)&i;
	 return x*(1.5f - xhalf*x*x); // Newton method for closer approximation
}

inline void Math_NormalizeFast(osg::Vec3 & aVec)
{
	aVec *= Math_InvSqrtFast(aVec*aVec);
}
#endif


// only constructor, requires netcdf file
SWWReader::SWWReader(const std::string& filename) :
	_valid(false),
	_px(NULL),
	_py(NULL),
	_pz(NULL),
	_ptime(NULL),
	_xoffset(0),
	_yoffset(0),
	_zoffset(0),
	_elevationAnimated(false)
{
PROFILE_BEGIN

	_fileChanged.watch(filename);

	// state initialization
	_state.bedslopetexturefilename = NULL;

	// netcdf filename
	_state.swwfilename = new std::string(filename);

	if (load())
	{
		_valid = true;
	}

	PROFILE_END
}



SWWReader::~SWWReader()
{
	_status.push_back( nc_close(_ncid) );
	clear();
}



void SWWReader::setBedslopeTexture( std::string filename )
{
	osg::notify(osg::INFO) << "[SWWReader::setBedslopetexture] filename: " << filename <<  std::endl;
	_state.bedslopetexturefilename = new std::string(filename);
	_bedslopegeodata.hasData = false;

	// GDAL Geospatial Image Library
	GDALDataset *pGDAL;
	GDALAllRegister();
	pGDAL = (GDALDataset *) GDALOpen( filename.c_str(), GA_ReadOnly );
	if( pGDAL == NULL )
		osg::notify(osg::INFO) << "[SWWReader::setBedslopetexture] GDAL unable to open file: " << filename <<  std::endl;
	else
	{
		osg::notify(osg::INFO) << "[SWWReader::setBedslopetexture] GDAL Driver: "
				  << pGDAL->GetDriver()->GetDescription() << "/" 
				  << pGDAL->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) 
				  << std::endl;

		_bedslopegeodata.xresolution = pGDAL->GetRasterXSize();
		_bedslopegeodata.yresolution = pGDAL->GetRasterYSize();
		osg::notify(osg::INFO) << "[SWWReader::setBedslopetexture] xresolution: " << _bedslopegeodata.xresolution <<  std::endl;
		osg::notify(osg::INFO) << "[SWWReader::setBedslopetexture] yresolution: " << _bedslopegeodata.yresolution <<  std::endl;

		// check if image contains geodata
		double geodata[6];
		if( pGDAL->GetGeoTransform( geodata ) == CE_None )
		{
			_bedslopegeodata.xorigin = geodata[0];
			_bedslopegeodata.yorigin = geodata[3];
			_bedslopegeodata.rotation = geodata[2];
			_bedslopegeodata.xpixel = geodata[1];
			_bedslopegeodata.ypixel = geodata[5];
			_bedslopegeodata.hasData = true;

			osg::notify(osg::INFO) << "[SWWReader::setBedslopetexture] xorigin: " << _bedslopegeodata.xorigin <<  std::endl;
			osg::notify(osg::INFO) << "[SWWReader::setBedslopetexture] yorigin: " << _bedslopegeodata.yorigin <<  std::endl;
			osg::notify(osg::INFO) << "[SWWReader::setBedslopetexture] xpixel: " << _bedslopegeodata.xpixel <<  std::endl;
			osg::notify(osg::INFO) << "[SWWReader::setBedslopetexture] ypixel: " << _bedslopegeodata.ypixel <<  std::endl;
			osg::notify(osg::INFO) << "[SWWReader::setBedslopetexture] rotation: " << _bedslopegeodata.rotation <<  std::endl;
		}
	}
}



osg::Image* SWWReader::getBedslopeTexture()
{
	return osgDB::readImageFile( _state.bedslopetexturefilename->c_str() );
}



osg::ref_ptr<osg::Vec2Array> SWWReader::getBedslopeTextureCoords()
{
	_bedslopetexcoords = new osg::Vec2Array;
	if( _bedslopegeodata.hasData )  // georeferenced bedslope texture
	{
		float xorigin = _bedslopegeodata.xorigin;
		float yorigin = _bedslopegeodata.yorigin;
		float xrange = _bedslopegeodata.xpixel * _bedslopegeodata.xresolution;
		float yrange = _bedslopegeodata.ypixel * _bedslopegeodata.yresolution;
		for( unsigned int iv=0; iv < _npoints; iv++ )
			_bedslopetexcoords->push_back( osg::Vec2( (_px[iv] + _xllcorner - xorigin)/xrange, 
																	1.0 - (_py[iv] + _yllcorner - yorigin)/yrange ) );
	}
	else
	{
		// decaled using (x,y) locations scaled by extents into range [0,1]
		for( unsigned int iv=0; iv < _npoints; iv++ )
			_bedslopetexcoords->push_back( osg::Vec2( (_px[iv]-_xoffset)*_xscale, (_py[iv]-_yoffset)*_yscale ) );
	}

	return _bedslopetexcoords;
}


bool SWWReader::loadBedslopeVertexArray(unsigned int aIndex)
{
	// netcdf open
	_status.push_back( nc_open(_state.swwfilename->c_str(), NC_NOWRITE, &_ncid) );
	if (this->_statusHasError())
	{
		return false;
	}

	float * pz = loadBedslopeZ(aIndex);
	assert(pz);

	getBedslopeBoundingVolume(pz);

	// bedslope vertex array, shifting and scaling vertices to unit cube
	// centred about the origin
	_bedslopevertices = new osg::Vec3Array;
	_bedslopevertices->reserve(_npoints);

	for (unsigned int iv=0; iv < _npoints; iv++)
	{
		_bedslopevertices->push_back( osg::Vec3( (_px[iv]-_xoffset)*_scale - _xcenter, 
															  (_py[iv]-_yoffset)*_scale - _ycenter, 
															  (pz[iv]-_zoffset)*_scale - _zcenter - DEFAULT_BEDSLOPEOFFSET) );
	}

	// calculate bedslope primitive normal and centroid arrays
	_bedslopenormals = new osg::Vec3Array;
	_bedslopecentroids = new osg::Vec3Array;

	_bedslopenormals->reserve(_nvolumes);
	_bedslopecentroids->reserve(_nvolumes);

	osg::Vec3 v1, v2, v3, side1, side2, nrm;
	for (unsigned int iv=0; iv < _nvolumes; iv++)
	{
		unsigned int v1index = _pvolumes[3*iv+0];
		unsigned int v2index = _pvolumes[3*iv+1];
		unsigned int v3index = _pvolumes[3*iv+2];

		if ((v1index >= _npoints) || (v2index >= _npoints) || (v3index >= _npoints))
		{
			// data out of bounds
			_status.push_back( nc_close(_ncid) );
			return false;
		}

		v1 = _bedslopevertices->at(v1index);
		v2 = _bedslopevertices->at(v2index);
		v3 = _bedslopevertices->at(v3index);

		side1 = v2 - v1;
		side2 = v3 - v2;
		nrm = side1^side2;
		nrm.normalize();

		_bedslopenormals->push_back( nrm );
		_bedslopecentroids->push_back( (v1+v2+v3)/3.0 );
	}

	_status.push_back( nc_close(_ncid) );

	if (_statusHasError())
	{
		return false;
	}


	return true;
}


bool SWWReader::loadStageVertexArray(unsigned int index)
{
	PROFILE_BEGIN

	assert(_bedslopevertices);

	size_t start[2], count[2], iv;
	const ptrdiff_t stride[2] = {1,1};
	start[0] = index;
	start[1] = 0;
	count[0] = 1;
	count[1] = _npoints;

	// netcdf open
	_status.push_back( nc_open(_state.swwfilename->c_str(), NC_NOWRITE, &_ncid) );
	if (this->_statusHasError())
	{
		return false;
	}

	// --- Check that the stage data hasn't shrunk
	size_t npoints = 0;
	_status.push_back( nc_inq_dimlen(_ncid, _npointsid, &npoints) );
	if (_statusHasError() || (npoints != _npoints))
	{
		// Our indices will not be out of bounds
		osg::notify(osg::FATAL) << "File changes have made it invalid! Please wait." <<  std::endl;
		return false;
	}

		// empty array for storing list of steep triangles
		osg::ref_ptr<osg::IntArray> steeptri = new osg::IntArray;

	// stage heights from netcdf file (x and y are same as bedslope)
	_status.push_back(nc_get_vars_float (_ncid, _stageid, start, count, stride, _pstage));

	if (_pxmomentum && _pymomentum)
	{
		// stage momentum from netcdf file (x and y are same as bedslope)
		_status.push_back(nc_get_vars_float (_ncid, _xmomentumid, start, count, stride, _pxmomentum));
		_status.push_back(nc_get_vars_float (_ncid, _ymomentumid, start, count, stride, _pymomentum));
	}

	_status.push_back( nc_close(_ncid) );

	// load stage vertex array, scaling and shifting vertices to lie in the unit cube
	_stagevertices = new osg::Vec3Array;
	_stagevertices->reserve(_npoints);

	for (iv=0; iv < _npoints; iv++)
	{
		_stagevertices->push_back( osg::Vec3( (_px[iv]-_xoffset)*_scale - _xcenter, 
														  (_py[iv]-_yoffset)*_scale - _ycenter, 
														  (_pstage[iv]-_zoffset)*_scale - _zcenter) );
	}

	// stage index, per primitive normal and centroid arrays
	_stageprimitivenormals = new osg::Vec3Array;
	_stageprimitivenormals->reserve(_nvolumes);
	osg::Vec3 v1b, v2b, v3b;
	osg::Vec3 v1s, v2s, v3s;
	osg::Vec3 side1, side2, nrm;
	unsigned int v1index, v2index, v3index;

	// cullangle given in degrees, test is against dot product
	float cullthreshold = cos(osg::DegreesToRadians(_state.cullangle));

	// over all stage triangles
	for (iv=0; iv < _nvolumes; iv++)
	{
		v1index = _pvolumes[3*iv+0];
		v2index = _pvolumes[3*iv+1];
		v3index = _pvolumes[3*iv+2];

		v1s = _stagevertices->at(v1index);
		v2s = _stagevertices->at(v2index);
		v3s = _stagevertices->at(v3index);

		// current triangle primitive normal
		side1 = v2s - v1s;
		side2 = v3s - v2s;
		nrm = side1^side2;
#ifdef USE_FAST_SQRT
		Math_NormalizeFast(nrm);
#else
	nrm.normalize();
#endif

		// store primitive normal
		_stageprimitivenormals->push_back( nrm );

		// identify steep triangles, store index
		if( fabs(nrm * osg::Vec3f(0,0,1)) < cullthreshold )
			steeptri->push_back( iv );
	}

	// stage height above bedslope mapped as alpha value
	//		alpha = min( a(h-hmin) + alphamin, alphamax),  h >= hmin
	//		alpha = 0,												 h < hmin
	// where a = (alphamax-alphamin)/(hmax-hmin)
	float alpha, height, alphascale;
	alphascale = (_state.alphamax - _state.alphamin) / (_state.heightmax - _state.heightmin);
	_stagecolors = new osg::Vec4Array;
	_stagecolors->reserve(_npoints);
	for (iv=0; iv < _npoints; iv++)
	{
		// water height above corresponding bedslope
		height = _stagevertices->at(iv).z() - _bedslopevertices->at(iv).z();
	
		if (height < _state.heightmin)
		{		
			alpha = 0.0;
		}
		else 
		{
			alpha = alphascale * (height - _state.heightmin) + _state.alphamin;
			if( alpha > _state.alphamax ) 
				alpha = _state.alphamax;
		}

	  if (_pxmomentum && _pymomentum)
	  {
		float intens = min(1, sqrt(_pxmomentum[iv]*_pxmomentum[iv]+_pymomentum[iv]*_pymomentum[iv])/2);
		_stagecolors->push_back( osg::Vec4( 1.0f-intens, (0.5f-fabs(intens - 0.5f))*2, intens, alpha ) );
	  }
	  else
	  {
		_stagecolors->push_back( osg::Vec4( 1, 1, 1, alpha ) );
	  }
	}

	// steep triangle vertices should have alpha=0, overwrite such vertex colours
	if( _state.culling )
	{
		//std::cout << "culling on step: " << index << "  steeptris: " << steeptri->size() << std::endl;
		for (iv=0; iv < steeptri->size() ; iv++)
		{
			int tri = steeptri->at(iv);
			v1index = _pvolumes[3*tri+0];
			v2index = _pvolumes[3*tri+1];
			v3index = _pvolumes[3*tri+2];

			_stagecolors->at(v1index) = osg::Vec4( 1, 1, 1, 0 );
			_stagecolors->at(v2index) = osg::Vec4( 1, 1, 1, 0 );
			_stagecolors->at(v3index) = osg::Vec4( 1, 1, 1, 0 );
		}
	}

	// per-vertex normals calculated as average of primitive normals
	// from contributing triangles
	_stagevertexnormals = new osg::Vec3Array;
	_stagevertexnormals->reserve(_npoints);

	int num_shared_triangles, triangle_index;
	for (iv=0; iv < _npoints; iv++)
	{
		nrm.set(0,0,0);
		num_shared_triangles = _connectivity.at(iv).size();	// There may be 2-7 triangles sharing a vertex

		for (int i=0; i<num_shared_triangles; i++ )
		{
			triangle_index = _connectivity.at(iv).at(i);
			nrm += _stageprimitivenormals->at(triangle_index);
		}

		nrm = nrm / num_shared_triangles;  // average

#ifdef USE_FAST_SQRT
		Math_NormalizeFast(nrm);
#else
	nrm.normalize();
#endif

		_stagevertexnormals->push_back(nrm);
	}

	PROFILE_END

	return true;
}


bool SWWReader::getTimeSeries(unsigned int aPolyIndex, TimeSeriesType aPlotType, osg::ref_ptr<osg::FloatArray> aData)
{
	PROFILE_BEGIN

	// we could get an average of 3 plots here, but it's probably overkill
	int stage_index = _pvolumes[aPolyIndex*3];


	size_t start[2], count[2];
	const ptrdiff_t stride[2] = {1,1};
	start[0] = 0;
	start[1] = stage_index;
	count[0] = _ntimesteps;
	count[1] = 1;

	// netcdf open
	_status.push_back( nc_open(_state.swwfilename->c_str(), NC_NOWRITE, &_ncid) );
		if (this->_statusHasError()) return false;

	aData->resize(count[0]);

	switch (aPlotType)
	{
		case TSTYPE_MOMENTUM_MAGNITUDE:
		{
			if (!_pxmomentum || !_pymomentum)
			{
				break;
			}

			osg::ref_ptr<osg::FloatArray> xmom = new osg::FloatArray;
			osg::ref_ptr<osg::FloatArray> ymom = new osg::FloatArray;

			xmom->resize(count[0]);
			ymom->resize(count[0]);

			// momentum from netcdf file (x and y are same as bedslope)
			_status.push_back(nc_get_vars_float (_ncid, _xmomentumid, start, count, stride, (float*)xmom->getDataPointer()));
			_status.push_back(nc_get_vars_float (_ncid, _ymomentumid, start, count, stride, (float*)ymom->getDataPointer()));

			for (int i=0; i<(int)aData->size(); i++)
			{
				aData->at(i) = sqrt(xmom->at(i)*xmom->at(i)+ymom->at(i)*ymom->at(i));
			}
			

			break;
		}

		case TSTYPE_STAGE:
		default:
		{
			// stage heights from netcdf file (x and y are same as bedslope)
			_status.push_back(nc_get_vars_float (_ncid, _stageid, start, count, stride, (float*)aData->getDataPointer()));
			break;
		}
	}

	_status.push_back( nc_close(_ncid) );

	if (_statusHasError())
	{
		return false;
	}

	PROFILE_END

	return true;
}



bool SWWReader::_statusHasError()
{
	bool haserror = false;  // assume success, trap failure

	std::vector<int>::iterator iter;
	for (iter=_status.begin(); iter != _status.end(); iter++)
	{
		if (*iter != NC_NOERR)
		{
			osg::notify(osg::FATAL) << "Error: " << nc_strerror(*iter) <<  std::endl;
			haserror = true;
			nc_close(_ncid);
			break;
		}
	}

	// on return start gathering result values afresh
	_status.clear();

	return haserror;
}


bool SWWReader::refresh()
{
	if (_fileChanged.isChanged())
	{
		clear();
		return load();
	}

	return true;
}


void SWWReader::clear()
{
	_valid = false;

	SAFE_DELETE_ARRAY(_pxmomentum);
	SAFE_DELETE_ARRAY(_pymomentum);
	SAFE_DELETE_ARRAY(_px);
	SAFE_DELETE_ARRAY(_py);
	SAFE_DELETE_ARRAY(_pz);
	SAFE_DELETE_ARRAY(_ptime);
	SAFE_DELETE_ARRAY(_pvolumes);
	SAFE_DELETE_ARRAY(_pstage);
}


bool SWWReader::load()
{
	if (!_state.swwfilename)
	{
		return false;
	}

	// netcdf open
	_status.push_back( nc_open(_state.swwfilename->c_str(), NC_NOWRITE, &_ncid) );
	if (this->_statusHasError())
	{
		return false;
	}

	// dimension ids
	_status.push_back( nc_inq_dimid(_ncid, "number_of_volumes", &_nvolumesid) );
	_status.push_back( nc_inq_dimid(_ncid, "number_of_vertices", &_nverticesid) );
	_status.push_back( nc_inq_dimid(_ncid, "number_of_points", &_npointsid) );
	_status.push_back( nc_inq_dimid(_ncid, "number_of_timesteps", &_ntimestepsid) );
	if (this->_statusHasError()) return false;

	// dimension values
	_status.push_back( nc_inq_dimlen(_ncid, _nvolumesid, &_nvolumes) );
	_status.push_back( nc_inq_dimlen(_ncid, _nverticesid, &_nvertices) );
	_status.push_back( nc_inq_dimlen(_ncid, _npointsid, &_npoints) );
	_status.push_back( nc_inq_dimlen(_ncid, _ntimestepsid, &_ntimesteps) );
	if (this->_statusHasError()) return false;

	// variable ids
	_status.push_back( nc_inq_varid (_ncid, "x", &_xid) );
	_status.push_back( nc_inq_varid (_ncid, "y", &_yid) );

	if (nc_inq_varid (_ncid, "elevation", &_zid) != NC_NOERR)
	{
		// If we failed to find the elevation var, this is likely to be an old-style file.
		osg::notify(osg::INFO) << "[SWWReader] Warning: variable 'elevation' not found. Trying old format 'z' instead." << std::endl;
		_status.push_back( nc_inq_varid (_ncid, "z", &_zid) );
	}
	else
	{
		// now we check for multiple bedslope frames
		int num_dims;
		_status.push_back(nc_inq_varndims(_ncid, _zid, &num_dims));
		if (num_dims == 2)
		{
			osg::notify(osg::INFO) << "[SWWReader] Animated elevation data found. " << std::endl;
			_elevationAnimated = true;
		}
		else
		{
			osg::notify(osg::INFO) << "[SWWReader] Static elevation data only. " <<  std::endl;
			assert(num_dims == 1 && "Wrong num of dimensions for elevation data - unknown file format!");
		}

		_status.push_back( NC_NOERR );
	}

	_status.push_back( nc_inq_varid (_ncid, "volumes", &_volumesid) );
	_status.push_back( nc_inq_varid (_ncid, "time", &_timeid) );
	_status.push_back( nc_inq_varid (_ncid, "stage", &_stageid) );
	if (this->_statusHasError())
	{
		return false;
	}

	// --- Look for momentum data
	if ((nc_inq_varid (_ncid, "xmomentum", &_xmomentumid) != NC_NOERR) ||
		(nc_inq_varid (_ncid, "ymomentum", &_ymomentumid) != NC_NOERR)) 
	{
		osg::notify(osg::INFO) << "[SWWReader] No momentum data found." << std::endl;
		_pxmomentum = NULL;
		_pymomentum = NULL;
	}
	else
	{
		_pxmomentum = new float[_npoints];
		_pymomentum = new float[_npoints];
	}


	// allocation of variable arrays, destructor responsible for cleanup
	_px = new float[_npoints];
	_py = new float[_npoints];
	_pz = new float[_npoints];	// bedslope z

	_ptime = new float[_ntimesteps];
	_pvolumes = new unsigned int[_nvertices * _nvolumes];
	_pstage = new float[_npoints];

	// loading variables from netcdf file
	_status.push_back( nc_get_var_float (_ncid, _xid, _px) );  // x vertices
	_status.push_back( nc_get_var_float (_ncid, _yid, _py) );  // y vertices
	
	_status.push_back( nc_get_var_float (_ncid, _timeid, _ptime) );  // time array
	_status.push_back( nc_get_var_int (_ncid, _volumesid, (int *) _pvolumes) );  // triangle indices

	if (this->_statusHasError()) return false;


	osg::notify(osg::INFO) << "[SWWReader] number of volumes: " << _nvolumes <<  std::endl;
	osg::notify(osg::INFO) << "[SWWReader] number of vertices: " << _nvertices <<  std::endl;
	osg::notify(osg::INFO) << "[SWWReader] number of points: " << _npoints <<  std::endl;
	osg::notify(osg::INFO) << "[SWWReader] number of timesteps: " << _ntimesteps <<  std::endl;

	// --- close file, we have finished with it
	_status.push_back( nc_close(_ncid) );

	// sww file can optionally contain bedslope texture image filename
	size_t attlen; // length of text attribute (if it exists)
	if( nc_inq_attlen(_ncid, NC_GLOBAL, "texture", &attlen) != NC_ENOTATT )
	{
		std::string texfilename;
		int status;
		status = nc_get_att_text(_ncid, NC_GLOBAL, "texture", (char*)texfilename.c_str());
		if( status == NC_NOERR )
		{
			texfilename[attlen] = '\0';  // ensure string is terminated, not a requirement for netcdf attributes
			osg::notify(osg::INFO) << "[SWWReader] embedded image filename: " << texfilename <<  std::endl;

			// if sww isn't in current directory, need to prepend sww path to the bedslope texture
			if( osgDB::getFilePath(*_state.swwfilename) == "" )
			{
				setBedslopeTexture(texfilename);
			}
			else
			{
				setBedslopeTexture( osgDB::getFilePath(*_state.swwfilename) + std::string("/") + texfilename);
			}
		}
	}

	// sww file can optionally contain georeference offset
	int status1 = nc_get_att_float(_ncid, NC_GLOBAL, "xllcorner", &_xllcorner);
	int status2 = nc_get_att_float(_ncid, NC_GLOBAL, "yllcorner", &_yllcorner);
	if( status1 == NC_NOERR && status2 == NC_NOERR)
	{
		osg::notify(osg::INFO) << "[SWWReader] xllcorner: " << _xllcorner <<  std::endl;
		osg::notify(osg::INFO) << "[SWWReader] yllcorner: " << _yllcorner <<  std::endl;
	}
	else
	{
		_xllcorner = 0.0;  // default value
		_yllcorner = 0.0;
	}


	// alpha-scaling defaults, can be overridden after construction by command line parameters
	_state.alphamin = DEFAULT_ALPHAMIN;
	_state.alphamax = DEFAULT_ALPHAMAX;
	_state.heightmin = DEFAULT_HEIGHTMIN;
	_state.heightmax = DEFAULT_HEIGHTMAX;

	// steepness culling default, can be overridden after construction by command line parameter
	_state.cullangle = DEFAULT_CULLANGLE;
	_state.culling = DEFAULT_CULLONSTART;

	// loop index
	size_t iv;
	// vertex indices
	unsigned int v1index, v2index, v3index;

	// compute triangle connectivity, a list (indexed by vertex number) 
	// of lists (indices of triangles sharing this vertex)
	_connectivity = std::vector<triangle_list>(_npoints);
	for (iv=0; iv < _nvolumes; iv++)
	{
		v1index = _pvolumes[3*iv+0];
		v2index = _pvolumes[3*iv+1];
		v3index = _pvolumes[3*iv+2];

		if (v1index<_npoints)
		{
			_connectivity.at(v1index).push_back(iv);
		}

		if (v2index<_npoints)
		{
			_connectivity.at(v2index).push_back(iv);
		}

		if (v3index<_npoints)
		{
			_connectivity.at(v3index).push_back(iv);
		}
	}


	// bedslope index array, pvolumes array indexes into x, y and z
	_bedslopeindices = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, _nvolumes*_nvertices);
	for (iv=0; iv < _nvolumes*_nvertices; iv++)
		(*_bedslopeindices)[iv] = _pvolumes[iv];


	if (_statusHasError())
	{
		return false;
	}

	// Load initial frame
	loadBedslopeVertexArray(0);

	return true;
}


float SWWReader::getTime(unsigned int index)
{
	if ((!_ptime) || (index >= getNumberOfTimesteps()))
	{
		// invalid data
		return 0.0f;
	}

	return *(_ptime+index);
}


void SWWReader::getBedslopeBoundingVolume(const float * aZ)
{
	// bedslope range and resultant scale factors (note, these don't take into
	// account any xllcorner, yllcorner offsets - used during texture coord assignment)
	float xmin, xmax, xrange;
	float ymin, ymax, yrange;
	float zmin, zmax, zrange;
	float aspect_ratio;

	assert(aZ);

	xmin = xmax = _px[0];
	ymin = ymax = _py[0];
	zmin = zmax =  aZ[0];
	for(unsigned int iv=1; iv < _npoints; iv++ )
	{
		getRange(xmin, xmax, _px[iv]);
		getRange(ymin, ymax, _py[iv]);
		getRange(zmin, zmax, aZ[iv]);
	}

	xrange = xmax - xmin;
	yrange = ymax - ymin;
	zrange = zmax - zmin;
	aspect_ratio = xrange/yrange;
	_xscale = 1.0 / xrange;
	_yscale = 1.0 / yrange;
	 
	// handle case of a flat bed that doesn't necessarily pass through z=0
	_zscale = (zrange == 0.0) ? 1.0 : 1.0/zrange;

	_xoffset = xmin;
	_yoffset = ymin;
	_zoffset = zmin;
	_xcenter = 0.5;
	_ycenter = 0.5;
	_zcenter = 0.0;

	if( aspect_ratio > 1.0 )
	{
		_scale = 1.0/xrange;
		_ycenter /= aspect_ratio;
	}
	else
	{
		_scale = 1.0/yrange;
		_xcenter *= aspect_ratio;
	}

	osg::notify(osg::INFO) << "[SWWReader] xmin: " << xmin <<  std::endl;
	osg::notify(osg::INFO) << "[SWWReader] xmax: " << xmax <<  std::endl;
	osg::notify(osg::INFO) << "[SWWReader] xrange: " << xrange <<  std::endl;
	osg::notify(osg::INFO) << "[SWWReader] xscale: " << _xscale <<  std::endl;
	osg::notify(osg::INFO) << "[SWWReader] xcenter: " << _xcenter <<  std::endl;
	osg::notify(osg::INFO) <<  std::endl;

	osg::notify(osg::INFO) << "[SWWReader] ymin: " << ymin <<  std::endl;
	osg::notify(osg::INFO) << "[SWWReader] ymax: " << ymax <<  std::endl;
	osg::notify(osg::INFO) << "[SWWReader] yrange: " << yrange <<  std::endl;
	osg::notify(osg::INFO) << "[SWWReader] yscale: " << _yscale <<  std::endl;
	osg::notify(osg::INFO) << "[SWWReader] ycenter: " << _ycenter <<  std::endl;
	osg::notify(osg::INFO) <<  std::endl;

	osg::notify(osg::INFO) << "[SWWReader] zmin: " << zmin <<  std::endl;
	osg::notify(osg::INFO) << "[SWWReader] zmax: " << zmax <<  std::endl;
	osg::notify(osg::INFO) << "[SWWReader] zrange: " << zrange <<  std::endl;
	osg::notify(osg::INFO) << "[SWWReader] zscale: " << _zscale <<  std::endl;
	osg::notify(osg::INFO) << "[SWWReader] zcenter: " << _zcenter <<  std::endl;
	osg::notify(osg::INFO) <<  std::endl;
}


float * SWWReader::loadBedslopeZ(unsigned int aTimestep)
{
	assert(_pz);

	// --- Check that the stage data hasn't shrunk
	size_t npoints = 0;
	_status.push_back( nc_inq_dimlen(_ncid, _npointsid, &npoints) );
	if (_statusHasError() || (npoints != _npoints))
	{
		// Our indices will not be out of bounds
		osg::notify(osg::FATAL) << "File changes have made it invalid! Please wait." <<  std::endl;
		return NULL;
	}

	if (!_elevationAnimated)
	{
		// Static bedslope, never changes
		_status.push_back( nc_get_var_float (_ncid, _zid, _pz) );
		return _pz;
	}

	size_t start[2], count[2];
	const ptrdiff_t stride[2] = {1,1};
	start[0] = aTimestep;
	start[1] = 0;
	count[0] = 1;
	count[1] = _npoints;

	// bedslope elevation from netcdf file
	_status.push_back(nc_get_vars_float (_ncid, _zid, start, count, stride, _pz));

	if (_statusHasError())
	{
		// Our indices will not be out of bounds
		osg::notify(osg::FATAL) << "Could not load variable bed elevation - check file format." <<  std::endl;
		return NULL;
	}

	return _pz;
}
