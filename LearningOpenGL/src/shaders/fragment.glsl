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
    // Componente ambiente
    float ambientStrength = 0.1;
    vec3 ambient = ambientColor * ambientStrength;

    // Componente difuso
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    
    vec3 diffuseColor;
    diffuseColor = texture(Texture, TexCoord).rgb;
    vec3 diffuse = diff * diffuseColor;

    // Componente especular
    float specularStrength = 0.4;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0); // Luz blanca especular

    // Resultado final
    vec3 result = ambient + diffuse + specular;
    if(!hasTexture) {
        result = Color; 
    }
    FragColor = vec4(result, 1.0);
}