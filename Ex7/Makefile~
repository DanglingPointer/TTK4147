#XENO_DESTDIR:=
XENO_CONFIG:=$(XENO_DESTDIR)/usr/bin/xeno-config
XENO_CFLAGS:=$(shell DESTDIR=$(XENO_DESTDIR) $(XENO_CONFIG) --skin native --cflags)
XENO_LIBS:=$(shell DESTDIR=$(XENO_DESTDIR) $(XENO_CONFIG) --skin native --ldflags)

# program executable name
TARGET = main

# compiler flags
CPPFLAGS = -g -Wall $(XENO_CFLAGS)

# linker flags
LDFLAGS = $(XENO_LIBS) -g -lcomedi -lrt -pthread -lnative -lm 

# list of sources
SOURCES = $(shell find -name "*.c")

# default rule, to compile everything
all: $(TARGET)

# define object files
OBJECTS = $(SOURCES:.c=.o)

# link programs
$(TARGET): $(OBJECTS)
	gcc  -o $@ $^ $(LDFLAGS)

# compile
%.o : %.c
	gcc  -c -o $@ $< $(CPPFLAGS)

# cleaning
clean:
	rm -f $(TARGET) $(OBJECTS)

