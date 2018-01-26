#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;
uniform sampler2D texture0;
uniform float time;
out vec4 finalColor;

#define distortion 0.15

vec2 barrelDistortion(vec2 coord) {
      vec2 cc = coord - 0.5;
      float dist = dot(cc, cc);
      return 0.5 + cc * (1.0 + (dist + distortion * dist * dist) * distortion) / (1.0 + (0.25 + distortion * 0.25 * 0.25) * distortion);
}

void main()
{
    vec2 tcoord = barrelDistortion(fragTexCoord * 512 / 512) * 512 / 512;
    
    vec4 c = texture(texture0, tcoord);
    vec3 scanlines = vec3( sin(tcoord.y*256*1.7+time) + sin((tcoord.y*1.)*12.0+time)*0.3 );
    if (scanlines.x < 0.1) {
        scanlines = vec3(0.0);
    } else {
        scanlines= vec3(1.0);
    }
    
    finalColor = c * vec4((1. + scanlines*0.1) * vec3(0.5,1.0,0.5), 1.0);
}
