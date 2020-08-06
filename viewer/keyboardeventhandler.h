/*
    SWWViewer

    An OpenSceneGraph viewer for ANUGA .sww files.
    Copyright (C) 2004, 2009 Geoscience Australia
*/

#ifndef KEYBOARDEVENTHANDLER_H
#define KEYBOARDEVENTHANDLER_H

#include <project.h>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>

/**
 * Handles all keyboard events input by the user.
 */
class KeyboardEventHandler : public osgGA::GUIEventHandler 
{
public:

public:
    KeyboardEventHandler( int nTimesteps, float tps);
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&);
    /* virtual void accept(osgGA::GUIEventHandlerVisitor&) {} */
    virtual bool isPaused(){return _paused;}
    virtual void setPaused(bool value){_paused = value;}
	virtual WireframeMode getWireframeMode()	{	return _wireframeMode;	}
    virtual bool toggleCulling();
    virtual bool toggleRecording();
	virtual GridMode getGridMode()	{	return _gridMode;	}
    virtual bool togglePlayback();
    virtual bool toggleSave();
	virtual bool checkWriteFrame() { bool curr = _writeframe; _writeframe = false; return curr;	}
	virtual bool checkReturnOrigin() { bool curr = _return_origin; _return_origin = false; return curr;	}
	virtual bool checkMouseClicked() { bool curr = _mouseclicked; _mouseclicked = false; return curr;	}
	virtual int	 getSelectedPoly()	{ return _picked_poly;	}
    virtual int	 getTimestep(){return (unsigned int) _timestep;}

	/**
	 * Set time in seconds
	 */
    virtual void setTime(float time);

	/**
	 * Get usage information
	 */
	static void getAppUsage(osg::ApplicationUsage& usage);

private:
	/**
	 * pick out a piece of geometry from the scene
	 * @return index of water polygon that was clicked on
	 */
	int pick(const double x, const double y, osgViewer::Viewer* viewer);

private:
	float _mX;	/**< Old mouse pos */
	float _mY;	/**< Old mouse pos */
    	int _direction, _timestep, _ntimesteps;
    	float _tps;	/**< Timesteps per second. */
	float _prevtime, _tpsorig;
    bool _paused;
    bool _togglewireframe, _toggleculling;
    bool _togglerecording;
	bool _writeframe;	/**< Write the next frame out */
	bool _return_origin;
	WireframeMode _wireframeMode;	/**< Wireframe mode */
	GridMode _gridMode;
	int _picked_poly;	/**< Which polygon was picked by the mouse. */
	bool _mouseclicked;
	bool _shift_held;	/**< Is the shift key held down. */
	bool _toggleplayback;
	bool _togglesave;
};

#endif  // KEYBOARDEVENTHANDLER_H
