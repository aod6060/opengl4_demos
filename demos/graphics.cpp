#include "sys.h"


// Texture2D
void Texture2D::init(std::string fn) {
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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