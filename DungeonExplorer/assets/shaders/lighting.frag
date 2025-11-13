// Lighting Fragment Shader for Dungeon Explorer
// UPDATED: Clamped intensity to prevent over-saturation (0.4 - 0.9 range)
#version 330 core

uniform sampler2D texture;
uniform vec2 playerPosition;      // Player position in screen coordinates
uniform float lightRadius;        // Light radius in pixels
uniform float ambientLevel;       // Ambient darkness level (0.0 - 1.0)

in vec2 texCoord;
out vec4 fragColor;

void main() {
    // Sample the texture
    vec4 texColor = texture2D(texture, texCoord);
    
    // Calculate current fragment position in screen space
    vec2 fragPos = gl_FragCoord.xy;
    
    // Calculate distance from fragment to player light source
    float distance = length(fragPos - playerPosition);
    
    // Enhanced soft falloff using multiple smoothstep layers
    // Inner bright zone (0 - 30% of radius)
    float innerIntensity = 1.0 - smoothstep(0.0, lightRadius * 0.4, distance);
    
    // Outer gradient zone (30% - 100% of radius)
    float outerIntensity = 1.0 - smoothstep(lightRadius * 0.4, lightRadius * 1.2, distance);
    
    // Combine for ultra-soft falloff
    float intensity = mix(outerIntensity * 0.6, 1.0, innerIntensity);
    
    // Add ambient light to prevent total darkness
    float finalIntensity = mix(ambientLevel, 1.0, intensity);
    
    // CLAMP intensity to prevent over-saturation (0.4 = dark areas, 0.9 = max brightness)
    finalIntensity = clamp(finalIntensity, 0.4, 0.9);
    
    // Apply lighting to the texture color
    vec3 litColor = texColor.rgb * finalIntensity;
    
    // Enhanced warm tint with softer gradient (reduced strength)
    if (distance < lightRadius * 0.7) {
        float warmth = (1.0 - distance / (lightRadius * 0.7)) * 0.08;  // Reduced from 0.12
        litColor += vec3(warmth * 0.3, warmth * 0.2, warmth * 0.04);  // Softer tint
    }
    
    // Output final color with original alpha
    fragColor = vec4(litColor, texColor.a);
}

