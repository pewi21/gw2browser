#version 330 core

/*
** Levels control (input (+gamma), output)
** http://blog.mouaif.org/2009/01/28/levels-control-shader/
** Copyright (c) 2012, Romain Dura romain@shazbits.com
**
** Permission to use, copy, modify, and/or distribute this software for any
** purpose with or without fee is hereby granted, provided that the above
** copyright notice and this permission notice appear in all copies.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
** WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
** MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
** SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
** WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
** ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
** IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
#define GammaCorrection(color, gamma)											pow(color, vec3(1.0f/gamma))
#define LevelsControlInputRange(color, minInput, maxInput)						min(max(color - vec3(minInput), vec3(0.0f)) / (vec3(maxInput) - vec3(minInput)), vec3(1.0f))
#define LevelsControlInput(color, minInput, gamma, maxInput)					GammaCorrection(LevelsControlInputRange(color, minInput, maxInput), gamma)
#define LevelsControlOutputRange(color, minOutput, maxOutput)					mix(vec3(minOutput), vec3(maxOutput), color)
#define LevelsControl(color, minInput, gamma, maxInput, minOutput, maxOutput)	LevelsControlOutputRange(LevelsControlInput(color, minInput, gamma, maxInput), minOutput, maxOutput)

// End of levels control code

struct Material {
	sampler2D diffuseMap;
	sampler2D normalMap;
	sampler2D lightMap;
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

float alpha_threshold = 0.5f;

void main( ) {
	vec4 finalColor;
	vec4 diffuseColor;
	vec3 alphaColor;

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

	// GW2 store specular and alpha in texture's alpha channel
	alphaColor = vec3( diffuseColor.a );

	if ( mode.lighting ) {
		// Get normal
		vec3 normal;
		if ( mode.normalMapping ) {
			// Obtain normal from normal map in range [0,1]
			normal = texture( material.normalMap, fs_in.TexCoords ).rgb;
			// Transform normal vector to range [-1,1]
			normal = normalize( normal * 2.0f - 1.0f );
		} else {
			normal = normalize( fs_in.Normal );
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
			// Extract specular map from diffuse texture's alpha channel
			vec3 specularTexture = LevelsControl( alphaColor, 128.0f / 255.0f, 1.0f, 255.0f / 255.0f, 0.0f, 255.0f / 255.0f );

			specular = light.specular * spec * specularTexture;
		}

		// Extract alpha from diffuse texture's alpha channel
		vec3 alphaMask = LevelsControl( alphaColor, 0.0f, 1.0f, 64.0f / 255.0f, 0.0f, 255.0f / 255.0f );

		finalColor = vec4( ambient + ( diffuse + specular ), alphaMask.r );

	} else {
		finalColor = diffuseColor;
	}

	// Alpha test
	if ( finalColor.a < alpha_threshold ) {
		discard;
	}

	FragColor = finalColor;
}
