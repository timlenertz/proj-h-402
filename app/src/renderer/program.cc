#include "program.h"

#include <memory>

namespace dypc {
	
void program::link_() {
	glLinkProgram(id_);
	if(! getParameter(GL_LINK_STATUS)) throw gl_error(std::string("Failed to link program. Info log: " + infoLog()));
}

std::string program::infoLog() const {
	GLsizei maxlen = getParameter(GL_INFO_LOG_LENGTH);
	GLsizei len;
	if(len == 0) return "";
	std::unique_ptr<GLchar[]> log(new GLchar[len]);
	glGetProgramInfoLog(id_, maxlen, &len, log.get());
	return std::string((char*)log.get(), len);
}

program::program(std::initializer_list<shader> shaders) {
	id_ = glCreateProgram();
	for(const shader& sh : shaders) glAttachShader(id_, sh.id());
	link_();
}

program::~program() {
	glDeleteProgram(id_);
}

}
