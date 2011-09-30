#version 330
layout(location = 0) in vec4 position;
out vec2 FragCoord;
void main()
{
	FragCoord = position.xy;
	gl_Position = position;
}