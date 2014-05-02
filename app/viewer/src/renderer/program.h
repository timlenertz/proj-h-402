#ifndef DYPC_RENDERER_PROGRAM_H_
#define DYPC_RENDERER_PROGRAM_H_

#include "../gl.h"
#include "shader.h"

#include <string>
#include <initializer_list>

namespace dypc {

/**
 * OpenGL program composed of shaders.
 */
class program {
private:
	GLuint id_; ///< OpenGL ID for program.
	
	void link_(); ///< Link the shaders into the program.
	
public:
	program(std::initializer_list<shader> shaders); ///< Create program from shaders created previously.
	~program(); ///< Deletes the OpenGL program.
	
	/**
	 * Get read parameter from program.
	 * @param par OpenGL parameter to read.
	 * @return OpenGL output value.
	 */
	GLint getParameter(GLenum par) const {
		GLint res;
		glGetProgramiv(id_, par, &res);
		return res;
	}
	
	/**
	 * Get OpenGL information log.
	 * Generated when program is linked.
	 */
	std::string infoLog() const;

	GLuint id() const { return id_; } ///< Get OpenGL ID for program.
	GLint uniform_location(const std::string& name) const { return glGetUniformLocation(id_, name.c_str()); } ///< Get OpenGL ID for uniform variable of program.

	void use() const { glUseProgram(id_); } ///< Use the program in rendering.
};

}

#endif
