#version 330 core
out vec4 FragColor;
in vec4 toColor;

void main()
{
    FragColor = toColor; // set alle 4 vector values to 1.0
}