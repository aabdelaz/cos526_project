#
# Unix/Linux makefile for assignment #1
#



# 
# List of source files
#

RAD_SRCS=radiation.cpp
RAD_OBJS=$(RAD_SRCS:.cpp=.o)

RADBF_SRCS=radiationbf.cpp
RADBF_OBJS=$(RADBF_SRCS:.cpp=.o)

KDTVIEW_SRCS=kdtview.cpp
KDTVIEW_OBJS=$(KDTVIEW_SRCS:.cpp=.o)


#
# Compile and link options
#

CC=g++
CPPFLAGS=-Wall -I. -g #-mmacosx-version-min=10.8
USER_CFLAGS=$(CPPFLAGS)
export USER_CFLAGS
LDFLAGS=-g



#
# Pkg Libraries
#

PKG_LIBS= \
  R3Graphics/libR3Graphics.a \
  R3Shapes/libR3Shapes.a \
  R2Shapes/libR2Shapes.a \
  RNBasics/libRNBasics.a \
  fglut/libfglut.a \
  jpeg/libjpeg.a 



#
# OpenGL Libraries
#

OS=$(shell uname -s)
ifeq ("$(findstring CYGWIN,$(OS))", "CYGWIN")
OPENGL_LIBS=-lglu32 -lopengl32 -lwinmm -lgdi32
else ifeq ("$(OS)","Darwin")
OPENGL_LIBS=-framework GLUT -framework opengl
CPPFLAGS+= -mmacosx-version-min=10.8
else
OPENGL_LIBS=-lGLU -lGL
endif



#
# Compile command
#

%.o: %.cpp 
	    $(CC) $(CPPFLAGS) -c $< -o $@



#
# GNU Make: targets that don't build files
#

.PHONY: all clean distclean



#
# Make targets
#

all: $(PKG_LIBS) radiation kdtview 

radiation: $(LIBS) $(RAD_OBJS) 
	    $(CC) -o radiation $(CPPFLAGS) $(LDFLAGS) $(RAD_OBJS) $(PKG_LIBS) $(OPENGL_LIBS) -lm

radiationbf: $(LIBS) $(RADBF_OBJS) 
	    $(CC) -o radiationbf $(CPPFLAGS) $(LDFLAGS) $(RADBF_OBJS) $(PKG_LIBS) $(OPENGL_LIBS) -lm

kdtview: $(LIBS) $(KDTVIEW_OBJS) 
	    $(CC) -o kdtview $(CPPFLAGS) $(LDFLAGS) $(KDTVIEW_OBJS) $(PKG_LIBS) $(OPENGL_LIBS) -lm

R3Graphics/libR3Graphics.a: 
	    cd R3Graphics; make

R3Shapes/libR3Shapes.a: 
	    cd R3Shapes; make

R2Shapes/libR2Shapes.a: 
	    cd R2Shapes; make

RNBasics/libRNBasics.a: 
	    cd RNBasics; make

fglut/libfglut.a: 
	    cd fglut; make

jpeg/libjpeg.a: 
	    cd jpeg; make

clean:
	    ${RM} -f */*.a */*/*.a *.o */*.o */*/*.o radiation radiation.exe kdtview kdtview.exe $(PKG_LIBS)

distclean:  clean
	    ${RM} -f *~ 






