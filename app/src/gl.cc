#include "gl.h"

namespace dypc {

void initialize_glew() {
	static bool done = false;
	if(done) return;
	glewExperimental = GL_TRUE;
	auto err = glewInit();
	if(err != GLEW_OK) throw gl_error(std::string("Failed to initialize GLEW: ") + (const char*)glewGetErrorString(err));
	done = true;
}

}
