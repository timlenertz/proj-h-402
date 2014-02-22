#ifndef DYPC_RENDERER_PROGRAM_H_
#define DYPC_RENDERER_PROGRAM_H_

#include "../gl.h"
#include "shader.h"

#include <string>
#include <initializer_list>

namespace dypc {

class program {
private:
	GLuint id_;
	
	void link_();
	
public:
	program(std::initializer_list<shader> shaders);
	~program();
	
	GLint getParameter(GLenum par) const {
		GLint res;
		glGetProgramiv(id_, par, &res);
		return res;
	}
	
	std::string infoLog() const;

	GLuint id() const { return id_; }
	GLint uniform_location(const std::string& name) const { return glGetUniformLocation(id_, name.c_str()); }

	void use() const { glUseProgram(id_); }
};

}

#endif
