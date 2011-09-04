/*
    CustomArgementParser class

    An OpenSceneGraph viewer for ANUGA .sww files.
    copyright (C) 2004-2005 Geoscience Australia
*/

#include <osg/ArgumentParser>
#include <istream>
#include <vector>
#include <string>


class CustomArgumentParser : public osg::ArgumentParser
{

public:

    CustomArgumentParser(int *argc, char **argv);
    void write(std::ostream& s);
    bool isSWM(){ return _isswm; };
    std::string getFilename(){ return *_filename; }

protected:
	void SetUsage();


protected:

    int _nargs;
    std::vector<std::string> _vargs;
    std::string* _filename;
    bool _isswm;
};

