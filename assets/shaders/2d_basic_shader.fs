#version 330 core
out vec4 FragColor;

in vec3 ourColor;

uniform vec4 color;

void main()
{
    FragColor = vec4(ourColor.x * color.x, ourColor.y * color.y, ourColor.z * color.z, 1.0);
}