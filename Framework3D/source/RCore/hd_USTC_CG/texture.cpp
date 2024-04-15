#include "texture.h"

#include "Utils/Logging/Logging.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
Texture2D::Texture2D()
{
    texture = nullptr;
}

Texture2D::Texture2D(SdfAssetPath path, HioImage::SourceColorSpace colorSpace)
    : textureFileName(path)
{
    texture = HioImage::OpenForReading(path.GetAssetPath(), 0, 0, colorSpace);
    if (texture) {
        logging(textureFileName.GetAssetPath() + " successfully loaded", Info);
        // Step 1: Get image information
        int width = texture->GetWidth();
        int height = texture->GetHeight();
        HioFormat format = texture->GetFormat();

        storageSpec.width = width;
        storageSpec.height = height;
        storageSpec.format = format;

        _component_count = HioGetComponentCount(storageSpec.format);
        storageSpec.data = malloc(width * height * texture->GetBytesPerPixel());
        if (!storageSpec.data) {
            return;
        }

        // Step 3: Read the image data
        if (!texture->Read(storageSpec)) {
            free(storageSpec.data);
        }
    }
    else {
        logging(textureFileName.GetAssetPath() + " not loaded", Info);
    }
}

static void _Interpolate(
    HioFormat format,
    float s,
    float t,
    const uint8_t *texel00,
    const uint8_t *texel01,
    const uint8_t *texel10,
    const uint8_t *texel11,
    float *dst)
{
    auto componentFormat = HioGetHioType(format);
    size_t componentCount = HioGetComponentCount(format);

    std::function<float(const uint8_t *, int bias)> preprocess;
    switch (componentFormat) {
        case HioTypeUnsignedByte:
            preprocess = [](const uint8_t *texel, int bias) { return texel[bias] / 256.0f; };
            break;
        case HioTypeUnsignedByteSRGB: break;
        case HioTypeSignedByte: break;
        case HioTypeUnsignedShort: break;
        case HioTypeSignedShort: break;
        case HioTypeUnsignedInt: break;
        case HioTypeInt: break;
        case HioTypeHalfFloat: break;
        case HioTypeFloat:
            preprocess = [](const uint8_t *texel, int bias) {
                return reinterpret_cast<const float *>(texel)[bias];
            };
            break;
        case HioTypeDouble: break;
        case HioTypeCount: break;
    }
    for (int i = 0; i < componentCount; ++i) {
        dst[i] = preprocess(texel00, i) * (1 - s) * (1 - t) + preprocess(texel10, i) * s * (1 - t) +
                 preprocess(texel01, i) * (1 - s) * t + preprocess(texel11, i) * s * t;
    }
    for (int i = componentCount; i < 4; ++i) {
        dst[i] = 1.0f;
    }
}

GfVec4f Texture2D::Evaluate(const GfVec2f &uv) const
{
    // Check if the texture is valid
    if (!texture) {
        return {};
    }

    // Convert UV coordinates to texture coordinates
    float u = uv[0];
    float v = uv[1];

    // Clamp texture coordinates to [0, 1]
    u = std::clamp(u, 0.0f, 1.0f);
    v = std::clamp(v, 0.0f, 1.0f);

    // Calculate texture coordinates in pixel space
    float x = u * (texture->GetWidth() - 2);
    float y = v * (texture->GetHeight() - 2);

    // Get the four nearest texel coordinates
    int x0 = static_cast<int>(std::floor(x));
    int x1 = x0 + 1;
    int y0 = static_cast<int>(std::floor(y));
    int y1 = y0 + 1;

    // Calculate the weights for bilinear interpolation
    float s = x - x0;
    float t = y - y0;

    // Sample the four nearest texels
    const unsigned char *texels = static_cast<const unsigned char *>(storageSpec.data);
    int texelSize = texture->GetBytesPerPixel();
    const unsigned char *texel00 = texels + (y0 * texture->GetWidth() + x0) * texelSize;
    const unsigned char *texel01 = texels + (y1 * texture->GetWidth() + x0) * texelSize;
    const unsigned char *texel10 = texels + (y0 * texture->GetWidth() + x1) * texelSize;
    const unsigned char *texel11 = texels + (y1 * texture->GetWidth() + x1) * texelSize;

    auto format = storageSpec.format;

    GfVec4f value4f;

    auto *dst = value4f.data();
    _Interpolate(format, s, t, texel00, texel01, texel10, texel11, dst);
    return value4f;
}

Texture2D::~Texture2D()
{
    free(storageSpec.data);
}
USTC_CG_NAMESPACE_CLOSE_SCOPE