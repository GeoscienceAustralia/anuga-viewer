
#ifndef TOUCHEDFILETEST_H_
#define TOUCHEDFILETEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/SourceLine.h>
#include <cppunit/TestAssert.h>

#include <stdio.h>


class TouchedFileTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( TouchedFileTest );
	CPPUNIT_TEST( testCreateFile );
	CPPUNIT_TEST( testNoChange );
	CPPUNIT_TEST( testChange );
	CPPUNIT_TEST( testInstantLengthChange );
	CPPUNIT_TEST( testNoExist );
	
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void testCreateFile();
	void testNoChange();
	void testNoExist();
	void testInstantLengthChange();
	void testChange();

private:

};

#endif // TOUCHEDFILETEST_H_

