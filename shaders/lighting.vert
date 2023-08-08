#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * model * vec4(aPos, 1.0f);
}