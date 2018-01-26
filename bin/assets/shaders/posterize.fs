#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;

out vec4 finalColor;

float gamma = 0.6;
float numColors = 24.0;

void main()
{
    vec3 c = texture(texture0, fragTexCoord).rgb;
    c = pow(c, vec3(gamma));
    c = c*numColors;
    c = floor(c);
    c = c/numColors;
    c = pow(c, vec3(1.0/gamma));
    finalColor = vec4(c, 1.0);
}
