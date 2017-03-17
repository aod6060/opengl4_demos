#include <sys.h>

class CameraApp : public IApp {
public:

	float maxTime = 0.0f;



	virtual void init() {

	}

	virtual void update(float delta) {

		if (input_isKeyHit(SDL_SCANCODE_TAB)) {
			input_toggleMouseGrab();
		}


	}

	virtual void render() {

	}

	virtual void release() {

	}

};
int main(int argc, char** argv) {


	CameraApp app;

	app_init("Camera App", 800, 600, &app);

	app_update();

	app_release();

	return 0;
}