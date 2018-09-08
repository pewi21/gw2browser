#version 330 core

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

// Input vertex data, different for all executions of this shader.
layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec3 normal;
layout( location = 2 ) in vec2 texCoords;
layout( location = 3 ) in vec3 tangent;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    vec3 Bitangent;
    mat3 TBN;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 viewPos;

uniform Light light;
uniform RenderMode mode;

void main( ) {
    // Output position of the vertex, in clip space : MVP * position
    gl_Position = projection * view * model * vec4( position, 1.0f );
    // Position of the vertex, in worldspace : M * position
    vs_out.FragPos = vec3( model * vec4( position, 1.0f ) );
    // UV of the vertex. No special space for this one.
    vs_out.TexCoords = texCoords;

    // todo: for performance reason, inverse the matrix before send to shader
    mat3 normalMatrix = transpose( inverse( mat3( model ) ) );
    vs_out.Normal = normalMatrix * normal;

    if ( mode.normalMapping ) {
        vec3 T = normalize( mat3( model ) * tangent );
        vec3 N = normalize( mat3( model ) * normal );
        // Gram-Schmidt orthogonalize : re-orthogonalize T with respect to N
        T = normalize(T - dot(T, N) * N);
        // then retrieve perpendicular vector B with the cross product of T and N
        vec3 B = cross(T, N);

        mat3 TBN = transpose( mat3( T, B, N ) );

        vs_out.TBN = TBN;

        vs_out.TangentLightPos = TBN * light.position;
        vs_out.TangentViewPos = TBN * viewPos;
        vs_out.TangentFragPos = TBN * vs_out.FragPos;

        vs_out.Tangent = T;
        vs_out.Bitangent = B;
    }
}
