#ifndef ORBS_H
#define ORBS_H

#include <random>
#include <vector>

#include <time.h>

#ifdef _WIN32
#include <SDL.h>
#define NOMINMAX
#include <windows.h>
#else
#include <SDL2/SDL.h>
#endif // _WIN32
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
	static constexpr float SIZE = 1.5f;
	static inline int COUNT = 20;

	Orb()
		: x(-SIZE / 2)
		, y(-SIZE / 2)
		, xv(random(-5.0f, 5.0f) / 100.0f)
		, yv(random(-5.0f, 5.0f) / 100.0f)
		, rot(random(0.0f, PI * 2.0f))
		, rotv(random(-0.07f, 0.07f))
	{}

	void checkcollision(Orb &other)
	{
		float xdist=x-other.x;
		float ydist=y-other.y;
		float distsquared=(xdist*xdist)+(ydist*ydist);
		if(!(distsquared<=SIZE*SIZE))
			return;
		double xVelocity = other.xv - xv;
		double yVelocity = other.yv - yv;
		double dotProduct = xdist*xVelocity + ydist*yVelocity;
		//Neat vector maths, used for checking if the objects moves towards one another.
		if(dotProduct > 0){
			float collisionScale = dotProduct / distsquared;
			float xCollision = xdist * collisionScale;
			float yCollision = ydist * collisionScale;
			//The Collision vector is the speed difference projected on the Dist vector,
			//thus it is the component of the speed difference needed for the collision.
			const float MASS = 1.0f;
			float combinedMass = MASS + MASS;
			float collisionWeightA = 2.0f * MASS / combinedMass;
			float collisionWeightB = 2.0f * MASS / combinedMass;
			xv += collisionWeightA * xCollision;
			yv += collisionWeightA * yCollision;
			other.xv -= collisionWeightB * xCollision;
			other.yv -= collisionWeightB * yCollision;
		}
	}

	float x, y, xv, yv, rot, rotv;

	static inline std::unique_ptr<float[]> attributes;
};

class Orbs
{
public:
	Orbs(int, int, int);
	void step();
	void render() const;
	void stop();
	void add();
	void remove();

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
