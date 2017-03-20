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
	glm::vec3 tangent;
	glm::vec3 bitangent;

	bool operator == (const Vertex& other) const {
		return this->vertex == other.vertex && 
			this->texCoord == other.texCoord && 
			this->normal == other.normal && 
			this->tangent == other.tangent && 
			this->bitangent == other.bitangent;
	}
};

// Object Fiel
struct MeshData {
	std::vector<glm::vec3> vertexList;
	std::vector<glm::vec2> texCoordList;
	std::vector<glm::vec3> normalList;
	std::vector<glm::vec3> tangentList;
	std::vector<glm::vec3> bitangentList;
	std::vector<GLuint> indexList;
};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator() (const Vertex& vm) const {
			size_t h1 = hash<glm::vec3>()(vm.vertex);
			size_t h2 = hash<glm::vec2>()(vm.texCoord);
			size_t h3 = hash<glm::vec3>()(vm.normal);
			size_t h4 = hash<glm::vec3>()(vm.tangent);
			size_t h5 = hash<glm::vec3>()(vm.bitangent);
			return h1 ^ (h2 << 1) & (h3 << 2) & (h4 << 3) & (h5 << 4);
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

	// This is were the tangents and bitangents will be created...
	std::vector<glm::vec3> vtan;
	std::vector<glm::vec3> vbtan;


	// unorder_maps
	std::unordered_map<Vertex, GLuint> uniqueVertex;
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	// Compute Vertices
	for (int i = 0; i < f.size(); i++) {

		// Compute Tangents and Bi Tangents
		// Compute Edges
		glm::vec3 edge1 = v[f[i].vertex[1]] - v[f[i].vertex[0]];
		glm::vec3 edge2 = v[f[i].vertex[2]] - v[f[i].vertex[0]];
		// Compute UV Edges
		glm::vec2 deltaUV1 = vt[f[i].texCoord[1]] - vt[f[i].texCoord[0]];
		glm::vec2 deltaUV2 = vt[f[i].texCoord[2]] - vt[f[i].texCoord[0]];
		// Caculate f
		float value = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		// Tangents
		glm::vec3 tangent;
		tangent.x = value * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = value * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = value * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent = glm::normalize(tangent);
		glm::vec3 bitangent;
		bitangent.x = (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent = glm::normalize(bitangent);

		for (int j = 0; j < 3; j++) {
			Vertex vertex = {};

			vertex.vertex = v[f[i].vertex[j]];

			vertex.texCoord = vt[f[i].texCoord[j]];

			vertex.normal = vn[f[i].normal[j]];

			vertex.tangent = tangent;

			vertex.bitangent = bitangent;

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
		mesh.tangentList.push_back(vertices[i].tangent);
		mesh.bitangentList.push_back(vertices[i].bitangent);
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
		glm::vec3 tangent = mesh.tangentList[i];
		glm::vec3 bitangent = mesh.bitangentList[i];

		out << "v ";
		out << vertex.x << " " << vertex.y << " " << vertex.z << " ";
		out << texCoord.x << " " << texCoord.y << " ";
		out << normal.x << " " << normal.y << " " << normal.z << " ";
		out << tangent.x << " " << tangent.y << " " << tangent.z << " ";
		out << bitangent.x << " " << bitangent.y << " " << bitangent.z << " ";
		out << std::endl;
	}

	// Save Indexs to file
	for (int i = 0; i < mesh.indexList.size(); i++) {
		out << "i " << mesh.indexList[i] << std::endl;
	}

	out.close();
}