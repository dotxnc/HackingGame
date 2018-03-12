#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0; // base
uniform sampler2D depthtexture; // depth
uniform sampler2D noisetexture; // noise
// uniform sampler2D noiseTexture;

uniform float m_Intensity = 0.9f;
uniform float m_Base = 0.15f;
uniform float m_Area = 0.0075f;
uniform float m_Falloff = 0.000001f;
uniform float m_Radius = 0.06f;

out vec4 finalColor;

const float near = 0.1f;
const float far = 100.f;
float linearDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return depth;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

vec3 normalFromDepth(float depth)
{
    const vec2 offset1 = vec2(0.0, 1);
    const vec2 offset2 = vec2(1, 0.0);
    
    float depth1 = linearDepth(texture(depthtexture, fragTexCoord+offset1).r);
    float depth2 = linearDepth(texture(depthtexture, fragTexCoord+offset2).r);
    
    vec3 p1 = vec3(offset1, depth1-depth);
    vec3 p2 = vec3(offset2, depth2-depth);
    
    vec3 normal = cross(p1, p2);
    normal.z = -normal.z;
    return normalize(normal);
}

vec3 reflection(vec3 v1,vec3 v2)
{
    vec3 result= 2.0 * dot(v2, v1) * v2;
    result=v1-result;
    return result;
}

void main()
{
    vec3 sample_sphere[16] = vec3[]( vec3( 0.5381, 0.1856,-0.4319), vec3( 0.1379, 0.2486, 0.4430),
                                      vec3( 0.3371, 0.5679,-0.0057), vec3(-0.6999,-0.0451,-0.0019),
                                      vec3( 0.0689,-0.1598,-0.8547), vec3( 0.0560, 0.0069,-0.1843),
                                      vec3(-0.0146, 0.1402, 0.0762), vec3( 0.0100,-0.1924,-0.0344),
                                      vec3(-0.3577,-0.5301,-0.4358), vec3(-0.3169, 0.1063, 0.0158),
                                      vec3( 0.0103,-0.5869, 0.0046), vec3(-0.0897,-0.4940, 0.3287),
                                      vec3( 0.7119,-0.0154,-0.0918), vec3(-0.0533, 0.0596,-0.5411),
                                      vec3( 0.0352,-0.0631, 0.5460), vec3(-0.4776, 0.2847,-0.0271)
                                    );
                                    
    vec3 random = normalize( texture(noisetexture, fragTexCoord * 4.0).rgb );
    float depth = texture(depthtexture, fragTexCoord).r;
    depth = linearDepth(depth);
    vec3 position = vec3(fragTexCoord.x,fragTexCoord.y,depth);
    vec3 normal = normalFromDepth(depth);
    normal = clamp(normal, 0.0, 1.0);
    
    
    float radiusDepth = m_Radius/depth;
    float occlusion = 0.0;

    int iterations = 16;
    for (int j = 0; j < iterations; ++j)
    {
        vec3 ray = radiusDepth * reflection(sample_sphere[j], random);
            vec3 hemiRay = position + sign(dot(ray,normal)) * ray;
    
            float occDepth = texture(depthtexture, clamp(hemiRay.xy,0.0,1.0)).r;
            float difference = depth - occDepth;
    
            occlusion += step(m_Falloff, difference) * (1.0-smoothstep(m_Falloff, m_Area, difference));
    }

    float ao = 1.0 - m_Intensity * occlusion * (1.0 / iterations);
    float final = clamp(ao + m_Base,0.0,1.0);
    vec4 ssao = vec4(final,final,final,1);
    
    vec4 c = texture(texture0, fragTexCoord);
    finalColor = ssao;//vec4(depth, depth, depth, 1);//vec4(c.r*ssao.r, c.g*ssao.r, c.b*ssao.r, 1.0);
    // finalColor = vec4(normal, 1);
    // finalColor = vec4(depth, depth, depth, 1);
    
}
