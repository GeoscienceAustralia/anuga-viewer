#include "anugahud.h"


AnugaHUD::AnugaHUD() :
	HeadsUpDisplay()
{
	osg::Geode* textnode = new osg::Geode;

	// --- add our status lines
	addStatusLine("wireframe", textnode);
	addStatusLine("recorder", textnode);
	addStatusLine("culling", textnode);
	addStatusLine("grid", textnode);
	addStatusLine("filename", textnode);

	_text_switch->addChild(textnode);
}


void AnugaHUD::setWireframe(WireframeMode aWireframeMode)
{
	std::string mode_string;
	switch(aWireframeMode)
	{
		case WF_BED:
			mode_string = "bed";
			break;

		case WF_BOTH:
			mode_string = "bed+water";
			break;

		case WF_WATER:
			mode_string = "water";
			break;

		case WF_NONE:
			mode_string = "off";
			break;
		
		default:
			assert(0);
			break;
	}

	setStatus("wireframe", mode_string);
}
