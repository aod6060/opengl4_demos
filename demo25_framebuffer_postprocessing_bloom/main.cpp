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

struct Screen_Test {
	GLuint vertexBufferObjects[2];

	GLuint vertexArray;

	GLuint size;

	void init() {
		// Create Vertices Buffer
		glGenBuffers(2, vertexBufferObjects);

		std::vector<glm::vec3> vertices = {
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f)
		};

		std::vector<glm::vec2> texCoords = {
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f)
		};

		size = vertices.size();

		for (int i = 0; i < texCoords.size(); i++) {
			texCoords[i].y = 1.0f - texCoords[i].y;
		}

		// Create Vertices Buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
		// Create TexCoord Buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[1]);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Create Vertex Array
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[1]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}

	void render() {
		glBindVertexArray(vertexArray);
		glDrawArrays(GL_TRIANGLES, 0, size);
		glBindVertexArray(0);
	}

	void release() {
		glDeleteVertexArrays(1, &vertexArray);
		glDeleteBuffers(2, this->vertexBufferObjects);
	}
};

class MaterialApp : public IApp {
private:
	// Shader
	Shader vertexShader;
	Shader fragmentShader;
	Program program;
	// Postprocess
	Shader postprocessVertexShader;
	Shader postprocessFragmentShader;
	Program postprocessProgram;
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
	Texture2D renderTexture;
	Texture2D colorTexture;
	Texture2D depthTexture;
	// Screen Test
	Screen_Test screen;

	float yrot = 0.0f;
public:

	virtual void init() {
		light.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
		light.diffuse = glm::vec3(0.9f, 0.9f, 0.9f);
		light.position = glm::vec3(-0.2f, -0.3f, -0.3f);
		light.intencity = 1.0f;
		glEnable(GL_DEPTH_TEST);
		// Shader
		vertexShader.init(GL_VERTEX_SHADER, "data/shaders/framebuffer_postprocessing_bloom/shader.vert");
		fragmentShader.init(GL_FRAGMENT_SHADER, "data/shaders/framebuffer_postprocessing_bloom/shader.frag");
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
		// Framebuffer
		framebuffer.init();
		renderTexture.initEmpty(app_getWidth(), app_getHeight(), GL_RGB16F, GL_RGB);
		renderTexture.bind();
		renderTexture.setParameters(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		renderTexture.setParameters(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		renderTexture.unbind();
		colorTexture.initEmpty(app_getWidth(), app_getHeight(), GL_RGB16F, GL_RGB);
		colorTexture.bind();
		colorTexture.setParameters(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		colorTexture.setParameters(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		colorTexture.unbind();
		//depthRenderbuffer.init(512, 512, GL_DEPTH24_STENCIL8);
		depthTexture.initEmpty(app_getWidth(), app_getHeight(), GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
		framebuffer.bind();
		framebuffer.attachTexture2D(&depthTexture, GL_DEPTH_STENCIL_ATTACHMENT);
		framebuffer.attachTexture2D(&renderTexture, GL_COLOR_ATTACHMENT0);
		framebuffer.attachTexture2D(&colorTexture, GL_COLOR_ATTACHMENT1);
		//framebuffer.attachRenderbuffer(&depthRenderbuffer, GL_DEPTH_STENCIL_ATTACHMENT);
		if (framebuffer.checkFramebuffer()) {
			std::cout << "Error: Framebuffer incomplete..." << std::endl;
		}
		framebuffer.unbind();
		// Screen Test
		screen.init();
		// Init post processing
		this->postprocessVertexShader.init(GL_VERTEX_SHADER, "data/shaders/framebuffer_postprocessing_bloom/postprocess.vert");
		this->postprocessFragmentShader.init(GL_FRAGMENT_SHADER, "data/shaders/framebuffer_postprocessing_bloom/postprocess.frag");
		this->postprocessProgram.attach(&postprocessVertexShader);
		this->postprocessProgram.attach(&postprocessFragmentShader);
		this->postprocessProgram.init();
		this->postprocessProgram.bind();
		this->postprocessProgram.create_uniform("projection");
		this->postprocessProgram.create_uniform("view");
		this->postprocessProgram.create_uniform("model");
		this->postprocessProgram.create_uniform("tex0");
		this->postprocessProgram.set1i("tex0", 0);
		this->postprocessProgram.create_uniform("color0");
		this->postprocessProgram.set1i("color0", 1);
		this->postprocessProgram.create_uniform("mouseVel");
		this->postprocessProgram.create_uniform("exposure");
		this->postprocessProgram.unbind();
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
		framebuffer.bindDrawBuffers();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		program.bind();

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

		program.unbind();

		framebuffer.unbind();
	}

	void render_scene() {
		glDepthMask(GL_FALSE);

		glClear(GL_COLOR_BUFFER_BIT);

		postprocessProgram.bind();

		glm::mat4 projection = glm::ortho(0.0f, (float)app_getWidth(), (float)app_getHeight(), 0.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 model = glm::scale(glm::vec3(app_getWidth(), app_getHeight(), 0));

		postprocessProgram.setMat4f("projection", projection);
		postprocessProgram.setMat4f("view", view);
		postprocessProgram.setMat4f("model", model);
		postprocessProgram.set2f("mouseVel", camera.getMouseVel());

		glm::vec3 dir = -glm::vec3(
			camera.projection[0][2],
			camera.projection[1][2],
			camera.projection[2][2]
		);

		postprocessProgram.set1f("exposure", 1.0);
		renderTexture.bind();
		colorTexture.bind(GL_TEXTURE1);
		screen.render();
		colorTexture.unbind(GL_TEXTURE1);
		renderTexture.unbind();

		postprocessProgram.unbind();

		glDepthMask(GL_TRUE);
	}
	virtual void release() {
		postprocessProgram.release();
		screen.release();
		depthTexture.release();
		colorTexture.release();
		renderTexture.release();
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
};

int main(int argc, char** argv) {
	MaterialApp app;
	app_init("Framebuffer Tests", 1366, 768, &app);
	app_update();
	app_release();
	return 0;
}