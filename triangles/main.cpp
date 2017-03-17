#include <sys.h>

class ExampleApp : public IApp {
public:

	virtual void init() {

	}

	virtual void update(float delta) {

	}

	virtual void render() {

	}

	virtual void release() {

	}
};

int main(int argc, char** argv) {

	ExampleApp app;

	app_init("Example Application", 800, 600, &app);

	app_update();

	app_release();

	return 0;
}