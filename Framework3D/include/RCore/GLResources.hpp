#pragma once

#include "USTC_CG.h"
#include "pxr/base/gf/vec2i.h"
#include "pxr/imaging/garch/glApi.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

#define RESOURCE_LIST Texture

struct TextureDesc {
    pxr::GfVec2i size;

    friend bool operator==(const TextureDesc& lhs, const TextureDesc& rhs)
    {
        return lhs.size == rhs.size;
    }

    friend bool operator!=(const TextureDesc& lhs, const TextureDesc& rhs)
    {
        return !(lhs == rhs);
    }
};

struct TextureHandle {
    TextureDesc desc;
    GLuint texture_id;
};

inline TextureHandle createTexture(const TextureDesc& desc)
{
    TextureHandle ret;
    ret.desc = desc;
    glCreateTextures(GL_TEXTURE_2D, 1, &ret.texture_id);
    return ret;
}

inline void destroyTexture(TextureHandle texture)
{
    glDeleteTextures(1, &texture.texture_id);
}

#define DESC_HANDLE_TRAIT(RESOURCE)        \
    template<>                             \
    struct ResouceDesc<RESOURCE##Handle> { \
        using Desc = RESOURCE##Desc;       \
    };

#define HANDLE_DESC_TRAIT(RESOURCE)        \
    template<>                             \
    struct DescResouce<RESOURCE##Desc> {   \
        using Resource = RESOURCE##Handle; \
    };

template<typename RESOURCE>
struct ResouceDesc {
    using Desc = void;
};

template<typename DESC>
struct DescResouce {
    using Resource = void;
};
USTC_CG_NAMESPACE_CLOSE_SCOPE