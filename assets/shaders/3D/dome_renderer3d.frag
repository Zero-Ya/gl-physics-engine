#version 330 core

out vec4 FinalColor;

in vec3 vNormal;
in vec2 TexCoords;

uniform vec3 u_Color;

uniform sampler2D texture1;

void main() {
    FinalColor = texture(texture1, TexCoords) * vec4(1.0, 1.0, 1.0, 0.1);
}
