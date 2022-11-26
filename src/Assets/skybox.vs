#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 Perspective;
uniform mat4 View;


void main()
{
    TexCoords = aPos;
    gl_Position = Perspective * View * vec4(aPos, 1.0);
}
