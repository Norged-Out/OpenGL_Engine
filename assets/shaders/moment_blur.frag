#version 330 core

in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D sourceTexture;
uniform vec2 texelSize;
uniform vec2 blurDirection;

void main()
{
    // Convert the chosen blur direction into a one-texel UV offset
    vec2 offset = texelSize * blurDirection;

    // Fixed 5-tap Gaussian blur:
    // center sample gets the most weight,
    // first neighbors get less,
    // second neighbors get the least
    vec4 result = texture(sourceTexture, texCoord) * 0.4026;
    result += texture(sourceTexture, texCoord + offset * 1.0) * 0.2442;
    result += texture(sourceTexture, texCoord - offset * 1.0) * 0.2442;
    result += texture(sourceTexture, texCoord + offset * 2.0) * 0.0545;
    result += texture(sourceTexture, texCoord - offset * 2.0) * 0.0545;

    // Output the blurred moment sample
    fragColor = result;
}
