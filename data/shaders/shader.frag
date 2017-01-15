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
	bool wireframe;
	bool textured;
	bool normalMapping;
	bool lighting;
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
	vec4 diffuseColor;

	vec4 black = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	vec4 white = vec4( 1.0f, 1.0f, 1.0f, 1.0f );

	if ( mode.textured ) {
		if ( mode.wireframe ) {
			diffuseColor = black;
		} else {
			diffuseColor = texture( material.diffuseMap, fs_in.TexCoords );
		}
	} else {
		if ( mode.wireframe ) {
			diffuseColor = black;
		} else {
			diffuseColor = white;
		}
	}

	if ( mode.lighting ) {
		// Get normal
		vec3 normal;
		if ( mode.normalMapping ) {
			// Obtain normal from normal map in range [0,1]
			normal = texture( material.normalMap, fs_in.TexCoords ).rgb;
			// Transform normal vector to range [-1,1]
			normal = normalize( normal * 2.0f - 1.0f );
		} else {
			normal = fs_in.Normal;
		}

		// Get diffuse color
		vec3 color = diffuseColor.rgb;
		// Ambient
		vec3 ambient = light.ambient * color;
		// Diffuse
		vec3 lightDir;
		if ( mode.normalMapping ) {
			lightDir = normalize( fs_in.TangentLightPos - fs_in.TangentFragPos );
		} else {
			lightDir = normalize( light.position - fs_in.FragPos );
		}
		float lambertian = max( dot( lightDir, normal ), 0.0f );
		vec3 diffuse = light.diffuse * lambertian * color;
		// Specular
		vec3 specular;
		if ( lambertian > 0.0f ) {
			vec3 viewDir;
			if ( mode.normalMapping ) {
				viewDir = normalize( fs_in.TangentViewPos - fs_in.TangentFragPos );
			} else {
				viewDir = normalize( viewPos - fs_in.FragPos );
			}
			// Blinn–Phong shading
			vec3 halfwayDir = normalize( lightDir + viewDir );
			float spec = pow( max( dot( normal, halfwayDir ), 0.0f ), material.shininess );
			// GW2 store specular in diffuse texture's alpha channel
			vec3 specularTexture = vec3( texture( material.diffuseMap, fs_in.TexCoords ).a );
			specular = light.specular * spec * specularTexture;
		}

		finalColor = vec4( ambient + ( diffuse + specular ), texture( material.diffuseMap, fs_in.TexCoords ).a );
	} else {
		finalColor = diffuseColor;
	}

	// Alpha test
	if ( finalColor.a < alpha_threshold ) {
		discard;
	}

	FragColor = finalColor;
}
