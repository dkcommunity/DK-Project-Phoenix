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
#VSHOME  = ../../..
STLDIR = 
BINDIR  = $(VSHOME)/bin
LIBDIR  = $(VSHOME)/lib
LOGDIR  = $(VSHOME)/log
DBINCDIR  = /usr/local/include/mysql
SRCDIR  = ../../../..
COREDIR = ../../../../Core
SERVDIR = ../../..
DBDIR   = ../../../database
MYSQLDIR = /usr/local/lib/mysql

#----------------------------------------------------------------------
# General Rules
#----------------------------------------------------------------------
.SUFFIXES    : .cpp .o

CC       = g++
DEFS     = -D__LINUX__ -D_REENTRANT -D__DEBUG__
INCLUDES =  -I$(COREDIR) -I$(SERVDIR) -I. -I../
CCFLAGS  = -Wall -c -g -std=c++98 -std=c++11 -U_GLIBCXX_ASSERTIONS -D_GLIBCXX_NO_ASSERTIONS -fno-stack-protector -fno-enforce-eh-specs
#CCFLAGS  = -Wall -c
LIBS     = -L$(LIBDIR) -lServerCore -lCore -lMofus
SYSLIBS  = -L$(MYSQLDIR) -static -lpthread  -lz

.cpp.o :
	$(CC) $(CCFLAGS) $(DEFS) $(INCLUDES) $<


#----------------------------------------------------------------------
# Objects
#----------------------------------------------------------------------
OBJS = \
	main.o GameServerManager.o GameServerPlayer.o MPacketManager.o \
	PKTConnectAskHandler.o PKTLogoutHandler.o PKTReceiveOKHandler.o \
	PKTResultHandler.o PKTUserInfoHandler.o

SRCS = $(OBJS:.o=.cpp)

TARGET = $(BINDIR)/mofusserver


#----------------------------------------------------------------------
# actions
#----------------------------------------------------------------------
$(TARGET) : $(OBJS) $(LIBDIR)/libServerCore.a $(LIBDIR)/libCore.a $(LIBDIR)/libMofus.a
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
