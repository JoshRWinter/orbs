CPPFLAGS := -Wall -g -c -std=c++17
LFLAGS := -lSDL2 -lGL
OBJECTS := main.o Orbs.o shaders.o

.PHONY: all clean

all: orbs
	./orbs

orbs: $(OBJECTS)
	g++ -o $@ $(OBJECTS) $(LFLAGS)

main.o: main.cpp Orbs.h
	g++ $(CPPFLAGS) $<

Orbs.o: Orbs.cpp Orbs.h
	g++ $(CPPFLAGS) $<

shaders.o: shaders.cpp
	g++ $(CPPFLAGS) $<

clean:
	rm $(OBJECTS)
