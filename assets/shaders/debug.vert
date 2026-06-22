#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 u_Model;
uniform mat4 u_Projection;

out vec2 TexCoords;

void main() {
	gl_Position = u_Projection * u_Model * vec4(aPos, 0.0, 1.0);
	TexCoords = vec2(aTexCoords.x, aTexCoords.y);
}