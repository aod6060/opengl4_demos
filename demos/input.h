#pragma once

enum MouseButtons {
	MB_LEFT = 0,
	MB_CENTER,
	MB_RIGHT,
	MB_NUM_BUTTONS
};


void input_init();

void input_update();

void input_doEvent(const SDL_Event& e);

// Keyboard
bool input_isKeyHit(SDL_Scancode code);

bool input_isKeyPress(SDL_Scancode code);

// Mouse
bool input_isMouseButtonHit(MouseButtons button);

bool input_isMouseButtonPress(MouseButtons button);

void input_getMouseCoords(glm::vec2& mc);

void input_setMouseGrab(bool b);

bool input_isMouseGrab();

void input_toggleMouseGrab();
