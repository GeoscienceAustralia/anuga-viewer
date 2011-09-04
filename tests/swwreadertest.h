
#ifndef SWWREADERTEST_H
#define SWWREADERTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/SourceLine.h>
#include <cppunit/TestAssert.h>
#include <swwreader.h>
#include <osg/Geometry>
#include <stdio.h>

// allowable distance between two Vectors to be considered equal
#define TOLERANCE 0.001


// custom assertion testing equivalence of two OSG Vec3
void checkVec3Equal( osg::Vec3 expected, osg::Vec3 actual, CppUnit::SourceLine sourceLine )
{  
    float distance = (expected-actual).length();
    if (distance < TOLERANCE)
        return;

    // test failed
    char s_expected[20], s_actual[20];
    sprintf(s_expected, "%10.3f", expected);
    sprintf(s_actual, "%10.3f", actual);
    ::CppUnit::Asserter::failNotEqual( s_expected, s_actual, sourceLine );
}

// macro shortcut to above assertion
#define CPPUNIT_ASSERT_VEC3_EQUAL( expected, actual ) \
    checkVec3Equal( expected, actual, CPPUNIT_SOURCELINE() )




class SWWReaderTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( SWWReaderTest );
  CPPUNIT_TEST( testValid );
  CPPUNIT_TEST( testNumberOfVertices );
  CPPUNIT_TEST( testTime );
  CPPUNIT_TEST( testBedslopeVertexArray );
  CPPUNIT_TEST( testBedslopeIndexArray );
  CPPUNIT_TEST( testBedslopeNormalArray );
  CPPUNIT_TEST( testConnectivity );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void testValid();
  void testNumberOfVertices();
  void testTime();
  void testBedslopeVertexArray();
  void testBedslopeIndexArray();
  void testBedslopeNormalArray();
  void testConnectivity();


private:
    SWWReader* _sww;
};

#endif
