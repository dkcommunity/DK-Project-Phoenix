#----------------------------------------------------------------------
# Directories Setting
#----------------------------------------------------------------------
VSHOME   = ../../../..
STLDIR = 
BINDIR   = $(VSHOME)/bin
LIBDIR   = $(VSHOME)/lib
LOGDIR   = $(VSHOME)/log
SRCDIR   = ../../..
COREDIR  = ../../../Core
SERVDIR  = ../..
DBDIR    = ../../database
GSDIR    = ..

#----------------------------------------------------------------------
# General Rules
#----------------------------------------------------------------------
.SUFFIXES    : .cpp .o

CC       = g++
DEFS     = -D__LINUX__ -D_REENTRANT
INCLUDES = -I/usr/include/lua5.1 -I$(COREDIR) -I$(SERVDIR) -I$(GSDIR) -I$(DBDIR)
CCFLAGS  = -Wall -c -g -std=c++98 -std=c++11 -U_GLIBCXX_ASSERTIONS -D_GLIBCXX_NO_ASSERTIONS -fno-stack-protector -fno-enforce-eh-specs
#CCFLAGS  = -Wall -c
#LIBS     = -L$(LIBDIR) -lItems -lQuest -lGameServerDatabase -lServerCore -lGameServerPackets -lSkill -lCore -lGameServerBilling -lSimpleQuest
#SYSLIBS  = -llua -llualib

.cpp.o :
	$(CC) $(CCFLAGS) $(DEFS) $(INCLUDES) $<

#main.o 
OBJS = lua_tinker.o \
		DEScript.o \
		DEScriptZone.o \
		DEScriptEffect.o \
		DEScriptSkill.o

SRCS = $(OBJS:.o=.cpp)

#TARGET = lt
TARGET = $(LIBDIR)/libDEScript.a

all : $(TARGET)
	$(MAKE)	$(TARGET)

$(TARGET) : $(OBJS)
#	$(CC) -o $@ $(OBJS) $(SYSLIBS)
	ar r $@ $(OBJS)

clean :
	rm -f *.o

cleanall :
	rm -f *.o $(TARGET)

depall :
	gccmakedep $(SRCS) $(INCLUDES)

dep :
	gccmakedep $(SRCS) $(INCLUDES)


# DO NOT DELETE
