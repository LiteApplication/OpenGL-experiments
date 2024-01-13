#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

// texture samplers
uniform sampler2D texture_atlas;

struct Light
{
		vec3 position;
		vec3 color;
		float ambientIntensity;
    float diffuseIntensity;
		int specularExponent;
		float specularIntensity;
};
uniform Light light;
uniform vec3 camera_position;

void main() {
  vec4 fragment_color	 = texture(texture_atlas, TexCoord);

  // ambient
  vec3 ambient = light.ambientIntensity * light.color;

  // diffuse
  vec3 lightDir = normalize(light.position - FragPos);
  float diff = max(dot(Normal, lightDir), 0.0);
  vec3 diffuse = diff * light.color * light.diffuseIntensity;

  // specular
  vec3 viewDir = normalize(camera_position - FragPos);
  vec3 reflectDir = reflect(-lightDir, Normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), light.specularExponent);
  vec3 specular = light.specularIntensity * spec * light.color;

  vec3 result = (ambient + diffuse + specular) * fragment_color.xyz	;
  FragColor = vec4(result, fragment_color.a);

  //FragColor.xyz = Normal;

}
