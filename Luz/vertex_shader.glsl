#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout(location = 2) in vec3 normal;

uniform mat4 modelViewProjection;
uniform mat4 model;

out vec4 Color;
out vec4 Normal;
out vec4 FragmentPosition;

void main()
{
    Color = color;
	Normal = vec4(normal, 1.0);
	FragmentPosition = model * vec4(position, 1.0);
    gl_Position = modelViewProjection * vec4(position, 1.0);
	//gl_Position = vec4(position, 1.0);
}