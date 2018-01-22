#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;
uniform sampler2D texture0;
uniform float time;
out vec4 finalColor;
void main()
{
    vec4 c = texture(texture0, fragTexCoord);
    vec3 scanlines = vec3( sin(fragTexCoord.y*256*1.7+time) + sin((fragTexCoord.y*1.)*12.0+time)*0.3 );
    if (scanlines.x < 0.2) {
        scanlines = vec3(0.0);
    } else {
        scanlines= vec3(1.0);
    }
    
    finalColor = c * vec4((1. + scanlines*0.1) * vec3(0.5,1.0,0.5), 1.0);
}
