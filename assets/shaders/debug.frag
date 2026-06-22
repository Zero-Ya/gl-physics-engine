#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 u_Color;

void main() {
    vec2 uv = TexCoords * 2.0 - 1.0;

    float distanceSq = dot(uv, uv);

    float radius = 0.8;
    float thickness = 0.02;

    float alpha = 1.0 - smoothstep(radius * radius, (radius + thickness) * (radius + thickness), distanceSq);

    FragColor = vec4(1.0, 0.5, 0.0, alpha); 
}