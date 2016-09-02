### Linux general
- [GNU Make in detail for beginners](http://opensourceforu.com/2012/06/gnu-make-in-detail-for-beginners/)
- [GNU Make reference](http://www.gnu.org/software/make/manual/make.html)
- [Gvim as C++ IDE](http://stackoverflow.com/a/24156/4432988)
- [Ubuntu terminal](https://help.ubuntu.com/community/UsingTheTerminal)
- [Linux-shit](https://www.quora.com/I-am-shifting-from-Windows-to-Linux-What-are-some-explanations-for-Linux-as-I-am-new-to-it/answer/Paul-Reiber?srid=uhnDj)

### Git
- [Git undo](https://github.com/blog/2019-how-to-undo-almost-anything-with-git)
- [Git update wrt gitignore](http://stackoverflow.com/a/19095988)
- [Git save username+password](http://stackoverflow.com/a/17979600)


### C++11
- [R-value references](http://thbecker.net/articles/rvalue_references/section_01.html)
- [std thread library](http://en.cppreference.com/w/cpp/thread)
- [Pointers to funcs and members](http://en.cppreference.com/w/cpp/language/pointer)

### Stolen C++ makefile example:

```makefile
TARGET=elevator
CC = gcc
COMMONFLAGS += -Wall -Wextra -pedantic -MD -MP -g
CPPFLAGS = $(COMMONFLAGS) -std=c++14
CFLAGS = $(COMMONFLAGS) -std=gnu99
LDFLAGS = -lstdc++ -lcomedi -lpthread -lm
MODULES = network driver driver/hw_interface dispatch_logic util
CPPSRC = main.cpp $(foreach m, $(MODULES), $(wildcard $(m)/*.cpp))
CSRC = $(foreach m, $(MODULES), $(wildcard $(m)/*.c))
OBJ = $(CPPSRC:%.cpp=%.o) $(CSRC:%.c=%.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(CPPSRC:%.cpp=%.d) $(CSRC:%.c=%.d)

clean:
	rm -f $(OBJ) $(CPPSRC:%.cpp=%.d) $(CSRC:%.c=%.d)

.PHONY: all clean
```


###Ex1

- [real, user, sys time](http://stackoverflow.com/a/556411)
- [Update to g++-4.9 version](http://superuser.com/a/796621)
- [Unlink symbol](http://stackoverflow.com/a/210130)
```
g++ --version
g++-4.9 -std=gnu++14 -lrt -pthread AssD.cpp AssD
g++-4.9 --version
```
```cmake
set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.2")
```
