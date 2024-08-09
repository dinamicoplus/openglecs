#version 330 core

out vec4 FragColor;
in vec3 Color;
in vec2 TexCoord;

uniform vec4 newColor;
uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float value;

void main()
{
    FragColor = mix(texture(texture1,TexCoord), texture(texture2, TexCoord), value) * vec4(Color, 1.0f) * newColor;
}