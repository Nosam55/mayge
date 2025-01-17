CXX=g++
CXXFLAGS=-w -Wall -Wextra -g
LDFLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf
OBJS=common.o image.o audio.o actor.o gui.o window.o app.o
DOBJS=$(addprefix obj/debug/, $(OBJS))
ROBJS=$(addprefix obj/release/, $(OBJS))
_ARCH=$(shell $(CXX) -dumpmachine)
_SOVER=0

.PHONY: clean remake all

all: debug release

build: lib/release/libmayge.so lib/debug/libmayge.so

install: build
	mkdir -p /usr/local/lib/$(_ARCH)
	mv lib/release/* /usr/local/lib/$(_ARCH)

uninstall:
	rm -r /usr/local/lib/$(_ARCH)/libmayge.so{,.$(_SOVER)} || :;

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
	