#ifndef DYPC_RENDERER_SHADER_H_
#define DYPC_RENDERER_SHADER_H_

#include "../gl.h"

#include <string>

namespace dypc {

class shader {
private:
	GLuint id_;
	
	void compile_(const std::string&);
	
public:
	static shader read_file(GLenum type, const std::string& filename);

	shader(GLenum type, const std::string& code);
	~shader();
	
	GLint getParameter(GLenum par) const {
		GLint res;
		glGetShaderiv(id_, par, &res);
		return res;
	}
	
	std::string infoLog() const;
	
	GLuint id() const { return id_; }
};
	
}

#endif
