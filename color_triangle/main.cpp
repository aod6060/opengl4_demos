#include <sys.h>


/*
This was create to make it easier...
*/
struct Shader_Test {
	GLuint vid;
	GLuint fid;
	GLuint pid;
};

/*
	Structure for Vertex Buffer
*/
struct Triangle {
	
	std::vector<glm::vec3> vertices_list;
	std::vector<glm::vec3> colors_list;

	GLuint vertexBufferObject[2];

	GLuint vertexArray = 0;

	void init() {
		vertices_list = {
			{ 0.0f, 0.5f, 0.0f },
			{ 0.5f, -0.5f, 0.0f },
			{ -0.5f, -0.5f, 0.0f }
		};

		colors_list = {
			{1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f}
		};

		// Create Vertex Buffer Objects
		glGenBuffers(2, this->vertexBufferObject);
		// Bind Vertices and fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObject[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices_list.size() * sizeof(glm::vec3), vertices_list.data(), GL_STATIC_DRAW);
		// Bind Color and fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObject[1]);
		glBufferData(GL_ARRAY_BUFFER, colors_list.size() * sizeof(glm::vec3), colors_list.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Create Vertex Array
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


	}

	void bind() {
		glBindVertexArray(this->vertexArray);
	}

	void unbind() {
		glBindVertexArray(0);
	}

	void release() {
		glDeleteVertexArrays(1, &vertexArray);
		glDeleteBuffers(2, this->vertexBufferObject);
	}

	GLuint size() {
		return vertices_list.size();
	}
};

class ColorTriangleApp : public IApp {
public:

	Triangle tris;

	Shader_Test shader;

	virtual void init() {
		tris.init();
		// Create Shader
		shader.vid = glCreateShader(GL_VERTEX_SHADER);
		shader.fid = glCreateShader(GL_FRAGMENT_SHADER);
		std::string vsrc = util_loadTextFile("data/shaders/color_tris/shader.vert");
		std::string fsrc = util_loadTextFile("data/shaders/color_tris/shader.frag");
		const char* cvsrc = vsrc.c_str();
		std::cout << "Vertex Shader: " << std::endl << cvsrc << std::endl;
		const char* cfsrc = fsrc.c_str();
		std::cout << "Fragment Shader: " << std::endl << cfsrc << std::endl;
		glShaderSource(shader.vid, 1, &cvsrc, 0);
		glShaderSource(shader.fid, 1, &cfsrc, 0);
		glCompileShader(shader.vid);
		glCompileShader(shader.fid);
		shader.pid = glCreateProgram();
		glAttachShader(shader.pid, shader.vid);
		glAttachShader(shader.pid, shader.fid);
		glLinkProgram(shader.pid);
	}

	virtual void update(float delta) {
		// Use this some other time
	}

	virtual void render() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader.pid);

		tris.bind();
		glDrawArrays(GL_TRIANGLES, 0, tris.size());
		tris.unbind();

		glUseProgram(0);
	}

	virtual void release() {
		glDetachShader(shader.pid, shader.vid);
		glDetachShader(shader.pid, shader.fid);
		tris.release();
	}
};

int main(int argc, char** argv) {

	ColorTriangleApp app;

	app_init("Color Triangle Application", 800, 600, &app);

	app_update();

	app_release();

	return 0;
}