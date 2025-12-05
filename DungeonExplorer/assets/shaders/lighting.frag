#version 330 core

uniform sampler2D texture;
uniform float ambientLevel;       // Ambient darkness level (0.0 - 1.0)

// Multi-light support
const int MAX_LIGHTS = 32;
uniform int lightCount;
uniform vec2 lightPositions[MAX_LIGHTS];   // Screen coordinates
uniform vec3 lightColors[MAX_LIGHTS];      // RGB (0-1)
uniform float lightRadiuses[MAX_LIGHTS];   // Pixels
uniform float lightIntensities[MAX_LIGHTS];// 0-1+

in vec2 texCoord;
out vec4 fragColor;

void main() {
    vec4 texColor = texture2D(texture, texCoord);
    vec2 fragPos = gl_FragCoord.xy;
    
    // Accumulate light from all sources
    vec3 totalLight = vec3(0.0);
    
    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (i >= lightCount) break;
        
        float dist = length(fragPos - lightPositions[i]);
        float radius = lightRadiuses[i];
        
        if (dist < radius) {
            // Smooth falloff
            float falloff = 1.0 - smoothstep(0.0, radius, dist);
            
            // Quadratic attenuation for more realistic light
            float attenuation = falloff * falloff;
            
            // Add light contribution
            totalLight += lightColors[i] * attenuation * lightIntensities[i];
        }
    }
    
    // Add ambient light
    totalLight += vec3(ambientLevel);
    
    // Clamp total light to prevent excessive blowout, but allow some brightness
    totalLight = clamp(totalLight, 0.0, 1.5);
    
    // Apply lighting to texture
    vec3 finalColor = texColor.rgb * totalLight;
    
    fragColor = vec4(finalColor, texColor.a);
}

