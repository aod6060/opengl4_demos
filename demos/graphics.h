#pragma once

struct Texture2D {
public:

	GLuint id;
	GLuint width;
	GLuint height;

	void init(std::string fn, GLenum min_filter = GL_LINEAR, GLenum mag_filter = GL_LINEAR, bool mipmap = false);

	void bind(GLenum e = GL_TEXTURE0);

	void unbind(GLenum e = GL_TEXTURE0);

	void release();

	GLuint getWidth();

	GLuint getHeight();

	GLuint getID();

};

struct MeshOBJ {


	struct Face {
		GLuint vertex[3];
		GLuint texCoord[3];
		GLuint normal[3];
	};

	struct Vertex {
		glm::vec3 vertex;
		glm::vec2 texCoord;
		glm::vec3 normal;

		bool operator == (const Vertex& other) const;

	};

	std::vector<glm::vec3> vertices_list;
	std::vector<glm::vec2> texCoords_list;
	std::vector<glm::vec3> normals_list;
	std::vector<GLuint> indices;

	// VBO ids

	GLuint vertexBufferObjects[3];

	GLuint indexBufferObject;

	// VAO id
	GLuint vertexArrays;

	void init(std::string fn);

	void handleFace(std::string str, GLuint& vertice, GLuint& texCoord, GLuint& normal);

	void render();

	void release();
};

namespace std {
	template<> struct hash<MeshOBJ::Vertex> {
		size_t operator()(const MeshOBJ::Vertex& vm) const {
			size_t h1 = hash<glm::vec3>()(vm.vertex);
			size_t h2 = hash<glm::vec2>()(vm.texCoord);
			size_t h3 = hash<glm::vec3>()(vm.normal);
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};
}

struct Mesh {
	// File information
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<GLuint> indencies;
	// VBO ids
	GLuint vertexBufferObjects[3];
	// Index id
	GLuint indexBufferObject;
	// Vertex Array
	GLuint vertexArray;

	void init(std::string fn);

	void render();

	void release();
};

struct Shader {
	GLuint id;

	void init(GLenum type, std::string fn);

	void release();
};

struct Program {

	std::vector<Shader*> shaders;

	GLuint id;

	std::map<std::string, GLuint> uniforms;

	void attach(Shader* shader);

	void init();

	void bind();

	void unbind();

	void release();

	void create_uniform(std::string name);

	// Integers
	void set1i(std::string name, int i);

	void set2i(std::string name, const glm::ivec2& v);

	void set3i(std::string name, const glm::ivec3& v);

	void set4i(std::string name, const glm::ivec4& v);

	// Floats
	void set1f(std::string name, float i);

	void set2f(std::string name, const glm::vec2& v);

	void set3f(std::string name, const glm::vec3& v);

	void set4f(std::string name, const glm::vec4& v);

	// Matrices
	void setMat2f(std::string name, const glm::mat2& m);

	void setMat3f(std::string name, const glm::mat3& m);

	void setMat4f(std::string name, const glm::mat4& m);

};

struct Camera {
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
		float speed);

	void render(Program& program);

	void update(float delta);

	glm::vec3 getPos();

	glm::vec2 getRot();

	float getSence();

	float getSpeed();

	void setPos(const glm::vec3& pos);

	void setRot(const glm::vec2& rot);

	void setSence(float sence);

	void setSpeed(float speed);
};