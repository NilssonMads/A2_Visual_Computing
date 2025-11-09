#version 330 core

// Input from vertex shader
in vec2 UV;

// Output color
out vec3 color;

// Texture sampler
uniform sampler2D textureSampler;

// Filter and transformation parameters
uniform int filterMode;        // 0=none, 1=pixelate, 2=grayscale
uniform int pixelSize;
uniform float uTranslateX;
uniform float uTranslateY;
uniform float uRotation;
uniform float uScale;

// Apply geometric transformation to UV coordinates
vec2 applyTransformation(vec2 uv) {
    // Center the coordinates
    vec2 centered = uv - 0.5;
    
    // Apply scale
    centered /= uScale;
    
    // Apply rotation
    float cosAngle = cos(uRotation);
    float sinAngle = sin(uRotation);
    vec2 rotated;
    rotated.x = centered.x * cosAngle - centered.y * sinAngle;
    rotated.y = centered.x * sinAngle + centered.y * cosAngle;
    
    // Apply translation (inverted to match expected behavior)
    rotated.x += uTranslateX;
    rotated.y -= uTranslateY;
    
    // Move back to [0,1] range
    return rotated + 0.5;
}

// Pixelation filter
vec3 applyPixelation(vec2 uv, int size) {
    vec2 texSize = textureSize(textureSampler, 0);
    float pixelSizeX = float(size) / texSize.x;
    float pixelSizeY = float(size) / texSize.y;
    
    vec2 pixelatedUV;
    pixelatedUV.x = floor(uv.x / pixelSizeX) * pixelSizeX;
    pixelatedUV.y = floor(uv.y / pixelSizeY) * pixelSizeY;
    
    return texture(textureSampler, pixelatedUV).rgb;
}

// Grayscale filter
vec3 applyGrayscale(vec3 color) {
    float gray = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
    return vec3(gray);
}

void main() {
    // Apply geometric transformation
    vec2 transformedUV = applyTransformation(UV);
    
    // Check if UV is out of bounds after transformation
    if (transformedUV.x < 0.0 || transformedUV.x > 1.0 || 
        transformedUV.y < 0.0 || transformedUV.y > 1.0) {
        color = vec3(0.1, 0.1, 0.2); // Background color
        return;
    }
    
    // Sample texture based on filter mode
    vec3 texColor;
    
    if (filterMode == 1) {
        // Pixelation filter
        texColor = applyPixelation(transformedUV, pixelSize);
    } else {
        // No filter or grayscale - sample normally
        texColor = texture(textureSampler, transformedUV).rgb;
        
        if (filterMode == 2) {
            // Grayscale filter
            texColor = applyGrayscale(texColor);
        }
    }
    
    color = texColor;
}