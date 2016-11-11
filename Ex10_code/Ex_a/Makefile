# program executable name
TARGET = ex_a

# compiler flags
CFLAGS = -g -Wall

# linker flags
LDFLAGS = -g

# list of sources
SOURCES = $(shell find -name "*.c")

#Compiler
COMPILER = /home/student/Desktop/TTK4147/Ex10/buildroot-avr32-v3.0.0_TTK4147/output/staging/usr/bin/avr32-linux-gcc

# default rule, to compile everything
all: $(TARGET)

# define object files
OBJECTS = $(SOURCES:.c=.o)

# link programs
$(TARGET): $(OBJECTS)
	$(COMPILER) $(LDFLAGS) -o $@ $^ 

# compile
%.o : %.c
	$(COMPILER) $(CFLAGS) -c -o $@ $<
# cleaning
clean:
	rm -f $(TARGET) $(OBJECTS)
