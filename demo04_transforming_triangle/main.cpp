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
	std::vector<glm::vec2> texCoords_list;

	GLuint vertexBufferObject[3];

	GLuint vertexArray = 0;

	void init() {
		vertices_list = {
			{ 0.0f, 0.5f, 0.0f },
			{ 0.5f, -0.5f, 0.0f },
			{ -0.5f, -0.5f, 0.0f }
		};

		colors_list = {
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f }
		};

		texCoords_list = {
			{ 0.5, 1.0f },
			{ 1.0f, 0.0f },
			{ 0.0f, 0.0f }
		};

		for (int i = 0; i < texCoords_list.size(); i++) {
			texCoords_list[i] = 1.0f - texCoords_list[i];
		}

		// Create Vertex Buffer Objects
		glGenBuffers(3, this->vertexBufferObject);
		// Bind Vertices and fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObject[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices_list.size() * sizeof(glm::vec3), vertices_list.data(), GL_STATIC_DRAW);
		// Bind Color and fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObject[1]);
		glBufferData(GL_ARRAY_BUFFER, colors_list.size() * sizeof(glm::vec3), colors_list.data(), GL_STATIC_DRAW);
		// Bind Texturecoord and fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObject[2]);
		glBufferData(GL_ARRAY_BUFFER, texCoords_list.size() * sizeof(glm::vec2), texCoords_list.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Create Vertex Array
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject[2]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

	}

	void bind() {
		glBindVertexArray(this->vertexArray);
	}

	void unbind() {
		glBindVertexArray(0);
	}

	void release() {
		glDeleteVertexArrays(1, &vertexArray);
		glDeleteBuffers(3, this->vertexBufferObject);
	}

	GLuint size() {
		return vertices_list.size();
	}
};

class TransformingTriangleApp : public IApp {
public:

	Triangle tris;

	Shader_Test shader;

	// Texture Uniform
	GLuint texture_uniform;
	// Transform Uniforms
	GLuint projection_uniform;
	GLuint view_uniform;
	GLuint model_uniform;

	Texture2D texture;

	glm::mat4 projection;
	glm::mat4 view;
	//glm::mat4 model;

	float yrot = 0.0f;

	virtual void init() {
		tris.init();
		// Create Shader
		shader.vid = glCreateShader(GL_VERTEX_SHADER);
		shader.fid = glCreateShader(GL_FRAGMENT_SHADER);
		std::string vsrc = util_loadTextFile("data/shaders/transforming_tris/shader.vert");
		std::string fsrc = util_loadTextFile("data/shaders/transforming_tris/shader.frag");
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

		glUseProgram(shader.pid);
		// Adding in Transformation
		projection_uniform = glGetUniformLocation(shader.pid, "projection");
		view_uniform = glGetUniformLocation(shader.pid, "view");
		model_uniform = glGetUniformLocation(shader.pid, "model");

		// Texture
		texture_uniform = glGetUniformLocation(shader.pid, "tex0");
		glUniform1i(texture_uniform, 0);
		glUseProgram(0);

		texture.init("data/textures/textured_tris/textured.png");

		projection = glm::perspective(glm::radians(60.0f), app_getWidth() / (float)app_getHeight(), 0.1f, 1024.0f);

		view = glm::mat4(1.0f);

		//model = glm::translate(glm::vec3(0.0f, 0.0f, -1.5f));

	}

	virtual void update(float delta) {
		// Use this some other time

		yrot += 90.0f * delta;


		if (yrot > 360.0f) {
			yrot -= 360.0f;
		}
	}

	virtual void render() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader.pid);

		glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(view_uniform, 1, GL_FALSE, &view[0][0]);


		glm::quat yrot = glm::angleAxis(glm::radians(this->yrot), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 trans = glm::translate(glm::vec3(0.0f, 0.0f, -1.5f));

		glm::mat4 rot = glm::toMat4(yrot);

		glm::mat4 model = trans * rot;

		glUniformMatrix4fv(model_uniform, 1, GL_FALSE, &model[0][0]);

		texture.bind();
		tris.bind();
		glDrawArrays(GL_TRIANGLES, 0, tris.size());
		tris.unbind();
		texture.unbind();

		glUseProgram(0);
	}

	virtual void release() {
		texture.release();
		glDetachShader(shader.pid, shader.vid);
		glDetachShader(shader.pid, shader.fid);
		tris.release();
	}
};

int main(int argc, char** argv) {

	TransformingTriangleApp app;

	app_init("Transforming Triangle Application", 800, 600, &app);

	app_update();

	app_release();

	return 0;
}