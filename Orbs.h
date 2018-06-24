#ifndef ORBS_H
#define ORBS_H

#include <random>
#include <vector>

#include <time.h>

#include <SDL2/SDL.h>
#include <GL/gl.h>

#include "press.h"

#define PI 3.1415926f

inline std::mt19937 generator;

inline float random(float low, float high)
{
	std::uniform_real_distribution<float> urd(low, high);

	return urd(generator);
}

struct Orb
{
	static constexpr float SIZE = 1.0f;
	static constexpr int COUNT = 10;

	Orb()
		: x(-SIZE / 2)
		, y(-SIZE / 2)
		, xv(random(-10.15f, 10.15f) / 100.0f)
		, yv(random(-10.15f, 10.15f) / 100.0f)
		, rot(random(0.0f, PI * 2.0f))
		, rotv(random(-0.1f, 0.1f))
	{}

	float x, y, xv, yv, rot, rotv;

	static inline float attributes[COUNT * 3];
};

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

	unsigned vao, vbo, ebo, vbo_attribute, program, texture;
	struct { int projection, size; } uniform;
	struct { float left, right, bottom, top; } world;
	std::vector<Orb> orb_list;
};

#endif // ORBS_H
