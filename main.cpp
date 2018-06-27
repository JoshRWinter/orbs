#include <stdexcept>
#include <chrono>

#include "Orbs.h"

static void go();

#undef main
#ifdef _WIN32
int WinMain(HINSTANCE, HINSTANCE, LPSTR args, int)
#else
int main(int argc, char *argv[])
#endif // _WIN32
{
#ifdef _WIN32
	const std::string cmd = args;
	if(cmd.find("/p") != std::string::npos)
		return 1;
#endif // _WIN32
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

	int width, height;
	SDL_GetWindowSize(window, &width, &height);

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
	Orbs orbs(width, height, 10);

	// event loop
	bool quit = false;
	auto start = std::chrono::high_resolution_clock::now();
	int mouse_move_count = 0;
	while(!quit)
	{
		orbs.step();
		orbs.render();

		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_MOUSEMOTION:
					if(mouse_move_count++ < 3)
						break;
				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_KP_PLUS)
					{
						orbs.add();
						break;
					}
					else if(event.key.keysym.sym == SDLK_KP_MINUS)
					{
						orbs.remove();
						break;
					}
				case SDL_QUIT:
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
