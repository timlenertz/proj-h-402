#version 330 core
in vec3 fragment_color;
in bool discard_point;
out vec3 color;

void main() {
	if(discard_point) discard;
	color = fragment_color;
}
