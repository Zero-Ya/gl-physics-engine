#version 330 core

// Per-Vertex Attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Per-Instance Attributes
layout (location = 3) in vec3  aInstancePos;
layout (location = 4) in float aInstanceRadius;
layout (location = 5) in vec4  aInstanceColor;
layout (location = 6) in vec4  aInstanceOrientation;

out vec3 FragPos;
out vec3 Normal;
out vec4 InstanceColor;

uniform mat4 u_Projection;

mat3 quaternionToRotationMatrix(vec4 q) {
    // Ensure the quaternion is normalized to prevent scaling artifacts
    q = normalize(q); 
    
    float x = q.x; float y = q.y; float z = q.z; float w = q.w;

    float xx = x * x; float yy = y * y; float zz = z * z;
    float xy = x * y; float xz = x * z; float yz = y * z;
    float wx = w * x; float wy = w * y; float wz = w * z;

    // Construct the 3x3 rotation matrix (Column-Major order)
    return mat3(
        1.0 - 2.0 * (yy + zz),       2.0 * (xy + wz),       2.0 * (xz - wy),
              2.0 * (xy - wz), 1.0 - 2.0 * (xx + zz),       2.0 * (yz + wx),
              2.0 * (xz + wy),       2.0 * (yz - wx), 1.0 - 2.0 * (xx + yy)
    );
}

void main() {
    InstanceColor = aInstanceColor;
    
    mat3 rotationMatrix = quaternionToRotationMatrix(aInstanceOrientation);

    // Scale unit sphere vertex by radius and translate to instance position
    vec3 worldPos = (aPos * rotationMatrix) * aInstanceRadius + aInstancePos;
    FragPos = worldPos;

    Normal = aNormal;

    gl_Position = u_Projection * vec4(worldPos, 1.0);
}
