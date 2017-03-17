#pragma once

struct Texture2D {
public:

	GLuint id;
	GLuint width;
	GLuint height;

	void init(std::string fn);

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