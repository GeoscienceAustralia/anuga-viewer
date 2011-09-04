#ifndef CUSTOMVIEWER_H_
#define CUSTOMVIEWER_H_

/**
 * Override some viewer functionality.
 * This class contains the bulk of the application logic.
 */
class CustomViewer : public osgViewer::Viewer
{
public:
	/**
	 * constructor
	 * @param aArguments Command line parameters
	 */
	CustomViewer(osg::ArgumentParser& aArguments);

	/**
	 * Toggle grid on and off
	 * @param aGridSwitch switch to toggle the graphical display
	 * @param aGe grid enable toggle
	 */
	void setGrid(osg::Switch * aGridSwitch, GridMode aGe);

protected:
	/**
	 * Do not let the viewer show its usage, we want to show our own (more verbose) help
	 * Overrides osgViewer::Viewer method.
	 * @param aUsage return parameter to which usage information will be added
	 */
	void getUsage(osg::ApplicationUsage& aUsage) const;
};

#endif // CUSTOMVIEWER_H_
