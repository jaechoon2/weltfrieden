#version 330


uniform float iGlobalTime;
uniform vec2 iResolution;
uniform float gain;
uniform float shape;
uniform float speed;

in vec4 gl_FragCoord;
//out vec4 fragColor;
uniform sampler2D tex;
layout(location = 0) out vec4 fragColor;



mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main()
{
  mat4 rot = rotationMatrix(vec3(0.5,0.5,0.5), shape);

  vec4 color = vec4(0.2, 0.3,0.5 + 0.5 * sin(iGlobalTime)*shape, gain);
  fragColor =  vec4 (1,1,1,1) - vec4(rot*color);
}