#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec2 v_Position;
in vec3 v_Color;
in float v_Radius;

void main() {
    float distance = length(v_Position);
    float edgeSoftness = fwidth(distance);
    float alpha = 1.0 - smoothstep(1.0 - edgeSoftness, 1.0, distance);

    if (alpha <= 0.0) {
        discard;
    }

    //vec2 uv = TexCoords * 2.0 - 1.0;
    //float distanceSq = dot(uv, uv);
    //float radius = v_Radius;
    //float thickness = 0.02;
    //float alpha = 1.0 - smoothstep(radius * radius, (radius + thickness) * (radius + thickness), distanceSq);

    FragColor = vec4(v_Color, alpha); 
}