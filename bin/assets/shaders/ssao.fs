#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;
// uniform sampler2D noiseTexture;

out vec4 finalColor;

vec3 normalFromDepth(float depth)
{
    const vec2 offset1 = vec2(0.0, 0.001);
    const vec2 offset2 = vec2(0.001, 0.0);
    
    float depth1 = texture(texture0, fragTexCoord+offset1).r;
    float depth2 = texture(texture0, fragTexCoord+offset2).r;
    
    vec3 p1 = vec3(offset1, depth1-depth);
    vec3 p2 = vec3(offset2, depth2-depth);
    
    vec3 normal = cross(p1, p2);
    normal.z = -normal.z;
    return normalize(normal);
}

void main()
{
    // finalColor = vec4(normalFromDepth(texture(texture0, fragTexCoord).r), 0);
    finalColor = texture(texture0, fragTexCoord);
}
