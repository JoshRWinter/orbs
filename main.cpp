#include <stdexcept>
#include <chrono>

#include "Orbs.h"

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

	return 0;
}

void go()
{
	if(SDL_Init(SDL_INIT_VIDEO))
		throw std::runtime_error("couldn't init sdl video");

	SDL_Window *window = SDL_CreateWindow("orbs", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
	if(window == NULL)
		throw std::runtime_error("couldn't create sdl window");

	SDL_ShowCursor(0);

	// gl settings
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if(context == NULL)
		throw std::runtime_error("couldn't create sdl gl context");

	// vsync
	SDL_GL_SetSwapInterval(1);

	// application object
	Orbs orbs(800, 450, 4);

	// event loop
	bool quit = false;
	auto start = std::chrono::high_resolution_clock::now();
	while(!quit)
	{
		orbs.step();
		orbs.render();

		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
				case SDL_KEYDOWN:
				case SDL_KEYUP:
				case SDL_MOUSEMOTION:
					quit = true;
			}
		}

		SDL_GL_SwapWindow(window);

		while(std::chrono::duration<float, std::nano>(std::chrono::high_resolution_clock::now() - start).count() < 16666000);
		start = std::chrono::high_resolution_clock::now();
	}

	orbs.stop();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
