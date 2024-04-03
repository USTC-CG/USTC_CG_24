#pragma once

#define BASIC_SOCKET_TYPES Int, String, Float

#define STAGE_SOCKET_TYPES Layer

#define RENDER_SOCKET_TYPES Lights, Camera

#define BUFFER_TYPES                                                                          \
    Int1Buffer, Int2Buffer, Int3Buffer, Int4Buffer, Float1Buffer, Float2Buffer, Float3Buffer, \
        Float4Buffer

#define GEO_SOCKET_TYPES Geometry, BUFFER_TYPES

#define ALL_SOCKET_TYPES \
    BASIC_SOCKET_TYPES, STAGE_SOCKET_TYPES, RENDER_SOCKET_TYPES, GEO_SOCKET_TYPES
