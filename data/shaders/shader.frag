#version 330 core

struct Material {
	sampler2D diffuseMap;
	sampler2D normalMap;
	float     shininess;
};

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct RenderMode {
	int normalMapping;
	int lighting;
};

out vec4 FragColor;

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec3 Tangent;
	vec3 Bitangent;
	mat3 TBN;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} fs_in;

uniform vec3 viewPos;

uniform Material material;
uniform Light light;
uniform RenderMode mode;

float alpha_threshold = 0.1f;

void main( ) {
	vec4 finalColor;

	if ( mode.lighting ) {
		vec3 normal = fs_in.Normal;

		if ( mode.normalMapping ) {
			// Obtain normal from normal map in range [0,1]
			normal = texture( material.normalMap, fs_in.TexCoords ).rgb;
			// Transform normal vector to range [-1,1]
			normal = normalize( normal * 2.0f - 1.0f );
		}

		// Get diffuse color
		vec3 color = texture( material.diffuseMap, fs_in.TexCoords ).rgb;
		// Ambient
		vec3 ambient = light.ambient * color;
		// Diffuse
		vec3 lightDir = normalize( light.position - fs_in.FragPos );
		if ( mode.normalMapping ) {
			lightDir = normalize( fs_in.TangentLightPos - fs_in.TangentFragPos );
		}
		float diff = max( dot( lightDir, normal ), 0.0f );
		vec3 diffuse = light.diffuse * diff * color;

		// Specular
		vec3 viewDir = normalize( viewPos - fs_in.FragPos );
		if ( mode.normalMapping ) {
			viewDir = normalize( fs_in.TangentViewPos - fs_in.TangentFragPos );
		}
		vec3 reflectDir = reflect( -lightDir, normal );
		vec3 halfwayDir = normalize( lightDir + viewDir );
		float spec = pow( max( dot( normal, halfwayDir ), 0.0f ), material.shininess );
		// GW2 store specular texture in texture alpha channel
		vec3 specularTexture = vec3( texture( material.diffuseMap, fs_in.TexCoords ).a );
		vec3 specular = light.specular * spec * specularTexture;

		finalColor = vec4( ambient + diffuse + specular, texture( material.diffuseMap, fs_in.TexCoords ).a );
	} else {
		finalColor = texture( material.diffuseMap, fs_in.TexCoords );
	}

	// Alpha test
	if ( finalColor.a < alpha_threshold ) {
		discard;
	}

	FragColor = finalColor;
}
