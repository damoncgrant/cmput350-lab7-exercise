#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <cstddef>  // size_t

#include "pack.h"  // Pack from prep
#include "registry.h"
#include <limits>
#include <cassert>
#include <type_traits>
#include <array>
#include <tuple>
#include <utility>

using EntityID = size_t;

template <typename... ComponentTypes>
class EntityManager {
    
    public:
        EntityID createEntity() {
            if (recycled.empty()) {
                EntityID id = validity.size();

                // extend each vector to include new entity
                for (auto& componentIndexes : ids) {
                    componentIndexes.push_back(invalid);
                }

                validity.push_back(true);
                return id;

            } else { 
                EntityID id = recycled.back();
                recycled.pop_back();

                // reset all entries
                for (auto& componentIndexes : ids) {
                    componentIndexes[id] = invalid;
                }

                validity[id] = true;
                return id;
            }
        }

        bool validEntity(EntityID id) const {
            return id < validity.size() && validity[id] == true;
        }

        template <typename ComponentType>
        bool hasComponent(EntityID id) const {
            static_assert(ComponentPack::template contains<ComponentType>);
            assert(validEntity(id));

            constexpr auto typeIndex = ComponentPack::template index<ComponentType>;

            return ids[typeIndex][id] != invalid;
        }

        template <typename ComponentType> 
        ComponentType& getComponent(EntityID id) {
            static_assert(ComponentPack::template contains<ComponentType>);
            assert(validEntity(id));

            constexpr auto typeIndex = ComponentPack::template index<ComponentType>;

            const auto entry = ids[typeIndex][id];
            assert(entry != invalid);

            return std::get<typeIndex>(components)[entry];
        }

        template <typename ComponentType> 
        void addComponent(EntityID id, ComponentType&& component) {
            
            using type = std::remove_reference_t<ComponentType>;

            static_assert(ComponentPack::template contains<type>);
            assert(validEntity(id));
            assert(!(hasComponent<type>(id)));

            constexpr auto typeIndex = ComponentPack::template index<type>;

            auto& registry = std::get<typeIndex>(components); // modify actual registry rather than copy
            auto entry = registry.store(std::forward<ComponentType>(component));
            
            ids[typeIndex][id] = entry;
        }

    private:
        using ComponentPack = Pack<ComponentTypes...>;

        static constexpr EntityID invalid = std::numeric_limits<size_t>::max();

        std::tuple<Registry<ComponentTypes>...> components;

        // ids[componentTypeIndex][entityId]
        std::array<std::vector<EntityID>, sizeof...(ComponentTypes)> ids;
        std::vector<bool> validity;
        std::vector<EntityID> recycled;
};

#endif  // ENTITY_MANAGER_H
