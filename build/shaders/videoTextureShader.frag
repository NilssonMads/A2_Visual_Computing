#version 330 core
in vec2 UV;
out vec4 color;                // ← change to vec4
uniform sampler2D textureSampler;
uniform int filterMode;
uniform int pixelSize;
uniform float uTranslateX;
uniform float uTranslateY;
uniform float uRotation;
uniform float uScale;

vec2 applyTransformation(vec2 uv) {
    vec2 centered = uv - 0.5;
    centered /= uScale;
    float cosAngle = cos(uRotation);
    float sinAngle = sin(uRotation);
    vec2 rotated;
    rotated.x = centered.x * cosAngle - centered.y * sinAngle;
    rotated.y = centered.x * sinAngle + centered.y * cosAngle;
    rotated.x -= uTranslateX;
    rotated.y -= uTranslateY;
    return rotated + 0.5;
}

vec3 applyPixelation(vec2 uv, int size) {
    vec2 texSize = textureSize(textureSampler, 0);
    float pixelSizeX = float(size) / texSize.x;
    float pixelSizeY = float(size) / texSize.y;
    vec2 pixelatedUV;
    pixelatedUV.x = floor(uv.x / pixelSizeX) * pixelSizeX;
    pixelatedUV.y = floor(uv.y / pixelSizeY) * pixelSizeY;
    return texture(textureSampler, pixelatedUV).rgb;
}

vec3 applyGrayscale(vec3 color) {
    float gray = dot(color, vec3(0.299, 0.587, 0.114));
    return vec3(gray);
}

void main() {
    vec2 transformedUV = applyTransformation(UV);
    if (transformedUV.x < 0.0 || transformedUV.x > 1.0 ||
        transformedUV.y < 0.0 || transformedUV.y > 1.0) {
        color = vec4(0.1, 0.1, 0.2, 1.0);   // ← add alpha
        return;
    }

    vec3 texColor;
    if (filterMode == 1)
        texColor = applyPixelation(transformedUV, pixelSize);
    else {
        texColor = texture(textureSampler, transformedUV).rgb;
        if (filterMode == 2)
            texColor = applyGrayscale(texColor);
    }

    color = vec4(texColor, 1.0);            // ← output vec4
}
