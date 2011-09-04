
#
#  Mac OS X / Linux Makefile
#

UNAME := $(shell uname)
TOPDIR           =  ..

OPTIMIZATION     =  -O2

NETCDF_INCLUDE   =  /sw/include
OSG_INCLUDE      =  ${OSGHOME}/include
LOCAL_INCLUDE    =  $(TOPDIR)/include
GDAL_INCLUDE     =  `gdal-config --cflags`
INCLUDES         =  -I $(LOCAL_INCLUDE) -I $(NETCDF_INCLUDE) -I $(OSG_INCLUDE) $(GDAL_INCLUDE)

ifeq ($(UNAME), Darwin)
	# OS X
	OSX_LIBS        = -framework Carbon -framework OpenGL -lobjc
	LIB_TYPE	= -dynamiclib -F/System/Library/Frameworks
	TARGET           =  $(TOPDIR)/bin/lib$(NAME).dylib
else
	# Linux
	LIB_TYPE	= -shared
	CFLAGS		= -fPIC -fno-strict-aliasing -Wall
	TARGET           =  $(TOPDIR)/bin/lib$(NAME).so
	TARGET_FINAL_DIR = /usr/local/lib/lib$(NAME).so.1
endif

NETCDF_LIBS      =  -lnetcdf
X_LIBS           =  -lX11
GDAL_LIBS        =  `gdal-config --libs`
OTHER_LIBS       =  -lm -lstdc++
LIBS            +=  -losg -losgDB -lOpenThreads $(NETCDF_LIBS) $(X_LIBS) $(OTHER_LIBS) $(OSX_LIBS) $(GDAL_LIBS)
LIBDIRS          =  -L/usr/lib -L/usr/X11R6/lib -L/sw/lib -L/usr/local/lib

COMPILER         =  g++
NAME             =  swwreader
OBJ              =  filechangedcheck.o swwreader.o


$(TARGET) : $(OBJ)
	$(COMPILER) $(OPTIMIZATION) $(LIB_TYPE) $(LIBDIRS) $(LIBS) $(OBJ) -o $(TARGET)

%.o : %.cpp
	$(COMPILER) $(OPTIMIZATION) $(CFLAGS) -c $(INCLUDES) $< -o $@


clean :
	rm -f *.o *~ $(TARGET)

install :
	# Linux - create symbolic link for version-independent access
	mv $(TARGET) $(TARGET_FINAL_DIR)
	ln -sf $(TARGET_FINAL_DIR) /usr/local/lib/lib$(NAME).so
	ldconfig


