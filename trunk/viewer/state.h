
/*
  state.h, encapsulate current camera view and animation settings

  An OpenSceneGraph viewer for ANUGA .sww files.
  Copyright (C) 2004-2005, 2009 Geoscience Australia
*/

#ifndef STATE_H
#define STATE_H

#include <project.h>
#include <osg/Quat>
#include <osg/Vec3>
#include <osg/Matrix>
#include <istream>
#include <vector>
#include <fstream>
#include <iostream>

/**
 * Store all state information for the viewer.
 * Stores:
 * - HUD display state (bool)
 * - animation time (int)
 * - display parameters (ints)
 * - texturing (bool)
 */
class State
{

public:

    State();
    virtual void setTimestep(unsigned int value){ _timestep = value; }
    virtual void setCulling(bool value){ _culling = value; }
    virtual void setWireframe(int value){ _wireframe = value; }
    virtual void setGrid(GridMode value){ _grid = value; }
    virtual void setLighting(bool value){ _lighting = value; }
	virtual void setMatrix(osg::Matrix value);
	virtual void setShowHUD(bool aValue) { _show_hud = aValue;	}
	virtual void setShowTexture(bool aValue) { _show_texture = aValue;	}

    virtual unsigned int getTimestep(){ return _timestep; }
    virtual osg::Matrix getMatrix();
    virtual bool getCulling(){ return _culling; }
    virtual int getWireframe(){ return _wireframe; }
    virtual GridMode getGrid(){ return (GridMode)_grid; }
    virtual bool getLighting(){ return _lighting; }
	virtual bool getShowHUD() {	return _show_hud;	}
	virtual bool getShowTexture() {	return _show_texture;	}

    virtual void write(std::ostream &s);
    virtual bool read(std::istream& s);

    virtual ~State();


protected:

	unsigned int _timestep;
	osg::Vec3d _position;
	osg::Quat _orientation;
	int _wireframe;
	int _grid;
	bool _culling;
	bool _show_hud;
	bool _show_texture;
    bool _lighting;
};




class StateList : public std::vector<State>
{

public:

   StateList();
   bool write(std::ostream& s);
   bool read(std::string filename);

   //void ~StateList();

protected:
 
};


#endif  // STATE_H
