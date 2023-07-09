BUILDROOT_HOME=$(HOME)/buildroot

CC=$(BUILDROOT_HOME)/output/host/usr/bin/arm-linux-gcc
CFLAGS=--sysroot=$(BUILDROOT_HOME)/output/staging
#INCLUDES=$(BUILDROOT_HOME)/output/staging/usr/include
INCLUDES=
LFLAGS=

#LIBS:=$(curl-config --libs)
LIBS:=-lcurl -uClibc -lc
SRC=./src/hw.c
OBJ=$(SRC:.c=.o)
STATIC_MAIN=static-main
DYNAMIC_MAIN=dynamic-main
MAIN=main

RM=rm -rf


.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


$(MAIN) : $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJ) $(LFLAGS) $(LIBS)


build:
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJ) $(LFLAGS) $(LIBS)


all: $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJ) $(LFLAGS) $(LIBS)
	$(CC) $(CFLAGS) -static $(INCLUDES) -o $(STATIC_MAIN) $(OBJ) $(LFLAGS) $(LIBS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(DYNAMIC_MAIN) $(OBJ) $(LFLAGS) $(LIBS)


clean:
	$(RM) $(DYNAMIC_MAIN) $(STATIC_MAIN) *.o *~
