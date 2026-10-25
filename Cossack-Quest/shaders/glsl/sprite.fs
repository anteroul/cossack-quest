#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 viewPos;
uniform float fogDensity;

// Output fragment color
out vec4 finalColor;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord)*colDiffuse*fragColor;

    // Sprites are cut out: drop transparent pixels so they don't write depth and hide what's behind them
    if (texelColor.a < 0.5) discard;

    // Same exponential fog as fog.fs
    const vec3 fogColor = vec3(0.05, 0.05, 0.05);
    float dist = length(viewPos - fragPosition);
    float fogFactor = clamp(1.0/exp((dist*fogDensity)*(dist*fogDensity)), 0.0, 1.0);

    finalColor = vec4(mix(fogColor, texelColor.rgb, fogFactor), 1.0);
}
