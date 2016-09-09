#version 330 compatibility

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projMatrix;

varying vec2 v_texcoord;
varying vec3 v_normal;

void main()
{
  //v_texcoord = gl_MultiTexCoord[0];
  v_normal = gl_Normal.xyz;

  gl_Position = u_projMatrix * u_viewMatrix * u_modelMatrix * gl_Vertex;
}