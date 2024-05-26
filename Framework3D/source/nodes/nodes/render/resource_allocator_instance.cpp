#include "resource_allocator_instance.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
std::unique_ptr<ResourceAllocator> resource_allocator = std::make_unique<ResourceAllocator>();
USTC_CG_NAMESPACE_CLOSE_SCOPE