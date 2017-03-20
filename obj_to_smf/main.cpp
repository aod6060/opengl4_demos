#include <sys.h>
// Structures
// Face
struct Face {
	GLuint vertex[3];
	GLuint texCoord[3];
	GLuint normal[3];
};

// Vertex for future releases (add tangent and bitangent to vertex to handle normal mapping)...
struct Vertex {
	glm::vec3 vertex;
	glm::vec2 texCoord;
	glm::vec3 normal;

	bool operator == (const Vertex& other) const {
		return this->vertex == other.vertex && this->texCoord == other.texCoord && this->normal == other.normal;
	}
};

// Object Fiel
struct MeshData {
	std::vector<glm::vec3> vertexList;
	std::vector<glm::vec2> texCoordList;
	std::vector<glm::vec3> normalList;
	std::vector<GLuint> indexList;
};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator() (const Vertex& vm) const {
			size_t h1 = hash<glm::vec3>()(vm.vertex);
			size_t h2 = hash<glm::vec2>()(vm.texCoord);
			size_t h3 = hash<glm::vec3>()(vm.normal);
			return h1 ^ (h2 << 1) & (h3 << 2);
		}
	};
}

// This will convet the OBJ file to SMF...
static void convertObjMeshToSMF(std::string fn, std::string converted);

static void saveMeshToSMF(MeshData& mesh, std::string fn);

static void handleFace(std::string str, GLuint& vertice, GLuint& texCoord, GLuint& normal);

int main(int argc, char** argv) {


	if (argc < 2) {
		std::cout << "obj_to_mesh [*.obj] [*.smf]" << std::endl;
		return 1;
	}
	else {
		convertObjMeshToSMF(argv[1], argv[2]);
	}
}

void convertObjMeshToSMF(std::string fn, std::string converted) {
	std::cout << fn << " " << converted << std::endl;
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
	std::vector<GLuint> indices;

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

	MeshData mesh;

	// Load vertices in mesh object
	for (int i = 0; i < vertices.size(); i++) {
		mesh.vertexList.push_back(vertices[i].vertex);
		mesh.texCoordList.push_back(vertices[i].texCoord);
		mesh.normalList.push_back(vertices[i].normal);
	}

	// Load Indexs for mesh
	for (int i = 0; i < indices.size(); i++) {
		mesh.indexList.push_back(indices[i]);
	}

	saveMeshToSMF(mesh, converted);
}

void handleFace(std::string str, GLuint& vertice, GLuint& texCoord, GLuint& normal) {
	std::vector<std::string> temp;
	util_strStrip(str, '/', temp);
	vertice = util_toInt(temp[0]) - 1;
	texCoord = util_toInt(temp[1]) - 1;
	normal = util_toInt(temp[2]) - 1;
	temp.clear();
}

void saveMeshToSMF(MeshData& mesh, std::string fn) {
	std::ofstream out(fn.c_str());

	// Save Vertex Data to File
	for (int i = 0; i < mesh.vertexList.size(); i++) {
		glm::vec3 vertex = mesh.vertexList[i];
		glm::vec2 texCoord = mesh.texCoordList[i];
		glm::vec3 normal = mesh.normalList[i];

		out << "v " << vertex.x << " " << vertex.y << " " << vertex.z << " " << texCoord.x << " " << texCoord.y << " " << normal.x << " " << normal.y << " " << normal.z << std::endl;
	}

	// Save Indexs to file
	for (int i = 0; i < mesh.indexList.size(); i++) {
		out << "i " << mesh.indexList[i] << std::endl;
	}

	out.close();
}