#include <sys.h>

#define NUM_LIGHTS 4
#define ROTS_DEGREES 360 / 3

struct Light {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 position;
	float intencity;

	// Attenuation
	float constant;
	float linear;
	float quad;

	Light() {}

	Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 position, float intencity) {
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->position = position;
		this->intencity = intencity;
		constant = 0.25f;
		linear = 0.05f;
		quad = 0.002f;
	}
};

struct LightManager {
	Light lights[NUM_LIGHTS];

	float yrot1 = 0.0f;
	
	float yrot2 = 0.0f + ROTS_DEGREES;
	
	float yrot3 = 0.0f + ROTS_DEGREES * 2.0;
	
	float dist = 16.0f;

	float rotSpeed = 0.0;

	LightManager() {}

	void createLights(Program& prog) {
		lights[0] = Light(glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.9f, 0.9f, 0.9f), glm::vec3(0.0f, 5.0f, 0.0f), 1.0f);

		float yrad = glm::radians(yrot1);
		lights[1] = Light(glm::vec3(0.1f, 0.0f, 0.0f), glm::vec3(0.9f, 0.0f, 0.0f), glm::vec3(glm::cos(yrad), 1.0f, glm::sin(yrad)) * dist, 1.0f);

		yrad = glm::radians(yrot2);
		lights[2] = Light(glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.0f, 0.9f, 0.0f), glm::vec3(glm::cos(yrad), 1.0f, glm::sin(yrad)) * dist, 1.0f);

		yrad = glm::radians(yrot3);
		lights[3] = Light(glm::vec3(0.0f, 0.0f, 0.1f), glm::vec3(0.0f, 0.0f, 0.9f), glm::vec3(glm::cos(yrad), 1.0f, glm::sin(yrad)) * dist, 1.0f);

		for (int i = 0; i < NUM_LIGHTS; i++) {
			std::stringstream ss;
			ss << "lights[" << i << "].ambient";
			prog.create_uniform(ss.str());
			prog.set3f(ss.str(), lights[i].ambient);
			std::cout << ss.str() << std::endl;
			ss.str("");
			ss << "lights[" << i << "].diffuse";
			prog.create_uniform(ss.str());
			prog.set3f(ss.str(), lights[i].diffuse);
			std::cout << ss.str() << std::endl;
			ss.str("");
			ss << "lights[" << i << "].position";
			prog.create_uniform(ss.str());
			prog.set3f(ss.str(), lights[i].position);
			std::cout << ss.str() << std::endl;
			ss.str("");
			ss << "lights[" << i << "].intencity";
			prog.create_uniform(ss.str());
			prog.set1f(ss.str(), lights[i].intencity);
			std::cout << ss.str() << std::endl;
			ss.str("");
			ss << "lights[" << i << "].constant";
			prog.create_uniform(ss.str());
			prog.set1f(ss.str(), lights[i].constant);
			std::cout << ss.str() << std::endl;
			ss.str("");
			ss << "lights[" << i << "].linear";
			prog.create_uniform(ss.str());
			prog.set1f(ss.str(), lights[i].linear);
			std::cout << ss.str() << std::endl;
			ss.str("");
			ss << "lights[" << i << "].quad";
			prog.create_uniform(ss.str());
			std::cout << ss.str() << std::endl;
			prog.set1f(ss.str(), lights[i].quad);
			ss.str("");
		}

	}

	void updateLights(float delta) {
		rotSpeed += 1.0f * delta;

		if (rotSpeed > 360.0f) {
			rotSpeed -= 360.0f;
		}

		yrot1 += rotSpeed * delta;
		yrot2 += rotSpeed * delta;
		yrot3 += rotSpeed * delta;

		if (yrot1 > 360.0f) {
			yrot1 -= 360.0f;
		}

		if (yrot2 > 360.0f) {
			yrot2 -= 360.0f;
		}

		if (yrot3 > 360.0f) {
			yrot3 -= 360.0f;
		}

		float yrad = glm::radians(yrot1);
		lights[1].position = glm::vec3(glm::cos(yrad), 1.0f, glm::sin(yrad)) * dist;
		this->lights[1].position.y = 5.0f;
		yrad = glm::radians(yrot2);
		lights[2].position = glm::vec3(glm::cos(yrad), 1.0f, glm::sin(yrad)) * dist;
		this->lights[2].position.y = 5.0f;
		yrad = glm::radians(yrot3);
		lights[3].position = glm::vec3(glm::cos(yrad), 1.0f, glm::sin(yrad)) * dist;
		this->lights[3].position.y = 5.0f;
	}

	void renderLights(Program& prog) {
		std::stringstream ss;

		for (int i = 0; i < NUM_LIGHTS; i++) {
			ss << "lights[" << i << "].position";
			prog.set3f(ss.str(), lights[i].position);
			ss.str("");
		}
	}
};

struct Material {
	Texture2D albedo;
	Texture2D specular;
	Texture2D normal;
	float shinny;

	static void createMaterialStructure(Program& prog) {
		prog.create_uniform("material.albedo");
		prog.set1i("material.albedo", 0);
		prog.create_uniform("material.specular");
		prog.set1i("material.specular", 1);
		prog.create_uniform("material.normal");
		prog.set1i("material.normal", 2);
		prog.create_uniform("material.shinny");
	}

	void bind(Program& prog) {
		prog.set1f("material.shinny", this->shinny);
		albedo.bind();
		specular.bind(GL_TEXTURE1);
		normal.bind(GL_TEXTURE2);
	}

	void unbind() {
		normal.bind(GL_TEXTURE2);
		specular.unbind(GL_TEXTURE1);
		albedo.unbind();
	}

	void release() {
		normal.release();
		specular.release();
		albedo.release();
	}
};

class MaterialApp : public IApp {
private:
	// Shader
	Shader vertexShader;
	Shader fragmentShader;
	Program program;
	// Textures
	Material dirt;
	Material grass;
	Material lava;
	Material panel;
	Material rubble;
	Material snow;
	// MeshOBJ
	Mesh barrel;
	Mesh cube;
	Mesh death_donut;
	Mesh floor;
	Mesh monkey;
	Mesh sphere;
	// Camera
	Camera camera;
	// Light
	LightManager lightManager;

	float yrot = 0.0f;

public:

	MaterialApp() {}

	virtual void init() {

		glEnable(GL_DEPTH_TEST);
		// Shader
		vertexShader.init(GL_VERTEX_SHADER, "data/shaders/multilights/shader.vert");
		fragmentShader.init(GL_FRAGMENT_SHADER, "data/shaders/multilights/shader.frag");
		program.attach(&vertexShader);
		program.attach(&fragmentShader);
		program.init();
		program.bind();
		program.create_uniform("projection");
		program.create_uniform("view");
		program.create_uniform("model");
		program.create_uniform("normalModel");
		program.create_uniform("cameraPos");
		lightManager.createLights(program);
		Material::createMaterialStructure(program);
		program.unbind();
		// Texture
		dirt.albedo.init("data/textures/dirt.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		dirt.specular.init("data/textures/dirt_specular.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		dirt.normal.init("data/textures/dirt_normal.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		dirt.shinny = 32;
		grass.albedo.init("data/textures/grass3.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		grass.specular.init("data/textures/grass3_specular.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		grass.normal.init("data/textures/grass3_normal.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		grass.shinny = 16;
		lava.albedo.init("data/textures/lava.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		lava.specular.init("data/textures/lava_specular.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		lava.normal.init("data/textures/lava_normal.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		lava.shinny = 256;
		panel.albedo.init("data/textures/panel.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		panel.specular.init("data/textures/panel_specular.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		panel.normal.init("data/textures/panel_normal.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		panel.shinny = 128;
		rubble.albedo.init("data/textures/rubble.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		rubble.specular.init("data/textures/rubble_specular.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		rubble.normal.init("data/textures/rubble_normal.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		rubble.shinny = 32;
		snow.albedo.init("data/textures/snow.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		snow.specular.init("data/textures/snow_specular.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		snow.normal.init("data/textures/snow_normal.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		snow.shinny = 128;
		// MeshOBJ
		Uint32 pre = SDL_GetTicks();
		barrel.init("data/meshes/barrel.smf");
		cube.init("data/meshes/cube.smf");
		death_donut.init("data/meshes/death_donut.smf");
		floor.init("data/meshes/floor.smf");
		monkey.init("data/meshes/monkey.smf");
		sphere.init("data/meshes/sphere.smf");
		float delta = ((float)SDL_GetTicks() - pre) / 1000.0f;
		std::cout << delta << " seconds..." << std::endl;
		// Camera
		camera.init(
			glm::vec3(0.0f, 3.0f, -10.0f),
			glm::vec2(0.0f, 0.0f),
			glm::perspective(glm::radians(60.0f), app_getWidth() / (float)app_getHeight(), 0.1f, 1024.0f),
			0.5f,
			0.5f
		);

	}

	virtual void update(float delta) {


		yrot += 90.0f * delta;

		if (yrot >= 360.0f) {
			yrot -= 360.0f;
		}
		if (input_isKeyHit(SDL_SCANCODE_TAB)) {
			input_toggleMouseGrab();
		}

		camera.update(delta);

		lightManager.updateLights(delta);
	}

	virtual void render() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		program.bind();

		glm::mat4 projection = glm::perspective(glm::radians(60.0f), app_getWidth() / (float)app_getHeight(), 0.1f, 1024.0f);
		glm::mat4 view = glm::translate(glm::vec3(0.0f, -3.0f, 0.0f));

		lightManager.renderLights(program);

		camera.render(program);

		program.set3f("cameraPos", camera.getPos());

		// Draw Floor
		this->draw(floor, grass, glm::vec3(0.0f, 0.0f, 0.0f));
		// Draw Mesh around the map
		this->draw(barrel, dirt, glm::vec3(0.0f, 1.0f, -5.0f));
		this->draw(cube, panel, glm::vec3(0.0f, 1.0f, 5.0f));
		this->draw(death_donut, lava, glm::vec3(-5.0f, 1.5f, 0.0f));
		this->draw(monkey, rubble, glm::vec3(5.0f, 1.0f, 0.0f));
		this->draw(sphere, snow, glm::vec3(0.0f, 1.0f, 0.0f));

		program.unbind();
	}

	virtual void release() {
		// Release MeshOBJ
		// Release Texture
		dirt.release();
		grass.release();
		lava.release();
		panel.release();
		rubble.release();
		snow.release();
		// Release Program
		program.release();
		// Don't need to release shaders because 
		// its get deinitialization
	}

	void draw(Mesh& mesh, Material& material, const glm::vec3& pos) {
		glm::mat4 model = glm::translate(pos);
		glm::mat4 normalModel = glm::inverseTranspose(model);
		program.setMat4f("model", model);
		program.setMat4f("normalModel", normalModel);
		material.bind(program);
		mesh.render();
		material.unbind();
	}

};

int main(int argc, char** argv) {
	MaterialApp app;
	app_init("Lighting Multi Lights", 1366, 768, &app);
	app_update();
	app_release();
	return 0;
}