#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

uniform sampler2D texture0;

uniform vec3 lightDirection = {1.f, -0.3f, -0.5f};
uniform vec4 diffuseLightColor = {1.f, 1.f, 1.f, 1.f};
uniform vec4 ambientLight = {0.15f, 0.15f, 0.15f, 1.f};

out vec4 finalColor;


void main()
{
    vec4 textureColor;
    vec4 color;
    vec3 lightDir;
    float lightIntensity;
    
    textureColor = texture(texture0, fragTexCoord);
    color = ambientLight;
    
    lightDir = -lightDirection;
    lightIntensity = clamp(dot(fragNormal, lightDir), 0.0f, 1.0f);
    
    if (lightIntensity > 0.0f) {
        color += (diffuseLightColor * lightIntensity);
    }
    
    color = clamp(color, 0.0f, 1.0f);
    
    finalColor = color*textureColor;
    
}
