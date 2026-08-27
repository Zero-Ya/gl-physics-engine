#version 330 core

// QuadVBO data
layout(location = 0) in vec2 aQuadVertex;
layout(location = 1) in vec2 aTexCoords;

// InstanceVBO data
layout(location = 2) in vec2 aPosition;
layout(location = 3) in vec3 aColor;
layout(location = 4) in float aRadius;

//uniform mat4 u_Model;
uniform mat4 u_Projection;

out vec2 TexCoords;
out vec2 v_Position;
out vec3 v_Color;
out float v_Radius;

void main() {
    v_Position = aQuadVertex * 2.0;
    v_Color    = aColor;
    v_Radius   = aRadius;

    vec2 worldPos = aPosition + (aQuadVertex * (aRadius * 2.0));

    gl_Position = u_Projection * vec4(worldPos, 0.0, 1.0);
	TexCoords = aTexCoords;
}