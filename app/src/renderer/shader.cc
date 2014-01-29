#include "shader.h"

#include <memory>
#include <string>
#include <fstream>

#include <iostream>

namespace dypc {

shader shader::read_file(GLenum type, const std::string& filename) {
	std::string line, code;
	std::ifstream str(filename);
	while(std::getline(str, line)) code += line + '\n';
	str.close();
	return shader(type, code);
}

void shader::compile_(const std::string& code) {
	const GLchar* code_str = code.c_str();
	const GLint code_len = code.size();
	
	glShaderSource(id_, 1, &code_str, &code_len);
	glCompileShader(id_);	
	if(! getParameter(GL_COMPILE_STATUS)) throw gl_error(std::string("Failed to compile shader. Info log: \n" + infoLog()));
}

shader::shader(GLenum type, const std::string& code) {	
	id_ = glCreateShader(type);
	compile_(code);
}

shader::~shader() {
	glDeleteShader(id_);
}

std::string shader::infoLog() const {
	GLsizei maxlen = getParameter(GL_INFO_LOG_LENGTH);
	GLsizei len;
	if(len == 0) return "";
	std::unique_ptr<GLchar[]> log(new GLchar[len]);
	glGetShaderInfoLog(id_, maxlen, &len, log.get());
	return std::string((char*)log.get(), len);
}

}
