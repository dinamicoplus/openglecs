#version 330 core

out vec4 FragColor;
in vec3 Color;
in vec2 TexCoord;

uniform bool hasTexture;
uniform vec4 newColor;
uniform sampler2D Texture;

void main()
{
    if(hasTexture) {
        FragColor = texture(Texture,TexCoord) * vec4(Color, 1.0f) * newColor;
    }
    else {
        FragColor = vec4(Color, 1.0f) * newColor;
    }
}