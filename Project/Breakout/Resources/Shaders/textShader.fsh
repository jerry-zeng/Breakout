#version 330 core

in vec2 TexCoords;

uniform sampler2D Texture0;
uniform vec4 textColor;

void main()
{
    float alpha = texture(Texture0, TexCoords).r;
    gl_FragColor = vec4(textColor.rgb, textColor.a * alpha);
}