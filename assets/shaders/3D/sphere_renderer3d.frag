#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec4 InstanceColor;

out vec4 FinalColor;

void main() {
    vec3 norm = normalize(Normal);

    FinalColor = vec4(norm * 0.5 + 0.5, 1.0);
}
