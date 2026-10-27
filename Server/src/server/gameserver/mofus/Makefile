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
SRCDIR   = $(VSHOME)/src
COREDIR  = $(SRCDIR)/Core
SERVDIR  = $(SRCDIR)/server
DBDIR    = $(SERVDIR)/database
GSDIR    = $(SERVDIR)/gameserver
LSDIR    = $(SERVDIR)/loginserver
BILLDIR  = $(GSDIR)/billing
MYSQLDIR = /usr/local/lib/mysql


#----------------------------------------------------------------------
# General Rules
#----------------------------------------------------------------------
.SUFFIXES    : .cpp .o .gs.o .ls.o

CC       = g++
DEFS     = -D__LINUX__ -D_REENTRANT -D__GAME_SERVER
INCLUDES =  -I$(COREDIR) -I$(SERVDIR) -I$(DBDIR) -I$(GSDIR) -I.
#CCFLAGS  = -Wall -c -g 
CCFLAGS  = -Wall -c -g -std=c++98 -std=c++11 -U_GLIBCXX_ASSERTIONS -D_GLIBCXX_NO_ASSERTIONS -fno-stack-protector -fno-enforce-eh-specs
#LIBS     = -L$(LIBDIR) -lServerCore -lCore
#SYSLIBS  = -static -lpthread -ldl  -lz

# < for Library >
TARGET     = $(LIBDIR)/libMofus.a

#BINTARGET	= $(BINDIR)/mofus

#----------------------------------------------------------------------
# Objects
#----------------------------------------------------------------------

#	main.o
OBJS = \
	Mofus.o \
	MPacketManager.o \
	MPlayer.o \
	MPlayerManager.o \
	PKTConnectAccept.o \
	PKTConnectAcceptHandler.o \
	PKTConnectAsk.o \
	PKTError.o \
	PKTErrorHandler.o \
	PKTLogout.o \
	PKTPowerPoint.o \
	PKTPowerPointHandler.o \
	PKTReceiveOK.o \
	PKTResult.o \
	PKTUserInfo.o \
	PKTSError.o

#BINOBJS = main.o StringParser.o

SRCS = $(OBJS:.o=.cpp)



.cpp.o :
	$(CC) $(CCFLAGS) $(DEFS) $(INCLUDES) $<

#.cpp.o :
#	$(CC) $(CCFLAGS) $(DEFS)  $(INCLUDES) -o $*.o $<

#----------------------------------------------------------------------
# actions
#----------------------------------------------------------------------
all : $(TARGET)

#bin : $(BINTARGET)

# < for Library >
$(TARGET) : $(OBJS)
	ar r $@ $(OBJS)

#$(BINTARGET) : $(BINOBJS) $(OBJS) $(LIBDIR)/libServerCore.a $(LIBDIR)/libCore.a
#	$(CC) -o $@ $(BINOBJS) $(OBJS) $(LIBS) $(SYSLIBS) -rdynamic

# < for Test >
#$(TARGET) : $(OBJS) $(LIBDIR)/libGameServerDatabase.a $(LIBDIR)/libServerCore.a $(LIBDIR)/libCore.a 
#	$(CC) -o $@ $(OBJS) $(LIBS) $(SYSLIBS)


clean :
	rm -rf *.o

cleanall :
	rm -rf *.o $(TARGET)

depall :
	gccmakedep $(SRCS) $(INCLUDES)

dep :
	gccmakedep $(SRCS) $(INCLUDES)

# DO NOT DELETE
