#include <osgViewer/Viewer>

#include "project.h"
#include "customviewer.h"
#include "anugahud.h"
#include "keyboardeventhandler.h"


extern AnugaHUD * g_hud;


CustomViewer::CustomViewer(osg::ArgumentParser& aArguments)	:
	osgViewer::Viewer(aArguments)
{
	getCamera()->setClearColor( osg::Vec4(DEF_BACKGROUND_COLOUR) );
	getCamera()->setComputeNearFarMode( osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);		
}


void CustomViewer::getUsage(osg::ApplicationUsage& usage) const
{
	usage.addKeyboardMouseBinding("+","Increase fusion distance (stereo mode only)");
	usage.addKeyboardMouseBinding("-","Decrease fusion distance (stereo mode only)");
	usage.addKeyboardMouseBinding("f","Toggle fullscreen");
	usage.addKeyboardMouseBinding("h","Display help");
	usage.addKeyboardMouseBinding("t","Toggle between realistic water and magnitude overlay");
	usage.addKeyboardMouseBinding("l","Toggle lighting");
	usage.addKeyboardMouseBinding("b","Toggle backface culling of polygons");
	usage.addKeyboardMouseBinding("s","Cycle through render performance statistics");
	usage.addKeyboardMouseBinding("O","Take screenshot, saved in screenshots folder");
	usage.addKeyboardMouseBinding("<","Decrease window size (windowed mode only)");
	usage.addKeyboardMouseBinding(">","Increase window size (windowed mode only)");

	// --- add this app's custom usage
	KeyboardEventHandler::getAppUsage(usage);
}


void CustomViewer::setGrid(osg::Switch * aGridSwitch, GridMode ge)
{
	static GridMode grid_enabled_last = GM_BOTH;

	if(ge != grid_enabled_last)
	{
		switch (ge)
		{
			case GM_NONE:
				g_hud->setStatus("grid", "none");
				g_hud->setShowIntensityScale(false);
				aGridSwitch->setAllChildrenOff();
				break;

			case GM_GRID:
				g_hud->setStatus("grid", "grid");
				g_hud->setShowIntensityScale(false);
				aGridSwitch->setAllChildrenOn();
				break;

			case GM_COLORBAR:
				g_hud->setStatus("grid", "colorbar");
				g_hud->setShowIntensityScale(true);
				aGridSwitch->setAllChildrenOff();
				break;

			case GM_BOTH:
				g_hud->setStatus("grid", "grid+colorbar");
				g_hud->setShowIntensityScale(true);
				aGridSwitch->setAllChildrenOn();
				break;
		}
	}

	grid_enabled_last = ge;
}
