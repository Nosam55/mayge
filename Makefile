CXX=g++
CXXFLAGS=-w -Wall -Wextra -g
LDFLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf
OBJS=common image audio actor gui window app
DOBJS=$(addsuffix .o, $(addprefix obj/debug/, $(OBJS)))
ROBJS=$(addsuffix .o, $(addprefix obj/release/, $(OBJS)))
LINCS=$(addsuffix .hpp, $(OBJS)) sdl_exception.hpp
_ARCH=$(shell $(CXX) -dumpmachine)
_SOVER=0

.PHONY: clean remake all

all: debug release

build: lib/release/libmayge.so lib/debug/libmayge.so

install: build
	mkdir -p /usr/local/include/mayge.d
	cp mayge /usr/local/include
	for file in $(LINCS); do cp "$$file" /usr/local/include/mayge.d; done;

	mkdir -p /usr/local/lib/$(_ARCH)
	for file in $(shell ls lib/release); do mv "lib/release/$$file" /usr/local/lib/$(_ARCH); done;

uninstall:
	rm -r /usr/local/lib/$(_ARCH)/libmayge.* || :;
	rm /usr/local/include/mayge || :;
	rm -r /usr/local/include/mayge.d || :;

release: CXXFLAGS+= -O2 
release: main.cpp lib/release/libmayge.so
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) -L./lib/release -lmayge -o $@

debug: CXXFLAGS+= -DDEBUG 
debug: main.cpp lib/debug/libmayge.so
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) -L./lib/debug -lmayge -o $@

run:
	LD_LIBRARY_PATH=lib/debug ./debug

lib/debug/libmayge.so: CXXFLAGS+= -fPIC
lib/debug/libmayge.so: $(DOBJS)
	$(CXX) $(CXXFLAGS) -shared -Wl,-soname,libmayge.so.0 $^ $(LDFLAGS) -o $@.$(_SOVER)
	[ -f $@.$(_SOVER) ] && ln libmayge.so.$(_SOVER) -s $@ || :;

lib/release/libmayge.so: CXXFLAGS+= -fPIC
lib/release/libmayge.so: $(ROBJS)
	$(CXX) $(CXXFLAGS) -shared -Wl,-soname,libmayge.so.0 $^ $(LDFLAGS) -o $@.$(_SOVER)
	[ -f $@.$(_SOVER) ] && ln libmayge.so.$(_SOVER) -s $@ || :;

obj/release/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $? -o $@
obj/debug/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $? -o $@

remake: clean all



clean:
	[ -f debug ] && rm debug || :;
	[ -f release ] && rm release || :;
	[ -d obj/debug/ ] && rm -r obj/debug/*
	[ -d obj/release/ ] && rm -r obj/release/* || :;
	[ -d lib/debug/ ] && rm -r lib/debug/* || :;
	[ -d lib/release/ ] && rm -r lib/release/* || :;
	