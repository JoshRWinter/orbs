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
	void load_texture();
	int get_uniform(const char*) const;

	static void *getproc(const char*);
	static void init_extensions();
	static void initortho(float*, float, float, float, float, float, float);

	unsigned vao, vbo, ebo, program, texture;
	struct { int projection; } uniform;
};

#endif // ORBS_H
