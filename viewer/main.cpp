/*
  SWWViewer

  An OpenSceneGraph viewer for ANUGA .sww files.
  Copyright (C) 2004-2005, 2009 Geoscience Australia
*/

#include <iostream>
#include <fstream>

#include <osg/Group>
#include <osg/Material>
#include <osg/MatrixTransform>
#include <osg/Notify>
#include <osg/PositionAttitudeTransform>
#include <osg/StateAttribute>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgViewer/ViewerEventHandlers> 

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>

#include <project.h>
#include <swwreader.h>
#include <bedslope.h>
#include <keyboardeventhandler.h>
#include <directionallight.h>
#include <state.h>
#include <watersurface.h>
#include <customargumentparser.h>

#include "skybox.h"
#include "anugahud.h"
#include "linegraph.h"
#include "customviewer.h"

// prototypes
extern const char* version();

unsigned int playback_index = 0;

static const char * S_VIEWER_TITLE = "ANUGA Viewer";

AnugaHUD * g_hud = NULL;


int main( int argc, char **argv )
{
   // use an ArgumentParser object to manage the program arguments.
   // this custom version detects if the last argument is a macro file
   // and modifies the argument list accordingly so the following code works ...
   CustomArgumentParser arguments( &argc, argv );

   // construct the viewer.
   CustomViewer viewer(arguments);

   osgViewer::ScreenCaptureHandler * cap_handler = new osgViewer::ScreenCaptureHandler;
   cap_handler->setKeyEventTakeScreenShot('O');
   viewer.addEventHandler(cap_handler); 

   // animation
   StateList statelist;
   bool playbackmode;
   bool recordingmode = false;
   bool savemovie = false;
   bool loop = false;
   std::string moviedir("screenshots");	// default screenshot directory

   if( arguments.isSWM() )
   {
	  playbackmode = true;
	  statelist.read( arguments.getFilename() );
	  if( arguments.read("-movie", moviedir) )	// choose a different folder
	  {
		 savemovie = true;  // flag to store frames and quit ...
	  }
	  if( arguments.read("-loop") ) 
	  {
		 loop = true;
	  }
   }
   else
   {
	  playbackmode = false;
	  savemovie = false;
	  loop = true;  // playback in none macro mode should loop (otherwise you don't get a chance to save)
   }

   // setup screenshot location
   osgDB::makeDirectory( moviedir );
   cap_handler->setCaptureOperation(new osgViewer::ScreenCaptureHandler::WriteToFile(moviedir+"/frame", "jpg", osgViewer::ScreenCaptureHandler::WriteToFile::SEQUENTIAL_NUMBER));

   // if user requested help, write it out to cout
   if( arguments.read("-help") || arguments.read("--help") || arguments.read("-h") )
   {
	   arguments.getApplicationUsage()->write(std::cout, osg::ApplicationUsage::HELP_ALL);
	  return 1;
   }


   // same for version info
   if( arguments.read("-version") )
   {
	  std::cout << version() << std::endl;
	  return 1;
   }


   // last argument is either an sww file or an swm macro recording
   int lastarg = arguments.argc()-1;
   std::string swwfile = arguments.argv()[lastarg];
   arguments.remove(lastarg);
   if( osgDB::getLowerCaseFileExtension(swwfile) != std::string("sww") )
   {
	  std::cout << "Require last argument be an .sww/.swm file ... quitting" << std::endl;
	  return 1; 
   }
   SWWReader *sww = new SWWReader(swwfile);
   if (sww->isValid() == false)
   {
	  std::cout << "Unable to load " << swwfile << " ... is this really an .sww file?" << std::endl;
	  return 1;
   }


   // need swollen binary directory which contains resource images
   char *ptr = getenv( "SWOLLEN_BINDIR" );
   if (ptr)
	  sww->setSwollenDir( std::string(ptr) );
   else if( osgDB::getFilePath(argv[0]) == "" )
	  sww->setSwollenDir( std::string(".") );
   else
          //sww->setSwollenDir( osgDB::getFilePath(argv[0]) );
	  sww->setSwollenDir( std::string( "/usr/local/etc/anuga-viewer" ) );
   std::cout << "SWOLLEN_BINDIR = " << sww->getSwollenDir() << std::endl;


   // default arguments and command line parameters
   float tmpfloat, tps, vscale;
   if( !arguments.read("-tps", tps) || tps <= 0.0 ) tps = DEF_TPS;
   if( !arguments.read("-scale", vscale) ) vscale = 1.0;
   if( arguments.read("-hmin",tmpfloat) ) sww->setHeightMin( tmpfloat );  
   if( arguments.read("-hmax",tmpfloat) ) sww->setHeightMax( tmpfloat );      
   if( arguments.read("-alphamin",tmpfloat) ) sww->setAlphaMin( tmpfloat );   
   if( arguments.read("-alphamax",tmpfloat) ) sww->setAlphaMax( tmpfloat );
   if( arguments.read("-cullangle",tmpfloat) ) sww->setCullAngle( tmpfloat );

   std::string bedslopetexture;
   if( arguments.read("-texture",bedslopetexture) ) sww->setBedslopeTexture( bedslopetexture );

   // root node
   osg::Group* rootnode = new osg::Group;

   // transform
   osg::PositionAttitudeTransform* model = new osg::PositionAttitudeTransform;
   model->setName("position_attitude_transform");

   // enscapsulates OpenGL state
   osg::StateSet* rootStateSet = new osg::StateSet;

   // Bedslope geometry
   BedSlope* bedslope = new BedSlope(sww);

   // Water geometry
   WaterSurface* water = new WaterSurface(sww);

   // Heads Up Display (text overlay)
   g_hud = new AnugaHUD();
   g_hud->setTitle(S_VIEWER_TITLE);

   // --- initial HUD status line values
	g_hud->setStatus("recorder", arguments.isSWM() ? "playback" : "paused");
	g_hud->setStatus("filename", swwfile);
	g_hud->setStatus("culling", water->getCulling() ? "on" : "off");
	g_hud->setStatus("wireframe", "off");

   // Lighting
   DirectionalLight* light = new DirectionalLight(rootStateSet);
   light->setPosition( osg::Vec3(1,1,1) );  // z is up

   std::string lightposstr;
   while (arguments.read("-lightpos",lightposstr))
   {
	  float x, y, z;
	  int count = sscanf( lightposstr.c_str(), "%f,%f,%f", &x, &y, &z );
	  if( count == 3 ) light->setPosition( osg::Vec3(x,y,z) );  // z is up
	  else osg::notify(osg::WARN) << "Invalid bedslope light position \"" << lightposstr << "\"" << std::endl;
   }

   // scenegraph hierarchy
   rootnode->setStateSet(rootStateSet);
   rootnode->addChild( g_hud->get() );
   rootnode->addChild( light->get() );
   rootnode->addChild(model);
   model->addChild( bedslope->get() );
   model->addChild( water->get() );

	// Load the initial frame so we can get grid extents
	sww->loadBedslopeVertexArray(0);
	bedslope->update();

	osg::Switch * grid_switch = new osg::Switch();
	grid_switch->addChild(Axes_Create(bedslope->getBound()));
	grid_switch->setAllChildrenOff();
	model->addChild(grid_switch);

   // allow vertical scaling from command line parameter
   model->setScale( osg::Vec3(1.0, 1.0, vscale) );

	// Allow us to switch the sky on and off if we don't want photo-realism
	osg::Switch * sky_switch = new osg::Switch();	

	if( !arguments.read("-nosky") )
	{
		std::string sky_tex_path = sww->getSwollenDir() + std::string("/images/sky_small.jpg");
		std::cout << "sky texture path: " << sky_tex_path << std::endl;
		
		// surrounding sky sphere
		sky_switch->addChild(Skybox_Create(10.0, sky_tex_path ));
		sky_switch->setAllChildrenOn();
		rootnode->addChild(sky_switch);
	}

   // add model to viewer.
   viewer.setSceneData(rootnode);
 
   // any option left unread are converted into errors to write out later.
   arguments.reportRemainingOptionsAsUnrecognized();
 
   // report any errors if they have occured when parsing the program aguments.
   if (arguments.errors())
   {
	  arguments.writeErrorMessages(std::cout);
	  return 1;
   }

	// set up the camera manipulators.

	//osgGA::MatrixManipulator * mman = new osgGA::FlightManipulator();
	osgGA::TerrainManipulator * mman = new osgGA::TerrainManipulator();
	mman->setNode(model);
	mman->setAutoComputeHomePosition( false );
	mman->setHomePosition(
	  osg::Vec3d(0,-3,3),    // camera location
	  osg::Vec3d(0,0,0),     // camera target
	  osg::Vec3d(0,1,1) );   // camera up vector
	mman->setMinimumDistance(0.000001);
	viewer.setCameraManipulator(mman);
	viewer.home();

	// add the state manipulator
	osgGA::StateSetManipulator * ssm = new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet());
	ssm->setKeyEventToggleBackfaceCulling('b');
	ssm->setKeyEventToggleLighting('l');
	ssm->setKeyEventToggleTexturing('t');
	ssm->setKeyEventCyclePolygonMode('\0');
	viewer.addEventHandler( ssm );

	// register additional event handler
	KeyboardEventHandler* event_handler = new KeyboardEventHandler(sww->getNumberOfTimesteps(), tps);
	viewer.addEventHandler(event_handler);

	// add the help handler
	viewer.addEventHandler(new osgViewer::HelpHandler);

	// add the window size toggle handler
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
		
	// add the stats handler
	viewer.addEventHandler(new osgViewer::StatsHandler);

	// create the windows and run the threads.
	viewer.realize();

	unsigned int timestep = 0;

	while( !viewer.done() )
	{
		if( !playbackmode )
		{
			 // current time in seconds
			 double time = viewer.getFrameStamp()->getReferenceTime();

			 event_handler->setTime( time );
			 timestep = event_handler->getTimestep();
			 water->setTimeStep(timestep);
			 bedslope->setTimeStep(timestep);
			 g_hud->setTime( sww->getTime(timestep) );

			// these methods do their own dirty checking
			water->setWireframe((event_handler->getWireframeMode() & WF_WATER) > 0);
			bedslope->setWireframe((event_handler->getWireframeMode() & WF_BED) > 0);

			if( event_handler->toggleCulling() )
			{
				bool culling = water->getCulling();
				water->setCulling(!culling);
				g_hud->setStatus("culling", culling ? "on" : "off");
			}

			GridMode ge = event_handler->getGridMode();
			viewer.setGrid(grid_switch, ge);

			if (event_handler->checkMouseClicked())
			{
				int sp = event_handler->getSelectedPoly();

				osg::ref_ptr<osg::FloatArray> time_series = new osg::FloatArray;

				if (sp >= 0)
				{
					sww->getTimeSeries(sp, ssm->getTextureEnabled() ? SWWReader::TSTYPE_STAGE : SWWReader::TSTYPE_MOMENTUM_MAGNITUDE, time_series);
				}

				g_hud->setTimeSeriesData(time_series, sww->getTime(sww->getNumberOfTimesteps()-1), ssm->getTextureEnabled() ? std::string("Stage Timeseries") : std::string("Momentum Timeseries"));
			}
			

			if( event_handler->checkReturnOrigin() )
			{
				viewer.home();
			}
		

			// '1' key starts/stops recording of view/position/setting info
			if( event_handler->toggleRecording() )
			{
				switch( recordingmode )
				{
				   case false : 
					  recordingmode = true; 		  
					  g_hud->setStatus("recorder", "recording");
					  break;
				   case true : 
					  recordingmode = false; 
					  g_hud->setStatus("recorder", "paused");
					  break;
				}
			}


			// '2' key starts playback of recorded frames
			if( event_handler->togglePlayback() && statelist.size() > 0 )
			{
				recordingmode = false;
				playbackmode = true; 
				g_hud->setStatus("recorder", "playback");
				event_handler->setPaused( true );
				playback_index = 0;
			}

			if( recordingmode )
			{
				State state = State();
				state.setTimestep( event_handler->getTimestep() );
				state.setCulling( sww->getCulling() );
				state.setWireframe( event_handler->getWireframeMode() );
				state.setGrid( event_handler->getGridMode() );
				state.setShowHUD( g_hud->isVisible());
				state.setShowTexture(ssm->getTextureEnabled());
				state.setLighting(ssm->getLightingEnabled());
				state.setMatrix(viewer.getCameraManipulator()->getMatrix());
				statelist.push_back( state );
			}
		}
		else
		{
			// in playback mode
			State state = statelist.at( playback_index );
			water->setTimeStep( state.getTimestep() );
			bedslope->setTimeStep( state.getTimestep() );
			water->setWireframe((state.getWireframe() & WF_WATER) > 0);
			bedslope->setWireframe((state.getWireframe() & WF_BED) > 0);
			water->setCulling( state.getCulling() );
			viewer.setGrid(grid_switch, state.getGrid());
			ssm->setTextureEnabled(state.getShowTexture());
			ssm->setLightingEnabled(state.getLighting());

			// update HUD
			g_hud->setTime( sww->getTime(state.getTimestep()) );
			g_hud->setVisible(state.getShowHUD());
			g_hud->setStatus("culling", state.getCulling() ? "on" : "off");
			g_hud->setWireframe((WireframeMode)state.getWireframe());

			// loop playback
			playback_index ++;
			if( playback_index == statelist.size() )
			{
				if( loop )
				{
					playback_index = 0;
				}
				else
				{
					viewer.setDone(true);
				}
			}

			// '2' key stops playback of recorded frames
			if( event_handler->togglePlayback() )
			{
				playbackmode = false; 
				g_hud->setStatus("recorder", "paused");
				event_handler->setPaused( true );
			}

			viewer.getCameraManipulator()->setByMatrix(state.getMatrix());
	  }


	  // '3' key causes compiled animation to be saved to disk
	  if( event_handler->toggleSave() )
	  {
		 std::fstream f;
		 f.open( "movie.swm", std::fstream::out );
		 if( f.is_open() )
		 {
			f << "# SWM 1.0 START" << std::endl;
			arguments.write( f );
			statelist.write( f );
			f << "# SWM END" << std::endl;
			f.close();
		 }
		 std::cout << "Wrote macro file movie.swm" << std::endl;

		g_hud->setStatus("filename", "saved as movie.swm");

	  }

		if (savemovie)
		{
			cap_handler->captureNextFrame(viewer);
		}

		// Toggle sky and update bed texture if we have toggled texturing
		bool tex_enabled = ssm->getTextureEnabled();
		static bool tex_enabled_last = tex_enabled;
		if (tex_enabled != tex_enabled_last)
		{
			if (tex_enabled)
			{
				sky_switch->setAllChildrenOn();
				bedslope->onRefreshTextured(true);
			}
			else
			{
				sky_switch->setAllChildrenOff();
				bedslope->onRefreshTextured(false);
			}
		}
		tex_enabled_last = tex_enabled;

		// scene-graph updates
		water->update();
		bedslope->update();
		g_hud->update();

		// fire off the cull and draw traversals of the scene.
		viewer.frame();
	}
	
   return 0;
}
