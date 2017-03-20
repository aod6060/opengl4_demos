#include <sys.h>

/*

	This is a new mesh format that that will be used to fix loading speed of
	meshes

	Sucessful format will replace OBJ files pretty soon...
*/
struct Mesh_Test {
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

	void init(std::string fn) {
		// Will do something with it later.
		//std::vector<std::string> content;
		std::vector<std::string> temp;
		//util_loadTextFile(fn, content);
		// Load from file

		std::ifstream in(fn.c_str());

		std::string str;

		while (std::getline(in, str)) {
			util_strStrip(str, ' ', temp);

			if (temp[0] == "v") {
				glm::vec3 vertex;
				glm::vec2 texCoord;
				glm::vec3 normal;
				// Grab Vertex
				vertex.x = util_toFloat(temp[1]);
				vertex.y = util_toFloat(temp[2]);
				vertex.z = util_toFloat(temp[3]);
				// Grab TexCoord
				texCoord.x = util_toFloat(temp[4]);
				texCoord.y = util_toFloat(temp[5]);
				// Grab Normal
				normal.x = util_toFloat(temp[6]);
				normal.y = util_toFloat(temp[7]);
				normal.z = util_toFloat(temp[8]);
				// Load Values into lists
				vertices.push_back(vertex);
				texCoords.push_back(texCoord);
				normals.push_back(normal);
			}
			else if (temp[0] == "i") {
				int index = util_toInt(temp[1]);
				indencies.push_back(index);
			}

			temp.clear();
		}

		in.close();

		// Generate VBOs
		glGenBuffers(3, this->vertexBufferObjects);
		// Create Vertex Buffer
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjects[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
		// Create TextureCoord Buffer
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjects[1]);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
		// Create Normal Buffer
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjects[2]);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Create Index Buffer
		glGenBuffers(1, &this->indexBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indencies.size() * sizeof(GLuint), indencies.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		// Generate VA
		glGenVertexArrays(1, &vertexArray);
		// Bind VertexArray
		glBindVertexArray(this->vertexArray);
		// Load Into VA
		// Enable Shader VertexAttribArrays
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		// Bind Buffer and set it as a reference to vertexArray
		// Vertex
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjects[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		// TexCoord
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjects[1]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		// Normal
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjects[2]);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		// Unbind Vertex Attrib Array
		glBindVertexArray(0);
		// Unbind VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Disable VertexAttribArrays
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}

	void render() {
		glBindVertexArray(this->vertexArray);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBufferObject);
		glDrawElements(GL_TRIANGLES, indencies.size(), GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void release() {
		// Delete Vertex Array
		glDeleteVertexArrays(1, &vertexArray);
		// Delete Index Buffer
		glDeleteBuffers(1, &this->indexBufferObject);
		// Delete Vertex Buffer Object
		glDeleteBuffers(3, this->vertexBufferObjects);
	}
};

class OptimizingLoadingOfMeshesApp : public IApp {
private:
	// Shader
	Shader vertexShader;
	Shader fragmentShader;
	Program program;
	// Textures
	Texture2D dirt;
	Texture2D grass;
	Texture2D lava;
	Texture2D panel;
	Texture2D rubble;
	Texture2D snow;
	// MeshOBJ
	Mesh_Test barrel;
	Mesh_Test cube;
	Mesh_Test death_donut;
	Mesh_Test floor;
	Mesh_Test monkey;
	Mesh_Test sphere;
	// Camera
	Camera camera;
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

	void draw(Mesh_Test& mesh, Texture2D& tex, const glm::vec3& pos) {
		glm::mat4 model = glm::translate(pos);
		program.setMat4f("model", model);
		tex.bind();
		mesh.render();
		tex.unbind();
	}

};

int main(int argc, char** argv) {
	OptimizingLoadingOfMeshesApp app;
	app_init("Testing New Mesh Format (*.smf files) App", 1366, 768, &app);
	app_update();
	app_release();
	return 0;
}