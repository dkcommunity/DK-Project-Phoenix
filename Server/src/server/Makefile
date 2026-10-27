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
VSHOME  = ../..
STLDIR = 
BINDIR  = $(VSHOME)/bin
LIBDIR  = $(VSHOME)/lib
LOGDIR  = $(VSHOME)/log
SRCDIR  = ..
COREDIR = ../Core
DBDIR    = ./database


#----------------------------------------------------------------------
# General Rules
#----------------------------------------------------------------------
.SUFFIXES    : .cpp .o

CC       = g++
DEFS     = -D__LINUX__ -D_REENTRANT -D__DEBUG__
INCLUDES =  -I$(COREDIR) -I$(DBDIR) -I.
CCFLAGS  = -Wall -c -g -std=c++98 -std=c++11 -U_GLIBCXX_ASSERTIONS -D_GLIBCXX_NO_ASSERTIONS -fno-stack-protector -fno-enforce-eh-specs
#CCFLAGS  = -Wall -c

.cpp.o : 
	$(CC) $(CCFLAGS) $(DEFS) $(INCLUDES) $<


#----------------------------------------------------------------------
# Server Core Library
#----------------------------------------------------------------------
SERVER_CORE_OBJS = CondVar.o LogData.o LogClient.o Mutex.o PlayerManager.o GameWorldInfoManager.o SystemAPI.o Thread.o ThreadAttr.o Timeval.o pthreadAPI.o Profile.o PacketProfile.o GameServerInfoManager.o PaySystem.o DBExecuteManager.o
SERVER_CORE_LIB  = $(LIBDIR)/libServerCore.a

SERVER_CORE_SRCS = $(SERVER_CORE_OBJS:.o=.cpp)

#----------------------------------------------------------------------
# actions
#----------------------------------------------------------------------
all : $(SERVER_CORE_LIB)
	$(MAKE) -C database
	$(MAKE) -C chinabilling
	$(MAKE) -C loginserver
	$(MAKE) -C gameserver
#	$(MAKE) -C updateserver
	$(MAKE) -C sharedserver

server : $(SERVER_CORE_LIB)

allgameserver : $(SERVER_CORE_LIB)
	$(MAKE) -C database
	$(MAKE) -C chinabilling
	$(MAKE) -C gameserver

allloginserver : $(SERVER_CORE_LIB)
	$(MAKE) -C database
	$(MAKE) -C chinabilling
	$(MAKE) -C loginserver

allsharedserver : $(SERVER_CORE_LIB)
	$(MAKE) -C database
	$(MAKE) -C sharedserver

alltheoneserver : $(SERVER_CORE_LIB)
	$(MAKE) -C theoneserver

$(SERVER_CORE_LIB) : $(SERVER_CORE_OBJS)
	ar rc $@ $^

clean :
	$(MAKE) -C database clean
	$(MAKE) -C chinabilling clean
	$(MAKE) -C loginserver clean
	$(MAKE) -C gameserver clean
	$(MAKE) -C updateserver clean
	$(MAKE) -C sharedserver clean
	rm -rf *.o

cleanloginserver :
	$(MAKE) -C database clean
	$(MAKE) -C chinabilling clean
	$(MAKE) -C loginserver clean
	rm -rf *.o

cleangameserver :
	$(MAKE) -C database clean
	$(MAKE) -C chinabilling clean
	$(MAKE) -C gameserver clean
	rm -rf *.o

cleansharedserver :
	$(MAKE) -C database clean
	$(MAKE) -C sharedserver clean
	rm -rf *.o

cleanall :
	$(MAKE) -C database cleanall
	$(MAKE) -C chinabilling cleanall
	$(MAKE) -C loginserver cleanall
	$(MAKE) -C gameserver cleanall
	$(MAKE) -C updateserver cleanall
	$(MAKE) -C sharedserver cleanall
	rm -rf *.o $(SERVER_CORE_LIB)

depall :
	$(MAKE) -C database depall
	$(MAKE) -C chinabilling depall
	$(MAKE) -C loginserver depall
	$(MAKE) -C gameserver depall
	$(MAKE) -C updateserver depall
	$(MAKE) -C sharedserver depall
	gccmakedep $(SERVER_CORE_SRCS) $(INCLUDES)

dep :
	gccmakedep $(SERVER_CORE_SRCS) $(INCLUDES)
# DO NOT DELETE
