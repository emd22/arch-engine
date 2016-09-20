#version 330

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projMatrix;

varying vec3 v_position;
varying vec2 v_texcoord;
varying vec3 v_normal;

void main()
{
  mat3 normalMatrix = mat3(transpose(inverse(u_modelMatrix)));
  
  v_position = (u_modelMatrix * vec4(a_position, 1.0)).xyz;
  v_texcoord = a_texcoord;
  v_normal = normalMatrix * a_normal;

  gl_Position = u_projMatrix * u_viewMatrix * u_modelMatrix * vec4(a_position, 1.0);
}