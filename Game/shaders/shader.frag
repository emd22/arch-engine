#version 330 compatibility

varying vec2 texcoord;
varying vec3 v_normal;

uniform float red;

void main()
{
  vec3 lightdir = normalize(vec3(1.0, 1.0, 1.0));
  float ndotl = dot(v_normal, lightdir);
  gl_FragColor = vec4(vec3(ndotl), 1.0);
}