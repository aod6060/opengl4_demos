#pragma once

std::string util_loadTextFile(std::string cap);

void util_loadTextFile(std::string fn, std::vector<std::string>& out);

void util_strStrip(std::string str, char delim, std::vector<std::string>& out);

int util_toInt(std::string value);

float util_toFloat(std::string value);

double util_toDouble(std::string value);

bool util_toBool(std::string value);

void util_printShaderLog(GLuint id);

void util_printProgramLog(GLuint id);
