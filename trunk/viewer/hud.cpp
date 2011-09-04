
/*
  HeadsUpDisplay class for orthographic foreground display of text

  An OpenSceneGraph viewer for ANUGA .sww files.
  Copyright (C) 2004, 2009 Geoscience Australia
*/

#include <stdio.h>
#include <sstream>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/StateSet>
#include <osgViewer/Viewer>

#include "linegraph.h"
#include "project.h"
#include "hud.h"

#define DEF_HUD_COLOUR   0.8, 0.8, 0.8, 1.0
osg::Vec4 COLORBAR_TEXT_COL(0.8, 0.8, 0.8, 1.0);


static const std::string FONT_PATH = "fonts/arial.ttf"; // This is the default font path as used by OSG

// constructor
HeadsUpDisplay::HeadsUpDisplay()
	: _linegraph(NULL),
	_font(NULL),
	_status_pos(256, 32),
	_status_visible(true),
	_status_visible_dirty(false)
{
   // a heads-up display requires an orthographic projection
   _projection = new osg::Projection;
   _projection->setMatrix( osg::Matrix::ortho2D(0,ORTHO2D_WIDTH,0,ORTHO2D_HEIGHT) );

   // font
	_font = osgText::readFontFile(FONT_PATH);	
   assert(_font);

   // title text
   _titletext = addText(osg::Vec3(20,20,0), *_font);
   _dirtytime = false;

   // timer text
   _timetext = addText(osg::Vec3(1100,20,0), *_font, true);
   _timevalue = 0.0;
   _dirtytime = true;

	_dirtytimeseries = false;

   // state
   osg::StateSet *state = _projection->getOrCreateStateSet();
   state->setRenderBinDetails(12, "RenderBin");
   state->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
   state->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

   // scenegraph
   _xfm = new osg::MatrixTransform;
   _xfm->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   _xfm->setMatrix(osg::Matrix::identity());
   osg::Geode* textnode = new osg::Geode;

   textnode->addDrawable( _titletext );
   textnode->addDrawable( _timetext );

   osg::Geode* intensity_scale_node = new osg::Geode;
   _intensity_scale_switch = new osg::Switch;
   _text_switch = new osg::Switch;


   {
		osgText::Text* intensityscale = addText(osg::Vec3(48,300,0), *_font);
		intensityscale->setText("> 2.0");
		intensityscale->setColor(COLORBAR_TEXT_COL);
		intensity_scale_node->addDrawable(intensityscale);
   }

   {
		osgText::Text* intensityscale = addText(osg::Vec3(48,300-256,0), *_font);
		intensityscale->setText("0.0");
		intensityscale->setColor(COLORBAR_TEXT_COL);
		intensity_scale_node->addDrawable(intensityscale);
   }
	intensity_scale_node->addDrawable(IntensityBar_Create(osg::Vec3(30,300,0), 16, 256));
	_intensity_scale_switch->addChild(intensity_scale_node);
   _intensity_scale_switch->setAllChildrenOff();

	
	_text_switch->addChild(textnode);
	_text_switch->setAllChildrenOn();

   _xfm->addChild( _text_switch);
   _xfm->addChild( _intensity_scale_switch );
   _projection->addChild( _xfm );
}


HeadsUpDisplay::~HeadsUpDisplay()
{
	if (_linegraph)
	{
		delete _linegraph;
		_linegraph = NULL;
	}
}


osgText::Text * HeadsUpDisplay::addText(const osg::Vec3 & aPos,  osgText::Font & aFont, bool aLarge)
{
	osgText::Text * text = new osgText::Text;

	text->setFont(&aFont);
	text->setColor(osg::Vec4(DEF_HUD_COLOUR));
	text->setCharacterSize(aLarge ? 30 : 20);
	text->setPosition(aPos);
	text->setFontResolution(40, 40);
	text->setDataVariance(osg::Object::DYNAMIC);	// Flag that it may change at any time, otherwise we will have threading issues

	return text;
}


void HeadsUpDisplay::setTime( float t )
{
   if( t != _timevalue )
   {
      _timevalue = t;
      _dirtytime = true;
   }
}


void HeadsUpDisplay::setTitle( const std::string & s )
{
   if( s != _titlestring )
   {
      _titlestring = s;
      _dirtytitle = true;
   }
}


void HeadsUpDisplay::setStatus(const std::string & aField, const std::string & aStatus)
{
	_statusmap[aField]._label = aStatus;
    _dirtystatus = true;
}


void HeadsUpDisplay::update()
{
   if( _dirtytime )
   {
      char timestr[256];
      sprintf(timestr, "t = %-7.2f", _timevalue);
      _timetext->setText(timestr);
      // hud time now updated ...
      _dirtytime = false;
   }


   if( _dirtystatus )
   {
		for(TStatusMap::iterator i = _statusmap.begin(); i!=_statusmap.end(); ++i)
		{
			std::ostringstream text;

			text << i->first << ": " << i->second._label;

			i->second._drawable->setText(text.str());
		}

      // hud "recording/playback" text now updated ...
      _dirtystatus = false;
   }


   if( _dirtytitle )
   {
      _titletext->setText(_titlestring);

      // hud title text now updated ...
      _dirtytitle = false;
   }

   if (_dirtytimeseries)
   {
		if (_linegraph)
		{
			_xfm->removeChild(_linegraph->getGeode());
			delete(_linegraph);
			_linegraph = NULL;
		}

		osg::FloatArray * fa = _graphdata._data.get();
		if (fa && fa->size()>1)
		{
			// If there is new data, show it.
			_linegraph = new LineGraph;
			_xfm->addChild(_linegraph->setUpScene(_graphdata._title, fa, _graphdata._timelength, osg::Vec3(16.0f, ORTHO2D_HEIGHT*0.66f-24.0f, 0), osg::Vec2(ORTHO2D_WIDTH - 400, 400)));
		}

		_dirtytimeseries = false;
   }

	if (_status_visible_dirty)
	{
		_status_visible_dirty = false;
		if (!_status_visible)
		{
			_text_switch->setAllChildrenOff();
		}
		else
		{
			_text_switch->setAllChildrenOn();
		}
	}
}

void HeadsUpDisplay::setTimeSeriesData(osg::ref_ptr<osg::FloatArray> aData, float aTimeLength, std::string aTitle)
{
	_dirtytimeseries = true;
	_graphdata._data = aData;
	_graphdata._timelength = aTimeLength;
	_graphdata._title = aTitle;
}


void HeadsUpDisplay::addStatusLine(const std::string & aLabel, osg::Geode* aParentGeode)
{
	StatusData data;

	assert(_font);

	data._drawable = addText(osg::Vec3(_status_pos.x(), _status_pos.y(), 0), *_font);

	_statusmap.insert(TStatusPair(aLabel, data));

	aParentGeode->addDrawable( data._drawable );
   _dirtystatus = true;

   if (_status_pos.x() >= 768)
   {
	   _status_pos.set(0, _status_pos.y()+40);
   }

   _status_pos += osg::Vec2(256, 0);
}
