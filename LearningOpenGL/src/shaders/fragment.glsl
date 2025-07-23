#version 330 core

out vec4 FragColor;

in vec3 Color;
in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos; 

uniform bool hasTexture;
uniform vec3 ambientColor;
uniform sampler2D Texture;
uniform vec3 lightPos; 
uniform vec3 viewPos; 

void main()
{
    float ambientStrength = 0.8;
    vec3 ambient = ambientColor * ambientStrength;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * Color * ambientColor;

    if(hasTexture) {
        vec4 result = texture(Texture,TexCoord) * vec4(ambient + diffuse, 1.0f);
        FragColor = result;
    }
    else {
        FragColor = vec4(Color, 1.0f);
    }
}