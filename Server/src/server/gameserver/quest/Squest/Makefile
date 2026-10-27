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
#VSHOME   = ../../../../..
STLDIR = 
BINDIR   = $(VSHOME)/bin
LIBDIR   = $(VSHOME)/lib
LOGDIR   = $(VSHOME)/log
SRCDIR   = ../../../..
COREDIR  = ../../../../Core
SERVDIR  = ../../..
DBDIR    = ../../../database
MYSQLDIR = /usr/local/lib/mysql
GSDIR    = ../..
SKILLDIR = ../../skill
ITEMDIR  = ../../item
QUESTDIR  = ..


#----------------------------------------------------------------------
# General Rules
#----------------------------------------------------------------------
.SUFFIXES    : .cpp .o

CC       = g++
DEFS     = -D__LINUX__ -D_REENTRANT -D__GAME_SERVER__ 
#-D__QUEST_TEST__
INCLUDES =  -I$(COREDIR) -I$(SERVDIR) -I$(DBDIR) -I$(GSDIR) -I$(SKILLDIR) -I$(ITEMDIR) -I$(QUESTDIR) -I.
CCFLAGS  = -Wall -c -g -std=c++98 -std=c++11 -U_GLIBCXX_ASSERTIONS -D_GLIBCXX_NO_ASSERTIONS -fno-stack-protector -fno-enforce-eh-specs
#CCFLAGS  = -Wall -c

#LIBS     = -L$(LIBDIR) -lGameServerDatabase -lServerCore -lGameServerPackets -lCore
#SYSLIBS  = -L$(MYSQLDIR) -static -lpthread  -lmysqlclient -lssl -lcrypto -lzstd -lssl -lcrypto -lz


.cpp.o :
	$(CC) $(CCFLAGS) $(DEFS) $(INCLUDES) $<

#----------------------------------------------------------------------
# Objects
#----------------------------------------------------------------------

SIMPLE_QUEST_OBJS = MonsterKillQuest.o  \
					QuestFactoryManager.o \
					Quest.o \
					QuestManager.o \
					QuestBoard.o  \
					QuestPrice.o \
					QuestPriceReward.o \
					QuestPricePenalty.o \
					SimpleQuest.o \
					SimpleQuestLoader.o \
					QuestEvent.o \
					MonsterSelector.o

#							\
					main.o \
					FakeMonsterInfo.o
					              
OBJS = $(SIMPLE_QUEST_OBJS)


SRCS = $(OBJS:.o=.cpp)

TARGET   = $(LIBDIR)/libSimpleQuest.a
#TARGET = qt


#----------------------------------------------------------------------
# actions
#----------------------------------------------------------------------
all : $(TARGET)

$(TARGET) : $(OBJS)
	ar rc $@ $^

#$(TARGET) : $(OBJS) $(LIBDIR)/libGameServerDatabase.a $(LIBDIR)/libCore.a $(LIBDIR)/libServerCore.a $(LIBDIR)/libGameServerPackets.a
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
