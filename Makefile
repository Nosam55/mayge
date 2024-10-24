CXX=g++
CXXFLAGS=-w -Wall -Wextra
LDFLAGS=-lSDL2 -lSDL2_image
OBJS=common.o image.o actor.o window.o app.o main.o
DOBJS=$(addprefix obj/debug/, $(OBJS))
ROBJS=$(addprefix obj/release/, $(OBJS))

.PHONY: clean remake all

release: CXXFLAGS+= -O2
release: $(ROBJS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

debug: CXXFLAGS += -DDEBUG -g
debug: $(DOBJS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

obj/release/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
obj/debug/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

remake: clean all

all: release debug

clean:
	[ -f debug ] && rm debug || :;
	[ -f release ] && rm release || :;
	[ -d obj/ ] && rm -r obj/debug/* && rm -r obj/release/* || :;