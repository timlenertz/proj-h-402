#version 330 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform vec3 fog_color;
out vec3 fragment_color;

const float minimal_fog_distance = 70.0;
const float maximal_fog_distance = 250.0;

void main() {
	vec4 v = vec4(vertex_position, 1);
	vec4 view_vertex_position = view_matrix * v;
	
	float distance = length(view_vertex_position);
	float fog_factor = (distance - minimal_fog_distance) / (maximal_fog_distance - minimal_fog_distance);
	fog_factor = clamp(fog_factor, 0.0, 0.8);
	
	gl_Position = projection_matrix * view_matrix * v;	
	fragment_color = mix(vertex_color, fog_color, fog_factor);
}
