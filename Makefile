OBJECTS := main.o
CPPFLAGS := -g -c -std=c++17
LFLAGS := -lSDL2

.PHONY: all clean

all: orbs
	./orbs

orbs: $(OBJECTS)
	g++ -o $@ $(OBJECTS) $(LFLAGS)

main.o: main.cpp orbs.h
	g++ $(CPPFLAGS) $<

clean:
	rm $(OBJECTS)
