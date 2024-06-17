#pragma once

#include "RCore/ResourceAllocator.hpp"
#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
extern ResourceAllocator resource_allocator;

template<typename T>
class RAII_resource_cleaner {
   public:
    RAII_resource_cleaner(ResourceAllocator& allocator) : allocator_(allocator)
    {
    }

    T set_data(T handle)
    {
        data = handle;
        return data;
    }

    ~RAII_resource_cleaner()
    {
        allocator_.destroy(data);
    }

   private:
    ResourceAllocator& allocator_;
    T data;
};

#define MARK_DESTROY_NVRHI_RESOURCE(resource)                     \
    RAII_resource_cleaner<decltype(resource)> resource##_cleaner( \
        resource_allocator);                                      \
    resource##_cleaner.set_data(resource)
USTC_CG_NAMESPACE_CLOSE_SCOPE