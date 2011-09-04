
#include <keyboardeventhandler.h>
#include <iostream>

#include <osg/MatrixTransform>

#include "anugahud.h"

extern AnugaHUD * g_hud;

KeyboardEventHandler::KeyboardEventHandler( int nTimesteps, float tps)	:
	_writeframe(false),
	_return_origin(false),
	_wireframeMode(WF_NONE),
	_gridMode(GM_NONE),
	_picked_poly(-1),
	_mouseclicked(false),
	_shift_held(false)
{
   _paused = DEF_PAUSED_START;
   _direction = 1;
   _ntimesteps = nTimesteps;
   _tps = tps;
   _tpsorig = tps;
   _timestep = 0;
   _prevtime = 0;
   _togglewireframe = false;
   _toggleculling = false;
   _togglerecording = false;
   _toggleplayback = false;
   _togglesave = false;
}


void KeyboardEventHandler::getAppUsage(osg::ApplicationUsage& usage)
{
	// --- bindings that this class implements
    usage.addKeyboardMouseBinding("Space","Toggle surface data time stepping");
	usage.addKeyboardMouseBinding("x","Reset view to default position");
	usage.addKeyboardMouseBinding("r","Reset animation to timestep 0");
    usage.addKeyboardMouseBinding("c","Toggle steep surface triangle culling");
    usage.addKeyboardMouseBinding("g","Toggle grid");
    usage.addKeyboardMouseBinding("i","Toggle information HUD");
	usage.addKeyboardMouseBinding("w","Cycle wireframe modes");
    usage.addKeyboardMouseBinding("1","Toggle recording.");
    usage.addKeyboardMouseBinding("2","Toggle playback of recorded information");
    usage.addKeyboardMouseBinding("3","Save recorded macro to \"movie.swm\"");
    usage.addKeyboardMouseBinding("Escape","Quit the application");
}


bool KeyboardEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>( &aa );

	if (!viewer)
	{
		return false;
	}

	switch( ea.getEventType() )
	{
		case osgGA::GUIEventAdapter::PUSH:
		case osgGA::GUIEventAdapter::MOVE:
		{
			// Record mouse location for the button press
			// and move events.
			_mX = ea.getX();
			_mY = ea.getY();
			return false;
		}

		case osgGA::GUIEventAdapter::RELEASE:
		{
			// If the mouse hasn't moved since the last button press or move event, and the shift key is held,
			// perform a pick. (Otherwise, the trackball manipulator will handle it.)
			if (_mX == ea.getX() && _mY == ea.getY())
			{
				if (!_shift_held)
				{
					// always clear plot if shift not held
					_picked_poly = -1;
					_mouseclicked = true;
					return true;
				}

				_mouseclicked = true;
				_picked_poly = pick( ea.getXnormalized(), ea.getYnormalized(), viewer );
				if (_picked_poly != -1)
				{
					return true;
				}
			}
			return false;
		}

		case osgGA::GUIEventAdapter::KEYUP:
		{
			int key = ea.getKey();
			if (key == osgGA::GUIEventAdapter::KEY_Shift_L || key == osgGA::GUIEventAdapter::KEY_Shift_R)
			{
				_shift_held = false;
				return true;
			}
			return false;
		}

		case osgGA::GUIEventAdapter::KEYDOWN:
		{
			switch( ea.getKey() )
			{
				case osgGA::GUIEventAdapter::KEY_Space:
					_paused = _paused ? false : true;
					return true;

				case osgGA::GUIEventAdapter::KEY_Up:
					if( !_paused ) _tps *= 1.5;
					return true;

				case osgGA::GUIEventAdapter::KEY_Down:
					if( !_paused ) _tps /= 1.5;
					return true;

				case osgGA::GUIEventAdapter::KEY_Shift_L:
				case osgGA::GUIEventAdapter::KEY_Shift_R:
					_shift_held = true;
					return true;

				case 'r':
					_paused = DEF_PAUSED_START;
					_tps = _tpsorig;
					_timestep = 0;
					return true;

				case 'x':
					_return_origin = true;
					return true;

				case 'g':
				{
					int gm = int(_gridMode);
					gm++;
					gm %= int(GM_NUM_OF);
					_gridMode = (GridMode)gm;
					return true;
				}

				case osgGA::GUIEventAdapter::KEY_Right:
					if( _paused )
					{
					   _timestep = (_timestep+1) % _ntimesteps;
					}
					else
					   _direction = +1;
					return true;

				case osgGA::GUIEventAdapter::KEY_Left:
					if( _paused ) 
					{
					   _timestep = _timestep-1;
					   if( _timestep < 0 ) _timestep = _ntimesteps-1;
					}
					else
					   _direction = -1;
					return true;

				case 'w':
				{
					int wfm = int(_wireframeMode);
					wfm++;
					wfm %= int(WF_NUM_OF);
					_wireframeMode = (WireframeMode)wfm;

					g_hud->setWireframe(_wireframeMode);

					return true;
				}

				case 'c':
					_toggleculling = true;
					return true;

				case 'i':
					g_hud->setVisible(!g_hud->isVisible());
					return true;

				case '1':
					_togglerecording = true;
					return true;

				case '2':
					_toggleplayback = true;
					return true;

				case '3':
					_togglesave = true;
					return true;

				default:
					return false;
				}
			}
		default:
			return false;
	}
	return false;
}


void KeyboardEventHandler::setTime( float time)
{
   //std::cout << "setTime: " << time << std::endl;
   //std::cout << "setTime: " << _tps << std::endl;
   if( !isPaused()  &&  time - _prevtime > 1.0/_tps )
   {
	   // If we are in freerunning mode, this code will kick in and advance the timestep automatically
      _prevtime = time;
      _timestep = _timestep + _direction;
	  if( _timestep == _ntimesteps )
	  {
	    _timestep = 0;
	  }
      else if( _timestep < 0 ) 
	  {
	    _timestep = _ntimesteps-1;
	  }
   }
}


bool KeyboardEventHandler::toggleCulling()
{
   if( _toggleculling )
   {
      _toggleculling = false;
      return true;
   }
   return false;
}


bool KeyboardEventHandler::toggleRecording()
{
   if( _togglerecording )
   {
      _togglerecording = false;
      return true;
   }
   return false;
}


bool KeyboardEventHandler::togglePlayback()
{
   if( _toggleplayback )
   {
      _toggleplayback = false;
      return true;
   }
   return false;
}


bool KeyboardEventHandler::toggleSave()
{
   if( _togglesave )
   {
      _togglesave = false;
      return true;
   }
   return false;
}


// Perform a pick operation.
int KeyboardEventHandler::pick( const double x, const double y, osgViewer::Viewer* viewer )
{
	if (!viewer->getSceneData())
	{
		// Nothing to pick.
		return -1;
	}

	osgUtil::LineSegmentIntersector* picker = new osgUtil::LineSegmentIntersector(
						osgUtil::Intersector::PROJECTION, x, y);

	osgUtil::IntersectionVisitor iv( picker );
	viewer->getCamera()->accept( iv );

	if (picker->containsIntersections())
	{
		osgUtil::LineSegmentIntersector::Intersection intersection = picker->getFirstIntersection();

		if (intersection.nodePath.back()->getName() == "watersurface")
		{
			// only care if the water surface was clicked
			return intersection.primitiveIndex;
		}
	}

	return -1;
}