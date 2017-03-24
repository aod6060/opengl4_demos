#include "sys.h"


// Texture2D
void Texture2D::init(std::string fn, GLenum min_filter, GLenum mag_filter, bool mipmap) {
	SDL_Surface* surface = IMG_Load(fn.c_str());

	if (surface == 0) {
		std::cout << fn << " doesn't exist..." << std::endl;
		return;
	}

	this->width = surface->w;
	this->height = surface->h;

	GLenum format = GL_RGB;

	if (surface->format->BytesPerPixel == 4) {
		format = GL_RGBA;
	}

	glGenTextures(1, &id);

	bind();

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		format,
		this->width,
		this->height,
		0,
		format,
		GL_UNSIGNED_BYTE,
		surface->pixels
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

	if (mipmap) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	unbind();

	SDL_FreeSurface(surface);
}

void Texture2D::bind(GLenum e) {
	glActiveTexture(e);
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture2D::unbind(GLenum e) {
	glActiveTexture(e);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::release() {
	glDeleteTextures(1, &id);
}

GLuint Texture2D::getWidth() {
	return this->id;
}

GLuint Texture2D::getHeight() {
	return this->id;
}

GLuint Texture2D::getID() {
	return this->id;
}

// MeshOBJ

bool MeshOBJ::Vertex::operator == (const MeshOBJ::Vertex& v) const {
	return this->vertex == v.vertex && this->texCoord == v.texCoord && this->normal == v.normal;
}

void MeshOBJ::init(std::string fn) {
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
	std::unordered_map<Vertex, GLuint> uniqueVertex;
	std::vector<Vertex> vertices;

	// Compute Vertices

	for (int i = 0; i < f.size(); i++) {
		for (int j = 0; j < 3; j++) {
			Vertex vertex = {};

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

}

void MeshOBJ::handleFace(std::string str, GLuint& vertice, GLuint& texCoord, GLuint& normal) {
	std::vector<std::string> temp;
	util_strStrip(str, '/', temp);
	vertice = util_toInt(temp[0]) - 1;
	texCoord = util_toInt(temp[1]) - 1;
	normal = util_toInt(temp[2]) - 1;
	temp.clear();
}

void MeshOBJ::render() {
	glBindVertexArray(this->vertexArrays);
	//glDrawArrays(GL_TRIANGLES, 0, vertices_list.size());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBufferObject);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MeshOBJ::release() {
	glDeleteVertexArrays(1, &this->vertexArrays);
	glDeleteBuffers(1, &this->indexBufferObject);
	glDeleteBuffers(3, this->vertexBufferObjects);
}
// Mesh
void Mesh::init(std::string fn) {
	// Will do something with it later.
	//std::vector<std::string> content;
	std::vector<std::string> temp;
	//util_loadTextFile(fn, content);
	std::ifstream in(fn.c_str());

	std::string str;

	while (std::getline(in, str)) {
		util_strStrip(str, ' ', temp);

		if (temp[0] == "v") {
			glm::vec3 vertex;
			glm::vec2 texCoord;
			glm::vec3 normal;
			glm::vec3 tangent;
			glm::vec3 bitangent;
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
			// Grab Tangents
			tangent.x = util_toFloat(temp[9]);
			tangent.y = util_toFloat(temp[10]);
			tangent.z = util_toFloat(temp[11]);
			// Grab Bitangents
			bitangent.x = util_toFloat(temp[12]);
			bitangent.x = util_toFloat(temp[13]);
			bitangent.x = util_toFloat(temp[14]);
			// Load Values into lists
			vertices.push_back(vertex);
			texCoords.push_back(texCoord);
			normals.push_back(normal);
			tangents.push_back(tangent);
			bitangents.push_back(bitangent);
		}
		else if (temp[0] == "i") {
			int index = util_toInt(temp[1]);
			indencies.push_back(index);
		}

		temp.clear();
	}

	in.close();

	// Generate VBOs
	glGenBuffers(5, this->vertexBufferObjects);
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
	// Create Tangent Buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjects[3]);
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), tangents.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Create BiTangent Buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjects[4]);
	glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), bitangents.data(), GL_STATIC_DRAW);
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
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
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
	// Tangent
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjects[3]);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// Bitangent
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObjects[4]);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// Unbind Vertex Attrib Array
	glBindVertexArray(0);
	// Unbind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Disable VertexAttribArrays
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
}

void Mesh::render() {
	glBindVertexArray(this->vertexArray);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBufferObject);
	glDrawElements(GL_TRIANGLES, indencies.size(), GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::release() {
	// Delete Vertex Array
	glDeleteVertexArrays(1, &vertexArray);
	// Delete Index Buffer
	glDeleteBuffers(1, &this->indexBufferObject);
	// Delete Vertex Buffer Object
	glDeleteBuffers(5, this->vertexBufferObjects);
}
// Shader
void Shader::init(GLenum type, std::string fn) {
	id = glCreateShader(type);
	std::string src = util_loadTextFile(fn);
	//std::cout << src << std::endl;
	const char* csrc = src.c_str();
	glShaderSource(id, 1, &csrc, 0);
	glCompileShader(id);
	util_printShaderLog(id);
}

void Shader::release() {
	glDeleteShader(id);
}

// Program
void Program::attach(Shader* shader) {
	shaders.push_back(shader);
}

void Program::init() {
	this->id = glCreateProgram();
	for (auto& shader : shaders) {
		glAttachShader(this->id, shader->id);
	}
	glLinkProgram(this->id);
	util_printProgramLog(this->id);
}

void Program::bind() {
	glUseProgram(this->id);
}

void Program::unbind() {
	glUseProgram(0);
}

void Program::release() {
	for (auto& shader : shaders) {
		glDetachShader(this->id, shader->id);
	}
	glDeleteProgram(this->id);
	for (auto& shader : shaders) {
		shader->release();
	}
	shaders.clear();
}

void Program::create_uniform(std::string name) {
	this->uniforms[name] = glGetUniformLocation(id, name.c_str());
}

// Integers
void Program::set1i(std::string name, int i) {
	glUniform1i(this->uniforms[name], i);
}

void Program::set2i(std::string name, const glm::ivec2& v) {
	glUniform2i(this->uniforms[name], v.x, v.y);
}

void Program::set3i(std::string name, const glm::ivec3& v) {
	glUniform3i(this->uniforms[name], v.x, v.y, v.z);
}

void Program::set4i(std::string name, const glm::ivec4& v) {
	glUniform4i(this->uniforms[name], v.x, v.y, v.z, v.w);
}

// Floats
void Program::set1f(std::string name, float i) {
	glUniform1f(this->uniforms[name], i);
}

void Program::set2f(std::string name, const glm::vec2& v) {
	glUniform2f(this->uniforms[name], v.x, v.y);
}

void Program::set3f(std::string name, const glm::vec3& v) {
	glUniform3f(this->uniforms[name], v.x, v.y, v.z);
}

void Program::set4f(std::string name, const glm::vec4& v) {
	glUniform4f(this->uniforms[name], v.x, v.y, v.z, v.w);
}

// Matrices
void Program::setMat2f(std::string name, const glm::mat2& m) {
	glUniformMatrix2fv(this->uniforms[name], 1, GL_FALSE, &m[0][0]);
}

void Program::setMat3f(std::string name, const glm::mat3& m) {
	glUniformMatrix3fv(this->uniforms[name], 1, GL_FALSE, &m[0][0]);
}

void Program::setMat4f(std::string name, const glm::mat4& m) {
	glUniformMatrix4fv(this->uniforms[name], 1, GL_FALSE, &m[0][0]);
}

// Camera
void Camera::init(
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

void Camera::render(Program& program) {
	program.setMat4f("projection", this->projection);

	glm::quat xrot = glm::angleAxis(glm::radians(rot.x), Vector3Utils::right);
	glm::quat yrot = glm::angleAxis(glm::radians(rot.y), Vector3Utils::up);

	glm::mat4 rot = glm::toMat4(xrot * yrot);

	glm::mat4 translate = glm::translate(-pos);

	view = rot * translate;

	program.setMat4f("view", view);
}

void Camera::grabMatrices(glm::mat4& view, glm::mat4& projection) {
	glm::quat xrot = glm::angleAxis(glm::radians(rot.x), Vector3Utils::right);
	glm::quat yrot = glm::angleAxis(glm::radians(rot.y), Vector3Utils::up);

	glm::mat4 rot = glm::toMat4(xrot * yrot);

	glm::mat4 translate = glm::translate(-pos);

	projection = this->projection;

	view = rot * translate;

}

void Camera::update(float delta) {

	if (input_isMouseGrab()) {
		glm::vec2 mc;
		input_getMouseCoords(mc);

		float actualSencs = this->sence * 100.0f;
		float actualSpeed = this->speed * 100.0f;

		rot.y += mc.x * actualSencs * (0.001f + delta);
		rot.x += mc.y * actualSencs * (0.001f + delta);

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

glm::vec3 Camera::getPos() {
	return this->pos;
}

glm::vec2 Camera::getRot() {
	return this->rot;
}

float Camera::getSence() {
	return this->sence;
}

float Camera::getSpeed() {
	return this->speed;
}

void Camera::setPos(const glm::vec3& pos) {
	this->pos = pos;
}

void Camera::setRot(const glm::vec2& rot) {
	this->rot = rot;
}

void Camera::setSence(float sence) {
	this->sence = sence;
}

void Camera::setSpeed(float speed) {
	this->speed = speed;
}

glm::vec3 Camera::getForward() {
	return glm::vec3(
		view[2][0],
		view[2][1],
		view[2][2]
	);
}