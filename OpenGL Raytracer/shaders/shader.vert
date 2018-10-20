#version 330 core

layout(location = 0) in vec3 inpos;
out vec2 pos;

void main() {
	gl_Position = vec4(inpos, 1.0f);
    pos = inpos.xy;
}