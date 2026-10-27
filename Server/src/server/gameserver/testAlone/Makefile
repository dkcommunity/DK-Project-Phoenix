#----------------------------------------------------------------------
#
# Filename    : Makefile
# Written By  : Reiot
# Description :
#
#----------------------------------------------------------------------

#----------------------------------------------------------------------
# Directories Setting
#----------------------------------------------------------------------
#VSHOME   = ../../../..
STLDIR = 
BINDIR   = $(VSHOME)/bin
LIBDIR   = $(VSHOME)/lib
LOGDIR   = $(VSHOME)/log
SRCDIR   = ../../..
COREDIR  = ../../../Core
SERVDIR  = ../..
DBDIR    = ../../database
SKILLDIR = ../skill
GSDIR    = ..
WARDIR	 = ../war
MYSQLDIR = /usr/local/lib/mysql
CPPUNITDIR = /usr/local/include/cppunit


#----------------------------------------------------------------------
# General Rules
#----------------------------------------------------------------------
.SUFFIXES    : .cpp .o

CC       = g++
DEFS     = -D__LINUX__ -D_REENTRANT -D__GAME_SERVER__
INCLUDES = -I.  -I$(COREDIR) -I$(SERVDIR) -I$(DBDIR) -I$(GSDIR) -I$(SKILLDIR) -I$(CPPUNITDIR) -I$(WARDIR)
CCFLAGS  = -Wall -c -g -std=c++98 -std=c++11 -U_GLIBCXX_ASSERTIONS -D_GLIBCXX_NO_ASSERTIONS -fno-stack-protector -fno-enforce-eh-specs
#CCFLAGS  = -Wall -c
LIBS	 = #-L$(LIBDIR) -lWar
SYSLIBS  = -lcppunit

.cpp.o :
	$(CC) $(CCFLAGS) $(DEFS) $(INCLUDES) $<

#----------------------------------------------------------------------
# Objects
#----------------------------------------------------------------------

OBJS = \
	main.o \
#	Scheduler.o \
#	Schedule.o \
#	VSDateTime.o \
#	StringStream.o \
#	ScheduleTest.o

SRCS = $(OBJS:.o=.cpp)

TARGET = testAlone


#----------------------------------------------------------------------
# actions
#----------------------------------------------------------------------
all : 
	$(MAKE) $(TARGET)

$(TARGET) : $(OBJS) #$(LIBDIR)/libWar.a
	$(CC) -o $@ $(OBJS) $(LIBS) $(SYSLIBS)

clean :
	rm -rf *.o

cleanall :
	rm -rf *.o $(TARGET)

depall :
	gccmakedep $(SRCS) $(INCLUDES)

dep :
	gccmakedep $(SRCS) $(INCLUDES)

# DO NOT DELETE
