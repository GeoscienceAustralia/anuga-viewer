
/*
    Project-wide includes and adjustable defaults

    An OpenSceneGraph viewer for ANUGA .sww files.
    Copyright (C) 2004, 2009 Geoscience Australia
*/


#ifndef PROJECT_H
#define PROJECT_H


// copied from OSG, must be Visual Studio idiosyncrocies ...
#if defined(WIN32) && !(defined(__CYGWIN__) || defined(__MINGW32__))
    #pragma warning( disable : 4244 )
    #pragma warning( disable : 4251 )
    #pragma warning( disable : 4267 )
    #pragma warning( disable : 4275 )
    #pragma warning( disable : 4290 )
    #pragma warning( disable : 4786 )
    #pragma warning( disable : 4305 )
#endif


// Compile-time defaults
#define DEF_PAUSED_START        true
#define DEF_BACKGROUND_COLOUR   0.5, 0.5, 0.5, 1.0    // R, G, B, Alpha (grey)
#define DEF_TPS                 10.0                  // sww timesteps per second

	/**
	 * Several wireframe modes, a bitfield detailing which parts of the scene geometry
	 * should be rendered in wireframe.
	 */
	enum WireframeMode
	{
		WF_NONE = 0,					/**< Normal rendering. */
		WF_WATER = 1,					/**< Water only. */
		WF_BED = 2,						/**< Bed surface only. */
		WF_BOTH = WF_WATER | WF_BED,	/**< All geometry. */

		WF_NUM_OF						/**< Number of above options. */
	};

	/**
	 * Which form of grid to display
	 */
	enum GridMode
	{
		GM_NONE,	/**< No grid */
		GM_GRID,	/**< Grid only */
		GM_COLORBAR,	/**< Colorbar only */
		GM_BOTH = GM_GRID | GM_COLORBAR,	/**<Colorbar and grid */

		GM_NUM_OF /**< Number of above options. */
	};


#ifndef NDEBUG
#include <iostream>
#include <sstream>

struct ThrowAssert  : public std::exception
{
	virtual const char* what() const throw () { return mMsg.c_str();}

	std::string mMsg;
	ThrowAssert(const std::string& msg, const char* file, int line ) 
	{
		std::stringstream ss;
		ss << "Assert (" << msg << ") in file " << file << " at line " << line; 
		mMsg = ss.str();

		std::cerr << mMsg;
	}

	ThrowAssert() {}
	virtual ~ThrowAssert() throw () {}
};


#ifndef assert
#define assert(a)  if (!(a)) throw ThrowAssert(std::string(#a),__FILE__,__LINE__);
#endif
#else
#define assert(a) {}
#endif


#endif // PROJECT_H

