#include "sys.h"


struct AppData {
	// Basic Information
	Uint32 width;
	Uint32 height;
	bool isRunning = true;
	IApp* app;
	// SDL Related Stuff
	SDL_Window* window;
	SDL_GLContext context;

};

static AppData data;

void app_init(std::string cap, Uint32 width, Uint32 height, IApp* app) {
	data.width = width;
	data.height = height;
	data.app = app;

	SDL_Init(SDL_INIT_EVERYTHING);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	data.window = SDL_CreateWindow(
		cap.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		data.width,
		data.height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
	);

	data.context = SDL_GL_CreateContext(data.window);

	glewInit();


	// Other Initis that I might want for an example

	if (data.app != nullptr) {
		data.app->init();
	}
}

void app_update() {

	SDL_Event e;

	Uint32 preTime = SDL_GetTicks();
	Uint32 currTime = 0;
	float deltaTime = 0.0f;

	while (data.isRunning) {

		currTime = SDL_GetTicks();
		deltaTime = (currTime - preTime) / 1000.0f;
		preTime = currTime;

		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				app_exit();
			}
		}



		if (data.app != nullptr) {
			data.app->update(deltaTime);
			data.app->render();
		}

		SDL_GL_SwapWindow(data.window);
	}
}

void app_release() {

	if (data.app != nullptr) {
		data.app->release();
		data.app = nullptr;
	}

	SDL_GL_DeleteContext(data.context);

	SDL_DestroyWindow(data.window);

	SDL_Quit();
}

Uint32 app_getWidth() {
	return data.width;
}

Uint32 app_getHeight() {
	return data.height;
}

void app_exit() {
	data.isRunning = false;
}
