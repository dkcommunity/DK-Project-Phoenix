#--------------------------------------------------------------------------------
#
# Filename    : Makefile
# Written By  : Reiot
#
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
# Directories
#--------------------------------------------------------------------------------
VSHOME  = ../../..
STLDIR = 
BINDIR  = $(VSHOME)/bin
LIBDIR  = $(VSHOME)/lib
SRCDIR  = $(VSHOME)/src
COREDIR = $(SRCDIR)/Core
SERVDIR = $(SRCDIR)/server
TOSDIR  = $(SERVDIR)/theoneserver
GSDIR   = $(SERVDIR)/gameserver
DBDIR	= $(SERVDIR)/database
LSDIR   = $(SERVDIR)/loginserver


#--------------------------------------------------------------------------------
# General Rules
#--------------------------------------------------------------------------------
.SUFFIXES : .cpp .tos.o .gs.o

CC       = g++
DEFS     = -D__LINUX__ -D_REENTRANT -D__DEBUG__
INCLUDES =  -I$(COREDIR) -I$(GSDIR) -I$(TOSDIR) -I$(DBDIR) -I$(LSDIR) -I$(SERVDIR)
#INCLUDES =  -I$(COREDIR) -I$(SERVDIR) -I$(TOSDIR)
CCFLAGS  = -Wall -c -g -std=c++98 -std=c++11 -U_GLIBCXX_ASSERTIONS -D_GLIBCXX_NO_ASSERTIONS -fno-stack-protector -fno-enforce-eh-specs
#CCFLAGS  = -Wall -c

TARGET1 = $(LIBDIR)/libTheOneServerPackets.a
TARGET2 = $(LIBDIR)/libGameServerPackets.a

OBJS1	= \
		GTOAcknowledgement.tos.o GTOAcknowledgementHandler.tos.o

OBJS2	= \
		GTOAcknowledgement.gs.o GTOAcknowledgementHandler.gs.o

SRCS	= $(OBJS1:.tos.o=.cpp)

.cpp.tos.o : 
	$(CC) $(CCFLAGS) $(DEFS) -D__THE_ONE_SERVER__ $(INCLUDES) -I$(TOSDIR) -I$(SERVDIR) -I$(DBDIR) -o $*.tos.o $<

.cpp.gs.o : 
	$(CC) $(CCFLAGS) $(DEFS) -D__GAME_SERVER__ $(INCLUDES) -I$(GSDIR) -I$(SERVDIR) -I$(DBDIR) -o $*.gs.o $<



#--------------------------------------------------------------------------------
# Actions
#--------------------------------------------------------------------------------

all : $(TARGET1) $(TARGET2)
#all : $(TARGET1)

alltheoneserver : $(TARGET1)

allgameserver : $(TARGET2)

$(TARGET1) : $(OBJS1)
	ar rc $@ $(OBJS1)

$(TARGET2) : $(OBJS2)
	ar r $@ $(OBJS2)

clean :
	rm -rf *.o

cleantheoneserver :
	rm -rf *.tos.o

cleangameserver :
	rm -rf *.gs.o

cleanall :
	rm -rf *.o $(TARGET1) $(TARGET2)

depall :
	gccmakedep  $(SRCS) $(INCLUDES) $(DEFS)

dep :
	gccmakedep  $(SRCS) $(INCLUDES) $(DEFS)

# DO NOT DELETE
