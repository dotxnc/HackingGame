#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;
in vec2 fragUV;
uniform sampler2D texture0;
uniform float time;
uniform ivec2 screensize = {512, 512};
out vec4 finalColor;

#define distortion 0.15

vec3 scanline(vec2 coord, vec3 screen){
    const float scale = .66;
    const float amt = 0.02;// intensity of effect
    const float spd = 1.0;//speed of scrolling rows transposed per second
    
	screen.rgb += sin((coord.y / scale - (time * spd * 6.28))) * amt;
	return screen;
}

vec2 fisheye(vec2 uv, float str )
{
    vec2 neg1to1 = uv;
    neg1to1 = (neg1to1 - 0.5) * 2.0;		
		
    vec2 offset;
    offset.x = ( pow(neg1to1.y,2.0)) * str * (neg1to1.x);
    offset.y = ( pow(neg1to1.x,2.0)) * str * (neg1to1.y);
	
    return uv + offset;	     
}

vec3 channelSplit(sampler2D tex, vec2 coord){
    const float spread = 0.008;
	vec3 frag;
	frag.r = texture(tex, vec2(coord.x - spread * sin(time), coord.y)).r;
	frag.g = texture(tex, vec2(coord.x, 					  coord.y)).g;
	frag.b = texture(tex, vec2(coord.x + spread * sin(time), coord.y)).b;
	return frag;
}

vec4 crt()
{
    vec4 c = vec4(1.0);
    vec2 fisheyeUV = fisheye(fragTexCoord, 0.03);
    c.rgb = channelSplit(texture0, fisheyeUV);
    vec2 screenSpace = fisheyeUV * screensize;
    c.rgb = scanline(screenSpace, c.rgb) * vec3(0.5, 1.0, 0.5);
    return c;
}

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
    finalColor = crt();
}
