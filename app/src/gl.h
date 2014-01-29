#ifndef DYPC_GL_H_
#define DYPC_GL_H_

#include <type_traits>
#include <system_error>
#include <string>

#include <GL/glew.h>

namespace dypc {

class gl_error_category : public std::error_category {
public:
	const char* name() const noexcept { return "opengl"; }
	std::string message(int condition) const noexcept { return ""; }
};

class gl_error : public std::system_error {
public:
	explicit gl_error(GLenum err) :
	std::system_error(std::error_code(err, gl_error_category())) { }
	
	explicit gl_error(const std::string& what) :
	std::system_error(std::error_code(0, gl_error_category()), what) {}
};

void initialize_glew();

}

#endif
