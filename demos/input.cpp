#include "sys.h"

enum InputState {
	IS_RELEASE = 0,
	IS_ONCE,
	IS_PRESSED
};

struct InputData {
	// Keyboard
	InputState keys[SDL_NUM_SCANCODES];
	// Mouse
	InputState mouseButtons[MB_NUM_BUTTONS];
	int x;
	int y;
	// Events
	void onKeyboardEvent(const SDL_KeyboardEvent& e);
	void onMouseButtonEvent(const SDL_MouseButtonEvent& e);
	void onMouseMotionEvent(const SDL_MouseMotionEvent& e);
};

static InputData data;

void input_init() {
	for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
		data.keys[i] = IS_RELEASE;
	}

	for (int i = 0; i < MB_NUM_BUTTONS; i++) {
		data.mouseButtons[i] = IS_RELEASE;
	}
}

void input_update() {
	for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
		if (data.keys[i] == IS_ONCE) {
			data.keys[i] = IS_PRESSED;
		}
	}

	for (int i = 0; i < MB_NUM_BUTTONS; i++) {
		if (data.mouseButtons[i] == IS_ONCE) {
			data.mouseButtons[i] = IS_PRESSED;
		}
	}

	if (input_isMouseGrab()) {
		data.x = 0;
		data.y = 0;
	}
}

void input_doEvent(const SDL_Event& e) {
	switch (e.type) {
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		data.onKeyboardEvent(e.key);
		break;
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		data.onMouseButtonEvent(e.button);
		break;
	case SDL_MOUSEMOTION:
		data.onMouseMotionEvent(e.motion);
		break;
	}
}

// Keyboard
bool input_isKeyHit(SDL_Scancode code) {
	return data.keys[code] == IS_ONCE;
}

bool input_isKeyPress(SDL_Scancode code) {
	return data.keys[code] == IS_ONCE || data.keys[code] == IS_PRESSED;
}

// Mouse
bool input_isMouseButtonHit(MouseButtons button) {
	return data.mouseButtons[button] == IS_ONCE;
}

bool input_isMouseButtonPress(MouseButtons button) {
	return data.mouseButtons[button] == IS_ONCE || data.mouseButtons[button] == IS_PRESSED;
}

void input_getMouseCoords(glm::vec2& mc) {
	mc.x = data.x;
	mc.y = data.y;
}

void input_setMouseGrab(bool b) {
	if (b) {
		app_warpMouse();
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
	else {
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
}

bool input_isMouseGrab() {
	return (SDL_GetRelativeMouseMode() == SDL_TRUE) ? true : false;
}

void input_toggleMouseGrab() {
	input_setMouseGrab(!input_isMouseGrab());
}


void InputData::onKeyboardEvent(const SDL_KeyboardEvent &e) {
	SDL_Scancode code = e.keysym.scancode;
	bool isPressed = (e.state == SDL_PRESSED) ? true : false;
	if (isPressed) {
		if (data.keys[code] == IS_RELEASE) {
			data.keys[code] = IS_ONCE;
		}
	}
	else {
		if (data.keys[code] != IS_RELEASE) {
			data.keys[code] = IS_RELEASE;
		}
	}
}

void InputData::onMouseButtonEvent(const SDL_MouseButtonEvent& e) {
	Uint8 button = e.button - 1;
	bool isPressed = (e.state == SDL_PRESSED) ? true : false;
	if (isPressed) {
		if (data.mouseButtons[button] == IS_RELEASE) {
			data.mouseButtons[button] = IS_ONCE;
		}
	}
	else {
		if (data.mouseButtons[button] != IS_RELEASE) {
			data.mouseButtons[button] = IS_RELEASE;
		}
	}
}

void InputData::onMouseMotionEvent(const SDL_MouseMotionEvent& e) {
	if (input_isMouseGrab()) {
		data.x = e.xrel;
		data.y = e.yrel;
	}
	else {
		data.x = e.x;
		data.y = e.y;
	}
}