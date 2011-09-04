/*
    State class

    An OpenSceneGraph viewer for ANUGA .sww files.
    Copyright (C) 2004-2005, 2009 Geoscience Australia
*/


#define MAX_LINE_LENGTH 200

#include <stdio.h>
#include <string.h>

#include "state.h"


// constructor
State::State()	:
	_wireframe(0),
	_grid(false),
	_culling(false),
	_show_hud(true),
	_show_texture(true),
	_lighting(true)
{

}


State::~State()
{
}


// serialize to named stream
void State::write(std::ostream& s)
{
    s << _timestep << " ";
    s << _position.x() << " " << _position.y() << " " << _position.z() << " ";
    s << _orientation.x() << " " << _orientation.y() << " " << _orientation.z() << " " << _orientation.w() << " ";
    s << _culling << " ";
	s << _grid << " ";
	s << _lighting << " ";
	s << _show_hud << " ";
	s << _show_texture << " ";
    s << _wireframe << std::endl;
}


// reconstitute self from named stream
bool State::read(std::istream& s)
{
   s >> _timestep
     >>  _position.x() >> _position.y() >> _position.z()
     >>  _orientation.x() >> _orientation.y() >> _orientation.z() >> _orientation.w()
     >> _culling
     >> _grid
	 >> _lighting
	 >> _show_hud
	 >> _show_texture
	 >> _wireframe;

   return s.fail() ? false : true;
}


void State::setMatrix(osg::Matrix m)
{
   m.get(_orientation);
   _position = m.getTrans();

}

osg::Matrix State::getMatrix()
{
   osg::Matrix m;
   m.set(_orientation);
   m.setTrans(_position);
   return m;
}




StateList::StateList() : std::vector<State>()
{
}


bool StateList::write(std::ostream& s)
{
   if( this->size() == 0 )
      return false;

   s << "# SWM DATA" << std::endl;
   StateList::iterator iter = this->begin();
   for( ; iter < this->end(); iter++ )
      (*iter).write( s );

   return true;
}


bool StateList::read(std::string filename)
{

   // attempt to open the macro file ...
   std::fstream f;
   f.open( filename.c_str(), std::fstream::in );
   if( f.is_open() )
   {
      char str[MAX_LINE_LENGTH];
      char *p;
      int index = 0;

      // search for data section
      // FIXME: not very safe, what if data section missing?
      do 
      {
         f.getline(str, MAX_LINE_LENGTH);
         p = strstr(str, "# SWM DATA");
         index++;

      } while( p != str );


      bool valid;
      index = 0;
      do
      {
         State* s = new State();
         valid = s->read(f);
         if( valid )
         {
            this->push_back( *s );
            index++;
         }
      } while( valid );

      std::cout << "Read " << index << " frames of animation for playback ..." << std::endl;

      f.close();

      // success only on non-empty animation
      if( index )
         return true;
   }

   return false;
}
