#version 330
#pragma GL_ARB_shading_language_420pack : enable

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;
in vec3 fragPos;

uniform sampler2D texture0;

uniform vec3 lightDirection = vec3(1.f, -0.3f, -0.5f);
uniform vec4 diffuseLightColor = vec4(1.f, 1.f, 1.f, 1.f);
uniform vec4 ambientLight = vec4(0.15f, 0.15f, 0.15f, 1.f);

uniform vec3 viewPos;

out vec4 finalColor;

float fog(const float dist, const float density)
{
    const float l2 = -1.442695;
    float d = density*dist;
    return 1.0 - clamp(exp2(d*d*l2), 0.0, 1.0);
}

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
    
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectdir = reflect(-lightDir, fragNormal);
    float spec = pow(max(dot(viewDir, reflectdir), 0.0), 1);
    vec3 specular = 0.1*spec*diffuseLightColor.rgb;
    
    color = clamp(color+vec4(specular, 1.0), 0.0f, 1.0f);
    
    vec4 c = color*textureColor;
    float fogDistance = gl_FragCoord.z/gl_FragCoord.w;
    float fogAmount = fog(fogDistance, 0.05f);
    const vec4 fogColor = vec4(0.0);
    
    finalColor = vec4(mix(c, fogColor, fogAmount).rgb, 1.0);
    
}
