#ifndef ORBS_H
#define ORBS_H

#include <SDL2/SDL.h>
#include <GL/gl.h>

#include "press.h"

class Orbs
{
public:
	Orbs(int, int, int);
	void step();
	void render() const;
	void stop();

private:
	static void *getproc(const char*);
	static void init_extensions();

	unsigned vao, vbo, program;
};

#endif // ORBS_H
