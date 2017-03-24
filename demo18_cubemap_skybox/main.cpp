#include <sys.h>


struct Light {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 position;
	float intencity;

	void createLightStructure(Program& prog) {
		prog.create_uniform("light.ambient");
		prog.create_uniform("light.diffuse");
		prog.create_uniform("light.position");
		prog.create_uniform("light.intencity");
	}

	void bindLightStructure(Program& prog) {
		prog.set3f("light.ambient", ambient);
		prog.set3f("light.diffuse", diffuse);
		prog.set3f("light.position", position);
		prog.set1f("light.intencity", this->intencity);
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

struct Cubemap_Test {
	GLuint id;

	enum Side {
		RIGHT = 0,
		LEFT,
		TOP,
		BOTTOM,
		BACK,
		FRONT,
		NUM_SIDES
	};

	std::vector<std::string> maps = std::vector<std::string>(6);

	void init() {
		glGenTextures(1, &id);
		bind();
		for (GLuint i = 0; i < maps.size(); i++) {
			SDL_Surface* surf = loadImage(maps[i]);
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				surf->w,
				surf->h,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				surf->pixels
			);
			SDL_FreeSurface(surf);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		unbind();
	}

	void bind(GLenum type = GL_TEXTURE0) {
		glActiveTexture(type);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	}

	void unbind(GLenum type = GL_TEXTURE0) {
		glActiveTexture(type);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void release() {
		glDeleteTextures(1, &id);
	}
	
	// 
	void setTexture(Side side, std::string fn) {
		maps[side] = fn;
	}

	SDL_Surface* loadImage(std::string fn) {
		SDL_Surface* surf = IMG_Load(fn.c_str());

		if (surf == nullptr) {
			std::cout << "Error: " << fn << " doesn't exist!!!" << std::endl;
		}

		return surf;
	}
};

class MaterialApp : public IApp {
private:
	// Shader
	// Regular Shaders
	Shader vertexShader;
	Shader fragmentShader;
	Program program;
	// Skybox Shader
	Shader skyboxVertexShader;
	Shader skyboxFragmentShader;
	Program skyboxProgram;
	// Textures
	Material dirt;
	Material grass;
	Material lava;
	Material panel;
	Material rubble;
	Material snow;
	// MeshOBJ
	Mesh barrel;
	Mesh cube; // Will use this for the model...
	Mesh death_donut;
	Mesh floor;
	Mesh monkey;
	Mesh sphere;
	// Camera
	Camera camera;
	// Light
	Light light;
	// Cubemap
	Cubemap_Test skybox;
	Mesh skybox_mesh;
public:

	virtual void init() {
		light.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
		light.diffuse = glm::vec3(0.9f, 0.9f, 0.9f);
		light.position = glm::vec3(0.2f, -0.3f, -0.3f);
		light.intencity = 1.0f;

		glEnable(GL_DEPTH_TEST);
		// Shader
		vertexShader.init(GL_VERTEX_SHADER, "data/shaders/skybox/shader.vert");
		fragmentShader.init(GL_FRAGMENT_SHADER, "data/shaders/skybox/shader.frag");
		program.attach(&vertexShader);
		program.attach(&fragmentShader);
		program.init();
		program.bind();
		program.create_uniform("projection");
		program.create_uniform("view");
		program.create_uniform("model");
		program.create_uniform("normalModel");
		program.create_uniform("cameraPos");
		light.createLightStructure(program);
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
		// Handle Skybox
		// Create Shader
		skyboxVertexShader.init(GL_VERTEX_SHADER, "data/shaders/skybox/skybox.vert");
		skyboxFragmentShader.init(GL_FRAGMENT_SHADER, "data/shaders/skybox/skybox.frag");
		skyboxProgram.attach(&skyboxVertexShader);
		skyboxProgram.attach(&skyboxFragmentShader);
		skyboxProgram.init();
		skyboxProgram.bind();
		skyboxProgram.create_uniform("projection");
		skyboxProgram.create_uniform("view");
		skyboxProgram.create_uniform("skybox");
		skyboxProgram.set1i("skybox", 0);
		skyboxProgram.unbind();
		// Skybox
		skybox.setTexture(Cubemap_Test::RIGHT, "data/textures/skybox/east.png");
		skybox.setTexture(Cubemap_Test::LEFT, "data/textures/skybox/west.png");
		skybox.setTexture(Cubemap_Test::TOP, "data/textures/skybox/up.png");
		skybox.setTexture(Cubemap_Test::BOTTOM, "data/textures/skybox/down.png");
		skybox.setTexture(Cubemap_Test::FRONT, "data/textures/skybox/south.png");
		skybox.setTexture(Cubemap_Test::BACK, "data/textures/skybox/north.png");
		skybox.init();
		skybox_mesh.init("data/meshes/skybox_mesh.smf");
	}

	virtual void update(float delta) {
		if (input_isKeyHit(SDL_SCANCODE_TAB)) {
			input_toggleMouseGrab();
		}
		camera.update(delta);
	}

	virtual void render() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection, view;
		camera.grabMatrices(view, projection);

		// Regular Shaders
		program.bind();
		program.setMat4f("projection", projection);
		program.setMat4f("view", view);
		light.bindLightStructure(program);
		//camera.render(program);
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

		// Skybox
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);
		//glDisable(GL_DEPTH_TEST);
		skyboxProgram.bind();
		//
		skyboxProgram.setMat4f("projection", projection);
		skyboxProgram.setMat4f("view", view);
		skybox.bind();
		skybox_mesh.render();
		skybox.unbind();
		//
		skyboxProgram.unbind();
		//glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);

	}

	virtual void release() {
		// Release Meshs
		skybox_mesh.release();
		// Release Skybox
		skybox.release();
		// Release MeshOBJ
		barrel.release();
		cube.release();
		death_donut.release();
		floor.release();
		monkey.release();
		sphere.release();
		// Release Texture
		dirt.release();
		grass.release();
		lava.release();
		panel.release();
		rubble.release();
		snow.release();
		// Release Program
		skyboxProgram.release();
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
	app_init("Cubemap: Skybox", 1366, 768, &app);
	app_update();
	app_release();
	return 0;
}