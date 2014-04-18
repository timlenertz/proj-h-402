#ifndef DYPC_PROJECTION_CANVAS_H_
#define DYPC_PROJECTION_CANVAS_H_

#include "canvas.h"
#include "../geometry/triangle.h"
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/norm.hpp>

namespace dypc {

template<class Value, std::size_t Width, std::size_t Height>
class projection_canvas : public canvas<Value, Width, Height> {
private:
	using pixel = typename canvas<Value, Width, Height>::pixel;
	
	glm::mat4 matrix_;

public:
	projection_canvas() = default;
	explicit projection_canvas(const glm::mat4& mat) : matrix_(mat) { }
	
	const glm::mat4& get_matrix() const { return matrix_; }
	void set_matrix(const glm::mat4& mat) { matrix_ = mat; }
	
	pixel project(glm::vec3) const;
	
	bool in_range(glm::vec3 pt) const { return canvas<Value, Width, Height>::in_range(project(pt)); }
	
	Value operator[](glm::vec3 pt) const { return canvas<Value, Width, Height>::operator[](project(pt)); }
	Value& operator[](glm::vec3 pt) { return canvas<Value, Width, Height>::operator[](project(pt)); }
	
	void fill_triangle(Value v, const triangle& tri) {
		fill_triangle(v, tri.a, tri.b, tri.c);
	}
	
	void fill_triangle(Value v, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
		canvas<Value, Width, Height>::fill_triangle(v, project(a), project(b), project(c));
	}
};



template<class Value, std::size_t Width, std::size_t Height>
auto projection_canvas<Value, Width, Height>::project(glm::vec3 pt) const -> pixel {
	glm::vec4 proj(pt.x, pt.y, pt.z, 1.0);
	proj = matrix_ * proj;
	proj.x /= proj.w;
	proj.y /= proj.w;
	
	if(isnan(proj.x) || isnan(proj.y)) return pixel::invalid(); 
	
	pixel px;
	px.x = Width * (1.0 + proj.x) / 2.0;
	px.y = Height * (1.0 + proj.y) / 2.0;
	return px;
}


}

#endif
