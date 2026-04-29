dependdir := $(shell ls .depend >/dev/null 2>&1)
ifneq ($(dependdir), .depend)
    dependdir := $(shell touch .depend)
endif

INSTALL_PATH=/home/gdmj_share/
CC=g++
GCC=gcc
COMP_PARA=-g -Wall -c -D__MULITI_THREAD__ -D__DEBUG__ -DNDEBUG -D_LINUX -D_mt
LINK_PARA=-mt #-pg

INCS=-I/usr/local/include -I/usr/include/  -I../interface/

LIBS=-L/usr/local/lib   -lpthread -lz
CSRCS   = $(wildcard *.c) 
CPPSRCS = $(wildcard *.cpp) 
COBJS   = $(patsubst %.c, %.o, $(CSRCS))
CPPOBJS = $(patsubst %.cpp, %.o, $(CPPSRCS))
OBJS = $(CPPOBJS) $(COBJS)

TARGET=../libs/libby_ctrl.a

all : $(TARGET)

$(TARGET): $(OBJS)
	ar -r  $(TARGET) *.o	

.cpp.o :
	${CC} ${COMP_PARA} $< ${INCS}

.c.o :
	${GCC} ${COMP_PARA} $< ${INCS}

clean :
	@-rm -f *.o *.so.* *.so *.a $(TARGET)
	@-rm -f ../libs/*.a

depend:
	gcc -E -c $(CFLAGS) $(INCS) -MM *.c >.depend
	g++ -E -c $(CPPFLAGS) $(INCS) -MM *.cpp >>.depend

include .depend
