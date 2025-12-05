#version 330 core

uniform sampler2D texture;
uniform vec4 flashColor;
uniform float flashIntensity;  // 0.0 to 1.0

in vec2 texCoord;
out vec4 fragColor;

void main() {
    vec4 texColor = texture2D(texture, texCoord);
    
    // Blend texture color with flash color based on intensity
    // Preserve alpha channel from original texture
    vec3 blended = mix(texColor.rgb, flashColor.rgb, flashIntensity);
    fragColor = vec4(blended, texColor.a);
}
