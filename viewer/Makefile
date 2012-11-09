
#
#  Mac OS X / Linux Makefile
#

UNAME := $(shell uname)
TOPDIR           =  ..

#OPTIMIZATION     =  -O2
OPTIMIZATION     =  -g

OSGHOME = /usr/local

OSG_INCLUDE      =  ${OSGHOME}/include
LOCAL_INCLUDE    =  $(TOPDIR)/include
INCLUDES         =  -I . -I $(LOCAL_INCLUDE) -I $(OSG_INCLUDE) -I/usr/X11R6/include

ifeq ($(UNAME), Darwin)
	# OS X
	OSX_LIBS        = -framework Carbon -framework OpenGL -lobjc
	LIB_TYPE	= -dynamiclib
	CPPFLAGS         =  -F/System/Library/Frameworks -Wall -DDARWIN_QUICKTIME \
		            $(OPTIMIZATION)
else
	# Linux
#	LIB_TYPE	= -shared
	CPPFLAGS         =  -Wall $(OPTIMIZATION)
endif

X_LIBS           =  -lX11
OTHER_LIBS       =  -lm -lstdc++ -lnetcdf `gdal-config --libs`
LIBS            +=  -losg -losgText -losgDB -losgUtil -losgGA \
                    -lOpenThreads -losgViewer -lswwreader \
                    $(GL_LIBS) $(X_LIBS) $(OTHER_LIBS) $(OSX_LIBS)
LIBDIRS          =  -L/usr/lib -L/usr/X11R6/lib -L/sw/lib -L/usr/local/lib64 -L/usr/local/lib -L$(TOPDIR)/bin

NAME             =  anuga_viewer
TARGETDIR        =  $(TOPDIR)/bin
TARGET           =  $(TARGETDIR)/$(NAME)

COMPILER         =  g++
OBJ              =  anugahud.o hud.o keyboardeventhandler.o watersurface.o main.o version.o \
                    bedslope.o skybox.o linegraph.o customviewer.o \
                    directionallight.o state.o meshobject.o customargumentparser.o



%.o : %.cpp
	$(COMPILER) -c $(INCLUDES) $(CPPFLAGS) $< -o $@


$(TARGET) : $(OBJ)
	$(COMPILER) $(CPPFLAGS) $(LIBDIRS) $(OBJ) $(LIBS) -o $(TARGET)


.PHONY : version.cpp

# on every build, recreate version() string based on SVN revision number
version.cpp:
	echo -n 'const char* version() { const char* s = "Revision: ' > version.cpp
	svnversion -n . >> version.cpp
	echo '"; return s; }' >> version.cpp


clean :
	rm -f *.o *~ $(TARGET)

