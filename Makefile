CPPFLAGS := -Wall -Wno-unused-variable -g -c -std=c++17
LFLAGS := -lSDL2 -lGL
OBJECTS := main.o Orbs.o shaders.o

.PHONY: all clean release

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

release: clean
	g++ -o orbs -std=c++17 -O2 -Wall -Wno-unused-variable *.cpp -lSDL2 -lGL -s -flto

clean:
	rm -f $(OBJECTS)
