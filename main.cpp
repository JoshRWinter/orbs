#include <stdexcept>

#include "press.h"
#include "orbs.h"

static void go();

int main()
{
	try
	{
		go();
	}
	catch(const std::exception &e)
	{
		press::writeln("a fatal exception was caught:\n{}", e.what());
		return 1;
	}

	press::writeln("goodbye");
	return 0;
}

void go()
{
	if(SDL_Init(SDL_INIT_VIDEO))
		throw std::runtime_error("couldn't init sdl video");

	SDL_Window *window = SDL_CreateWindow("orbs", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
	if(window == NULL)
		throw std::runtime_error("couldn't create sdl window");

	// gl settings
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if(context == NULL)
		throw std::runtime_error("couldn't create sdl gl context");

	// vsync
	SDL_GL_SetSwapInterval(1);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
