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

struct Renderbuffer_Test {
	GLuint id;

	void init(GLuint width, GLuint height, GLenum type) {
		glGenRenderbuffers(1, &id);
		glBindRenderbuffer(GL_RENDERBUFFER, id);
		glRenderbufferStorage(GL_RENDERBUFFER, type, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void bind() {
		glBindRenderbuffer(GL_RENDERBUFFER, id);
	}

	void unbind() {
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void release() {
		glDeleteRenderbuffers(1, &id);
	}

	GLuint getID() {
		return id;
	}
};

struct Framebuffer_Test {
	GLuint id;

	std::vector<GLenum> drawBuffers;

	void init() {
		glGenFramebuffers(1, &id);
	}

	void attachTexture2D(Texture2D* texture, GLenum attachment) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->getID(), 0);
		if (attachment != GL_DEPTH_STENCIL_ATTACHMENT) {
			drawBuffers.push_back(attachment);
		}
	}

	void attachRenderbuffer(Renderbuffer_Test* texture, GLenum attachment) {
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, texture->getID());
	}

	bool checkFramebuffer() {
		GLenum test = this->getFramebufferStatus();
		printStatus(test);
		return test != GL_FRAMEBUFFER_COMPLETE;
	}

	GLuint getFramebufferStatus() {
		return glCheckFramebufferStatus(GL_FRAMEBUFFER);
	}

	void bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, id);
	}

	void bindDrawBuffers() {
		glDrawBuffers(drawBuffers.size(), drawBuffers.data());
	}

	void unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void release() {
		glDeleteFramebuffers(1, &id);
	}

	void printStatus(GLenum type) {
		switch (type) {
		case GL_FRAMEBUFFER_UNDEFINED:
			std::cout << "Framebuffer incomplte..." << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			std::cout << "Framebuffer incomplete attachment" << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			std::cout << "Framebuffer incomplete missing attachment!!!" << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			std::cout << "Framebuffer incomplete draw buffer" << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			std::cout << "Framebuffer incomplete read buffer" << std::endl;
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			std::cout << "Framebuffer unsupported..." << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			std::cout << "Framebuffer incomplete multisample..." << std::endl;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			std::cout << "Framebuffer incomplete layer targets..." << std::endl;
			break;
		case GL_INVALID_ENUM:
			std::cout << "Invalid Enum..." << std::endl;
			break;
		case GL_FRAMEBUFFER_COMPLETE:
			std::cout << "Framebuffer complete???" << std::endl;
			break;
		default:
			std::cout << "Either framebuffer complete or some other error!" << std::endl;
			break;
		}
	}
};

class MaterialApp : public IApp {
private:
	// Shader
	Shader vertexShader;
	Shader fragmentShader;
	Program program;
	// Postprocess
	Shader bufferVertexShader;
	Shader bufferFragmentShader;
	Program bufferProgram;
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
	Light light;
	// Framebuffer
	Framebuffer_Test framebuffer;
	Texture2D depthTexture;
	float yrot = 0.0f;
public:

	virtual void init() {
		light.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
		light.diffuse = glm::vec3(0.9f, 0.9f, 0.9f);
		light.position = glm::vec3(-0.2f, -0.3f, -0.3f);
		light.intencity = 1.0f;
		glEnable(GL_DEPTH_TEST);
		// Shader
		vertexShader.init(GL_VERTEX_SHADER, "data/shaders/shadow_mapping/shader.vert");
		fragmentShader.init(GL_FRAGMENT_SHADER, "data/shaders/shadow_mapping/shader.frag");
		program.attach(&vertexShader);
		program.attach(&fragmentShader);
		program.init();
		program.bind();
		program.create_uniform("projection");
		program.create_uniform("view");
		program.create_uniform("model");
		program.create_uniform("normalModel");
		program.create_uniform("cameraPos");
		program.create_uniform("depthMap");
		program.set1i("depthMap", 3);
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
		// Framebuffer
		framebuffer.init();
		depthTexture.initEmpty(1024, 1024, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
		depthTexture.bind();
		depthTexture.setParameters(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		depthTexture.setParameters(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		depthTexture.setParameters(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		depthTexture.setParameters(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		depthTexture.unbind();
		framebuffer.bind();
		framebuffer.attachTexture2D(&depthTexture, GL_DEPTH_ATTACHMENT);
		//framebuffer.attachRenderbuffer(&depthRenderbuffer, GL_DEPTH_STENCIL_ATTACHMENT);
		if (framebuffer.checkFramebuffer()) {
			std::cout << "Error: Framebuffer incomplete..." << std::endl;
		}
		framebuffer.unbind();
		bufferVertexShader.init(GL_VERTEX_SHADER, "data/shaders/shadow_mapping/buffer.vert");
		bufferFragmentShader.init(GL_FRAGMENT_SHADER, "data/shaders/shadow_mapping/buffer.frag");
		bufferProgram.attach(&bufferVertexShader);
		bufferProgram.attach(&bufferFragmentShader);
		bufferProgram.init();
		bufferProgram.bind();
		bufferProgram.create_uniform("projection");
		bufferProgram.create_uniform("view");
		bufferProgram.create_uniform("model");
		bufferProgram.unbind();
	}

	virtual void update(float delta) {
		if (input_isKeyHit(SDL_SCANCODE_TAB)) {
			input_toggleMouseGrab();
		}

		camera.update(delta);
	}

	virtual void render() {
		this->render_scene_framebuffer();
		this->render_scene();
	}

	void render_scene_framebuffer() {
		framebuffer.bind();
		glViewport(0, 0, 1024, 1024);
		glClear(GL_DEPTH_BUFFER_BIT);

		bufferProgram.bind();
		float near_plane = 1.0f, far_plane = 7.5f;
		float size = 10.0f;
		glm::mat4 projection = glm::ortho(-size, size, -size, size, near_plane, far_plane);
		glm::mat4 view = glm::lookAt(-light.position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		bufferProgram.setMat4f("projection", projection);
		bufferProgram.setMat4f("view", view);
		// Draw Floor
		this->drawShadow(floor, glm::vec3(0.0f, 0.0f, 0.0f));
		// Draw Mesh around the map
		this->drawShadow(barrel, glm::vec3(0.0f, 1.0f, -5.0f));
		this->drawShadow(cube, glm::vec3(0.0f, 1.0f, 5.0f));
		this->drawShadow(death_donut, glm::vec3(-5.0f, 1.5f, 0.0f));
		this->drawShadow(monkey, glm::vec3(5.0f, 1.0f, 0.0f));
		this->drawShadow(sphere, glm::vec3(0.0f, 1.0f, 0.0f));

		bufferProgram.unbind();

		framebuffer.unbind();

	}

	void render_scene() {

		glViewport(0, 0, app_getWidth(), app_getHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		program.bind();

		depthTexture.bind(GL_TEXTURE3);
		light.bindLightStructure(program);

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
		depthTexture.unbind(GL_TEXTURE3);
		program.unbind();
	}
	virtual void release() {
		//postprocessProgram.release();
		depthTexture.release();
		//depthRenderbuffer.release();
		framebuffer.release();
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

	void drawShadow(Mesh& mesh, const glm::vec3& pos) {
		glm::mat4 model = glm::translate(pos);
		bufferProgram.setMat4f("model", model);
		mesh.render();
	}
};

int main(int argc, char** argv) {
	MaterialApp app;
	app_init("Framebuffer Tests", 1366, 768, &app);
	app_update();
	app_release();
	return 0;
}