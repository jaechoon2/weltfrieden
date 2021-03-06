#version 330

uniform float now;
uniform vec2 res;
uniform float gain;
uniform float shape;
uniform float speed;
uniform float start;
uniform float end;
uniform float elapsed;
uniform float cps;

in vec4 gl_FragCoord;
layout(location = 0) out vec4 frag_color;


mat4 rotation_matrix(vec3 axis, float angle)
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
  vec2 uv = gl_FragCoord.xy / res.xy;
  if ((uv.y >= start && (uv.y <= end))) {
    mat4 rot = rotation_matrix(vec3(0.5,0.5,0.5), shape);
    vec4 color = vec4(uv, 0.5+0.5*sin(now*speed*cps), gain);
    frag_color =  vec4(rot*color);
  }
  else {
    frag_color = vec4(0,0,0,0.5 + 0.5 * sin(now));
  }
}
