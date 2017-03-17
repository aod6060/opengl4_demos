#include "sys.h"

std::string util_loadTextFile(std::string cap) {
	std::ifstream in(cap.c_str());
	std::stringstream ss;
	std::string temp;
	while (std::getline(in, temp)) {
		ss << temp << std::endl;
	}
	in.close();
	return ss.str();
}

void util_loadTextFile(std::string fn, std::vector<std::string>& out) {
	std::ifstream in(fn.c_str());
	std::string temp;
	while (std::getline(in, temp)) {
		out.push_back(temp);
	}
	in.close();
}

void util_strStrip(std::string str, char delim, std::vector<std::string>& out) {
	std::stringstream ss(str);
	std::string temp;
	while (std::getline(ss, temp, delim)) {
		out.push_back(temp);
	}
}

int util_toInt(std::string value) {
	return atoi(value.c_str());
}

float util_toFloat(std::string value) {
	return atof(value.c_str());
}

double util_toDouble(std::string value) {
	return atof(value.c_str());
}

bool util_toBool(std::string value) {
	return (value == "true") ? true : false;
}

void util_printShaderLog(GLuint id) {
	int len;
	char infoLog[1024];
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);

	if (len > 0) {
		glGetShaderInfoLog(id, len, 0, infoLog);

		std::cout << infoLog << std::endl;
	}
}

void util_printProgramLog(GLuint id) {
	int len;
	char infoLog[1024];
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		glGetProgramInfoLog(id, len, 0, infoLog);
		std::cout << infoLog << std::endl;
	}
}
