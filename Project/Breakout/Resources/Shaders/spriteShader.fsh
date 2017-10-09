#version 330 core

in vec2 TexCoords;

uniform sampler2D Texture0;
uniform vec4 color;

void main()
{
    gl_FragColor = texture(Texture0, TexCoords) * color;
}