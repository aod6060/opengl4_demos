#include <sys.h>

struct Shader_Test {
	GLuint id;

	void init(GLenum type, std::string fn) {
		id = glCreateShader(type);
		std::string src = util_loadTextFile(fn);
		std::cout << src << std::endl;
		const char* csrc = src.c_str();
		glShaderSource(id, 1, &csrc, 0);
		glCompileShader(id);
		util_printShaderLog(id);
	}

	void release() {
		glDeleteShader(id);
	}
};

struct Program_Test {

	std::vector<Shader_Test*> shaders;

	GLuint id;

	std::map<std::string, GLuint> uniforms;

	void attach(Shader_Test* shader) {
		shaders.push_back(shader);
	}

	void init() {
		this->id = glCreateProgram();
		for (auto& shader : shaders) {
			glAttachShader(this->id, shader->id);
		}
		glLinkProgram(this->id);
		util_printProgramLog(this->id);
	}

	void bind() {
		glUseProgram(this->id);
	}

	void unbind() {
		glUseProgram(0);
	}

	void release() {
		for (auto& shader : shaders) {
			glDetachShader(this->id, shader->id);
		}
		glDeleteProgram(this->id);
		for (auto& shader : shaders) {
			shader->release();
		}
		shaders.clear();
	}

	void create_uniform(std::string name) {
		this->uniforms[name] = glGetUniformLocation(id, name.c_str());
	}

	// Integers
	void set1i(std::string name, int i) {
		glUniform1i(this->uniforms[name], i);
	}

	void set2i(std::string name, const glm::ivec2& v) {
		glUniform2i(this->uniforms[name], v.x, v.y);
	}

	void set3i(std::string name, const glm::ivec3& v) {
		glUniform3i(this->uniforms[name], v.x, v.y, v.z);
	}

	void set4i(std::string name, const glm::ivec4& v) {
		glUniform4i(this->uniforms[name], v.x, v.y, v.z, v.w);
	}

	// Floats
	void set1f(std::string name, float i) {
		glUniform1f(this->uniforms[name], i);
	}

	void set2f(std::string name, const glm::vec2& v) {
		glUniform2f(this->uniforms[name], v.x, v.y);
	}

	void set3f(std::string name, const glm::vec3& v) {
		glUniform3f(this->uniforms[name], v.x, v.y, v.z);
	}

	void set4f(std::string name, const glm::vec4& v) {
		glUniform4f(this->uniforms[name], v.x, v.y, v.z, v.w);
	}

	// Matrices
	void setMat2f(std::string name, const glm::mat2& m) {
		glUniformMatrix2fv(this->uniforms[name], 1, GL_FALSE, &m[0][0]);
	}

	void setMat3f(std::string name, const glm::mat3& m) {
		glUniformMatrix3fv(this->uniforms[name], 1, GL_FALSE, &m[0][0]);
	}

	void setMat4f(std::string name, const glm::mat4& m) {
		glUniformMatrix4fv(this->uniforms[name], 1, GL_FALSE, &m[0][0]);
	}

};

struct Camera_Test {
	glm::vec3 pos;
	glm::vec2 rot;

	glm::mat4 projection;

	float sence;

	float speed;

	void init(
		const glm::vec3& pos, 
		const glm::vec2& rot, 
		const glm::mat4& projection,
		float sence,
		float speed) {

		this->pos = pos;
		this->rot = rot;
		this->projection = projection;
		this->sence = sence;
		this->speed = speed;
	}

	void render(Program_Test& program) {
		program.setMat4f("projection", this->projection);

		glm::quat xrot = glm::angleAxis(glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat yrot = glm::angleAxis(glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 rot = glm::toMat4(xrot * yrot);

		glm::mat4 translate = glm::translate(-pos);

		glm::mat4 view = rot * translate;

		program.setMat4f("view", view);
	}

	void update(float delta) {

		if (input_isMouseGrab()) {
			glm::vec2 mc;
			input_getMouseCoords(mc);

			float actualSencs = this->sence * 100.0f;
			float actualSpeed = this->speed * 100.0f;

			std::cout << actualSencs << std::endl;

			rot.y += mc.x * actualSencs * delta;
			rot.x += mc.y * actualSencs * delta;

			if (rot.y <= -360.0f) {
				rot.y += 360.0f;
			}
			else if (rot.y >= 360.0f) {
				rot.y -= 360.0f;
			}

			rot.x = glm::clamp(rot.x, -90.0f, 90.0f);

			float yrad = glm::radians(rot.y);

			if (input_isKeyPress(SDL_SCANCODE_W)) {
				pos.x += glm::sin(yrad) * actualSpeed * delta;
				pos.z -= glm::cos(yrad) * actualSpeed * delta;
			}

			if (input_isKeyPress(SDL_SCANCODE_S)) {
				pos.x -= glm::sin(yrad) * actualSpeed * delta;
				pos.z += glm::cos(yrad) * actualSpeed * delta;
			}

			if (input_isKeyPress(SDL_SCANCODE_A)) {
				pos.x -= glm::cos(yrad) * actualSpeed * delta;
				pos.z -= glm::sin(yrad) * actualSpeed * delta;
			}

			if (input_isKeyPress(SDL_SCANCODE_D)) {
				pos.x += glm::cos(yrad) * actualSpeed * delta;
				pos.z += glm::sin(yrad) * actualSpeed * delta;
			}
		}
	}

	glm::vec3 getPos() {
		return this->pos;
	}

	glm::vec2 getRot() {
		return this->rot;
	}

	float getSence() {
		return this->sence;
	}

	float getSpeed() {
		return this->speed;
	}

	void setPos(const glm::vec3& pos) {
		this->pos = pos;
	}

	void setRot(const glm::vec2& rot) {
		this->rot = rot;
	}

	void setSence(float sence) {
		this->sence = sence;
	}

	void setSpeed(float speed) {
		this->speed = speed;
	}
};

class CameraApp : public IApp {
private:
	// Shader
	Shader_Test vertexShader;
	Shader_Test fragmentShader;
	Program_Test program;
	// Textures
	Texture2D dirt;
	Texture2D grass;
	Texture2D lava;
	Texture2D panel;
	Texture2D rubble;
	Texture2D snow;
	// MeshOBJ
	MeshOBJ barrel;
	MeshOBJ cube;
	MeshOBJ death_donut;
	MeshOBJ floor;
	MeshOBJ monkey;
	MeshOBJ sphere;
	// Camera
	Camera_Test camera;
public:

	virtual void init() {
		glEnable(GL_DEPTH_TEST);
		// Shader
		vertexShader.init(GL_VERTEX_SHADER, "data/shaders/camera/shader.vert");
		fragmentShader.init(GL_FRAGMENT_SHADER, "data/shaders/camera/shader.frag");
		program.attach(&vertexShader);
		program.attach(&fragmentShader);
		program.init();
		program.bind();
		program.create_uniform("projection");
		program.create_uniform("view");
		program.create_uniform("model");
		program.create_uniform("tex0");
		program.set1i("tex0", 0);
		program.unbind();
		// Texture
		dirt.init("data/textures/dirt.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		grass.init("data/textures/grass3.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		lava.init("data/textures/lava.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		panel.init("data/textures/panel.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		rubble.init("data/textures/rubble.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		snow.init("data/textures/snow.png", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true);
		// MeshOBJ
		Uint32 pre = SDL_GetTicks();
		barrel.init("data/meshes/barrel.obj");
		cube.init("data/meshes/cube.obj");
		death_donut.init("data/meshes/death_donut.obj");
		floor.init("data/meshes/floor.obj");
		monkey.init("data/meshes/monkey.obj");
		sphere.init("data/meshes/sphere.obj");
		float delta = ((float)SDL_GetTicks() - pre) / 1000.0f;
		std::cout << delta << "seconds..." << std::endl;
		// Camera
		camera.init(
			glm::vec3(0.0f, 3.0f, 0.0f),
			glm::vec2(0.0f, 0.0f),
			glm::perspective(glm::radians(60.0f), app_getWidth() / (float)app_getHeight(), 0.1f, 1024.0f),
			1.0f,
			0.9f
		);

	}

	virtual void update(float delta) {

		if (input_isKeyHit(SDL_SCANCODE_TAB)) {
			input_toggleMouseGrab();
		}

		camera.update(delta);
	}

	virtual void render() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		program.bind();

		glm::mat4 projection = glm::perspective(glm::radians(60.0f), app_getWidth() / (float)app_getHeight(), 0.1f, 1024.0f);
		glm::mat4 view = glm::translate(glm::vec3(0.0f, -3.0f, 0.0f));

		camera.render(program);

		// Draw Floor
		this->draw(floor, grass, glm::vec3(0.0f, 0.0f, 0.0f));
		// Draw Mesh around the map
		this->draw(barrel, dirt, glm::vec3(0.0f, 1.0f, -5.0f));
		this->draw(cube, panel, glm::vec3(0.0f, 1.0f, 5.0f));
		this->draw(death_donut, lava, glm::vec3(-5.0f, 1.0f, 0.0f));
		this->draw(monkey, rubble, glm::vec3(5.0f, 1.0f, 0.0f));
		this->draw(sphere, snow, glm::vec3(0.0f, 5.0f, 0.0f));

		program.unbind();
	}

	virtual void release() {
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

	void draw(MeshOBJ& mesh, Texture2D& tex, const glm::vec3& pos) {
		glm::mat4 model = glm::translate(pos);
		program.setMat4f("model", model);
		tex.bind();
		mesh.render();
		tex.unbind();
	}

};

int main(int argc, char** argv) {
	CameraApp app;
	app_init("Camera App", 800, 600, &app);
	app_update();
	app_release();
	return 0;
}