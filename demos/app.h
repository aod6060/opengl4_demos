#pragma once




class IApp {
public:

	virtual void init() = 0;
	virtual void update(float delta) = 0;
	virtual void render() = 0;
	virtual void release() = 0;
};


void app_init(std::string cap, Uint32 width, Uint32 height, IApp* app = 0);

void app_update();

void app_release();

Uint32 app_getWidth();

Uint32 app_getHeight();

void app_exit();

void app_warpMouse();

