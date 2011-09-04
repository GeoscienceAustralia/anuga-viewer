
#
#  Mac OS X / Linux Makefile
#

UNAME := $(shell uname)

OPTIMIZATION     =  -O2

NETCDF_INCLUDE   =  /sw/include
OSG_INCLUDE      =  ${OSGHOME}/include
LOCAL_INCLUDE    =  ../include
GDAL_INCLUDE     =  `gdal-config --cflags`
INCLUDES         =  -I $(LOCAL_INCLUDE) -I $(NETCDF_INCLUDE) -I $(OSG_INCLUDE) $(GDAL_INCLUDE)

ifeq ($(UNAME), Darwin)
	# OS X
	OSX_LIBS        = -framework Carbon -framework OpenGL -lobjc
	TARGET           =  tests
else
	# Linux
	CFLAGS		=  -Wall
	TARGET           =  tests
endif

NETCDF_LIBS      =  -lnetcdf
X_LIBS           =  -lX11
GDAL_LIBS        =  `gdal-config --libs`
OTHER_LIBS       =  -lm -lstdc++
LIBS            +=  -losg -losgDB -lOpenThreads $(NETCDF_LIBS) $(X_LIBS) $(OTHER_LIBS) $(OSX_LIBS) $(GDAL_LIBS) -lcppunit
LIBDIRS          =  -L/usr/lib -L/usr/X11R6/lib -L/sw/lib -L/usr/local/lib
TESTLIBS	= -lswwreader

COMPILER         =  g++
NAME             =  swwreader
OBJ              =  touchedfiletest.o swwreadertest.o


$(TARGET) : $(OBJ)
	$(COMPILER) $(OPTIMIZATION) $(LIBDIRS) $(LIBS) $(TESTLIBS) $(OBJ) -o $(TARGET)

%.o : %.cpp
	$(COMPILER) $(OPTIMIZATION) $(CFLAGS) -c $(INCLUDES) $< -o $@


clean :
	rm -f *.o *~ $(TARGET)

test : clean $(TARGET)
	./test
