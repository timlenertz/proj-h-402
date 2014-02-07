#version 330 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

out vec3 fragment_color;
out bool discard_point;

uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform bool fog;
uniform vec3 background_color;
uniform float fog_distance;
uniform bool shadow;
uniform uint shadow_point_size;
uniform float maximal_shadow_distance;

float minimal_fog_distance = fog_distance;
float maximal_fog_distance = 6.0 * fog_distance;
float shadow_offset = 0.02;

void main() {
	vec4 v = vec4(vertex_position, 1);
	vec4 view_vertex_position = view_matrix * v;
	
	float distance = length(view_vertex_position);
		
	discard_point = false;
	gl_Position = projection_matrix * view_matrix * v;
	if(shadow && distance > maximal_shadow_distance) {
		discard_point = true;
	} else if(shadow) {
		gl_PointSize = shadow_point_size;
		gl_Position[2] += shadow_offset;
		fragment_color = background_color;
	} else if(fog) {
		gl_PointSize = 1;
		float fog_factor = (distance - minimal_fog_distance) / (maximal_fog_distance - minimal_fog_distance);
		fog_factor = clamp(fog_factor, 0.0, 0.8);
		fragment_color = mix(vertex_color, background_color, fog_factor);
	} else {
		gl_PointSize = 1;
		fragment_color = vertex_color;
	} 
}
