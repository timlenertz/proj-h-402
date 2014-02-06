#version 330 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

out vec3 fragment_color;
out bool discard_point;

uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform vec3 fog_color;
uniform bool shadow;

const float minimal_fog_distance = 70.0;
const float maximal_fog_distance = 250.0;
const float maximal_shadow_distance = 125.0;

void main() {
	vec4 v = vec4(vertex_position, 1);
	vec4 view_vertex_position = view_matrix * v;
	
	float distance = length(view_vertex_position);
	float fog_factor = (distance - minimal_fog_distance) / (maximal_fog_distance - minimal_fog_distance);
	fog_factor = clamp(fog_factor, 0.0, 0.8);
	
	discard_point = false;
	gl_Position = projection_matrix * view_matrix * v;
	if(shadow && distance > maximal_shadow_distance) {
		discard_point = true;
	} else if(shadow) {
		gl_PointSize = 10;
		gl_Position[2] += 0.02;
		fragment_color = fog_color;
	} else {
		gl_PointSize = 1;
		fragment_color = mix(vertex_color, fog_color, fog_factor);
	} 
}
