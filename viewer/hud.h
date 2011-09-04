
/*
    Head's up viewer class

    An OpenSceneGraph viewer for ANUGA .sww files.
    Copyright (C) 2004, 2009 Geoscience Australia
*/


#ifndef HUD_H
#define HUD_H

#include <project.h>
#include <osg/Projection>
#include <osgText/Font>
#include <osgText/Text>
#include <osgViewer/Viewer>

// A resolution-independent canvas for our HUD layout
#define ORTHO2D_WIDTH 1280
#define ORTHO2D_HEIGHT 1024

/**
 * The orthographically projected heads up display.
 */
class HeadsUpDisplay
{

public:

    HeadsUpDisplay();
	/**
	 * Set the time display.
	*/
	virtual void setTime(float t);

	/**
	 * Set the application's title.
	 */
    	virtual void setTitle(const std::string & s);

	/**
	 * Set the status text of a field.
	 */
		virtual void setStatus(const std::string & aField, const std::string & aStatus);

		virtual void setShowIntensityScale(bool aSet)	{ if (aSet) _intensity_scale_switch->setAllChildrenOn(); else _intensity_scale_switch->setAllChildrenOff();	}
		
		/**
		 * Set the HUD text as visible or invisible
		 * @param aVisible tre if visible
		 */
		virtual void setVisible(bool aVisible) {	_status_visible_dirty = true;	_status_visible = aVisible;	}

		/**
		 * Get if the HUD text is visible or invisible
		 * @return true if visible
		 */
		virtual bool isVisible() {	return _status_visible;	}

	/**
	 * Tick the hud
	 */
    	virtual void update();

	/**
	 * Get the hud's projection for rendering.
	 */
    	virtual osg::Projection* get(){ return _projection; }

		/**
		 * Set time series data.
		 * @param aData 1D timeseries array.
		 */
		void setTimeSeriesData(osg::ref_ptr<osg::FloatArray> aData, float aTimeLength, std::string aTitle);

protected:
	/**
	 * Add a text button to the HUD.
	 * Its data may be changed on-the-fly.
	 * @param aPos Screen position
	 * @param aFont Font to use
	 * @param aLarge true if large font size is to be used
	 */
	osgText::Text * addText(const osg::Vec3 & aPos,  osgText::Font & aFont, bool aLarge = false);

	/**
	 * Add a status line
	 * Creates a status line.
	 */
	void addStatusLine(const std::string & aLabel, osg::Geode* aParentGeode);

protected:

	// @todo refactor this into linegraph, pass as parameter
	struct TimeseriesGraphData
	{
		osg::ref_ptr<osg::FloatArray> _data;
		float _timelength;
		std::string _title;
	};

	struct StatusData
	{
		std::string _label;
		osgText::Text * _drawable;
	};

	typedef std::map<std::string, StatusData> TStatusMap;
	typedef std::pair<std::string, StatusData> TStatusPair;


	TimeseriesGraphData _graphdata;
	class LineGraph * _linegraph;

    osg::Projection* _projection;
	osg::Switch * _intensity_scale_switch;
	osg::Switch * _text_switch;	/**< Switch text off and on */
    osgText::Text* _titletext;
    osgText::Text* _timetext;
    float _timevalue;
	TStatusMap _statusmap;
    std::string _titlestring;
    bool _dirtytime, _dirtystatus, _dirtytitle, _dirtytimeseries;
    virtual ~HeadsUpDisplay();

	osg::MatrixTransform* _xfm;

	osgText::Font* _font;

	osg::Vec2 _status_pos; /**< Current position of righthand edge of status grid */
	bool _status_visible;	/**< True if HUD status lines are visible */ 
	bool _status_visible_dirty;	/**< True if the visibility has changed state */

};

#endif  // HUD_H
