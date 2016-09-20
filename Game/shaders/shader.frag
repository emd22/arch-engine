#version 330

varying vec3 v_position;
varying vec2 v_texcoord;
varying vec3 v_normal;

uniform sampler2D u_diffuseTexture;
uniform vec3 u_cameraPosition;
uniform vec4 u_ambientColor;
uniform bool u_hasTexture;

uniform float u_shininess;
uniform float u_roughness;

struct PointLightInfo {
  vec3 position;
  vec4 color;
};

uniform PointLightInfo u_pointLight[4];
uniform int u_numPointLights;

float sqr(float x)
{
  return x * x;
}

float GGX(float alpha, float cosThetaM)
{
  const float PI = 3.141592654;
  float CosSquared = cosThetaM * cosThetaM;
  float TanSquared = (1.0 - CosSquared) / CosSquared;
  return (1.0 / PI) * sqr(alpha / (CosSquared * (alpha * alpha + TanSquared)));
}

float SchlickFresnel(float f0, float f90, float u)
{
  return f0 + (f90 - f0) * pow(1.0 - u, 5.0);
}

float CookTorranceG(float NdotL, float NdotV, float LdotH, float NdotH)
{
  return min(1.0, 2.0 * (NdotH / LdotH) * min(NdotL, NdotV));
}

float SpecularDirectional(vec3 n, vec3 v, vec3 lightDir, float roughness)
{
  vec3 vHalfVector = normalize(lightDir + v);

	float NdotH = dot(n, vHalfVector);
	float LdotH = dot(lightDir, vHalfVector);

	float D = GGX(roughness, NdotH);
	float F = SchlickFresnel(0.2, 1.0, LdotH);

	return F * D;
}

void main()
{
  
  
  vec4 lightColor = vec4(0.9, 0.7, 0.5, 1.0);
  vec3 lightdir = normalize(vec3(0.2, 1.0, 0.2));
  
  vec4 textureColor = vec4(1.0);
  if (u_hasTexture) {
    textureColor = texture2D(u_diffuseTexture, v_texcoord);
  }
  
  // ambient light
  vec4 ambient = u_ambientColor;
  
  // diffuse light
  float ndotl = min(max(dot(v_normal, lightdir), 0.0), 1.0);
  vec4 diffuse = (lightColor * ndotl + ambient) * textureColor;
  
  vec3 n = normalize(v_normal.xyz);
  vec3 v = normalize(u_cameraPosition - v_position.xyz);
  float specular = max(min(SpecularDirectional(n, v, lightdir, u_roughness), 1.0), 0.0);
    
  gl_FragColor = clamp(diffuse * (1.0-u_shininess) + 
    vec4(specular, specular, specular, 0.0) * u_shininess, 0.0, 1.0);
}