#version 330 compatibility

varying vec2 texcoord;
varying vec3 v_normal;

uniform float red;

void main()
{
  gl_FragColor = vec4(v_normal, 1.0);
}