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
VSHOME  = ../../..
STLDIR =
BINDIR  = $(VSHOME)/bin
LIBDIR  = $(VSHOME)/lib
LOGDIR  = $(VSHOME)/log
DBINCDIR  = /usr/local/mysql/include/mysql/
SRCDIR  = ../..
COREDIR = ../../Core
SERVDIR = ..
DBDIR   = ../database
MYSQLDIR = /usr/local/mysql/lib/mysql
PYTHONDIR = /usr/local/include/python2.2

#----------------------------------------------------------------------
# General Rules
#----------------------------------------------------------------------
.DEFAULT_GOAL := all

.SUFFIXES    : .cpp .o

CC       = g++
DEFS     = -D__LINUX__ -D_REENTRANT -D__SHARED_SERVER__ -D__DEBUG__
INCLUDES =  -I$(COREDIR) -I$(SERVDIR) -I$(DBDIR) -I$(DBINCDIR) -I$(PYTHONDIR) -I.
CCFLAGS  = -Wall -c -O2 -std=c++98 -std=c++11
#CCFLAGS  = -Wall -c
LIBS     = -L$(LIBDIR) -lDatabase -lSharedServerPackets -lGameServerPackets -lServerCore -lCore
SYSLIBS  = -L$(MYSQLDIR) -lmysqlclient -lssl -lcrypto -lzstd -lssl -lcrypto -lpthread  -lz
#SYSLIBS  = -L$(MYSQLDIR) -lmysqlclient -lssl -lcrypto -lzstd -lssl -lcrypto -lpthread  -lz -ldl -lutil -lpython2.2 -rdynamic

.cpp.o :
	$(CC) $(CCFLAGS) $(DEFS) $(INCLUDES) $<


#----------------------------------------------------------------------
# Objects
#----------------------------------------------------------------------
OBJS = \
	Guild.o GuildManager.o ResurrectLocationManager.o GameServerPlayer.o \
	HeartbeatManager.o GameServerInfoManager.o \
	GameServerGroupInfoManager.o GameServerManager.o \
	SharedServer.o main.o StringPool.o \
	FriendManager.o

SRCS = $(OBJS:.o=.cpp)

TARGET = $(BINDIR)/sharedserver
PACKET_CONTRACT_HEADER = $(COREDIR)/Packet.h
GameServerPlayer.o: $(PACKET_CONTRACT_HEADER)


#----------------------------------------------------------------------
# actions
#----------------------------------------------------------------------
all : $(TARGET)

$(TARGET) : $(OBJS) $(LIBDIR)/libDatabase.a $(LIBDIR)/libGameServerPackets.a $(LIBDIR)/libSharedServerPackets.a $(LIBDIR)/libServerCore.a $(LIBDIR)/libCore.a
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
