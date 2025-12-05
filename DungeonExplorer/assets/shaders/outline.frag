#version 330 core

uniform sampler2D texture;
uniform vec2 textureSize;
uniform vec4 outlineColor;
uniform float outlineThickness;
uniform bool enableOutline;

in vec2 texCoord;
out vec4 fragColor;

void main() {
    vec4 color = texture2D(texture, texCoord);
    
    if (!enableOutline) {
        fragColor = color;
        return;
    }
    
    // Sobel edge detection using alpha channel
    float offset = outlineThickness / textureSize.x;
    
    // Sample surrounding pixels in 3x3 grid
    float alpha = 0.0;
    for (float x = -1.0; x <= 1.0; x += 1.0) {
        for (float y = -1.0; y <= 1.0; y += 1.0) {
            if (x == 0.0 && y == 0.0) continue;  // Skip center
            vec2 samplePos = texCoord + vec2(x, y) * offset;
            alpha += texture2D(texture, samplePos).a;
        }
    }
    
    // If current pixel is transparent but neighbors are opaque, draw outline
    if (color.a < 0.1 && alpha > 0.1) {
        fragColor = outlineColor;
    } else {
        fragColor = color;
    }
}
