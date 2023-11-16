#version 400

/* ==============================================================================
        Stage Inputs
 ==============================================================================
 */
// Everything in world coordinates
in FragData {
  vec3 position;
  vec3 normal;
  vec2 texCoords;
  mat3 TBN;
}
vsIn;

/* ==============================================================================
        Structures
 ==============================================================================
 */
struct Light {
  vec3 position; // Light position / direction for dir lights
  float auxA;
  vec3 emission; // Non normalized emission (already multiplied by intensity)
  int type;
  bool state; // On/off flag
};

/* ==============================================================================
        Uniforms
 ==============================================================================
 */
uniform rendererBlock {
  float gamma;
  float exposure;

  // Uncharted tone map parameters (see common.fs)
  float A, B, C, D, E, J, W;

  bool perturbNormals;
  bool envLighting;
};

uniform cameraBlock {
  mat4 ViewMatrix;
  mat4 ProjMatrix;
  mat4 ViewProjMatrix;
  vec3 ViewPos;
};

const int NUM_LIGHTS = 4;

uniform lightBlock { Light lights[NUM_LIGHTS]; };

// Material parameters
uniform sampler2D diffuseTex;
uniform sampler2D normalTex;
uniform sampler2D metallicTex;
uniform sampler2D roughTex;

uniform vec3 diffuse;
uniform float metallic;
uniform float roughness;
uniform vec3 spec;

// IBL precomputation
uniform samplerCube irradianceTex;
uniform samplerCube ggxTex;
uniform sampler2D brdfTex;

/* ==============================================================================
        Imports
 ==============================================================================
 */
vec3 toLinearRGB(vec3 c, float gamma);
vec3 simpleToneMap(vec3 c, float exp);
vec3 unchartedTonemap(vec3 c, float exp);
vec3 unchartedTonemapParam(vec3 c, float exp, float A, float B, float C,
                           float D, float E, float J, float W);
vec3 toInverseGamma(vec3 c, float gamma);
vec3 fresnelSchlickUnreal(float cosTheta, vec3 F0);

float geoGGX(float NdotV, float roughness);
float geoSmith(vec3 N, vec3 V, vec3 L, float roughness);
float distGGX(vec3 N, vec3 H, float roughness);

const float PI = 3.14159265358979;

/* ==============================================================================
        Stage Outputs
 ==============================================================================
 */
out vec4 outColor;

vec3 perturbNormal(in sampler2D normalMap) {
  vec3 normal = texture(normalMap, vsIn.texCoords).rgb;
  normal = normal * 2.0 - 1.0;
  return normalize(vsIn.TBN * normal);

  // Fetch normal from map and adjust to linear space
  /*vec3 normal = texture(normalMap, vsIn.texCoords).xyz * 2.0 - 1.0;

  // Calculate uv derivatives
  vec2 duvdx = dFdx(vsIn.texCoords);
  vec2 duvdy = dFdy(vsIn.texCoords);

  // Calculate position derivatives
  vec3 dpdx = dFdx(vsIn.position);
  vec3 dpdy = dFdy(vsIn.position);

  // Find tangent from derivates and build the TBN basis
  vec3 N = normalize(vsIn.normal);
  vec3 T = normalize(dpdx * duvdy.t - dpdy * duvdx.t);
  vec3 B = -normalize(cross(N, T));

  return normalize(mat3(T, B, N) * normal);*/
}

const float MAX_GGX_LOD = 4.0;

float fetchParameter(sampler2D samp, float val) {
  if (val >= 0.0)
    return val;
  else
    return texture(samp, vsIn.texCoords).r;
}

vec3 fetchDiffuse() {
  if (diffuse.r >= 0)
    return diffuse;
  else
    return toLinearRGB(texture(diffuseTex, vsIn.texCoords).rgb, gamma);
}

void main(void) {
  vec3 V = normalize(ViewPos - vsIn.position);

  vec3 N = vec3(0.0);
  if (perturbNormals)
    N = perturbNormal(normalTex);
  else
    N = normalize(vsIn.normal);

  vec3 R = reflect(-V, N);

  float NdotV = max(dot(N, V), 0.0);

  float rough = fetchParameter(roughTex, roughness);
  float metal = fetchParameter(metallicTex, metallic);

  /* ==============================================================================
          Environment
  ==============================================================================
*/
  vec3 retColor = vec3(0.0);

  // Diffuse component
  vec3 kd = fetchDiffuse();
  vec3 irradiance = texture(irradianceTex, N).rgb;
  vec3 diffuse = kd * irradiance; // Appendix, formula X

  // Specular component
  vec3 F0 = mix(spec, kd, metal);
  vec3 F = fresnelSchlickUnreal(NdotV, spec);

  // Fetch precomputed integrals
  vec3 prefGGX = textureLod(ggxTex, R, rough * MAX_GGX_LOD).rgb;
  vec3 brdf = texture(brdfTex, vec2(NdotV, rough)).rgb;

  vec3 brdfInt = F0 * brdf.r + brdf.g; // Appendix, formula Y
  vec3 specular = prefGGX * brdfInt;   // Appendix, formula Z

  if (envLighting) {
    // Total ambient lighting
    retColor = (1.0 - F) * (1.0 - metal) * diffuse + specular;
  }

  /* ==============================================================================
          Lights
  ==============================================================================
*/
  vec3 Lrad = vec3(0.0);
  for (int i = 0; i < NUM_LIGHTS; ++i) {
    if (!lights[i].state)
      continue;

    vec3 toLight = lights[i].position - vsIn.position;

    vec3 L = normalize(toLight);
    vec3 H = normalize(V + L);

    float HdotV = max(dot(H, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);

    float dist = length(toLight);
    vec3 Li = lights[i].emission / (dist * dist);

    float fGGX = distGGX(N, H, rough);
    float Geo = geoSmith(N, V, L, rough);
    vec3 Fr = fresnelSchlickUnreal(HdotV, F0);

    vec3 nom = fGGX * Geo * Fr;
    float denom = 4 * NdotV * NdotL + 0.0001;
    vec3 contrib = nom / denom;

    // Compute scattering formula for the light
    Lrad += ((vec3(1.0) - Fr) * (1.0 - metal) * kd / PI + contrib) * Li * NdotL;
  }

  // Sum ambient and lights
  retColor = retColor + Lrad;

  /* ==============================================================================
          Post-processing
  ==============================================================================
*/
  // Tonemapping and gamma correction
  retColor = unchartedTonemapParam(retColor, exposure, A, B, C, D, E, J, W);
  retColor = toInverseGamma(retColor, gamma);

  outColor = vec4(retColor, 1.0);
}
