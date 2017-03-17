#include <sys.h>


/*
This was create to make it easier...
*/
struct Shader {
	GLuint vid;
	GLuint fid;
	GLuint pid;
};

struct VertexMesh_Test {
	glm::vec3 vertex;
	glm::vec2 texCoord;
	glm::vec3 normal;

	bool operator == (const VertexMesh_Test& other) const {
		return vertex == other.vertex && 
			   texCoord == other.texCoord &&
			   normal == other.normal;
	}
};

namespace std {
	template<> struct hash<VertexMesh_Test> {
		size_t operator()(const VertexMesh_Test& vm) const {
			size_t h1 = hash<glm::vec3>()(vm.vertex);
			size_t h2 = hash<glm::vec2>()(vm.texCoord);
			size_t h3 = hash<glm::vec3>()(vm.normal);
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};
}
struct MeshOBJ_Test {


	struct Face {
		GLuint vertex[3];
		GLuint texCoord[3];
		GLuint normal[3];
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

	void init(std::string fn) {

		int preTime = SDL_GetTicks();

		std::vector<std::string> contents;
		std::vector<std::string> temp;

		std::vector<glm::vec3> v;
		std::vector<glm::vec2> vt;
		std::vector<glm::vec3> vn;
		std::vector<Face> f;

		util_loadTextFile(fn, contents);

		for (GLint i = 0; i < contents.size(); i++) {
			util_strStrip(contents[i], ' ', temp);

			if (temp[0] == "v") {
				glm::vec3 t;
				t.x = util_toFloat(temp[1]);
				t.y = util_toFloat(temp[2]);
				t.z = util_toFloat(temp[3]);
				v.push_back(t);
			}

			else if (temp[0] == "vt") {
				glm::vec2 t;
				t.x = util_toFloat(temp[1]);
				t.y = 1.0f - util_toFloat(temp[2]);
				vt.push_back(t);
			}

			else if (temp[0] == "vn") {
				glm::vec3 t;
				t.x = util_toFloat(temp[1]);
				t.y = util_toFloat(temp[2]);
				t.z = util_toFloat(temp[3]);
				vn.push_back(glm::normalize(t));
			}

			else if (temp[0] == "f") {
				Face face;
				handleFace(temp[1], face.vertex[0], face.texCoord[0], face.normal[0]);
				handleFace(temp[2], face.vertex[1], face.texCoord[1], face.normal[1]);
				handleFace(temp[3], face.vertex[2], face.texCoord[2], face.normal[2]);
				f.push_back(face);
			}

			temp.clear();
		}

		// unorder_maps
		std::unordered_map<VertexMesh_Test, GLuint> uniqueVertex;
		std::vector<VertexMesh_Test> vertices;

		// Compute Vertices

		for (int i = 0; i < f.size(); i++) {
			for (int j = 0; j < 3; j++) {
				VertexMesh_Test vertex = {};

				vertex.vertex = v[f[i].vertex[j]];

				vertex.texCoord = vt[f[i].texCoord[j]];

				vertex.normal = vn[f[i].normal[j]];

				if (uniqueVertex.count(vertex) == 0) {
					uniqueVertex[vertex] = vertices.size();
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertex[vertex]);
			}
		}

		// Push them to vertex lists
		for (int i = 0; i < vertices.size(); i++) {
			this->vertices_list.push_back(vertices[i].vertex);
			this->texCoords_list.push_back(vertices[i].texCoord);
			this->normals_list.push_back(vertices[i].normal);
		}

		/*
		for (GLint i = 0; i < f.size(); i++) {
			// Vertices
			this->vertices_list.push_back(v[f[i].vertex.v1]);
			this->vertices_list.push_back(v[f[i].vertex.v2]);
			this->vertices_list.push_back(v[f[i].vertex.v3]);
			// TexCoords
			this->texCoords_list.push_back(vt[f[i].texcoord.v1]);
			this->texCoords_list.push_back(vt[f[i].texcoord.v2]);
			this->texCoords_list.push_back(vt[f[i].texcoord.v3]);
			// Normals
			this->normals_list.push_back(vn[f[i].normal.v1]);
			this->normals_list.push_back(vn[f[i].normal.v2]);
			this->normals_list.push_back(vn[f[i].normal.v3]);

			this->indices.push_back(this->indices.size());
			this->indices.push_back(this->indices.size());
			this->indices.push_back(this->indices.size());
		}
		*/



		glGenBuffers(3, this->vertexBufferObjects);
		// Create Vertex Buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjects[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices_list.size() * sizeof(glm::vec3), vertices_list.data(), GL_STATIC_DRAW);
		// Create TextureCoord Buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjects[1]);
		glBufferData(GL_ARRAY_BUFFER, texCoords_list.size() * sizeof(glm::vec2), texCoords_list.data(), GL_STATIC_DRAW);
		// Create Normal Buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjects[2]);
		glBufferData(GL_ARRAY_BUFFER, normals_list.size() * sizeof(glm::vec3), normals_list.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Create Indencies
		glGenBuffers(1, &this->indexBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Create Vertex Array
		glGenVertexArrays(1, &this->vertexArrays);

		glBindVertexArray(this->vertexArrays);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjects[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjects[1]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjects[2]);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);


		float delta = (SDL_GetTicks() - preTime) / 1000.0f;
		
		std::cout << "Loading Time: " << delta << std::endl;

		std::cout << "Number Vertices: " << vertices_list.size() << std::endl;

		std::cout << "Number Indencies: " << indices.size() << std::endl;

		std::cout << "Size of vec3:   " << sizeof(glm::vec3) << std::endl;
		std::cout << "Size of vec2:   " << sizeof(glm::vec2) << std::endl;
		std::cout << "Size of GLuint: " << sizeof(GLuint) << std::endl;
	}

	void handleFace(std::string str, GLuint& vertice, GLuint& texCoord, GLuint& normal) {
		std::vector<std::string> temp;
		util_strStrip(str, '/', temp);
		vertice = util_toInt(temp[0]) - 1;
		texCoord = util_toInt(temp[1]) - 1;
		normal = util_toInt(temp[2]) - 1;
		temp.clear();
	}

	void render() {
		glBindVertexArray(this->vertexArrays);
		//glDrawArrays(GL_TRIANGLES, 0, vertices_list.size());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBufferObject);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void release() {
		glDeleteVertexArrays(1, &this->vertexArrays);
		glDeleteBuffers(1, &this->indexBufferObject);
		glDeleteBuffers(3, this->vertexBufferObjects);
	}
};

class OptimizingMeshLoadingApp : public IApp {
public:

	//Triangle tris;

	Shader shader;

	// Texture Uniform
	GLuint texture_uniform;
	// Transform Uniforms
	GLuint projection_uniform;
	GLuint view_uniform;
	GLuint model_uniform;

	Texture2D texture;

	MeshOBJ_Test monkey;

	glm::mat4 projection;
	glm::mat4 view;
	//glm::mat4 model;

	float yrot = 0.0f;

	float maxTime = 1.0f;

	unsigned int frame = 0;
	float dt = 0.0f;

	virtual void init() {
		// Make sure that this is enabled
		glEnable(GL_DEPTH_TEST);
		//tris.init();
		// Create Shader
		shader.vid = glCreateShader(GL_VERTEX_SHADER);
		shader.fid = glCreateShader(GL_FRAGMENT_SHADER);
		std::string vsrc = util_loadTextFile("data/shaders/load_mesh/shader.vert");
		std::string fsrc = util_loadTextFile("data/shaders/load_mesh/shader.frag");
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
		monkey.init("data/meshes/loading_mesh/monkey.obj");
	}

	virtual void update(float delta) {
		// Use this some other time
		frame++;

		dt += delta;

		float fps = frame / dt;


		yrot += 90.0f * delta;


		if (yrot > 360.0f) {
			yrot -= 360.0f;
		}

		if (maxTime >= 1.0f) {
			//std::cout << "fps: " << fps << std::endl;
			maxTime = 0.0f;
			frame = 0;
			dt = 0;
		}
		else {
			maxTime += delta;
		}
	}

	virtual void render() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader.pid);

		glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(view_uniform, 1, GL_FALSE, &view[0][0]);


		glm::quat yrot = glm::angleAxis(glm::radians(this->yrot), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 trans = glm::translate(glm::vec3(0.0f, 0.0f, -3.0f));

		glm::mat4 rot = glm::toMat4(yrot);

		glm::mat4 model = trans * rot;

		glUniformMatrix4fv(model_uniform, 1, GL_FALSE, &model[0][0]);

		texture.bind();
		monkey.render();
		texture.unbind();

		glUseProgram(0);
	}

	virtual void release() {
		monkey.release();
		texture.release();
		glDetachShader(shader.pid, shader.vid);
		glDetachShader(shader.pid, shader.fid);
		//tris.release();
	}
};

int main(int argc, char** argv) {

	OptimizingMeshLoadingApp app;

	app_init("Optimizing Mesh Loading Application", 800, 600, &app);

	app_update();

	app_release();

	return 0;
}