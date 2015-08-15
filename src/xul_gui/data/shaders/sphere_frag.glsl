// -*-Mode: C++;-*-
//
//  Default fragment shader for OpenGL ES2
//

// define default precision for float, vec, mat.
// precision highp float;

varying vec2 v_impos;
varying vec4 v_ecpos;
varying float v_radius;

vec4 Ambient;
vec4 Diffuse;
vec4 Specular;

void DirectionalLight(in int i, in vec3 normal)
{
  float nDotVP;         // normal . light direction
  float nDotHV;         // normal . light half vector
  float pf;             // power factor
  
  nDotVP = max(0.0, dot(normal,
                        normalize(vec3(gl_LightSource[i].position))));
  nDotHV = max(0.0, dot(normal, vec3(gl_LightSource[i].halfVector)));
  
  if (nDotVP == 0.0)
    pf = 0.0;
  else
    pf = pow(nDotHV, gl_FrontMaterial.shininess);
  
  Ambient  += gl_LightSource[i].ambient;
  Diffuse  += gl_LightSource[i].diffuse * nDotVP;
  Specular += gl_LightSource[i].specular * pf;
}

vec4 flight(in vec3 normal, in vec4 ecPosition, in vec4 matcol)
{
  vec4 color;
  vec3 ecPosition3;
  vec3 eye;

  ecPosition3 = (vec3 (ecPosition)) / ecPosition.w;
  eye = vec3 (0.0, 0.0, 1.0);

  // Clear the light intensity accumulators
  Ambient  = vec4 (0.0);
  Diffuse  = vec4 (0.0);
  Specular = vec4 (0.0);

  DirectionalLight(0, normal);

  color = gl_LightModel.ambient * matcol;
  color += Ambient  * matcol;
  color += Diffuse  * matcol;
  color += Specular * gl_FrontMaterial.specular;

  return color;
}

void main()
{
  float dist = length(v_impos);
  if (dist>1.0) {
    discard;
  }
  else {
    float nd = sqrt(1.0-dist*dist);
    vec3 normal = vec3(v_impos.xy, nd);

    float depth = nd * v_radius;

    float far=gl_DepthRange.far;
    float near=gl_DepthRange.near;

    vec4 ecpos = v_ecpos;
    ecpos.z += depth;
    vec4 clip_space_pos = gl_ProjectionMatrix * ecpos;

    float ndc_depth = clip_space_pos.z / clip_space_pos.w;

    gl_FragDepth = (((far-near) * ndc_depth) + near + far) / 2.0;

    vec4 col = flight(normal, ecpos, vec4(1.0, 1.0, 0.5, 1.0));

    gl_FragColor = col; //vec4(dist, 1.0-dist, 0.5, 1.0);//gl_Color;
  }
}
