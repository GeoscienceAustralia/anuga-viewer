
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "swwreadertest.h"


// Registers the fixture
CPPUNIT_TEST_SUITE_REGISTRATION( SWWReaderTest );



void SWWReaderTest::setUp()
{
    _sww = new SWWReader("../tests/tests.sww");
}


void SWWReaderTest::testValid()
{
    CPPUNIT_ASSERT( _sww->isValid() );
}



void SWWReaderTest::testNumberOfVertices()
{
    const size_t nvertices = 20;
    CPPUNIT_ASSERT( _sww->isValid() );
    CPPUNIT_ASSERT_EQUAL( _sww->getNumberOfVertices(), nvertices );
}



void SWWReaderTest::testTime()
{
    const unsigned int ntimesteps = 3;
    CPPUNIT_ASSERT( _sww->isValid() );
    CPPUNIT_ASSERT_EQUAL( _sww->getNumberOfTimesteps(), ntimesteps );

    // hard-coded values from sww file
    const float time_array[3] = {0.0, 0.5, 1.0};
    for (unsigned int i=0; i<ntimesteps; i++)
	{
        CPPUNIT_ASSERT_EQUAL( _sww->getTime(i), time_array[i] );
	}
}




void SWWReaderTest::testBedslopeVertexArray()
{
    osg::ref_ptr<osg::Vec3Array> actual = _sww->getBedslopeVertexArray();
    CPPUNIT_ASSERT( _sww->isValid() );
    CPPUNIT_ASSERT( actual );

    // expected number of bedslope vertices
    const size_t nvertices = 20;
    CPPUNIT_ASSERT_EQUAL( actual->size(), nvertices );

    // hard-coded values extracted from sww file - note that these values are normalised by the reader
    using osg::Vec3;
    using osg::Vec3Array;
    Vec3Array *expected = new Vec3Array(20);
    (*expected)[ 0] = Vec3(-0.5, -0.5, 0.333333);
    (*expected)[ 1] = Vec3(-0.5, -0.166667, 0.444444);
    (*expected)[ 2] = Vec3(-0.5, 0.166667, 0.555556);
    (*expected)[ 3] = Vec3(-0.5, 0.5, 0.666667);
    (*expected)[ 4] = Vec3(-0.25, -0.5, 0.25);
    (*expected)[ 5] = Vec3(-0.25, -0.166667, 0.361111);
    (*expected)[ 6] = Vec3(-0.25, 0.166667, 0.472222);
    (*expected)[ 7] = Vec3(-0.25, 0.5, 0.583333);
    (*expected)[ 8] = Vec3(0, -0.5, 0.166667);
    (*expected)[ 9] = Vec3(0, -0.166667, 0.277778);
    (*expected)[10] = Vec3(0, 0.166667, 0.388889);
    (*expected)[11] = Vec3(0, 0.5, 0.5);
    (*expected)[12] = Vec3(0.25, -0.5, 0.0833333);
    (*expected)[13] = Vec3(0.25, -0.166667, 0.194444);
    (*expected)[14] = Vec3(0.25, 0.166667, 0.305556);
    (*expected)[15] = Vec3(0.25, 0.5, 0.416667);
    (*expected)[16] = Vec3(0.5, -0.5, 0);
    (*expected)[17] = Vec3(0.5, -0.166667, 0.111111);
    (*expected)[18] = Vec3(0.5, 0.166667, 0.222222);
    (*expected)[19] = Vec3(0.5, 0.5, 0.333333);

    for (size_t i=0; i<nvertices; i++)
	{
        CPPUNIT_ASSERT_VEC3_EQUAL( expected->at(i), actual->at(i) );
	}
}



void SWWReaderTest::testBedslopeIndexArray()
{
    osg::ref_ptr<osg::DrawElementsUInt> actual = _sww->getBedslopeIndexArray();
    CPPUNIT_ASSERT( _sww->isValid() );
    CPPUNIT_ASSERT( actual );

    // expected number of bedslope indices
    const size_t nindices = 24*3;
    CPPUNIT_ASSERT_EQUAL( actual->size(), nindices );

    // hard-coded values extracted from sww file
    const unsigned int expected[72] = { 4, 5, 0,  1, 0, 5,  5, 6, 1, 
        2, 1, 6,  6, 7, 2,  3, 2, 7,  8, 9, 4,  5, 4, 9,  9, 10, 5, 
        6, 5, 10,  10, 11, 6,  7, 6, 11,  12, 13, 8,  9, 8, 13,
        13, 14, 9,  10, 9, 14,  14, 15, 10,  11, 10, 15,  16, 17, 12, 
        13, 12, 17,  17, 18, 13,  14, 13, 18,  18, 19, 14,  15, 14, 19 };

    for (size_t i=0; i<nindices; i++)
        CPPUNIT_ASSERT_EQUAL( actual->at(i), expected[i] );
}



void SWWReaderTest::testConnectivity()
{
    CPPUNIT_ASSERT( _sww->isValid() );

    // shared vertices for triangle 17
    triangle_list actual = _sww->getConnectivity(17);

    const size_t nshared = 3;
    CPPUNIT_ASSERT_EQUAL( actual.size(), nshared );
    const unsigned int expected[nshared] = { 18, 19, 20 };
    for (size_t i=0; i<nshared; i++)
        CPPUNIT_ASSERT_EQUAL( actual.at(i), expected[i] );
}



void SWWReaderTest::testBedslopeNormalArray()
{
    CPPUNIT_ASSERT( _sww->isValid() );

    osg::ref_ptr<osg::Vec3Array> actual = _sww->getBedslopeNormalArray();
    CPPUNIT_ASSERT( actual );

    // expected number of bedslope normals
    const size_t nvertices = 24;
    CPPUNIT_ASSERT_EQUAL( actual->size(), nvertices );

    // hard-coded values (bedslope is flat plane)
    using osg::Vec3;
    using osg::Vec3Array;
    Vec3Array *expected = new Vec3Array; 
    expected->assign(24, Vec3( 0.301511, -0.301511, 0.904534 ) );

    for (size_t i=0; i<nvertices; i++)
        CPPUNIT_ASSERT_VEC3_EQUAL( actual->at(i), expected->at(i) );
}




void SWWReaderTest::tearDown()
{
}




int main(int argc, char* argv[])
{
  // Get the top level suite from the registry
  CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

  // Add test
  CppUnit::TextUi::TestRunner runner;
  runner.addTest( suite );

  // Change default outputter to a compiler error format outputter
  runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(), std::cerr ) );

  // Run tests
  bool wasSucessful = runner.run();

//	while(1);

  // Error code 1 if any tests failed
  return wasSucessful ? 0 : 1;
}
