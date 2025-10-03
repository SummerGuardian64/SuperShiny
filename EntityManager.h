#pragma once
#include "StepContext.h"
#include "Entity.h"
#include <list>
#include <memory>
#include <vector>
#include <iterator>

namespace ssge
{
    // Forward declare
    class Entity;
    class EntityAllocator;
    class EntityReference;
    enum class EntityClassID : int;

    // Reference (non-owning, like weak_ptr)
    class EntityReference {
        std::weak_ptr<Entity> ref;
    public:
        EntityReference() = default;
        EntityReference(const std::weak_ptr<Entity>& r) : ref(r) {}
        EntityReference(const std::shared_ptr<Entity>& r) : ref(r) {}

        std::weak_ptr<Entity>& get() { return ref; }
        const std::weak_ptr<Entity>& get() const { return ref; }

        operator std::weak_ptr<Entity>& () { return ref; }
        operator const std::weak_ptr<Entity>& () const { return ref; }

        // <-- NEW: make it usable in if(ref) checks
        explicit operator bool() const noexcept { return !ref.expired(); }
    };

    // Allocator (owning, like shared_ptr)
    class EntityAllocator {
        std::shared_ptr<Entity> ptr;
    public:
        EntityAllocator() = default;
        EntityAllocator(const std::shared_ptr<Entity>& p) : ptr(p) {}

        Entity* get() const { return ptr.get(); }
        Entity& operator*() const { return *ptr; }
        Entity* operator->() const { return ptr.get(); }

        bool operator==(const EntityAllocator& other) const { return ptr == other.ptr; }
        bool operator!=(const EntityAllocator& other) const { return ptr != other.ptr; }

        // Allow implicit conversion back to shared_ptr when needed
        operator std::shared_ptr<Entity>&() { return ptr; }
        operator const std::shared_ptr<Entity>&() const { return ptr; }
        
        // implicit conversion to EntityReference
        operator EntityReference() const { return EntityReference(ptr); }

        // <-- NEW: make it usable in if(entity) checks
        explicit operator bool() const noexcept { return static_cast<bool>(ptr); }
    };

    // Collection (list of owners)
    class EntityCollection {
        std::list<EntityAllocator> entities;
    public:
        using iterator = std::list<EntityAllocator>::iterator;
        using const_iterator = std::list<EntityAllocator>::const_iterator;

        void add(EntityAllocator alloc) { entities.push_back(std::move(alloc)); }

        // We only push allocators, since this is the owner container
        void push_back(EntityAllocator alloc) {
            entities.push_back(std::move(alloc));
        }

        iterator begin() { return entities.begin(); }
        iterator end() { return entities.end(); }
        const_iterator begin() const { return entities.begin(); }
        const_iterator end() const { return entities.end(); }
        
        // expose erase (standard list erase)
        iterator erase(iterator pos) { return entities.erase(pos); }
        iterator erase(iterator first, iterator last) { return entities.erase(first, last); }

        bool empty() const { return entities.empty(); }
        size_t size() const { return entities.size(); }
    };

    // Query result (vector of refs)
    class EntityQueryResult {
        std::vector<EntityReference> refs;

    public:
        using iterator = std::vector<EntityReference>::iterator;
        using const_iterator = std::vector<EntityReference>::const_iterator;

        EntityQueryResult() = default;

        // Add from an EntityReference directly
        void push_back(const EntityReference& ref) {
            refs.push_back(ref);
        }

        // Add from an EntityAllocator (convert to weak_ptr)
        void push_back(const EntityAllocator& alloc) {
            refs.push_back(alloc);
        }

        // Optional: alias for "add" if you want consistency
        void add(const EntityReference& ref) { push_back(ref); }

        iterator begin() { return refs.begin(); }
        iterator end() { return refs.end(); }
        const_iterator begin() const { return refs.begin(); }
        const_iterator end() const { return refs.end(); }

        bool empty() const { return refs.empty(); }
        size_t size() const { return refs.size(); }

        EntityReference operator[](size_t i) const { return refs[i]; }
    };

	class EntityManager
	{

    public:
        void step(GameWorldStepContext& context);
        void draw(DrawContext& context);

        EntityCollection entities;
        EntityCollection::iterator getEntitiesBegin();
        EntityCollection::iterator getEntitiesEnd();
        EntityReference addEntity(EntityAllocator entity);
        bool deleteEntity(EntityAllocator entity);
        Entity* findEntity(EntityClassID entityClassID);
        const Entity* findConstEntity(EntityClassID entityClassID) const;
        EntityQueryResult findAllEntities(EntityClassID entityClassID);
        int countAllEntities() const;
        int countAllEntities(EntityClassID entityClassID) const;
        void destroyScheduledEntities(GameWorldStepContext& context);
	};
}