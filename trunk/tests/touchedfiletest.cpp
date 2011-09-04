
#include<sys/stat.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#define OS_SLEEP_MS(x) Sleep(x);
#else
#include <unistd.h>
#define OS_SLEEP_MS(x) usleep(x*1000);
#endif

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <filechangedcheck.h>

#include "touchedfiletest.h"

// local data
static const char * s_testFilename = "touchtest.txt";


// Registers the fixture
CPPUNIT_TEST_SUITE_REGISTRATION( TouchedFileTest );



void TouchedFileTest::setUp()
{
	FILE * fh;
	const char * txt = "some test text.\n";
	fh = fopen(s_testFilename, "wt");
	fwrite(txt, strlen(txt), 1, fh);
	fclose(fh);
}


void TouchedFileTest::testCreateFile()
{
	struct stat buf;

	CPPUNIT_ASSERT(stat(s_testFilename, &buf) == 0);	// File exists
}

void TouchedFileTest::testNoChange()
{
	FileChangedCheck check;

	CPPUNIT_ASSERT(check.watch(std::string(s_testFilename)));

	CPPUNIT_ASSERT(!check.isChanged());
}

void TouchedFileTest::testNoExist()
{
	FileChangedCheck check;

	CPPUNIT_ASSERT(!check.watch("not_here.blah"));
}


void TouchedFileTest::testChange()
{
	FileChangedCheck check;

	check.watch(std::string(s_testFilename));

	CPPUNIT_ASSERT(!check.isChanged());	// File should not have been changed since creation

	OS_SLEEP_MS(1100);	// let timestamp change

	const char * txt = "some tezt text.\n";
	FILE * fh = fopen(s_testFilename, "wt");
	fwrite(txt, strlen(txt), 1, fh);
	fclose(fh);

	CPPUNIT_ASSERT(check.isChanged());	// File should have been changed
}


void TouchedFileTest::testInstantLengthChange()
{
		FileChangedCheck check;

	check.watch(std::string(s_testFilename));

	CPPUNIT_ASSERT(!check.isChanged());	// File should not have been changed since creation

	const char * txt = "some test text - length has changed.\n";
	FILE * fh = fopen(s_testFilename, "wt");
	fwrite(txt, strlen(txt), 1, fh);
	fclose(fh);

	CPPUNIT_ASSERT(check.isChanged());	// File should have been changed, even though the timestamp may not have changed
}


void TouchedFileTest::tearDown()
{
#ifdef WIN32
	_unlink(s_testFilename);
#else
	unlink(s_testFilename);
#endif
}

