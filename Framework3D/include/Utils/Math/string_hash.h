#pragma once
#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
constexpr uint32_t hash_str_to_uint32(const std::string& str)
{
    uint32_t hash = 5381; // Initial hash value

    // Iterate through each character in the string
    for (size_t i = 0; i < str.size(); ++i) {
        hash = ((hash << 5) + hash) + static_cast<uint8_t>(str[i]);
    }

    return hash;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
