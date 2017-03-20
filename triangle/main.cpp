#include <sys.h>


std::vector<glm::vec3> vertices = {
	{0.0f, 0.5f, 0.0f},
	{0.5f, -0.5f, 0.0f},
	{-0.5f, -0.5f, 0.0f}
};

/*
	This was create to make it easier...
*/
struct Shader_Test {
	GLuint vid;
	GLuint fid;
	GLuint pid;
};

class TriangleApp : public IApp {
public:

	GLuint vbo = 0;
	GLuint vao = 0;

	Shader_Test shader;

	virtual void init() {
		// Creating Vertex Buffer Object
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Create Vertex Array Object ( This will store vertex buffer objects to prevent ton of api calls)
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		// Create Shader
		shader.vid = glCreateShader(GL_VERTEX_SHADER);
		shader.fid = glCreateShader(GL_FRAGMENT_SHADER);
		std::string vsrc = util_loadTextFile("data/shaders/tris/shader.vert");
		std::string fsrc = util_loadTextFile("data/shaders/tris/shader.frag");
		const char* cvsrc = vsrc.c_str();
		std::cout << "Vertex Shader: " << vsrc << std::endl;
		const char* cfsrc = fsrc.c_str();
		std::cout << "Fragment Shader: " << fsrc << std::endl;
		glShaderSource(shader.vid, 1, &cvsrc, 0);
		glShaderSource(shader.fid, 1, &cfsrc, 0);
		glCompileShader(shader.vid);
		util_printShaderLog(shader.vid);
		glCompileShader(shader.fid);
		util_printShaderLog(shader.fid);
		shader.pid = glCreateProgram();
		glAttachShader(shader.pid, shader.vid);
		glAttachShader(shader.pid, shader.fid);
		glLinkProgram(shader.pid);
		util_printProgramLog(shader.pid);
	}

	virtual void update(float delta) {
		// Use this some other time
	}

	virtual void render() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader.pid);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);

		glUseProgram(0);
	}

	virtual void release() {
		glDetachShader(shader.pid, shader.vid);
		glDetachShader(shader.pid, shader.fid);
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);

	}
};

int main(int argc, char** argv) {

	TriangleApp app;

	app_init("Triangle Application", 800, 600, &app);

	app_update();

	app_release();

	return 0;
}