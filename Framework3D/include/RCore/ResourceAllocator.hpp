#pragma once

#include <algorithm>
#include <cassert>

#include "USTC_CG.h"
#include "Utils/Functions/GenericPointer.hpp"
#include "Utils/Macro/map.h"
USTC_CG_NAMESPACE_OPEN_SCOPE

MACRO_MAP(DESC_HANDLE_TRAIT, RESOURCE_LIST)
MACRO_MAP(HANDLE_DESC_TRAIT, RESOURCE_LIST)

template<typename RESOURCE>
using desc = typename ResouceDesc<RESOURCE>::Desc;

template<typename DESC>
using resc = typename DescResouce<DESC>::Resource;

class ResourceAllocator {
#define CACHE_NAME(RESOURCE)   m##RESOURCE##Cache
#define INUSE_NAME(RESOURCE)   mInUse##RESOURCE
#define PAYLOAD_NAME(RESOURCE) RESOURCE##CachePayload
#define CACHE_SIZE(RESOURCE)   m##RESOURCE##CacheSize

#define JUDGE_RESOURCE_DYNAMIC(RSC) if (CPPType::get<RSC##Handle>() == *handle.type())
#define JUDGE_RESOURCE(RSC)         if constexpr (std::is_same_v<RSC##Handle, RESOURCE>)

#define RESOLVE_DESTROY_DYNAMIC(RESOURCE)                                                  \
    RESOURCE##Handle h;                                                                    \
    handle.type()->copy_construct(handle.get(), &h);                                       \
    if (h) {                                                                               \
        PAYLOAD_NAME(RESOURCE) payload{ h, mAge, 0 };                                      \
        resolveCacheDestroy(                                                               \
            h, CACHE_SIZE(RESOURCE), payload, CACHE_NAME(RESOURCE), INUSE_NAME(RESOURCE)); \
    }

#define RESOLVE_DESTROY(RESOURCE)                      \
    PAYLOAD_NAME(RESOURCE) payload{ handle, mAge, 0 }; \
    resolveCacheDestroy(                               \
        handle, CACHE_SIZE(RESOURCE), payload, CACHE_NAME(RESOURCE), INUSE_NAME(RESOURCE));

   public:
    explicit ResourceAllocator() noexcept;

#define CHECK_EMPTY(RESOURCE)             \
    assert(!CACHE_NAME(RESOURCE).size()); \
    assert(!INUSE_NAME(RESOURCE).size());

    ~ResourceAllocator() noexcept
    {
        {
            terminate();
            MACRO_MAP(CHECK_EMPTY, RESOURCE_LIST)
        }
    }

#define CLEAR_CACHE(RESOURCE)                                                         \
    assert(!INUSE_NAME(RESOURCE).size());                                             \
    for (auto it = CACHE_NAME(RESOURCE).begin(); it != CACHE_NAME(RESOURCE).end();) { \
        it->second.handle = nullptr;                                                  \
        it = CACHE_NAME(RESOURCE).erase(it);                                          \
    }

    void terminate() noexcept
    {
        MACRO_MAP(CLEAR_CACHE, RESOURCE_LIST)
    }

#define FOREACH_DESTROY_DYNAMIC(RESOURCE) \
    JUDGE_RESOURCE_DYNAMIC(RESOURCE)      \
    {                                     \
        RESOLVE_DESTROY_DYNAMIC(RESOURCE) \
    }

    void destroy(GMutablePointer handle) noexcept
    {
        if constexpr (mEnabled) {
            // If code runs here, It means some of your output resource is not created;
            MACRO_MAP(FOREACH_DESTROY_DYNAMIC, RESOURCE_LIST)
        }
        else {
            handle = nullptr;
        }
    }

#define FOREACH_DESTROY(NAMESPACE, RESOURCE) \
    JUDGE_RESOURCE(NAMESPACE, RESOURCE)      \
    {                                        \
        RESOLVE_DESTROY(NAMESPACE, RESOURCE) \
    }

    template<typename RESOURCE>
    void destroy(RESOURCE& handle) noexcept
    {
        if constexpr (mEnabled) {
            MACRO_MAP(FOREACH_DESTROY, RESOURCE_LIST)
        }
        else {
            handle = nullptr;
        }
    }

#define GC_TYPE(RSC) gc_type<RSC##Handle>(CACHE_SIZE(RSC), CACHE_NAME(RSC));

    void gc() noexcept
    {
        MACRO_MAP(GC_TYPE, RESOURCE_LIST)
    }

#define RESOLVE_CREATE(RESOURCE) \
    resolveCacheCreate(          \
        handle, desc, CACHE_SIZE(RESOURCE), CACHE_NAME(RESOURCE), INUSE_NAME(RESOURCE), rest...);

#define FOREACH_CREATE(RESOURCE) \
    JUDGE_RESOURCE(RESOURCE)     \
    {                            \
        RESOLVE_CREATE(RESOURCE) \
    }

    template<typename DESC, typename RESOURCE = resc<DESC>, typename... Args>
    RESOURCE create(const DESC& desc, Args&&... rest)
    {
        RESOURCE handle;

        if constexpr (mEnabled) {
            MACRO_MAP(FOREACH_CREATE, RESOURCE_LIST)
        }
        else {
            handle = create_resource(desc, std::forward<Args>(rest)...);
        }
        return handle;
    }

#define DEFINEContainer(RESOURCE)                                                                  \
    struct PAYLOAD_NAME(RESOURCE) {                                                                \
        RESOURCE##Handle handle;                                                                   \
        size_t age = 0;                                                                            \
        uint32_t size = 0;                                                                         \
    };                                                                                             \
    using RESOURCE##CacheContainer = AssociativeContainer<RESOURCE##Desc, RESOURCE##CachePayload>; \
    using RESOURCE##InUseContainer = AssociativeContainer<RESOURCE##Handle, RESOURCE##Desc>;       \
    RESOURCE##CacheContainer CACHE_NAME(RESOURCE);                                                 \
    RESOURCE##InUseContainer INUSE_NAME(RESOURCE);                                                 \
    uint32_t CACHE_SIZE(RESOURCE) = 0;

#define PURGE(RESOURCE)                                                                     \
    RESOURCE##CacheContainer::iterator purge(const RESOURCE##CacheContainer::iterator& pos) \
    {                                                                                       \
        pos->second.handle = nullptr;                                                       \
        m##RESOURCE##CacheSize -= pos->second.size;                                         \
        return CACHE_NAME(RESOURCE).erase(pos);                                             \
    }

   private:
#define CREATE_CONCRETE(RESOURCE)               \
    JUDGE_RESOURCE(RESOURCE)                    \
    {                                           \
        return create##RESOURCE(desc, rest...); \
    }

    template<typename RESOURCE, typename... Args>
    RESOURCE create_resource(const desc<RESOURCE>& desc, Args&&... rest)
    {
        MACRO_MAP(CREATE_CONCRETE, RESOURCE_LIST)
    }

    template<typename RESOURCE>
    void resolveCacheCreate(
        RESOURCE& handle,
        auto& desc,
        auto& cacheSize,
        auto&& cache,
        auto&& inUseCache,
        auto&&... rest)
    {
        auto it = cache.find(desc);
        if (it != cache.end()) {
            // we do, move the entry to the in-use list, and remove from the cache
            handle = it->second.handle;
            cacheSize -= it->second.size;
            cache.erase(it);
        }
        else {
            handle = create_resource<RESOURCE>(desc, rest...);
        }
        inUseCache.emplace(handle, desc);
    }

    template<typename RESOURCE>
    uint32_t calcSize(desc<RESOURCE>& key)
    {
        return 0;
    }

    template<typename RESOURCE>
    void resolveCacheDestroy(
        RESOURCE& handle,
        auto& cacheSize,
        auto& cachePayload,
        auto&& cache,
        auto&& inUseCache)
    {
        // find the texture in the in-use list (it must be there!)
        auto it = inUseCache.find(handle);
        assert(it != inUseCache.end());

        // move it to the cache
        auto key = it->second;

        cachePayload.size = calcSize<RESOURCE>(key);

        // cache.emplace(key, CachePayload{ handle, mAge, size });
        cache.emplace(key, cachePayload);
        cacheSize += cachePayload.size;

        // remove it from the in-use list
        inUseCache.erase(it);
    }

    template<typename RESOURCE>
    void gc_type(auto& cacheSize, auto&& cache_in)
    {
        const size_t age = mAge++;

        for (auto it = cache_in.begin(); it != cache_in.end();) {
            const size_t ageDiff = age - it->second.age;
            if (ageDiff >= CACHE_MAX_AGE) {
                it = purge(it);
                if (cacheSize < CACHE_CAPACITY) {
                    break;
                }
            }
            else {
                ++it;
            }
        }

        if ((cacheSize >= CACHE_CAPACITY)) {
            using ContainerType = std::remove_cvref_t<decltype(cache_in)>;
            using Vector = std::vector<
                std::pair<typename ContainerType::key_type, typename ContainerType::value_type>>;
            auto cache = Vector();
            std::copy(cache_in.begin(), cache_in.end(), std::back_insert_iterator<Vector>(cache));

            std::sort(cache.begin(), cache.end(), [](const auto& lhs, const auto& rhs) {
                return lhs.second.age < rhs.second.age;
            });

            auto curr = cache.begin();
            while (cacheSize >= CACHE_CAPACITY) {
                purge(cache_in.find(curr->first));
                ++curr;
            }

            size_t oldestAge = cache.front().second.age;
            for (auto& it : cache_in) {
                it.second.age -= oldestAge;
            }
            mAge -= oldestAge;
        }
    }

    static constexpr size_t CACHE_CAPACITY = 64u << 20u;  // 64 MiB
    static constexpr size_t CACHE_MAX_AGE = 30u;

    template<typename T>
    struct Hasher {
        std::size_t operator()(const T& s) const noexcept
        {
            return hash_value(s);
        }
    };

    void dump(bool brief = false, uint32_t cacheSize = 0) const noexcept;

    template<typename Key, typename Value, typename Hasher = Hasher<Key>>
    class AssociativeContainer {
        // We use a std::vector instead of a std::multimap because we don't expect many items
        // in the cache and std::multimap generates tons of code. std::multimap starts getting
        // significantly better around 1000 items.
        using Container = std::vector<std::pair<Key, Value>>;
        Container mContainer;

       public:
        AssociativeContainer();
        ~AssociativeContainer() noexcept;
        using iterator = typename Container::iterator;
        using const_iterator = typename Container::const_iterator;
        using key_type = typename Container::value_type::first_type;
        using value_type = typename Container::value_type::second_type;

        size_t size() const
        {
            return mContainer.size();
        }

        iterator begin()
        {
            return mContainer.begin();
        }

        const_iterator begin() const
        {
            return mContainer.begin();
        }

        iterator end()
        {
            return mContainer.end();
        }

        const_iterator end() const
        {
            return mContainer.end();
        }

        iterator erase(iterator it);
        const_iterator find(const key_type& key) const;
        iterator find(const key_type& key);
        template<typename... ARGS>
        void emplace(ARGS&&... args);
    };

#define CONTAINER_RELATED(RESOURCE) \
    DEFINEContainer(RESOURCE);      \
    PURGE(RESOURCE)

    MACRO_MAP(CONTAINER_RELATED, RESOURCE_LIST);

    size_t mAge = 0;
    static constexpr bool mEnabled = true;
};

template<typename K, typename V, typename H>
ResourceAllocator::AssociativeContainer<K, V, H>::AssociativeContainer()
{
    mContainer.reserve(128);
}

template<typename K, typename V, typename H>

ResourceAllocator::AssociativeContainer<K, V, H>::~AssociativeContainer() noexcept
{
}

template<typename K, typename V, typename H>
typename ResourceAllocator::AssociativeContainer<K, V, H>::iterator
ResourceAllocator::AssociativeContainer<K, V, H>::erase(iterator it)
{
    return mContainer.erase(it);
}

template<typename K, typename V, typename H>
typename ResourceAllocator::AssociativeContainer<K, V, H>::const_iterator
ResourceAllocator::AssociativeContainer<K, V, H>::find(const key_type& key) const
{
    return const_cast<AssociativeContainer*>(this)->find(key);
}

template<typename K, typename V, typename H>
typename ResourceAllocator::AssociativeContainer<K, V, H>::iterator
ResourceAllocator::AssociativeContainer<K, V, H>::find(const key_type& key)
{
    return std::find_if(
        mContainer.begin(), mContainer.end(), [&key](const auto& v) { return v.first == key; });
}

template<typename K, typename V, typename H>
template<typename... ARGS>
void ResourceAllocator::AssociativeContainer<K, V, H>::emplace(ARGS&&... args)
{
    mContainer.emplace_back(std::forward<ARGS>(args)...);
}

inline ResourceAllocator::ResourceAllocator() noexcept
{
}
USTC_CG_NAMESPACE_CLOSE_SCOPE