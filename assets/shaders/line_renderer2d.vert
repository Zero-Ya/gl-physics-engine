#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 u_Projection;

out vec4 Color;

void main() {
	gl_Position = u_Projection * vec4(aPos, 0.0, 1.0);
	Color = vec4(aColor, 1.0);
}