#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;

out vec4 finalColor;

float gamma = 0.6;
float numColors = 24.0;

void main()
{
    vec4 c1 = texture(texture0, fragTexCoord);
    vec3 c = pow(c1.rgb, vec3(gamma));
    c = c*numColors;
    c = floor(c);
    c = c/numColors;
    c = pow(c, vec3(1.0/gamma));
    
    if (c1==vec4(0, 0, 0, 0)) {
        // finalColor = vec4(c1, 0);
        // return;
        discard;
    }
    
    finalColor = vec4(c, c1.a);
}
