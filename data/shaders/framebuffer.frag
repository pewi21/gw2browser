#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float exposure;
uniform float whitePoint;

vec3 Uncharted2Tonemap( vec3 x ) {
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;

    return ( ( x * ( A * x + C * B ) + D * E ) / ( x * ( A * x + B ) + D * F ) ) - E / F;
}

void main( ) {
    //const float gamma = 2.2f;
    //const float exposure = 6.0f;
    //const float whitePoint = 11.2f;

    vec3 hdrColor = texture( screenTexture, TexCoords ).rgb;
    // Uncharted 2 tone mapping
    // from http://filmicworlds.com/blog/filmic-tonemapping-operators/
    vec3 whiteScale = 1.0f / Uncharted2Tonemap( vec3( whitePoint ) );
    vec3 result = Uncharted2Tonemap( hdrColor * exposure ) * whiteScale;

    // Gamma correction
    //result = pow( result, vec3( 1.0f / gamma ) );

    FragColor = vec4( result, 1.0f );
}
