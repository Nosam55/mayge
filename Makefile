CXX=g++
CXXFLAGS=-w -Wall -Wextra
LDFLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf
OBJS=common.o image.o audio.o actor.o gui.o window.o app.o asteroids_app.o
DOBJS=$(addprefix obj/debug/, $(OBJS))
ROBJS=$(addprefix obj/release/, $(OBJS))

.PHONY: clean remake all

release: CXXFLAGS+= -O2
release: $(ROBJS) main.cpp
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

debug: CXXFLAGS+= -DDEBUG -g
debug: $(DOBJS) main.cpp
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

obj/release/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
obj/debug/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

lib/debug/%.so: $()
	$(CXX) $(CXXFLAGS)

remake: clean all

all: release debug


clean:
	[ -f debug ] && rm debug || :;
	[ -f release ] && rm release || :;
	[ -d obj/debug/ ] && rm -r obj/debug/*
	[ -d obj/release/ ] && rm -r obj/release/* || :;